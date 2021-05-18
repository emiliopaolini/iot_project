package project;

import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapResponse;
import org.eclipse.californium.core.coap.MediaTypeRegistry;

public class ExampleClient {
	public static void main(String args[]){
		CoapClient client = new CoapClient("coap://127.0.0.1/registration");
		
		CoapResponse response = client.post("10 C°", MediaTypeRegistry.TEXT_PLAIN);
	}
}
