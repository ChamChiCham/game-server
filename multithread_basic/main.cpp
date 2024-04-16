#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <print>
#include <mutex>
#include <queue>
#include <concurrent_queue.h>

using namespace std::chrono;

volatile int g_sum = 0;
std::mutex sum_lock;

// compile error
// 복잡한 자료구조는 atomic으로 만들 수 없다.
// 그렇지만 copyable 객체는 atomic하게 만들 수 있다
// std::atomic<std::queue<int>> my_queue;

// intel에서 오픈소스로 공개한 concurrnt_queue를 사용

void thread_func(const int my_id)
{
	//// 이건 아얘 의미가 없음.
	//sum_lock.lock();
	//for (int i = 0; i < 50'000'000 / my_id; ++i) {
	//	g_sum = g_sum + 2;
	//}
	//sum_lock.unlock();

	
	// 병렬성은 있는데 lock이 너무 자주 호출되어서 너무 느림.
	/*for (int i = 0; i < 50'000'000 / my_id; ++i) {
		sum_lock.lock();
		g_sum = g_sum + 2;
		sum_lock.unlock();
	}*/

	// 최적화를 잘 한 경우
	volatile int local_sum{};
	for (int i = 0; i < 50'000'000 / my_id; ++i)
	{
		local_sum = local_sum + 2;
	}
	sum_lock.lock();
	g_sum = g_sum + local_sum;
	sum_lock.unlock();
}

std::atomic<int> a_sum;
void atomic_thread_func(const int my_id)
{
	// 여태 한거 중에 제일 빠른 결과가 나온다.
	for (int i = 0; i < 50'000'000 / my_id; ++i) {
		a_sum += 2;
	}
}


int main()
{
	{
		// 최적화하지 마라. 
		volatile int sum = 0;
		auto start = high_resolution_clock::now();
		for (int i = 0; i < 50'000'000; ++i)
			sum = sum + 2;
		auto end = high_resolution_clock::now();
		auto exec_t = end - start;
		auto exec_ms = duration_cast<milliseconds>(exec_t).count();
		std::cout << "1 thread sum: " << sum << " time: " << exec_ms << std::endl;
	}

	{
		for (int num_threads = 2; num_threads <= 16; num_threads *= 2) {
			g_sum = 0;
			std::vector<std::thread> threads;

			auto start = high_resolution_clock::now();
			for (int i = 0; i < num_threads; ++i) {
				threads.emplace_back(thread_func, num_threads);
			}
			for (auto& thread : threads) {
				thread.join();
			}
			auto end = high_resolution_clock::now();
			auto exec_t = end - start;
			auto exec_ms = duration_cast<milliseconds>(exec_t).count();

			std::cout << num_threads << " threads sum: " << g_sum << " time: " << exec_ms << std::endl;
		}
	}

	{
		for (int num_threads = 2; num_threads <= 16; num_threads *= 2) {
			g_sum = 0;
			std::vector<std::thread> threads;

			auto start = high_resolution_clock::now();
			for (int i = 0; i < num_threads; ++i) {
				threads.emplace_back(atomic_thread_func, num_threads);
			}
			for (auto& thread : threads) {
				thread.join();
			}
			auto end = high_resolution_clock::now();
			auto exec_t = end - start;
			auto exec_ms = duration_cast<milliseconds>(exec_t).count();

			std::cout << num_threads << " threads sum: " << a_sum << " time: " << exec_ms << std::endl;
		}
	}
}