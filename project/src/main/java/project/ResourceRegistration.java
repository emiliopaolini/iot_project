package project;

import org.eclipse.californium.core.CoapResource;
import org.eclipse.californium.core.coap.CoAP.ResponseCode;
import org.eclipse.californium.core.coap.MediaTypeRegistry;
import org.eclipse.californium.core.coap.Response;
import org.eclipse.californium.core.server.resources.CoapExchange;

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
		//exchange.respond(ResponseCode.CREATED);
		Response response = new Response(ResponseCode.CONTENT);
		if(exchange.getRequestOptions().getAccept() == MediaTypeRegistry.APPLICATION_JSON) {
			response.setPayload("registered");
			response.getOptions().setContentFormat(MediaTypeRegistry.APPLICATION_JSON);
		}
		exchange.respond(response);

}}
