

#include "dev/button-hal.h"
#include "dev/leds.h"
#include "sys/log.h"





#include “contiki.h”
#include "coap-engine.h“
#include "coap-blocking-api.h"

// log 
#define LOG_MODULE "Oxygen_node"
#define LOG_LEVEL LOG_LEVEL_DBG

//utility
#define SERVER_EP "coap://[fd00::1]:5683"

#define TIMER_PERIOD 60





// sensor
extern coap_resource_t oxygen_measuring_device; // sensor for oxygen measurement

bool registered = false;




static coap_endpoint_t actuator_ep;
static coap_message_t request[1];
static coap_endpoint_t server_ep;
    


void wait_for_ack(coap_message_t *response) {
    if(response == NULL) { 
        LOG_DBG("No response to registration..."); 
        return;
    }
    
    if(strcmp((const char *)response->payload, "registered") == 0)
    	registered =  true;
}



void get_actuator_status(coap_message_t *response) {
    if(response == NULL) { 
        LOG_DBG("No actuators..."); 
        return;
    }

    if(strcmp("{\"status\":1}", (const char *)response->payload) == 0 && !actuator_status){
		actuator_status = true;
	}
	else if(strcmp("{\"status\":0}", (const char *)response->payload) == 0 && actuator_status){
		actuator_status = false;
	} 
}


void server_registration(){
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
}



void check_actuator_status(){
    char *service_url = "/temperature_actuator";
    // Prepare the message
    coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
    coap_set_header_uri_path(request, service_url);
    // Set the payload 
    const char msg[] = "";
    coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);
    // Issue the request in a blocking manner
    // The client will wait for the server to reply (or the transmission to timeout)
    COAP_BLOCKING_REQUEST(&actuator_ep, request, get_actuator_status);
}


PROCESS(oxygen_sensor, "Oxygen sensor");
AUTOSTART_PROCESSES(&oxygen_sensor);



PROCESS_THREAD(temperature_sensor, ev, data) {

	
    PROCESS_BEGIN();
    
    LOG_INFO("Oxygen sensor: starting.... \n");
    
    coap_activate_resource(&oxygen_measuring_device, "oxygen");
    
	coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep); //initialize server endpoint

    // SERVER REGISTRATION 
    do{
        server_registration();
    }while(!registered);
    // END SERVER REGISTRATION

	
	//initialize the timer
	static struct etimer timer; //timer to randomly change the oxygen
	
    etimer_set(&timer, CLOCK_SECOND*TIMER_PERIOD);
	
	printf("Timer inizialized\n");

    while (1) {

        PROCESS_YIELD_UNTIL(etimer_expired(&timer) || etimer_expired(&et));
       	
        if (etimer_expired(&timer)){
            oxygen_measuring_device.trigger(); //this will call the res_event_handler of the resource that will randomly change the oxygen level
		   //reset random timer
		    etimer_set(&timer, CLOCK_SECOND*TIMER_PERIOD);
		}
		    
    }

    PROCESS_END();
}