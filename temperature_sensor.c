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

#define TIMER_PERIOD 60

// sensor
extern coap_resource_t thermometer; // sensor for oxygen measurement


PROCESS(temperature_sensor, "Temperature sensor");
AUTOSTART_PROCESSES(&temperature_sensor);




PROCESS_THREAD(temperature_sensor, ev, data) {

	static coap_endpoint_t actuator_ep;
	static coap_message_t request[1];
	static coap_endpoint_t server_ep;
    
    PROCESS_BEGIN();
    
    LOG_INFO("Temperature sensor: starting.... \n");
    
    coap_activate_resource(&thermometer, "sensor/temperature");
    
	coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep); //initialize server endpoint

    // SERVER REGISTRATION 
    

    // END SERVER REGISTRATION


	
	//initialize the timer
	static struct etimer timer; //timer to randomly change the oxygen
	//static struct etimer et; 
	etimer_set(&timer, CLOCK_SECOND*TIMER_PERIOD);
	//etimer_set(&et, CLOCK_SECOND*10);
	
	printf("Timer inizialized\n");

    while (1) {

        PROCESS_YIELD_UNTIL(etimer_expired(&timer) || etimer_expired(&et));
       	
        if (etimer_expired(&timer)){
            air_oxygen_measuring_device.trigger(); //this will call the res_event_handler of the resource

		   //reset random timer
		    etimer_set(&timer, CLOCK_SECOND*TIMER_PERIOD);
		}
		
		if(etimer_expired(&et)){

            // discovery of the actuator
            
            // getting the status of the actuator

            // change the oxygen according to the actuator status and notify subscribers

            // check if the oxygen has become normal

            // if so, turns off the actuator

            // otherwise keep the actuator on/turn on the actuator (PUT/POST REQUEST)

			
			}
			
		    etimer_set(&et, CLOCK_SECOND*3);
	    }
    }

    PROCESS_END();
}