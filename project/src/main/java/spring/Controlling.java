package spring;

import java.io.IOException;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.servlet.mvc.method.annotation.SseEmitter;

import project.Node;
import project.Server;

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
		model.addAttribute("sensor", sensors);
		model.addAttribute("actuator", actuators);

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
