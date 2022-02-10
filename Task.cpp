class Task {
private:
	Socket in;
	Socket out;
	HttpRequest request;
public:
	Task(Socket&& in, Socket&& out, const HttpRequest& request) : in(in), out(out), request(request) {}

	void execute() {
		HttpResponse response = request.handle(proxy_server_socket);
		client_proxy_socket.sendResponse(response);
		if (response.status == HttpResponse::OK && request.type == HttpRequest::CONNECT) {
			BlindForwarder forwarder(std::move(client_proxy_socket), std::move(proxy_server_socket));
			forwarder.forward()
		}
	}
}