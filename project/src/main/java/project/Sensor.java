public class Node{
    private String nodeIP;
    private String nodeType;
    private String currentValue;//this can be "on/off" for actuators or the actual value for sensors

    public Node(){
        nodeIP = nodeType = currentValue = "";
    }

    public Node(String nodeIP,String nodeType){
        this.nodeIP = nodeIP;
        this.nodeType = nodeType;
        this.currentValue = "";
    }

    
}