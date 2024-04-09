#include <iostream>
#include <thread>
#include <vector>

void thread_func(int my_id)
{
	// 멀티스레드는 동시에 실행될 수 있다.
	std::cout << "Hello world! From: " << my_id << std::endl;
}


int main()
{
	std::vector<std::thread> threads{};

	for (int i{ 0 }; i < 10; ++i) {
		threads.push_back(std::thread{ thread_func, i });
	}

	// 프로세스가 끝나기 전에 스레드를 종료해 주어야 함.
	for (auto& t : threads) {
		t.join();
	}
}