package project;

import org.eclipse.californium.core.CoapServer;



public class Server extends CoapServer {
	public static void main(String args[]) {
		
		Server server = new Server(); 
		server.add(new ResourceRegistration("registration")); 
		server.start();
		System.out.println("Running it!");
	} 
}