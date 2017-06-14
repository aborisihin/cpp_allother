#pragma once

#include <mutex>
#include <queue>
#include <condition_variable>

//шаблочнный класс очереди
template <class T>
class safe_queue
{

	std::mutex queue_mutex;
	std::condition_variable queue_cv;

public:

	safe_queue();

	//добавить элемент в очередь
	void push(T value);

	//вернуть первый элемент из очереди
	T pop();

	//остановить работу очереди; все дальнейшие попытки работы с очередью вызовут исключение std::logic_error
	void terminate();

protected:

	std::queue<T> queue;
	bool terminated;

	void check_terminated();

};

template <class T>
safe_queue<T>::safe_queue() : terminated(false)
{
}

template <class T>
void safe_queue<T>::push(T value)
{
	//std::cout << "thread " << std::this_thread::get_id() << " enter in push()" << std::endl;

	check_terminated();

	std::unique_lock<std::mutex> locker(queue_mutex);

	//std::cout << "thread " << std::this_thread::get_id() << " push " << value << std::endl;

	//операция std::queue::push() no-exception
	/*std::queue<T> tmp_queue(queue);
	tmp_queue.push(value);
	std::swap(queue, tmp_queue);*/

	queue.push(value);

	queue_cv.notify_one();
	
	//std::cout << "push notify!" << std::endl;
}

template <class T>
T safe_queue<T>::pop()
{
	//std::cout << "thread " << std::this_thread::get_id() << " enter in pop()" << std::endl;

	std::unique_lock<std::mutex> locker(queue_mutex);

	while (queue.empty() && !terminated)
	{
		//std::cout << "thread " << std::this_thread::get_id() << " waiting in pop()..." << std::endl;
		queue_cv.wait(locker);
	}

	//std::cout << "thread " << std::this_thread::get_id() << " notified" << std::endl;

	check_terminated();

	T value = queue.front();

	//операция std::queue::pop() no-exception
	/*std::queue<T> tmp_queue(queue);
	tmp_queue.pop();
	std::swap(queue, tmp_queue);*/

	queue.pop();

	//std::cout << "thread " << std::this_thread::get_id() << " pop " << value << std::endl;

	return value;
}

template <class T>
void safe_queue<T>::terminate()
{
	//std::cout << "terminate!" << std::endl;

	terminated = true;

	queue_cv.notify_all();

	//std::cout << "terminate notify!" << std::endl;
}

template <class T>
void safe_queue<T>::check_terminated()
{
	if (terminated)
	{
		//std::cout << "thread " << std::this_thread::get_id() << " throwing exception" << std::endl;
		throw std::logic_error("queue was terminated!");
	}
}
