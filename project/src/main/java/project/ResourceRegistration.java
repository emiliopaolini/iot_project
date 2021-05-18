package project;

import org.eclipse.californium.core.CoapResource;
import org.eclipse.californium.core.coap.CoAP.ResponseCode;
import org.eclipse.californium.core.coap.MediaTypeRegistry;
import org.eclipse.californium.core.coap.Response;
import org.eclipse.californium.core.server.resources.CoapExchange;
import org.json.JSONObject;

public class ResourceRegistration extends CoapResource {
	public ResourceRegistration(String name) {
		super(name);
		setObservable(true);
	}

	/*
	 * public void handleGET(CoapExchange exchange) {
	 * exchange.respond("hello world"); }
	 */
	public void handlePOST(CoapExchange exchange) {
		System.out.println("Request of a new registration received...");
		// parsing data from the sensor
		JSONObject contentJson = new JSONObject(new String(exchange.getRequestPayload()));
		String nodeIP = exchange.getSourceAddress().getHostAddress();
		System.out.println("Node ["+nodeIP+"] registration...");
		
		String nodeType = (String) contentJson.get("Type");
		String nodeResource = (String) contentJson.get("Resource");
		System.out.println("NodeType: "+nodeType);
		System.out.println("NodeResource: "+nodeResource);
		System.out.println("======================================================");
		Response response = new Response(ResponseCode.CONTENT);
		//if(exchange.getRequestOptions().getAccept() == MediaTypeRegistry.APPLICATION_JSON) {
			response.setPayload("registered");
			//response.getOptions().setContentFormat(MediaTypeRegistry.APPLICATION_JSON);
		//}
		exchange.respond(response);

		// here we have to create the coap client for the resource of the sensor/actuator

		// a coap client for each registered node to observe the resource

}}
