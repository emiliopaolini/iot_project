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
		
		response.setPayload("registered");
		exchange.respond(response);


		// a coap client for each registered node to observe the resource
		CoapClient client = new CoapClient("coap://[" + nodeIP + "]/"+nodeResource);
		final Node a = new Node(nodeIP,nodeType,nodeResource);
		Server.nodes.add(a);
		
		CoapObserveRelation relation = client.observe(new CoapHandler() {
							public void onLoad(CoapResponse response) {
								
								String content = response.getResponseText(); 
								System.out.println(content);
								if(content == null || content.equals(""))
									return;
								JSONObject contentJ = new JSONObject(content);
								//here we retrieve the data
								String valueReceived = ""; 
								if(a.getNodeType().equalsIgnoreCase("sensor")){
									valueReceived = ""+contentJ.get("value");
								}
								if(a.getNodeType().equalsIgnoreCase("actuator")){
									valueReceived = ""+contentJ.get("status");
								}
								a.setCurrentValue(valueReceived);
								printStatus();
							}
							public void onError() {
								System.err.println("-Failed--------"); }
							});
		
	}

	public static void printStatus(){
		for (Node temp : Server.nodes) {
			System.out.println(temp.toString());
        }
	}
}
