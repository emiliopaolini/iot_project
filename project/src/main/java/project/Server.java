package project;

import java.util.ArrayList;



import org.eclipse.californium.core.CoapServer;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.web.bind.annotation.RestController;

import com.mongodb.MongoClient;
import com.mongodb.client.MongoCollection;
import com.mongodb.client.MongoDatabase;

import spring.Application;
import org.bson.Document;  

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
		
		provaMongo();
//		
		
		
	} 
	
	public static void provaMongo() {
		try{  
			//---------- Connecting DataBase -------------------------//  
			MongoClient mongoClient = new MongoClient( "localhost" , 27017 );  
			//---------- Creating DataBase ---------------------------//  
			MongoDatabase db = mongoClient.getDatabase("iot_project");  
			//---------- Creating Collection -------------------------//  
			MongoCollection<Document> table = db.getCollection("iot");  
			//---------- Creating Document ---------------------------//    
			Document doc = new Document("name", "Peter John");  
			doc.append("id",12);  
			//----------- Inserting Data ------------------------------//  
			table.insertOne(doc);  
		}catch(Exception e){  
			System.out.println(e);  
		}  
	}
}