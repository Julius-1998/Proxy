#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "Socket.hpp"
#include "SocketBuilder.hpp"
#include "BlindForwarder.hpp"
#include "Task.hpp"
#include "BlockingQueue.hpp"
#include "ThreadPool.hpp"


void thread_func();

SocketBuilder socket_buidler("80");
BlockingQueue<Socket> socket_queue(64);

int main(int argc, char const *argv[]) {
	ThreadPool thread_pool(16);
	thread_pool.execute(thread_func);
	while (1) {
        socket_queue.push(std::move(socket_buidler.acceptTCPConnection()));  // NOTE: move can be omited
	}
	return 0;
}



void thread_func() {
	while (1) {
		Socket client_proxy_socket = std::move(socket_queue.pop());  // NOTE: move can be omited
		HttpRequest request = client_proxy_socket.recvRequest();
		Socket proxy_server_socket = std::move(socket_buidler.openTCPConnection(request.getHost(), request.getPort()));  // NOTE: move can be omited
		Task task(std::move(client_proxy_socket), std::move(proxy_server_socket), request);
		task.execute();
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
    









