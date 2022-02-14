#ifndef __TASK_HPP__
#define __TASK_HPP__
class Task {
private:
	Socket in;
	Socket out;
	HttpRequestWrapper request;
public:
	Task(Socket&& in, Socket&& out, HttpRequestWrapper&& request) : in(std::move(in)), out(std::move(out)), request(std::move(request)) {}

	void execute() {
        printf("-----------Received Request-----------\n");
        std::cout << request.getRawData().data() << std::endl; 
        printf("-----------End of Request-------------\n");
        out.sendRequest(request);
		 // HttpResponse response = request.handle(out);
		HttpResponse response = out.recvResponse();
		in.sendResponse(response);
		if (response.status == HttpResponse::OK && request.getMethod() == HttpRequest::CONNECT) {
			BlindForwarder forwarder(std::move(in), std::move(out));
			forwarder.forward();
            return;
		}
        keep_alive();
	}

    void keep_alive() {
        if (request.getField("Connection") != "keep-alive")
            return;
        request = std::move(in.recvRequest());
        HttpResponse response = request.handle(out);
        in.sendResponse(response);
        // TODO
        // if request or response is empty, return
        keep_alive();
    }
};

#endif
