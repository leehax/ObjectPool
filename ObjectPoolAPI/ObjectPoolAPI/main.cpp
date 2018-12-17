#include "ObjectPool.h"
#include <iostream>
#include "IPoolable.h"



class TestPoolable : public IPoolable
{
public:
	TestPoolable() {}
	TestPoolable(const int name)
	{
		test = 5;
		_name = name;
	}

	int _name = 0;
private:
	int test = 0;
};

void DoSomethingExpensive() //approx 20ms 
{
	for (int i = 0; i < 10000; i++)
	{
		for (int j = 0; j < 1000; j++) {

		}
	}
}

int main()
{

	ObjectPool<TestPoolable> tPool(2, 1);
	std::cout << "popping\n";
	auto objj = tPool.Pop();
	std::cout << "popped\n";

	ObjectPool<TestPoolable> pool(5, 0);

	auto obj = pool.Pop();
	obj->_name = 0;
	DoSomethingExpensive();
	auto obj1 = pool.Pop();
	obj1->_name = 1;
	DoSomethingExpensive();
	auto obj2 = pool.Pop();
	obj2->_name = 2;
	DoSomethingExpensive();
	auto obj3 = pool.Pop();
	obj3->_name = 3;
	DoSomethingExpensive();
	auto obj4 = pool.Pop();
	obj4->_name = 4;

	
	return 0;
}
