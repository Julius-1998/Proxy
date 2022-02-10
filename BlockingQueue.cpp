#include <queue>
#include <condition_variable>
#include <mutex>
template <typename T>
class BlockingQueue {
private:
	std::queue<T> q;
	size_t capacity;
	std::condition_variable cv;
	std::mutex mutex;
public:
	BlockingQueue(size_t capacity) : q(), capacity(capacity), cv(), mutex() {}

	void push(T obj) {
		std::unique_lock<std::mutex> lock(mutex);
		while (q.size() == capacity) {
			cv.wait(lock);
		}
		q.push(obj);
		cv.notify_all();
	}

	T pop() {
		std::unique_lock<std::mutex> lock(mutex);
		while (q.empty()) {
			cv.wait(lock);
		}
		T ret = q.top();
		q.pop();
		cv.notify_one();
		return ret;
	}
};