package project;

public class Sensor{
    private String nodeIP;
    private String nodeType;
    private String currentValue;//this can be "on/off" for actuators or the actual value for sensors

    public Sensor(){
        nodeIP = nodeType = currentValue = "";
    }

    public Sensor(String nodeIP,String nodeType){
        this.nodeIP = nodeIP;
        this.nodeType = nodeType;
        this.currentValue = "";
    }

    
}