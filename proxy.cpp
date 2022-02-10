SocketBuilder socket_buidler("80");

int main(int argc, char const *argv[]) {
	BlockingQueue<Socket> socket_queue;
	ThreadPool thread_pool(16);
	thread_pool.execute(thread_func);
	while (1) {
        Socket socket = SocketBuilder.acceptTCPConnection();
        socket_queue.push(socket);
	}
	return 0;
}



void thread_func() {
	while (1) {
		Socket client_proxy_socket = socket_queue.pop();
		HttpRequest request = client_proxy_socket.readRequest();
		Socket proxy_server_socket = socket_buidler.openTCPConnection(request.getHost(), request.getPort());
		Task task(std::move(client_proxy_socket), std::move(proxy_server_socket), request);
		Task.execute();
	}
}
		// // OPTION 1
		// HttpResponse response = request.handle(proxy_server_socket);
		// client_proxy_socket.sendResponse(response);
		// if (request.type == HttpRequest::CONNECT) {
		// 	BlindForwarder forwarder(std::move(client_proxy_socket), std::moveA(proxy_server_socket));
		// 	forwarder.forward()
		// }

		// OPTION 2
		


		// HttpResponse response;
		// optional<Socket> optional_socket;
		// tie(response, optional_socket) = request.handle();
		// client_proxy_socket.write(response);

		// // OPTION 1
		// // if (request.type == HttpResponse::CONNECT) {
		// // 	BlindForwarder forwarder(client_proxy_socket, proxy_server_socket);
		// // 	forwarder.forward();
		// // }
		// // // OPTION 2
		// // if (optional_socket.hasValue()) {
		// // 	Socket proxy_server_socket = optional_socket.value();
		// // 	BlindForwarder forwarder(client_proxy_socket, proxy_server_socket);
		// // 	forwarder.forward();
		// // 	proxy_server_socket.close();
    









