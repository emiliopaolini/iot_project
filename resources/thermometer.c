#include "contiki.h"
#include "coap-engine.h"
#include "sys/log.h"

#include "coap-engine.h"


static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler();

EVENT_RESOURCE(
    thermometer,
    "title=\"Temperature sensor\"; GET; rt=\"sensor\"; obs\n",
    res_get_handler, NULL, NULL, NULL, res_event_handler);

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
    //this will respond to GET request
    //send oxygen measurement in a random way
}

static void res_event_handler(){
    // Notify all the observers
    // Before sending the notification the handler
    // associated with the GET methods is called
    coap_notify_observers(&res_event);
}

