package project;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.Statement;
import java.util.ArrayList;

import org.eclipse.californium.core.CoapServer;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.web.bind.annotation.RestController;

import spring.Application;


@SpringBootApplication
@RestController
public class Server extends CoapServer  {

	public static ArrayList<Node> nodes = new ArrayList<Node>();

	public static Connection con;  
	
	public static void main(String args[]) {
		
		SpringApplication.run(Server.class, args);
		
		Server server = new Server(); 
		server.add(new ResourceRegistration("registration"));
		server.add(new ResourceDiscovery("discovery"));
		server.start();
		
		
		System.out.println("Running it!");
//		
		try{  
			con = DriverManager.getConnection("jdbc:mysql://localhost:3306/iot_project","root","root");  
			System.out.println("Connection to mysql done!");
		}catch(Exception e){ System.out.println(e);}  
		
	}
		
	 
}