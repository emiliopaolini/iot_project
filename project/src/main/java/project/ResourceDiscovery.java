package project;

import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapHandler;
import org.eclipse.californium.core.CoapObserveRelation;
import org.eclipse.californium.core.CoapResource;
import org.eclipse.californium.core.CoapResponse;
import org.eclipse.californium.core.coap.CoAP.ResponseCode;
import org.eclipse.californium.core.coap.MediaTypeRegistry;
import org.eclipse.californium.core.coap.Response;
import org.eclipse.californium.core.server.resources.CoapExchange;
import org.json.JSONObject;

public class ResourceDiscovery extends CoapResource {
	public ResourceDiscovery(String name) {
		super(name);
		setObservable(true);
	}

	
	public void handleGET(CoapExchange exchange) {
		System.out.println("HELLO I'm here");
		JSONObject contentJson = new JSONObject(new String(exchange.getRequestPayload()));
		String nodeIP = exchange.getSourceAddress().getHostAddress();
		System.out.println("A request from the actuator with ip: "+nodeIP+" has arrived...");
	
		String nodeResource = (String) contentJson.get("Resource");
		System.out.println("It is looking for " + nodeResource);

		Response response = new Response(ResponseCode.CONTENT);
		boolean found = false;
		for (Node temp : Server.nodes) {
			if(temp.getNodeType().equalsIgnoreCase("Sensor") && !temp.isAssigned()){
				response.setPayload(temp.getNodeIP());
				found = true;
			}
			
        }
		if(!found){
			response.setPayload("NONE");
		}

		exchange.respond(response);
		
	}
	 
	
}
