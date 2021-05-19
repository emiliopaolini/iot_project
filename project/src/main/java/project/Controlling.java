package project;

import java.io.IOException;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.coap.MediaTypeRegistry;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.servlet.mvc.method.annotation.SseEmitter;

@Controller
public class Controlling {

	@GetMapping("/")
	public String run(Model model) {
		return init(model);
	}

	@GetMapping("/home")
	public String init(Model model) {
		ArrayList<Node> sensors = new ArrayList<>();
		ArrayList<Node> actuators = new ArrayList<>();
		for (Node n : Server.nodes) {
			if (n.getNodeType().equalsIgnoreCase("sensor")) {
				sensors.add(n);
			} else {
				actuators.add(n);
			}
		}
		model.addAttribute("sensors", sensors);
		model.addAttribute("actuators", actuators);

		return "home";
	}
	
	@RequestMapping("/changeStatus")
	public String changeStatus(Model model) {
		
		CoapClient client = new CoapClient("coap://[" + Server.nodes.get(1).getNodeIP() + "]/"+Server.nodes.get(1).getNodeResource());
		client.post("status="+1, MediaTypeRegistry.TEXT_PLAIN);
		
		System.out.println("i am inside change status");
		System.out.println("coap://[" + Server.nodes.get(1).getNodeIP() + "]/"+Server.nodes.get(1).getNodeResource());
		return "home";
	}
	

	@RequestMapping("/updatePage")
	public String updatePage(Model model) {
		ArrayList<Node> sensors = new ArrayList<>();
		ArrayList<Node> actuators = new ArrayList<>();
		for (Node n : project.Server.nodes) {
			if (n.getNodeType().equalsIgnoreCase("sensor")) {
				sensors.add(n);
			} else {
				actuators.add(n);
			}
		}
		model.addAttribute("sensors", sensors);
		model.addAttribute("actuators", actuators);
		return "home";
	}
	
	
	
//	private final List<SseEmitter> sseEmitter = new LinkedList<>();
//
//	@RequestMapping (path = "/home", method = RequestMethod.GET)
//	public SseEmitter update() throws IOException {
//
//	    SseEmitter emitter = new SseEmitter();
//	    synchronized (sseEmitter) {
//	        sseEmitter.add(emitter);
//	    }
//	    emitter.onCompletion(() -> sseEmitter.remove(emitter));
//
//	    return emitter;
//	}

}
