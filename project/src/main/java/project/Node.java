package project;

public class Node{
    private String nodeIP;
    private String nodeType;
    private String nodeResource;
    private String currentValue;//this can be "on/off" for actuators or the actual value for sensors

    public Node(){
        nodeIP = nodeType = currentValue = "";
    }

    public Node(String nodeIP,String nodeType,String nodeResource){
        this.nodeIP = nodeIP;
        this.nodeType = nodeType;
        this.nodeResource = nodeResource;
        this.currentValue = "";
    }
    
    public String getNodeIP() {
    	return this.nodeIP;
    }

    public void setCurrentValue(String currentValue){
        this.currentValue = currentValue;
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

    
}