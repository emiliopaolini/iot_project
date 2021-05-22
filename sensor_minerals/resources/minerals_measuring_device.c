#include "contiki.h"
#include "coap-engine.h"
#include "sys/log.h"

#include "coap-engine.h"



static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler();


float minerals_level = 60; // normally should be between 50 and 70
#define MAXIMUM_MINERALS_LEVEL 100
#define MINIMUM_MINERALS_LEVEL 1
#define MAX_AGE 60

extern bool actuator_status;

float randInRange(float min, float max){
  
  return (max-min) * ((float)rand()/RAND_MAX)+min;
}



EVENT_RESOURCE(
    minerals_measuring_device,
    "title=\"Minerals sensor\"; GET; rt=\"sensor\"; obs\n",
    res_get_handler, NULL, NULL, NULL, res_event_handler);


static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
    //this will respond to GET request

    


    unsigned int accept = APPLICATION_JSON;
    coap_get_header_accept(request, &accept);

    if (accept == APPLICATION_JSON) {
        coap_set_header_content_format(response, APPLICATION_JSON);
        snprintf((char *)buffer, COAP_MAX_CHUNK_SIZE, "{\"value\":%.1f}", minerals_level);
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

    //randomly change level minerals
    //make sure that the changes will be between MINIMUM_MINERALS_LEVEL and MAXIMUM_MINERALS_LEVEL
    minerals_level += randInRange(-5.0,5.0);

    if(minerals_level>=MAXIMUM_MINERALS_LEVEL)
	minerals_level = MAXIMUM_MINERALS_LEVEL;

    if(minerals_level<=MINIMUM_MINERALS_LEVEL)
	minerals_level = MINIMUM_MINERALS_LEVEL;
    //notify coap clients subscribed to this resource
    coap_notify_observers(&minerals_measuring_device);
}

