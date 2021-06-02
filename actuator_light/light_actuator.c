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
#define LOG_MODULE "Light_node"
#define LOG_LEVEL LOG_LEVEL_APP

//utility
#define SERVER_EP "coap://[fd00::1]:5683"
#define DISCOVERY_PERIOD 30

static char registered = '0';
static char sensor_discovered = '0';
static char sensor_address[29];

static coap_endpoint_t sensor_ep;
static coap_message_t request[1];
static coap_endpoint_t server_ep;

static coap_observee_t *obs;



extern enum leds {GREEN,YELLOW,RED} alert_level;
// sensor
extern coap_resource_t light_generator; // resource for generating oxygen
extern int GOOD_LIGHT_LEVEL;

float light_level = 0;


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



void wait_for_discovery(coap_message_t *response) {
   
	if(response == NULL) { 
		printf("No response to discovery..."); 
		return;
	}

	//LOG_DBG("Sensor IP: %s\n", response->payload);
	if(strcmp((const char *)response->payload, "NONE") == 0){
        printf("No available sensors from the server..");
        return;
    }

	strcpy(sensor_address, "coap://[");
	strcat(sensor_address, (const char *)response->payload);

	strcat(sensor_address,"]");   

	sensor_discovered = '1';

	printf("address received: %s\n",sensor_address);
	printf("len address received: %d\n",strlen(sensor_address));
	coap_endpoint_parse(sensor_address, strlen(sensor_address), &sensor_ep); //initialize sensor endpoint
	LOG_INFO_COAP_EP(&sensor_ep);
	LOG_INFO("\n");
}


PROCESS(light_actuator, "Light actuator");
AUTOSTART_PROCESSES(&light_actuator);


static void light_update_callback(coap_observee_t *obs, void *notification, coap_notification_flag_t flag){
    int len = 0;
    const uint8_t *payload = NULL;
    printf("Received an update from the light sensor!\n");

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

	    light_level = atof(ptr);
	    light_generator.trigger();
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


PROCESS_THREAD(light_actuator, ev, data) {


    static struct etimer discovery_timer;
    
    PROCESS_BEGIN();
    
    LOG_INFO("Light actuator: starting.... \n");
    
    coap_activate_resource(&light_generator, "light");
    
	coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep); //initialize server endpoint

    // SERVER REGISTRATION 
	
	do{
		char *service_url = "/registration";
		// Prepare the message
		coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
		coap_set_header_uri_path(request, service_url);
		// Set the payload 
		const char msg[COAP_MAX_CHUNK_SIZE];

		snprintf((char*)msg, COAP_MAX_CHUNK_SIZE, "{\"Type\":\"actuator\", \"Resource\":\"light\", \"threshold\":%d}", GOOD_LIGHT_LEVEL);
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
        const char msg[] = "{\"Resource\":\"light\"}";
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
    printf("registering to light sensor...\n");
    obs = coap_obs_request_registration(&sensor_ep, "/light", light_update_callback, NULL);

    while(1){
	
	PROCESS_WAIT_EVENT();
	if(ev== button_hal_press_event){
		//force the resource to trigger
		light_generator.trigger();
	    	update_leds();
	}
    }
	
    PROCESS_END();
}
