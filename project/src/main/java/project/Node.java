package project;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

//TODO: specify the assigned actuator for the sensor in order to represent on the UI
public class Node {
	private String nodeIP;
	private String nodeType;
	private String nodeResource;
	private String currentValue;// this can be "on/off" for actuators or the actual value for sensors
	private Node actuator;
	Map<String, String> thresholds; 

	public Node() {
		nodeIP = nodeType = currentValue = "";
	}

	public Node(String nodeIP, String nodeType, String nodeResource) {
		this.nodeIP = nodeIP;
		this.nodeType = nodeType;
		this.nodeResource = nodeResource;
		this.currentValue = "";
		this.actuator = null;
		this.thresholds =  new HashMap<String, String>();
	}

	public String toString() {
		String temp = "";
		temp += "Node IP: " + nodeIP + "\n";
		temp += "Node Type: " + nodeType + "\n";
		temp += "Node Resource: " + nodeResource + "\n";
		if (this.nodeType.equalsIgnoreCase("Actuator"))
			temp += "Node current status: " + currentValue;
		else
			temp += "Node current value: " + currentValue;
		return temp;
	}

	public void addThreshold(String thresholdName,String thresholdValue) {
		this.thresholds.put(thresholdName, thresholdValue);
	}
	
	public Map<String, String> getThresholds(){
		return this.thresholds;
	}
	
	public String getNodeType() {
		return nodeType;
	}

	public void setNodeType(String nodeType) {
		this.nodeType = nodeType;
	}

	public String getNodeResource() {
		return nodeResource;
	}

	public void setNodeResource(String nodeResource) {
		this.nodeResource = nodeResource;
	}

	public String getCurrentValue() {
		return currentValue;
	}

	public void setNodeIP(String nodeIP) {
		this.nodeIP = nodeIP;
	}

	public String getNodeIP() {
		return this.nodeIP;
	}

	public void setCurrentValue(String currentValue) {
		this.currentValue = currentValue;
	}

	public boolean hasActuator() {
		return actuator != null;
	}

	public void setActuator(Node actuator) {
		this.actuator = actuator;
	}

	public Node getActuator() {
		return actuator;
	}

	@Override
	public boolean equals(Object o) {
		// self check
		if (this == o)
			return true;
		// null check
		if (o == null)
			return false;
		// type check and cast
		if (getClass() != o.getClass())
			return false;
		Node sensor = (Node) o;
		// field comparison
		return Objects.equals(this.nodeIP, sensor.nodeIP);
	}

}