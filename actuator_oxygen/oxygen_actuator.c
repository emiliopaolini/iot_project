#include "coap-blocking-api.h"
#include "coap-observe-client.h"
#include "coap-engine.h"
#include "contiki-net.h"
#include "contiki.h"
#include "dev/button-hal.h"
#include "dev/leds.h"
#include "sys/log.h"
#include "coap-log.h"


//utility
#define SERVER_EP "coap://[fd00::1]:5683"
#define DISCOVERY_PERIOD 30
#define COAP_OBSERVE_CLIENT 1

static char registered = '0';
static char sensor_discovered = '0';
static char sensor_address[29];


static coap_endpoint_t sensor_ep;
static coap_message_t request[1];
static coap_endpoint_t server_ep;
// observer for sensor resource
static coap_observee_t *obs;


// variable that represents leds status. Updated inside the resource
extern enum leds {GREEN,YELLOW,RED} alert_level;
// sensor
extern coap_resource_t oxygen_generator; // resource for generating oxygen
extern int GOOD_OXYGEN_LEVEL;

float oxygen_level = 0;

// function that updates leds according to alert_level
void update_leds(){
	printf("updating leds\n");
	if(alert_level==RED){
		printf("red\n");
		leds_set(LEDS_NUM_TO_MASK(LEDS_RED));
	}
	if(alert_level==YELLOW){
		printf("yellow\n");
		leds_set(LEDS_NUM_TO_MASK(LEDS_YELLOW));
	}
	if(alert_level==GREEN){
		printf("green\n");
		leds_set(LEDS_NUM_TO_MASK(LEDS_GREEN));
	}
}

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


// callback function for discovery
void wait_for_discovery(coap_message_t *response) {
   	

	if(response == NULL) { 
		printf("No response to discovery...\n"); 
		return;
	}
	
	if(strcmp((const char *)response->payload, "none") == 0){
        	printf("No available sensors from the server..\n");
        	return;
    	}
    // parsing payload 
	strcpy(sensor_address, "coap://[");
	strcat(sensor_address, (const char *)response->payload);

	strcat(sensor_address,"]");   

	sensor_discovered = '1';

	printf("address received: %s\n",sensor_address);
	printf("len address received: %d\n",strlen(sensor_address));
	coap_endpoint_parse(sensor_address, strlen(sensor_address), &sensor_ep); //initialize sensor endpoint

}


PROCESS(oxygen_actuator, "Oxygen actuator");
AUTOSTART_PROCESSES(&oxygen_actuator);

// callback function for observing sensor updates
static void oxygen_update_callback(coap_observee_t *obs, void *notification, coap_notification_flag_t flag){
    int len = 0;
    const uint8_t *payload = NULL;
    printf("Received an update from the sensor!\n");

    char delim[] = ":";
    
    if(notification) 
	len = coap_get_payload(notification, &payload); 
    switch(flag) {
        case NOTIFICATION_OK:
            printf("NOTIFICATION OK: %*s\n", len, (char *)payload);
            //handle value
            char * ptr = strtok((char*) payload,delim);

            ptr = strtok(NULL," ");	

            ptr = strtok(ptr,"}");

            oxygen_level = atof(ptr);
            // trigger the resource to see if its status should change
            oxygen_generator.trigger();
            update_leds();
            break;
        case OBSERVE_OK:  //server accepted observation request 
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
   		obs = NULL;
            break; 
        }
    
}


PROCESS_THREAD(oxygen_actuator, ev, data) {


    static struct etimer discovery_timer;
    
    PROCESS_BEGIN();
    
    printf("Oxygen actuator: starting.... \n");
    
    coap_activate_resource(&oxygen_generator, "oxygen");
    
	coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep); //initialize server endpoint

    // SERVER REGISTRATION 
	
	do{
		char *service_url = "/registration";
		// Prepare the message
		coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
		coap_set_header_uri_path(request, service_url);
		// Set the payload 
		const char msg[COAP_MAX_CHUNK_SIZE];

		snprintf((char*)msg, COAP_MAX_CHUNK_SIZE, "{\"Type\":\"actuator\", \"Resource\":\"oxygen\", \"threshold\":%d}", GOOD_OXYGEN_LEVEL);
		coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);
		// Issue the request in a blocking manner
		// The client will wait for the server to reply (or the transmission to timeout)
		COAP_BLOCKING_REQUEST(&server_ep, request, wait_for_ack);
		// if the registration was not successful, we have to do it again

	}while(registered=='0');
	

    // END SERVER REGISTRATION


    //DISCOVERY OF THE SENSOR
    etimer_set(&discovery_timer, CLOCK_SECOND * DISCOVERY_PERIOD);

    do{
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&discovery_timer));

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

        if(sensor_discovered=='0') etimer_reset(&discovery_timer);

	}while(sensor_discovered=='0');

    etimer_stop(&discovery_timer);
    //END DISCOVERY
    
    // register the actuator as a coap client to the sensor 
    printf("registering to oxygen sensor...\n");
    obs = coap_obs_request_registration(&sensor_ep, "/oxygen", oxygen_update_callback, NULL);

   
    PROCESS_END();
}
