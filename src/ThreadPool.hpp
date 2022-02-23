#ifndef __THREADPOOL_HPP__
#define __THREADPOOL_HPP__

#include <thread>


class ThreadPool {
private:
	int numOfThreads;
public:
	ThreadPool(int num) : numOfThreads(num) {}
	
	template<typename Fn, typename... Args>
	void execute(Fn&& fn, Args&&... args) {
		for (int i = 0; i < numOfThreads; ++i)
			std::thread(fn, args...).detach();
	}
};

#endif
