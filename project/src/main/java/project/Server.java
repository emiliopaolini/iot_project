package project;

import java.util.ArrayList;

import org.eclipse.californium.core.CoapServer;



public class Server extends CoapServer {

	public static ArrayList<Node> nodes = new ArrayList<Node>();

	public static void main(String args[]) {
		
		Server server = new Server(); 
		server.add(new ResourceRegistration("registration"));
		server.add(new ResourceDiscovery("discovery"));
		server.start();
		System.out.println("Running it!");
	} 
}