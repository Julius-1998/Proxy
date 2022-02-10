#include <thread>
#include <function>


ThreadPool thread_pool {
private:
	int numOfThreads;
public:
	ThreadPool(int num) : numOfThreads(num) {}
	
	template<typename Fn, typename... Args>
	void execute(Fn&& fn, Args&&... args) {
		for (int i = 0; i < numOfThreads; ++i)
			std::thread(fn, args).detach();
	}
};