#pragma once
#include <mutex>
#include <condition_variable>


template<typename T>
class threadsafe_wait_queue
{
public:
	threadsafe_wait_queue() {}
	~threadsafe_wait_queue() {}

	void push(T new_data)
	{
		std::lock_guard<std::mutex> lk(mut);            // 1.全局加锁  
		data_queue.push(std::move(new_data));           // 2.push时独占锁  
		cond.notify_one();
	}
	void wait_and_pop(T& val)
	{
		std::unique_lock<std::mutex> ulk(mut);                    // 3.全局加锁  
		cond.wait(ulk, [this]() { return !data_queue.empty(); });  // 4.front 和 pop_front时独占锁  
		val = std::move(data_queue.front());
		data_queue.pop();
	}
	std::shared_ptr<T> wait_and_pop()
	{
		std::unique_lock<std::mutex> ulk(mut);
		cond.wait(ulk, [this]() { return !data_queue.empty(); });
		std::shared_ptr<T> val(std::make_shared<T>(std::move(data_queue.front())));
		data_queue.pop();
		return val;
	}
	bool try_pop(T& val)
	{
		std::lock_guard<std::mutex> lk(mut);
		if (data_queue.empty())
			return false;
		val = std::move(data_queue.front());
		data_queue.pop();
		return true;
	}
	std::shared_ptr<T> try_pop()
	{
		std::shared_ptr<T> val;
		std::lock_guard<std::mutex> lk(mut);
		if (data_queue.empty())
			return val;
		val = std::make_shared<T>(std::move(data_queue.front()));
		data_queue.pop();
		return val;
	}
	bool empty()
	{
		std::lock_guard<std::mutex> lk(mut);
		return data_queue.empty();
	}
private:
	std::queue<T> data_queue;
	std::mutex mut;
	std::condition_variable cond;
};

template<typename T>
class threadsafe_not_wait_queue
{
public:
	threadsafe_not_wait_queue() {}
	~threadsafe_not_wait_queue() {}

	void push(T new_data)
	{
		mut.lock();
		data_queue.push(std::move(new_data));        // 2.push时独占锁  
		mut.unlock();
	}
	void pop(T& val)
	{
		if (mut.try_lock())
		{
			if (!data_queue.empty())
			{
				val = std::move(data_queue.front());
				data_queue.pop();
			}
			mut.unlock();
		}
	}
	std::shared_ptr<T> pop()
	{
		std::shared_ptr<T> val;
		if (mut.try_lock())
		{
			if (!data_queue.empty())
			{
				val = std::make_shared<T>(std::move(data_queue.front()));
				data_queue.pop();

			}
			mut.unlock();
		}
		return val;
	}
	bool empty()
	{
		std::lock_guard<std::mutex> lk(mut);
		return data_queue.empty();
	}
private:
	std::queue<T> data_queue;
	std::mutex mut;
};
