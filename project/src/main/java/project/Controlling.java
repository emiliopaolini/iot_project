package project;

import java.io.IOException;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.coap.MediaTypeRegistry;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.servlet.mvc.method.annotation.SseEmitter;

@Controller
public class Controlling {

	@RequestMapping("/")
	public String run(Model model) {
		return init(model);
	}

	@RequestMapping("/home")
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

	@RequestMapping(value = "/setActuator")
	public String setActuator(@RequestParam(required = true) String nodeIP,
			@RequestParam(required = true) String currentValue, Model model) {
		Node node = Server.nodes.stream().filter(n -> n.getNodeIP().equals(nodeIP)).findAny().get();
		node.setCurrentValue(currentValue);
		CoapClient client = new CoapClient("coap://[" + node.getNodeIP() + "]/" + node.getNodeResource());
		System.out.println("new status is sent: " + node.getCurrentValue());
		client.post("status=" + node.getCurrentValue(), MediaTypeRegistry.TEXT_PLAIN);

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
	
	@RequestMapping(value = "/getData")
	public String getData(@RequestParam(required = true) String nodeIP) {
		System.out.println("Ip received: "+nodeIP);
		
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
