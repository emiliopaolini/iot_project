package project;

import java.io.IOException;
import java.net.URISyntaxException;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.coap.MediaTypeRegistry;
import org.json.JSONArray;
import org.json.JSONObject;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.servlet.mvc.method.annotation.SseEmitter;

import com.google.gson.Gson;

@Controller
public class Controlling {

	@RequestMapping("/")
	public String run(Model model) {
		return home(model);
	}

	@RequestMapping("/home")
	public String home(Model model) {
		/*
		ArrayList<Node> sensors = new ArrayList<>();
		ArrayList<Node> actuators = new ArrayList<>();
		JSONObject mainObj = new JSONObject();
		JSONArray ja = new JSONArray();
		
		for (Node n : Server.nodes) {
			
			JSONObject jo = new JSONObject();
	    	jo.put("IP", n.getNodeIP());
	    	jo.put("Type", n.getNodeType());
	    	jo.put("Resource", n.getNodeResource());
	    	jo.put("Value", n.getCurrentValue());
	    	ja.put(jo);
			
			if (n.getNodeType().equalsIgnoreCase("sensor")) {
				sensors.add(n);
			} else {
				actuators.add(n);
			}
		}
		mainObj.put("jsonarray", ja);*/
        return "home";
        /*
		model.addAttribute("sensors", sensors);
		model.addAttribute("actuators", actuators);

		return "home";
		*/
	}

	// function called to set a new status for the actuator
	@RequestMapping(value = "/setActuator")
	public String setActuator(@RequestParam(required = true) String nodeIP,
			@RequestParam(required = true) String currentValue, Model model) {
		// retrieve the actuator from the registered nodes 
		Node node = Server.nodes.stream().filter(n -> n.getNodeIP().equals(nodeIP)).findAny().get();
		// updating its status
		node.setCurrentValue(currentValue);
		// sending to the node the new status
		CoapClient client = new CoapClient("coap://[" + node.getNodeIP() + "]/" + node.getNodeResource());
		client.post("status=" + node.getCurrentValue(), MediaTypeRegistry.TEXT_PLAIN);
		System.out.println("Sent a new status to the actuator!");
		return "home";
	}
	
	// function called to change manual mode for the actuator
	@RequestMapping(value = "/setManual")
	public String setManual(@RequestParam(required = true) String nodeIP,
			@RequestParam(required = true) String currentValue, Model model) {
		// retrieve the actuator from the registered nodes 
		Node node = Server.nodes.stream().filter(n -> n.getNodeIP().equals(nodeIP)).findAny().get();
		// updating its manual mode
		if(currentValue.equalsIgnoreCase("0"))
			node.setManualMode(0);
		else
			node.setManualMode(1);
		// sending to the node the new manual mode
		CoapClient client = new CoapClient("coap://[" + node.getNodeIP() + "]/" + node.getNodeResource());
		System.out.println("new manual mode is sent: " + currentValue);
		client.post("manualMode=" + currentValue, MediaTypeRegistry.TEXT_PLAIN);

		return "home";
	}

	

	
	// function called to change threshold for the actuator
	@RequestMapping("/changeThreshold")
	public String changeThreshold(@RequestParam(required = true) String nodeIP,@RequestParam(required = true) String thresholdName,@RequestParam(required = true) String thresholdValue) {
		// retrieve the actuator from the registered nodes
		Node node = Server.nodes.stream().filter(n -> n.getNodeIP().equals(nodeIP)).findAny().get();
		// updating its threshold
		node.addThreshold(thresholdName+"_threshold", thresholdValue);
		// sending to the node the new threshold 
		CoapClient client = new CoapClient("coap://[" + node.getNodeIP() + "]/" + node.getNodeResource());
		client.post(thresholdName+"_threshold=" + thresholdValue, MediaTypeRegistry.TEXT_PLAIN);
		System.out.println("Sent a new threshold to the actuator!");
		return "home";
	}
	

	// function called to retrieve the data that will be shown inside the log chart
	@ResponseBody
	@RequestMapping(value = "/getData")
	public String getData(@RequestParam(required = true) String nodeIP,@RequestParam(required = true) String date) {
		System.out.println("Ip received: "+nodeIP+"\n Retrieving all its data for "+date+"\n");
		
		String sql = "SELECT value,EXTRACT(HOUR from  date) as hour,EXTRACT(MINUTE from  date) as minute FROM measurement WHERE ip=? AND DATE(date)=?";  
		Map<String, Object> hm = new HashMap<>();
		JSONObject mainObj = new JSONObject();
		JSONArray ja = new JSONArray();

		// prepare the response 
		try {
		    PreparedStatement ps = Server.con.prepareStatement(sql);
		    ps.setString(1,nodeIP);
		    ps.setString(2,date);
		    
		    ResultSet rs=ps.executeQuery();
		    
		    while (rs.next()) {
		    	String value= rs.getString("value");
		    	String hour = rs.getString("hour");
		    	String minute = rs.getString("minute");
		    	int temp = Integer.parseInt(minute);
		    	if(temp<10)
		    		minute = "0"+minute;
		    	
		    	String time = hour+"_"+minute; 
		    	
		    	JSONObject jo = new JSONObject();
		    	jo.put("time", time);
		    	jo.put("value",  value);
		    	ja.put(jo);
		
		    }
	    } catch (SQLException e) {
	      System.out.println(e);
	    }
		// sending the response
		mainObj.put("jsonarray", ja);
        return mainObj.toString();
	}
	
	
	
	// function called every second from the javascript to update the web interface
	@ResponseBody
	@RequestMapping(value = "/getSensors")
	public String getSensors() {
		JSONObject mainObj = new JSONObject();
		JSONArray ja = new JSONArray();
		
		for (Node n : Server.nodes) {
			
			JSONObject jo = new JSONObject();
	    	jo.put("IP", n.getNodeIP());
	    	jo.put("Type", n.getNodeType());
	    	jo.put("Resource", n.getNodeResource());
	    	jo.put("Value", n.getCurrentValue());
	    	
	    	if(n.getNodeType().equalsIgnoreCase("actuator")){
	    		Map<String,String> thresholds = n.getThresholds();
	    		if(n.getNodeResource().equalsIgnoreCase("oxygen")) {
	    			jo.put("oxygen_threshold", thresholds.get("oxygen_threshold"));
	    		}
	    		if(n.getNodeResource().equalsIgnoreCase("light")) {
	    			jo.put("light_threshold", thresholds.get("light_threshold"));
	    		}
	    		if(n.getNodeResource().equalsIgnoreCase("water")) {
	    			jo.put("ph_threshold", thresholds.get("ph_threshold"));
	    			jo.put("minerals_threshold", thresholds.get("minerals_threshold"));
	    		}
	    		jo.put("ManualMode", n.getManualMode());
	    	}
	    	
	    	if(n.getNodeType().equalsIgnoreCase("sensor") && n.hasActuator())
	    		jo.put("assignTo", n.getActuator().getNodeIP());
	    	ja.put(jo);
		
		}
		mainObj.put("jsonarray", ja);
        return mainObj.toString();
	}


}
