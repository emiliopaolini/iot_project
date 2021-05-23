package project;

import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.Statement;
import java.sql.Timestamp;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

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
		
		Map<String,String> thresholds = new HashMap<String,String>();
		if(nodeType.equalsIgnoreCase("actuator")) {
			if(nodeResource.equalsIgnoreCase("oxygen")) {
				String nodeThreshold = ""+(Integer)contentJson.get("threshold");
				System.out.println("threshold: "+nodeThreshold);
				thresholds.put("oxygen_threshold",nodeThreshold);
			}
			if(nodeResource.equalsIgnoreCase("water")) {
				String ph_threshold = ""+(Integer)contentJson.get("ph");
				String minerals_threshold = ""+(Integer)contentJson.get("minerals");
				thresholds.put("ph_threshold",ph_threshold);
				thresholds.put("minerals_threshold",minerals_threshold);
			}
		}
		
		
		
		System.out.println("======================================================");
		Response response = new Response(ResponseCode.CONTENT);
		
		response.setPayload("registered");
		exchange.respond(response);


		// a coap client for each registered node to observe the resource
		CoapClient client = new CoapClient("coap://[" + nodeIP + "]/"+nodeResource);
		final Node a = new Node(nodeIP,nodeType,nodeResource,thresholds);
		Server.nodes.add(a);
		
		insertInDB(a);
		
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
									//  update the corresponding sensor in order to make the simulation consistent
								}
								a.setCurrentValue(valueReceived);
								if(a.getNodeType().equalsIgnoreCase("actuator")) {
									Thread t = new Thread() {				
										public void run() {
											updateSensorOnActuatorStatus(a,a.getCurrentValue());
										}
									};
									t.start();
									
								}
								insertInDB(a);
								//printStatus();
							}
							public void onError() {
								System.err.println("-Failed--------"); }
							});
		
	}
	
	public static void updateSensorOnActuatorStatus(Node a,String valueReceived) {
		for (Node n : Server.nodes) {
			if(n.getNodeType().equalsIgnoreCase("sensor")) {
				if(n.hasActuator()) {
					if(n.getActuator().equals(a)) {
						// send a post request to the sensor
						CoapClient client2 = new CoapClient("coap://[" + n.getNodeIP() + "]/" + n.getNodeResource());
						System.out.println("new status of actuator is sent: " + valueReceived);
						System.out.println("COAP ENDPOINT: coap://[" + n.getNodeIP() + "]/" + n.getNodeResource());
						client2.post("actuator_status=" + valueReceived, MediaTypeRegistry.TEXT_PLAIN);
					}
				}
			}
		}
	}
	
	public static void insertInDB(Node a) {
		//first check if the node already exists
		try {
			
			String sql = "SELECT * FROM sensor WHERE ip=?";  

		    PreparedStatement ps = Server.con.prepareStatement(sql);
		    ps.setString(1,a.getNodeIP());
		    
		    ResultSet rs=ps.executeQuery();
			
			if(!rs.isBeforeFirst()){
			    System.out.println("Registering the node..");
			    sql = "INSERT INTO sensor VALUES(?,?,?);";
			    ps = Server.con.prepareStatement(sql);
			    ps.setString(1, a.getNodeIP());
			    ps.setString(2, a.getNodeResource());
			    ps.setString(3, a.getNodeType());
			    ps.executeUpdate();
			    System.out.println("DONE REGISTERED");
			    return;
			}
			
			if(a.getCurrentValue().equals("")) return;
			
			
			//inserting the new value 
			sql = "INSERT INTO measurement(date,ip,value) VALUES(?,?,?);";
		    ps = Server.con.prepareStatement(sql);
		    
		    long now = System.currentTimeMillis();
		    Timestamp sqlTimeStamp = new Timestamp(now);
		    ps.setTimestamp(1, sqlTimeStamp);
		    ps.setString(2, a.getNodeIP());
		    ps.setString(3, a.getCurrentValue());
		    
		    ps.executeUpdate();
		
			
		}catch(Exception e) {
			
		}
	}

	public static void printStatus(){
		for (Node temp : Server.nodes) {
			System.out.println(temp.toString());
        }
	}
}
