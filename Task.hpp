#ifndef __TASK_HPP__
#define __TASK_HPP__
class Task {
private:
	Socket in;
	Socket out;
	HttpRequest request;
public:
	Task(Socket&& in, Socket&& out, const HttpRequest& request) : in(std::move(in)), out(std::move(out)), request(request) {}

	void execute() {
		HttpResponse response = request.handle(out);
		in.sendResponse(response);
		if (response.status == HttpResponse::OK && request.method == HttpRequest::CONNECT) {
			BlindForwarder forwarder(std::move(in), std::move(out));
			forwarder.forward();
            return;
		}
        keep_alive();
	}

    void keep_alive() {
        if (request.connection != HttpRequest::KEEP_ALIVE)
            return
        request = in.recvRequest();
        HttpResponse response = request.handle(out);
        in.sendResponse(response);
        // TODO
        // if request or response is empty, return
        keep_alive();
    }
};

#endif
