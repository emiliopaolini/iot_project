

#include "dev/button-hal.h"
#include "dev/leds.h"
#include "sys/log.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"
#include "coap-log.h"

// log 
#define LOG_MODULE "Oxygen_node"
#define LOG_LEVEL LOG_LEVEL_DBG

//utility
#define SERVER_EP "coap://[fd00::1]:5683"

#define TIMER_PERIOD 60
#define REGISTRATION_PERIOD 30





// sensor
extern coap_resource_t oxygen_measuring_device; // sensor for oxygen measurement

bool registered = false;





void wait_for_ack(coap_message_t *response) {
    printf("response received\n");
    if(response == NULL) { 
        LOG_DBG("No response to registration..."); 
        return;
    }
    
    if(strcmp((const char *)response->payload, "registered") == 0)
    	registered =  true;
}



PROCESS(oxygen_sensor, "Oxygen sensor");
AUTOSTART_PROCESSES(&oxygen_sensor);



PROCESS_THREAD(oxygen_sensor, ev, data) {

	//static coap_endpoint_t actuator_ep;
	static coap_message_t request[1];
	static coap_endpoint_t server_ep;
    //initialize the timer
	static struct etimer timer; //timer to randomly change the oxygen
    static struct etimer registration_timer; //timer to randomly change the oxygen
    PROCESS_BEGIN();
    
    LOG_INFO("Oxygen sensor: starting.... \n");
    
    coap_activate_resource(&oxygen_measuring_device, "oxygen");
    
	coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep); //initialize server endpoint
    

	
	
	
    etimer_set(&timer, CLOCK_SECOND*TIMER_PERIOD);
    etimer_set(&registration_timer, CLOCK_SECOND*REGISTRATION_PERIOD);
	
	printf("Timer inizialized\n");

    while (1) {

        
        PROCESS_YIELD_UNTIL(etimer_expired(&timer) || etimer_expired(&registration_timer));
       	
        if (etimer_expired(&registration_timer)){
            // SERVER REGISTRATION 
            if(!registered){
                //server_registration();
                char *service_url = "/registration";
                // Prepare the message
                coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
                coap_set_header_uri_path(request, service_url);
                // Set the payload 
                const char msg[] = "{\"Type\":\"sensor\", \"Resource\":\"oxygen\"}";
                coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);
                // Issue the request in a blocking manner
                // The client will wait for the server to reply (or the transmission to timeout)
                printf("before blocking...\n");
                COAP_BLOCKING_REQUEST(&server_ep, request, wait_for_ack);
                printf("after blocking...\n");
                // if the registration was not successful, we have to do it again
                if(!registered){
                    etimer_reset(&registration_timer);
                }
                
                // END SERVER REGISTRATION
            }
            
        }
        
        else if (etimer_expired(&registration_timer)){
                if(registered){
                    printf("trigerring device...\n");
                    oxygen_measuring_device.trigger(); //this will call the res_event_handler of the resource that will randomly change the oxygen level
		            //reset timer
		            etimer_reset(&timer);
                    printf("device is triggered...\n");
                }
            }
		
		    
    }

    PROCESS_END();
}
