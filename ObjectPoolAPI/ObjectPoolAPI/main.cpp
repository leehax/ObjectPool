#include "ObjectPool.h"
#include <iostream>


int * a;
int main()
{
	ObjectPool<int*, 10> pool = ObjectPool<int*, 10>();
	auto obj = pool.GetObject();
	auto i = new int;
	*i = 5;
	std::cout << pool.Count() << '\n';
	std::cout << pool.ActiveCount() << '\n';
	std::cout << pool.Count() << '\n';
	std::cout << pool.ActiveCount() << '\n';
	return 0;
}
