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
// ������ �ڷᱸ���� atomic���� ���� �� ����.
// �׷����� copyable ��ü�� atomic�ϰ� ���� �� �ִ�
// std::atomic<std::queue<int>> my_queue;

// intel���� ���¼ҽ��� ������ concurrnt_queue�� ���

void thread_func(const int my_id)
{
	//// �̰� �ƾ� �ǹ̰� ����.
	//sum_lock.lock();
	//for (int i = 0; i < 50'000'000 / my_id; ++i) {
	//	g_sum = g_sum + 2;
	//}
	//sum_lock.unlock();

	
	// ���ļ��� �ִµ� lock�� �ʹ� ���� ȣ��Ǿ �ʹ� ����.
	/*for (int i = 0; i < 50'000'000 / my_id; ++i) {
		sum_lock.lock();
		g_sum = g_sum + 2;
		sum_lock.unlock();
	}*/

	// ����ȭ�� �� �� ���
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
	// ���� �Ѱ� �߿� ���� ���� ����� ���´�.
	for (int i = 0; i < 50'000'000 / my_id; ++i) {
		a_sum += 2;
	}
}


int main()
{
	{
		// ����ȭ���� ����. 
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