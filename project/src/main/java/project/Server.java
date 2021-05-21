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
public class Server extends CoapServer {

	public static ArrayList<Node> nodes = new ArrayList<Node>();

	public static void main(String args[]) {
		
		SpringApplication.run(Server.class, args);
		
		Server server = new Server(); 
		server.add(new ResourceRegistration("registration"));
		server.add(new ResourceDiscovery("discovery"));
		server.start();
		System.out.println("Running it!");
//		
		try{  
			Connection con=DriverManager.getConnection(  
			"jdbc:mysql://localhost:3306/iot_project","root","root");  
			//here sonoo is database name, root is username and password  
			Statement stmt=con.createStatement();  
			ResultSet rs=stmt.executeQuery("select * from sensor");  
			while(rs.next())  
				System.out.println(rs.getString(1)+"  "+rs.getString(2));  
			con.close();  
		}catch(Exception e){ System.out.println(e);}  
		
	}
		
	 
}