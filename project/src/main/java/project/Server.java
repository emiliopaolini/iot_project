package project;

import java.util.ArrayList;

import org.eclipse.californium.core.CoapServer;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.web.bind.annotation.RestController;

import spring.Application;


@SpringBootApplication
@RestController
public class Server extends CoapServer {

	public static ArrayList<Node> nodes = new ArrayList<Node>();

	public static void main(String args[]) {
		
		SpringApplication.run(Server.class, args);
		
		Server server = new Server(); 
		server.add(new ResourceRegistration("registration"));
		server.add(new ResourceDiscovery("discovery"));
		server.start();
		System.out.println("Running it!");
		
		
		
	} 
}