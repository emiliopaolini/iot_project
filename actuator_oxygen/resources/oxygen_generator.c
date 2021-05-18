#include "contiki.h"
#include "coap-engine.h"
#include "sys/log.h"

#include "coap-engine.h"

#define GOOD_OXYGEN_LEVEL 25



static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_put_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler();

extern float current_oxygen_level;

bool status = false;

EVENT_RESOURCE(oxygen_generator,
        "title=\"Oxygen actuator\"; GET/POST/PUT; status=on|off; rt=\"actuator\";\n",
        res_get_handler, res_post_put_handler, res_post_put_handler, NULL, res_event_handler);

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
    //this will respond to GET request
    //it will send the status of the air oxygen generator
}

static void res_post_put_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
    //this will respond to POST/PUT request
    //it will update the status of the air oxygen generator
}

static void res_event_handler() {
    if(current_oxygen_level <= GOOD_OXYGEN_LEVEL){
        status = false;
    }
    else   
        status = true;
}
