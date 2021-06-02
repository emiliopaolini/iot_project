#include "contiki.h"
#include "coap-engine.h"
#include "sys/log.h"

#include "coap-engine.h"



static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
// this is for making the simulation consistent
static void res_post_put_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler();


float oxygen_level = 22; // normally should be between 19 and 22
#define MAXIMUM_OXYGEN_LEVEL 100
#define MINIMUM_OXYGEN_LEVEL 1
#define MAX_AGE 60


int actuatorStatus = 0;

float randInRange(float min, float max){
  
  return (max-min) * ((float)rand()/RAND_MAX)+min;
}



EVENT_RESOURCE(
    oxygen_measuring_device,
    "title=\"Oxygen sensor\"; GET; rt=\"sensor\"; obs\n",
    res_get_handler, res_post_put_handler, res_post_put_handler, NULL, res_event_handler);


static void res_post_put_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
	printf("Receive an update from the actuator\n");
	const char *value = NULL;
	static char *string_received;
	
	//size_t len = coap_get_query_variable(request,"status",&value);
	size_t len = coap_get_post_variable(request,"actuator_status",&value);
	printf("len is equal to: %d \n",len);
	
	if(len !=0){
		// receive a status
		string_received = malloc(len+1);
		memcpy(string_received,value,len);
		string_received[len+1] = '\0';	
		int newStatus = atoi(string_received);
		printf("new status received is %d\n",newStatus);
		if(newStatus != actuatorStatus){
			actuatorStatus = newStatus;
		}
	}

}


static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
    //this will respond to GET request

    unsigned int accept = APPLICATION_JSON;
    coap_get_header_accept(request, &accept);

    if (accept == APPLICATION_JSON) {
        coap_set_header_content_format(response, APPLICATION_JSON);
        snprintf((char *)buffer, COAP_MAX_CHUNK_SIZE, "{\"value\":%.1f}", oxygen_level);
        coap_set_payload(response, buffer, strlen((char *)buffer));
    } else {
        coap_set_status_code(response, NOT_ACCEPTABLE_4_06);
        const char *msg = "Supporting content-type application/json";
        coap_set_payload(response, msg, strlen(msg));
    }

    coap_set_header_max_age(response, MAX_AGE);
    
}

static void res_event_handler(){
    // Notify all the observers
    // Before sending the notification the handler
    // associated with the GET method is called
    // 

    // make simulation consistent
    if(actuatorStatus==0){
	printf("actuator is off.. oxygen is decreasing\n");
    	oxygen_level += randInRange(-2,0);
    }
    else{
	printf("actuator is on.. oxygen is increasing\n");
	oxygen_level += randInRange(2,4.5);
    }
    
    //make sure that the changes will be between MINIMUM_OXYGEN_LEVEL and MAXIMUM_OXYGEN_LEVEL
    

    if(oxygen_level>=MAXIMUM_OXYGEN_LEVEL)
	    oxygen_level = MAXIMUM_OXYGEN_LEVEL;

    if(oxygen_level<=MINIMUM_OXYGEN_LEVEL)
	    oxygen_level = MINIMUM_OXYGEN_LEVEL;
    //notify coap clients subscribed to this resource
    coap_notify_observers(&oxygen_measuring_device);
}

