package project;

public class Node{
    private String nodeIP;
	private String nodeType;
    private String nodeResource;
    private String currentValue;//this can be "on/off" for actuators or the actual value for sensors
    private boolean isAssigned;
    
    public Node(){
        nodeIP = nodeType = currentValue = "";
    }

    public Node(String nodeIP,String nodeType,String nodeResource){
        this.nodeIP = nodeIP;
        this.nodeType = nodeType;
        this.nodeResource = nodeResource;
        this.isAssigned = false;
        this.currentValue = "";
    }
    
   

    

	public String toString(){
        String temp = "";
        temp += "Node IP: "+nodeIP+"\n";
        temp += "Node Type: "+nodeType+"\n";
        temp += "Node Resource: "+nodeResource+"\n";
        if(this.nodeType.equalsIgnoreCase("Actuator"))
            temp += "Node current status: "+currentValue;
        else
            temp += "Node current value: "+currentValue;
        return temp;
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

    public void setCurrentValue(String currentValue){
        this.currentValue = currentValue;
    }

    public boolean isAssigned() {
		return isAssigned;
	}

	public void setAssigned(boolean isAssigned) {
		this.isAssigned = isAssigned;
	}
    
}