#include "ObjectPool.h"
#include <iostream>


int * a;
int main()
{
	auto a = std::make_shared<int>(6);
	auto b = std::make_shared<int>(7);
	std::cout << a << '\n';
	std::cout << b << '\n';
	a.swap(b);
	std::cout << a << '\n';
	std::cout << b << '\n';

	ObjectPool<int> pool = ObjectPool<int>();
	pool.Push(std::make_unique<int>(50));
	pool.Push(std::make_unique<int>(50));
	pool.Push(std::make_unique<int>(50));
	pool.Push(std::make_unique<int>(50));
	pool.Push(std::make_unique<int>(50));
	pool.Push(std::make_unique<int>(50));
	pool.Push(std::make_unique<int>(50));
	pool.Push(std::make_unique<int>(50));
	pool.Push(std::make_unique<int>(50));

	//auto obj = pool.Pop();
	auto i = new int;
	*i = 5;
	std::cout << pool.Count() << '\n';
	std::cout << pool.ActiveCount() << '\n';
//	pool.Pop();
	std::cout << "entering scope\n";
	{
		auto obj = pool.Pop();

	}
	std::cout << "exiting scope\n";
	std::cout << pool.Count() << '\n';
	std::cout << pool.ActiveCount() << '\n';

	
	auto pool1 = ObjectPool<int>();
	std::cout << "pool1 shared" << pool1._sharedThis << '\n';
	for (int i = 0; i < 10; i++)
	{
		pool1.Push(std::make_unique<int>(i));
	}
	auto pool2 = ObjectPool<int>();
	std::cout << "pool2 shared" << pool2._sharedThis << '\n';
	//pool2=(std::move(pool1));
	for (int i = 0; i < 10; i++)
	{
		pool2.Push(std::make_unique<int>(i));
	}
	{
		std::cout << "pool2 shared" << pool2._sharedThis << '\n';
		auto obj = pool2.Pop();

	}
	std::cout << "pool2 shared" << pool2._sharedThis << '\n';
	return 0;
}
