#include "coap-blocking-api.h"
#include "coap-observe-client.h"
#include "coap-engine.h"
#include "contiki-net.h"
#include "contiki.h"
#include "dev/button-hal.h"
#include "dev/leds.h"
#include "sys/log.h"
#include "coap-log.h"

// log 
#define LOG_MODULE "Oxygen_node"
#define LOG_LEVEL LOG_LEVEL_APP

//utility
#define SERVER_EP "coap://[fd00::1]:5683"

static char registered = '0';
static char sensor_discovered = '0';
static char sensor_address[39];

static coap_endpoint_t sensor_ep;
static coap_message_t request[1];
static coap_endpoint_t server_ep;

static coap_observee_t *obs;

// sensor
extern coap_resource_t oxygen_generator; // resource for generating oxygen

float current_oxygen_level = 0;

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



void wait_for_discovery(coap_message_t *response) {
   
	if(response == NULL) { 
		printf("No response to discovery..."); 
		return;
	}

	//LOG_DBG("Sensor IP: %s\n", response->payload);
	

	char *temp_address = "coap://[fd00:0:0:0:202:2:2:2]";
	strcpy(sensor_address, "coap://[");
	//strcat(sensor_address, (const char *)response->payload);
	//strcat(sensor_address,temp);
	strcat(sensor_address,"]");   

	sensor_discovered = '1';
	
	printf("address received: %s\n",temp_address);
	printf("len address received: %d\n",strlen(temp_address));
	coap_endpoint_parse(temp_address, strlen(temp_address), &sensor_ep); //initialize sensor endpoint
	LOG_INFO_COAP_EP(&sensor_ep);
	LOG_INFO("\n");
}


PROCESS(oxygen_actuator, "Oxygen actuator");
AUTOSTART_PROCESSES(&oxygen_actuator);


static void oxygen_update_callback(coap_observee_t *obs, void *notification, coap_notification_flag_t flag){
    //int len = 0;
    //const uint8_t *payload = NULL;
    printf("Received an update from the sensor!\n");
    LOG_INFO("I am inside oxygen_update_callback\n");
    /*
    if(notification) len = coap_get_payload(notification, &payload); 
    switch(flag) {
        case NOTIFICATION_OK:
            printf("NOTIFICATION OK: %*s\n", len, (char *)payload);
            //handle value
		    current_oxygen_level = atof((char*) payload);
		    oxygen_actuator.trigger();
            break;
        case OBSERVE_OK:  server accepted observation request 
            printf("OBSERVE_OK: %*s\n", len, (char *)payload);
            break;
        case OBSERVE_NOT_SUPPORTED:
            printf("OBSERVE_NOT_SUPPORTED: %*s\n", len, (char *)payload); obs = NULL;
            break;
        case ERROR_RESPONSE_CODE:
            printf("ERROR_RESPONSE_CODE: %*s\n", len, (char *)payload); obs = NULL;
            break;
        case NO_REPLY_FROM_SERVER: 
            printf("NO_REPLY_FROM_SERVER: removing observe registration with token %x%x\n",obs->token[0], obs->token[1]);
            obs->token[0], obs->token[1]); obs = NULL;
            break; 
        }
    */
}


PROCESS_THREAD(oxygen_actuator, ev, data) {


	
    
    PROCESS_BEGIN();
    
    LOG_INFO("Oxygen actuator: starting.... \n");
    
    coap_activate_resource(&oxygen_generator, "actuator_oxygen");
    
	coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep); //initialize server endpoint

    // SERVER REGISTRATION 
	
	do{
		char *service_url = "/registration";
		// Prepare the message
		coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
		coap_set_header_uri_path(request, service_url);
		// Set the payload 
		const char msg[] = "{\"Type\":\"actuator\", \"Resource\":\"oxygen\"}";
		coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);
		// Issue the request in a blocking manner
		// The client will wait for the server to reply (or the transmission to timeout)
		COAP_BLOCKING_REQUEST(&server_ep, request, wait_for_ack);
		// if the registration was not successful, we have to do it again

	}while(registered=='0');
	
	printf("registration completed!\n");
    // END SERVER REGISTRATION


    //DISCOVERY OF THE SENSOR
    
    do{
		char *service_url = "/discovery";
		// Prepare the message
		coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
		coap_set_header_uri_path(request, service_url);
		// Set the payload 
		const char msg[] = "{\"Resource\":\"oxygen\"}";
		coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);
		// Issue the request in a blocking manner
		// The client will wait for the server to reply (or the transmission to timeout)
		COAP_BLOCKING_REQUEST(&server_ep, request, wait_for_discovery);
		// if the registration was not successful, we have to do it again

	}while(sensor_discovered=='0');

    
    //END DISCOVERY
    
    // register the actuator as a coap client to the sensor 
    printf("registering to oxygen sensor...\n");
    obs = coap_obs_request_registration(&sensor_ep, "/oxygen", oxygen_update_callback, NULL);
	
    PROCESS_END();
}
