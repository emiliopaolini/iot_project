#include "dev/button-hal.h"
#include "dev/leds.h"
#include "sys/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"
#include "coap-log.h"
#include <time.h>

// log 
#define LOG_MODULE "Ph_node"
#define LOG_LEVEL LOG_LEVEL_DBG

//utility
#define SERVER_EP "coap://[fd00::1]:5683"

#define TIMER_PERIOD 20
#define REGISTRATION_PERIOD 10


static char registered = '0';


// sensor
extern coap_resource_t ph_measuring_device; // sensor for ph measurement

void wait_for_ack(coap_message_t *response) {
   
    if(response == NULL) {
	printf("no response to registration\n"); 
        //LOG_DBG("No response to registration..."); 
        return;
    }
    
    if(strcmp((const char *)response->payload, "registered") == 0){
    	registered =  '1';
	printf("registration successful\n"); 
    }
}

PROCESS(ph_sensor, "Ph sensor");
AUTOSTART_PROCESSES(&ph_sensor);



PROCESS_THREAD(ph_sensor, ev, data) {
	

	//static coap_endpoint_t actuator_ep;
	static coap_message_t request[1];
	static coap_endpoint_t server_ep;
    	//initialize the timer
	static struct etimer timer; //timer to randomly change the oxygen

    	//static struct etimer registration_timer; //timer for CoAP registration
	srand(time(0));
    	PROCESS_BEGIN();


    
    	LOG_INFO("Ph sensor: starting.... \n");
    
    	coap_activate_resource(&ph_measuring_device, "ph");
    
	coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep); //initialize server endpoint
    	
	
	
	do{
		char *service_url = "/registration";
		// Prepare the message
		coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
		coap_set_header_uri_path(request, service_url);
		// Set the payload 
		const char msg[] = "{\"Type\":\"sensor\", \"Resource\":\"ph\"}";
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
	    			ph_measuring_device.trigger(); //this will call the res_event_handler of the resource that will randomly change the ph level
		
	    			etimer_reset(&timer);//reset timer
	    		
			}	    
		if(ev == button_hal_press_event){
			//button is used to force the sensor to send the ph level
			ph_measuring_device.trigger();
			printf("button pressed\n");
		}

	}

    PROCESS_END();
}
