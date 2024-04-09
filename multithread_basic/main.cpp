#include <iostream>
#include <thread>
#include <vector>

void thread_func(int my_id)
{
	// ��Ƽ������� ���ÿ� ����� �� �ִ�.
	std::cout << "Hello world! From: " << my_id << std::endl;
}


int main()
{
	std::vector<std::thread> threads{};

	for (int i{ 0 }; i < 10; ++i) {
		threads.push_back(std::thread{ thread_func, i });
	}

	// ���μ����� ������ ���� �����带 ������ �־�� ��.
	for (auto& t : threads) {
		t.join();
	}
}