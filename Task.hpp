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
        if (request.getMethod() == HttpRequest::CONNECT) {
            HttpResponse response;
            response.appendRawData("HTTP1.1 200 OK\r\n\r\n");
            in.sendResponse(response);
            BlindForwarder forwarder(std::move(in), std::move(out));
			forwarder.forward();
            return;
        }

        out.sendRequest(request);
		 // HttpResponse response = request.handle(out);
		HttpResponse response = out.recvResponse();
        printf("-----------Received Response-----------\n");
        std::cout << response.getRawData().data() << std::endl; 
        printf("-----------End of Response-------------\n");
        in.sendResponse(response);
        keep_alive();
	}

    void keep_alive() {
        if (request.getField("CONNECTION") != "keep-alive" && request.getField("PROXY-CONNECTION") != "keep-alive")
            return;
        printf("-----------Request in Keep-Alive-----------\n");
        request = std::move(in.recvRequest());
        if (request.getRawData().empty())
            return;
        out.sendRequest(request);
        printf("-----------Response in Keep-Alive-----------\n");
        HttpResponse response = out.recvResponse();
        if (response.getRawData().empty())
            return;
        in.sendResponse(response);
        // TODO
        // if request or response is empty, return
        keep_alive();
    }
};

#endif
