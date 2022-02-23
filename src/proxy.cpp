#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "Socket.hpp"
#include "SocketBuilder.hpp"
#include "BlindForwarder.hpp"
#include "Task.hpp"
#include "BlockingQueue.hpp"
#include "ThreadPool.hpp"
#include "logger.hpp"
#include <iostream>
void thread_func();
void handler(int);
SocketBuilder socket_buidler("12345");
BlockingQueue<Socket> socket_queue(64);
int cnt;
int main(int argc, char const *argv[]) {
    pid_t pid, sid;

	/* Fork off the parent process */
	pid = fork();
	if (pid < 0)
	{
		exit(EXIT_FAILURE);
	}
	/* If we got a good PID, then
	   we can exit the parent process. */
	if (pid > 0)
	{
		exit(EXIT_SUCCESS);
	}

	/* Change the file mode mask */
	umask(0);

	/* Open any logs here */

	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0)
	{
		/* Log the failure */
		exit(EXIT_FAILURE);
	}

	/* Change the current working directory */
	if ((chdir("/")) < 0)
	{
		/* Log the failure */
		exit(EXIT_FAILURE);
	}

	//fork for the second time
	pid = fork();
	if (pid < 0)
	{
		exit(EXIT_FAILURE);
	}
	/* If we got a good PID, then
	   we can exit the parent process. */
	if (pid > 0)
	{
		exit(EXIT_SUCCESS);
	}

	/* Close out the standard file descriptors */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	/* Daemon-specific initialization goes here */
	Signal(SIGPIPE, handler);
	ThreadPool thread_pool(32);
	thread_pool.execute(thread_func);
	int id_counter = 1;
	/* The Big Loop */
	while (1)
	{
		/* Do some task here ... */
		socket_queue.push(std::move(socket_buidler.acceptTCPConnection(id_counter++))); // NOTE: move can be omited

		// sleep(30); /* wait 30 seconds */
	}
	exit(EXIT_SUCCESS);
}



void thread_func() {
	while (1) {
		Socket client_proxy_socket = std::move(socket_queue.pop());  // NOTE: move can be omited
		HttpRequestWrapper request = client_proxy_socket.recvRequest();
        if (request.getRawData().empty())
            continue; 
		Socket proxy_server_socket = std::move(socket_buidler.openTCPConnection(request.getHost(), request.getPort()));  // NOTE: move can be omited
		Task task(std::move(client_proxy_socket), std::move(proxy_server_socket), std::move(request));
		task.execute();
	}
}

void handler(int sig) {
    return;
}
		


