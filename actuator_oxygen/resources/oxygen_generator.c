#include "contiki.h"
#include "coap-engine.h"
#include "sys/log.h"

#include "coap-engine.h"

#define GOOD_OXYGEN_LEVEL 25

#define MAX_AGE 60

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_put_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler();

extern float oxygen_level;

int status = 0;

EVENT_RESOURCE(oxygen_generator,
        "title=\"Oxygen actuator\"; GET/POST/PUT; status=1|0; rt=\"actuator\";\n",
        res_get_handler, res_post_put_handler, res_post_put_handler, NULL, res_event_handler);

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
    //this will respond to GET request
    unsigned int accept = APPLICATION_JSON;
    coap_get_header_accept(request, &accept);

    if (accept == APPLICATION_JSON) {
        coap_set_header_content_format(response, APPLICATION_JSON);
        snprintf((char *)buffer, COAP_MAX_CHUNK_SIZE, "{\"status\":%d}", status);
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
}

static void res_event_handler() {
    
    if(oxygen_level <= GOOD_OXYGEN_LEVEL){
        status = 1;
    }
    else   
        status = 0;
    printf("My status is : %d\n",status);
    coap_notify_observers(&oxygen_generator);
}
