#include "dev/button-hal.h"
#include "dev/leds.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"
#include <time.h>


//utility
#define SERVER_EP "coap://[fd00::1]:5683"

#define TIMER_PERIOD 10 //period before acquiring a new measurement


static char registered = '0';

// sensor
extern coap_resource_t oxygen_measuring_device; // sensor for oxygen measurement

// callback function for registration
void wait_for_ack(coap_message_t *response) {
   
    if(response == NULL) {
	printf("no response to registration\n"); 
        return;
    }
    
    if(strcmp((const char *)response->payload, "registered") == 0){
    	registered =  '1';
	printf("registration successful\n"); 
    }
}

PROCESS(oxygen_sensor, "Oxygen sensor");
AUTOSTART_PROCESSES(&oxygen_sensor);



PROCESS_THREAD(oxygen_sensor, ev, data) {
	

	//static coap_endpoint_t actuator_ep;
	static coap_message_t request[1];
	static coap_endpoint_t server_ep;
    //initialize the timer
	static struct etimer timer; //timer to acquire a new measurement of the oxygen

	srand(time(0));
	PROCESS_BEGIN();


	coap_activate_resource(&oxygen_measuring_device, "oxygen");

	coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep); //initialize server endpoint
    	
	
	// Server registration
	do{
		char *service_url = "/registration";
		// Prepare the message
		coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
		coap_set_header_uri_path(request, service_url);
		// Set the payload 
		const char msg[] = "{\"Type\":\"sensor\", \"Resource\":\"oxygen\"}";
		coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);
		// Issue the request in a blocking manner
		// The client will wait for the server to reply (or the transmission to timeout)
		COAP_BLOCKING_REQUEST(&server_ep, request, wait_for_ack);
		// if the registration was not successful, we have to do it again

	}while(registered=='0');
	

	etimer_set(&timer, CLOCK_SECOND*TIMER_PERIOD);
	
	while (1) {
		PROCESS_YIELD();
		
		if(ev == PROCESS_EVENT_TIMER)
			if (etimer_expired(&timer)){
				printf("device is triggered...\n");
	    		oxygen_measuring_device.trigger(); //this will call the res_event_handler of the resource
	    		etimer_reset(&timer);//reset timer
			}	    
		if(ev == button_hal_press_event){
			//button is used to force the sensor to send the oxygen level
			oxygen_measuring_device.trigger();
			printf("button pressed\n");
		}

	}

    PROCESS_END();
}
