#include "contiki.h"
#include "coap-engine.h"
#include "sys/log.h"

#include "coap-engine.h"



#define MAX_AGE 60





enum leds {GREEN, YELLOW,RED} alert_level = GREEN;

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_put_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler();

extern float oxygen_level;

int status = 0;
int GOOD_OXYGEN_LEVEL = 25;
int manualMode = 0;



static void checkAlertLevel(){
	if(oxygen_level <= GOOD_OXYGEN_LEVEL){
		alert_level = RED;
	}
	else{
		 if(oxygen_level<=(GOOD_OXYGEN_LEVEL+GOOD_OXYGEN_LEVEL*0.3))
	    		alert_level = YELLOW;
	    	else
			alert_level = GREEN;
	}

}

EVENT_RESOURCE(oxygen_generator,
        "title=\"Oxygen actuator\"; rt=\"actuator\";obs\n",
        res_get_handler, res_post_put_handler, res_post_put_handler, NULL, res_event_handler);

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
    //this will respond to GET request
    unsigned int accept = APPLICATION_JSON;
    coap_get_header_accept(request, &accept);

    if (accept == APPLICATION_JSON) {
        coap_set_header_content_format(response, APPLICATION_JSON);
        snprintf((char *)buffer, COAP_MAX_CHUNK_SIZE, "{\"status\":%d,\"threshold\":%d}", status,GOOD_OXYGEN_LEVEL);
        coap_set_payload(response, buffer, strlen((char *)buffer));
    } else {
        coap_set_status_code(response, NOT_ACCEPTABLE_4_06);
        const char *msg = "Supporting content-type application/json";
        coap_set_payload(response, msg, strlen(msg));
    }

    coap_set_header_max_age(response, MAX_AGE);
}

static void res_post_put_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
    //this will respond to POST/PUT request
    //it will update the status of the air oxygen generator
	printf("inside post/put handler\n");
	const char *value = NULL;
	

	static char *string_received;
	
	//size_t len = coap_get_query_variable(request,"status",&value);
	size_t len = coap_get_post_variable(request,"status",&value);
	printf("len is equal to: %d \n",len);
	
	if(len !=0){
		// receive a status
		printf("receive a status\n");
		string_received = malloc(len+1);
		memcpy(string_received,value,len);
		string_received[len+1] = '\0';	
		int newStatus = atoi(string_received);
		printf("new status receives id %d\n",newStatus);
		if(newStatus != status){
			status = newStatus;
			coap_notify_observers(&oxygen_generator);
		}
	}

	//len = coap_get_query_variable(request,"threshold",&value);
	len = coap_get_post_variable(request,"oxygen_threshold",&value);	
	if(len != 0){
		//receive a threshold
		printf("receive a threshold\n");
		string_received = malloc(len+1);
		memcpy(string_received,value,len);
		string_received[len+1] = '\0';	
		int newThreshold = atoi(string_received);
		GOOD_OXYGEN_LEVEL = newThreshold;	
	}

	//len = coap_get_query_variable(request,"threshold",&value);
	len = coap_get_post_variable(request,"manualMode",&value);	
	if(len != 0){
		//receive a threshold
		printf("receive manual mode\n");
		string_received = malloc(len+1);
		memcpy(string_received,value,len);
		string_received[len+1] = '\0';	
		manualMode = atoi(string_received);
		printf("manual mode received: %d\n",manualMode);
		
	}
}



static void res_event_handler() {
    	checkAlertLevel();
	if(manualMode == 0){
		int newStatus = 0;
		if(oxygen_level <= GOOD_OXYGEN_LEVEL){
			newStatus= 1;
		}
		else{
			newStatus = 0;
		}
		
		if(newStatus!=status){
			status=newStatus;
			coap_notify_observers(&oxygen_generator);
		}
		printf("My status is : %d\n",status);
	}
}



