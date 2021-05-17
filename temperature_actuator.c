#include "coap-blocking-api.h"
#include "coap-engine.h"
#include "contiki-net.h"
#include "contiki.h"
#include "dev/button-hal.h"
#include "dev/leds.h"
#include "sys/log.h"


// log 
#define LOG_MODULE "Temperature_node"
#define LOG_LEVEL LOG_LEVEL_DBG

//utility
#define SERVER_EP "coap://[fd00::1]:5683"


// sensor
extern coap_resource_t air_conditioner; // sensor for oxygen measurement


PROCESS(temperature_actuator, "Temperature actuator");
AUTOSTART_PROCESSES(&temperature_actuator);




PROCESS_THREAD(temperature_actuator, ev, data) {

	static coap_endpoint_t actuator_ep;
	static coap_message_t request[1];
	static coap_endpoint_t server_ep;
    
    PROCESS_BEGIN();
    
    LOG_INFO("Temperature actuator: starting.... \n");
    
    coap_activate_resource(&air_conditioner, "actuator/temperature");
    
	coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep); //initialize server endpoint

    // SERVER REGISTRATION 
    
    // @TODO

    // END SERVER REGISTRATION


	
	
    }

    PROCESS_END();
}