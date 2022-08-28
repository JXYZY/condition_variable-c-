// condition_variable.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>

std::condition_variable cv;
std::mutex mtx;
bool flag = false;
void print_int(int i)
{
	//这里不能用std::lock_guard 因为这个对象只有在析构的时候会在析构函数中解锁，
	//而condition_variable的wait函数一旦调用，会解锁，所以只有unique_lock才满足函数执行当中解锁，上锁
	std::unique_lock<std::mutex> lck(mtx);
	while (!flag)
	{
		// wait会解锁并且阻塞当前线程
		cv.wait(lck);
	}
	std::cout << "thread[" << i << "]:" << std::this_thread::get_id() << std::endl;
}

void go()
{
	std::lock_guard<std::mutex> lck(mtx);
	flag = true;
	cv.notify_all();
	//cv.notify_one();
}

std::condition_variable produce, consume;

int cargo = 0;     // shared value by producers and consumers

void consumer() {
	
	std::unique_lock<std::mutex> lck(mtx);
	/*std::cout << "thread id:" << std::this_thread::get_id() << std::endl;*/
	while (cargo == 0) 
	{
		std::cout << "cargo test:" << cargo << std::endl;
		consume.wait(lck);
	}
	std::cout << cargo << '\n';
	//std::this_thread::sleep_for(std::chrono::seconds(10));
	cargo = 0;
	//produce.notify_one();
}

void producer(int id) {
	std::unique_lock<std::mutex> lck(mtx);
	/*while (cargo != 0)
	{
		std::cout << "12111111111" << std::endl;
		produce.wait(lck);
	}*/
		
	cargo = id;
	consume.notify_one();
}

int main()
{
#if 0
	//notify_all
	std::thread t[10];
	for (int i = 0; i < 10; ++i)
		t[i] = std::thread(print_int,i);
	go();
	for (int i = 0; i < 10; ++i)
		t[i].join();


#endif
#if 1
	//重点中的重点：wait函数调用的时候，会阻塞线程，并且接触lck,如果通知接触wait阻塞的话，那么lck会锁定
	//notify_one
	std::thread consumers[10], producers[10];
	// spawn 10 consumers and 10 producers:
	for (int i = 0; i < 10; ++i) {
		consumers[i] = std::thread(consumer);
		producers[i] = std::thread(producer, i + 1);
	}

	// join them back:
	for (int i = 0; i < 10; ++i) {
		producers[i].join();
		consumers[i].join();
	}
#endif
    std::cout << "Hello World!\n";
	std::cin.get();
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
