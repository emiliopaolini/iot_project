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
#define LOG_MODULE "Water_node"
#define LOG_LEVEL LOG_LEVEL_APP

//utility
#define SERVER_EP "coap://[fd00::1]:5683"
#define DISCOVERY_PERIOD 30

static char registered = '0';

static char ph_sensor_discovered = '0';
static char minerals_sensor_discovered = '0';
static char ph_sensor_address[29];
static char minerals_sensor_address[29];

static coap_endpoint_t ph_sensor_ep;
static coap_endpoint_t minerals_sensor_ep;
static coap_message_t request[1];
static coap_endpoint_t server_ep;

static coap_observee_t *minerals_obs;
static coap_observee_t *ph_obs;


extern enum leds {GREEN,YELLOW,RED} alert_level;
// sensor
extern coap_resource_t water_generator; // resource for generating water

float minerals_level = 0;
float ph_level = 0;



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



void wait_for_minerals_discovery(coap_message_t *response) {
   
	if(response == NULL) { 
		printf("No response to discovery..."); 
		return;
	}

	//LOG_DBG("Sensor IP: %s\n", response->payload);
	if(strcmp((const char *)response->payload, "NONE") == 0){
		printf("No available sensors from the server..");
		return;
    	}

	strcpy(minerals_sensor_address, "coap://[");
	strcat(minerals_sensor_address, (const char *)response->payload);

	strcat(minerals_sensor_address,"]");   

	minerals_sensor_discovered = '1';

	printf("address received: %s\n",minerals_sensor_address);
	printf("len address received: %d\n",strlen(minerals_sensor_address));
	coap_endpoint_parse(minerals_sensor_address, strlen(minerals_sensor_address), &minerals_sensor_ep); //initialize sensor endpoint
	LOG_INFO_COAP_EP(&minerals_sensor_ep);
	LOG_INFO("\n");
}

void wait_for_ph_discovery(coap_message_t *response) {
   
	if(response == NULL) { 
		printf("No response to discovery..."); 
		return;
	}

	//LOG_DBG("Sensor IP: %s\n", response->payload);
	if(strcmp((const char *)response->payload, "NONE") == 0){
        printf("No available sensors from the server..");
        return;
    }

	strcpy(ph_sensor_address, "coap://[");
	strcat(ph_sensor_address, (const char *)response->payload);

	strcat(ph_sensor_address,"]");   

	ph_sensor_discovered = '1';

	printf("address received: %s\n",ph_sensor_address);
	printf("len address received: %d\n",strlen(ph_sensor_address));
	coap_endpoint_parse(ph_sensor_address, strlen(ph_sensor_address), &ph_sensor_ep); //initialize sensor endpoint
	LOG_INFO_COAP_EP(&ph_sensor_ep);
	LOG_INFO("\n");
}


PROCESS(water_actuator, "Water actuator");
AUTOSTART_PROCESSES(&water_actuator);


static void minerals_update_callback(coap_observee_t *minerals_obs, void *notification, coap_notification_flag_t flag){
    int len = 0;
    const uint8_t *payload = NULL;
    printf("Received an update from the minerals sensor!\n");

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

	    minerals_level = atof(ptr);
	    water_generator.trigger();
	    update_leds();
            break;
        case OBSERVE_OK:  //server accepted observation request 
            printf("OBSERVE_OK: %*s\n", len, (char *)payload);
            break;
        case OBSERVE_NOT_SUPPORTED:
            printf("OBSERVE_NOT_SUPPORTED: %*s\n", len, (char *)payload); minerals_obs = NULL;
            break;
        case ERROR_RESPONSE_CODE:
            printf("ERROR_RESPONSE_CODE: %*s\n", len, (char *)payload); minerals_obs = NULL;
            break;
        case NO_REPLY_FROM_SERVER: 
            printf("NO_REPLY_FROM_SERVER: removing observe registration with token %x%x\n",minerals_obs->token[0], minerals_obs->token[1]);
   		minerals_obs = NULL;
            break; 
        }
    
}

static void ph_update_callback(coap_observee_t *ph_obs, void *notification, coap_notification_flag_t flag){
    int len = 0;
    const uint8_t *payload = NULL;
    printf("Received an update from the ph sensor!\n");

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

	    ph_level = atof(ptr);
	    water_generator.trigger();
	    update_leds();
            break;
        case OBSERVE_OK:  //server accepted observation request 
            printf("OBSERVE_OK: %*s\n", len, (char *)payload);
            break;
        case OBSERVE_NOT_SUPPORTED:
            printf("OBSERVE_NOT_SUPPORTED: %*s\n", len, (char *)payload); ph_obs = NULL;
            break;
        case ERROR_RESPONSE_CODE:
            printf("ERROR_RESPONSE_CODE: %*s\n", len, (char *)payload); ph_obs = NULL;
            break;
        case NO_REPLY_FROM_SERVER: 
            printf("NO_REPLY_FROM_SERVER: removing observe registration with token %x%x\n",ph_obs->token[0], ph_obs->token[1]);
   		ph_obs = NULL;
            break; 
        }
    
}


PROCESS_THREAD(water_actuator, ev, data) {


    static struct etimer discovery_timer;
    
    PROCESS_BEGIN();
    
    LOG_INFO("Water actuator: starting.... \n");
    
    coap_activate_resource(&water_generator, "water");
    
	coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep); //initialize server endpoint

    // SERVER REGISTRATION 
	
	do{
		char *service_url = "/registration";
		// Prepare the message
		coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
		coap_set_header_uri_path(request, service_url);
		// Set the payload 
		const char msg[] = "{\"Type\":\"actuator\", \"Resource\":\"water\"}";
		coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);
		// Issue the request in a blocking manner
		// The client will wait for the server to reply (or the transmission to timeout)
		COAP_BLOCKING_REQUEST(&server_ep, request, wait_for_ack);
		// if the registration was not successful, we have to do it again

	}while(registered=='0');
	

    // END SERVER REGISTRATION


    //DISCOVERY OF THE MINERALS SENSOR
    etimer_set(&discovery_timer, CLOCK_SECOND * DISCOVERY_PERIOD);

    do{
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&discovery_timer));

        char *service_url = "/discovery";
        // Prepare the message
        coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
        coap_set_header_uri_path(request, service_url);
        // Set the payload 
        const char msg[] = "{\"Resource\":\"minerals\"}";
        coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);
        // Issue the request in a blocking manner
        // The client will wait for the server to reply (or the transmission to timeout)
        COAP_BLOCKING_REQUEST(&server_ep, request, wait_for_minerals_discovery);
        // if the registration was not successful, we have to do it again

        if(minerals_sensor_discovered=='0') etimer_reset(&discovery_timer);

	}while(minerals_sensor_discovered=='0');

    etimer_stop(&discovery_timer);
    //END DISCOVERY
   

    //DISCOVERY OF THE PH SENSOR
    etimer_set(&discovery_timer, CLOCK_SECOND * DISCOVERY_PERIOD);

    do{
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&discovery_timer));

        char *service_url = "/discovery";
        // Prepare the message
        coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
        coap_set_header_uri_path(request, service_url);
        // Set the payload 
        const char msg[] = "{\"Resource\":\"ph\"}";
        coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);
        // Issue the request in a blocking manner
        // The client will wait for the server to reply (or the transmission to timeout)
        COAP_BLOCKING_REQUEST(&server_ep, request, wait_for_ph_discovery);
        // if the registration was not successful, we have to do it again

        if(ph_sensor_discovered=='0') etimer_reset(&discovery_timer);

	}while(ph_sensor_discovered=='0');

    etimer_stop(&discovery_timer);
    //END DISCOVERY

    // register the actuator as a coap client to the ph sensor 
    printf("registering to ph sensor...\n");
    
    minerals_obs = coap_obs_request_registration(&minerals_sensor_ep, "/minerals", minerals_update_callback, NULL);
    ph_obs = coap_obs_request_registration(&ph_sensor_ep, "/ph", ph_update_callback, NULL);
	
    PROCESS_END();
}
