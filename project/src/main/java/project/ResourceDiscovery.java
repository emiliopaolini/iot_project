package project;

import java.util.Arrays;

import org.eclipse.californium.core.CoapResource;
import org.eclipse.californium.core.coap.CoAP.ResponseCode;
import org.eclipse.californium.core.coap.Response;
import org.eclipse.californium.core.server.resources.CoapExchange;
import org.json.JSONObject;

public class ResourceDiscovery extends CoapResource {
	public ResourceDiscovery(String name) {
		super(name);
		setObservable(true);
	}


	// function called when an actuator wants to discover a sensor
	public void handleGET(CoapExchange exchange) {
		JSONObject contentJson = new JSONObject(new String(exchange.getRequestPayload()));
		String nodeIP = exchange.getSourceAddress().getHostAddress();
		System.out.println("A request from the actuator with ip: " + nodeIP + " has arrived...");

		String nodeResource = (String) contentJson.get("Resource");
		System.out.println("It is looking for " + nodeResource);


		Response response = new Response(ResponseCode.CONTENT);
		boolean found = false;
		Node actuator = Server.nodes.stream().filter(n -> n.getNodeIP().equals(nodeIP)).findAny().get();
		
		//searching for a free sensor with requested resource
		for (Node n : Server.nodes) {
			if (n.getNodeType().equalsIgnoreCase("Sensor") && !n.hasActuator() && n.getNodeResource().equalsIgnoreCase(nodeResource)) {
				response.setPayload(n.getNodeIP());
				n.setActuator(actuator);
				found = true;
			}

		}
		// if no sensors have been found sending NONE as payload
		if (!found) {
			response.setPayload("NONE");
		}

		exchange.respond(response);

	}

}
