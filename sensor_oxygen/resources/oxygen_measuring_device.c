#include "contiki.h"
#include "coap-engine.h"
#include "sys/log.h"

#include "coap-engine.h"

#include <time.h>

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler();


float oxygen_level = 22; // normally should be between 19 and 22
#define MAX_OXYGEN_LEVEL 40 
#define MAX_AGE 60

extern bool actuator_status;

float randInRange(float min, float max){
  return (max-min) * ((float)rand()/RAND_MAX)+min;
}



EVENT_RESOURCE(
    oxygen_measuring_device,
    "title=\"Oxygen sensor\"; GET; rt=\"sensor\"; obs\n",
    res_get_handler, NULL, NULL, NULL, res_event_handler);


static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
    //this will respond to GET request
    unsigned int accept = APPLICATION_JSON;
    coap_get_header_accept(request, &accept);

    if (accept == APPLICATION_JSON) {
        coap_set_header_content_format(response, APPLICATION_JSON);
        snprintf((char *)buffer, COAP_MAX_CHUNK_SIZE, "{\"oxygen\":%.1f}", oxygen_level);
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
    // associated with the GET methods is called

    //randomly change oxygen level
    oxygen_level += randInRange(-2.0,2.0);

    //notify coap clients subscribed to this resource
    coap_notify_observers(&oxygen_measuring_device);
}

