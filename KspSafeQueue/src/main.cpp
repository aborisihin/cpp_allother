#include <iostream>
#include <chrono>
#include <thread>

#include "safe_queue.h"

void threadFunctionPush(int value, safe_queue<int> &q)
{
	try
	{
		q.push(value);
	}
	catch (...) {}

	//std::cout << "sleep in push..." << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(1));

	try
	{
		q.push(value+1);
	}
	catch (...) {}

	//std::cout << "sleep in push..." << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(1));
}

void threadFunctionPop(safe_queue<int> &q)
{
	try
	{
		q.pop();
	}
	catch (...) {}
}

int main()
{
	safe_queue<int> sq;

	sq.push(-2);
	sq.push(-1);
	sq.push(0);

	std::thread t1(threadFunctionPop, std::ref(sq));
	std::thread t4(threadFunctionPush, 1, std::ref(sq));
	t1.join();
	t4.join();

	std::thread t2(threadFunctionPop, std::ref(sq));
	std::thread t21(threadFunctionPop, std::ref(sq));
	std::thread t22(threadFunctionPop, std::ref(sq));
	std::thread t23(threadFunctionPop, std::ref(sq));
	t2.join();
	t21.join();
	t22.join();
	t23.join();

	std::thread t3(threadFunctionPop, std::ref(sq));

	std::this_thread::sleep_for(std::chrono::seconds(3));

	sq.terminate();

	std::thread t5(threadFunctionPush, 3, std::ref(sq));
	std::thread t6(threadFunctionPop, std::ref(sq));

	t3.join();
	t5.join();
	t6.join();

	std::cout << "done!" << std::endl;

	return 0;
}


