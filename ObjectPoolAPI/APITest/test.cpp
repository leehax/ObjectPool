#include "pch.h"
#include "../ObjectPoolAPI/ObjectPool.h"
#include "../ObjectPoolAPI/IPoolable.h"

class TestPoolable : public IPoolable
{
public:
	TestPoolable(){}

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
TEST(ObjectPool, IsDefaultConstructible)
{
	ObjectPool<TestPoolable> pool(20,1);
	EXPECT_EQ(pool.Count(),20);

}

TEST(ObjectPool, ConstructorSetsMaxSize)
{
	{
		ObjectPool<TestPoolable> pool(1, 1);
		EXPECT_EQ(pool.Count(), 1);
	}
	{
		ObjectPool<TestPoolable> pool(10, 1);
		EXPECT_EQ(pool.Count(), 10);
	}
	{
		ObjectPool<TestPoolable> pool(660, 1);
		EXPECT_EQ(pool.Count(), 660);
	}
	{
		ObjectPool<TestPoolable> pool(-10, 1);
		EXPECT_EQ(pool.Count(), 10);
	}
	{
		ObjectPool<TestPoolable> pool(100, 1);
		EXPECT_EQ(pool.Count(), 100);
	}
	{
		ObjectPool<TestPoolable> pool(5, 1);
		EXPECT_EQ(pool.Count(), 5);
	}


	//pool.Push(std::make_unique<int>(5));

}

TEST(ObjectPool, ThrowsOnOutOfRange)
{
	ObjectPool<TestPoolable> pool(0, 1);
	EXPECT_THROW(pool.Pop(), std::out_of_range);
}


TEST(ObjectPool, CanGet)
{
	ObjectPool<TestPoolable> pool(10, 1);


	EXPECT_EQ(pool.Count(), 10);
	auto obj = pool.Pop();
	EXPECT_EQ(pool.Count(), 9);
}


TEST(ObjectPool, PointersReturnToPoolAfterReset)
{
	ObjectPool<TestPoolable> pool(1, 1);
	//pool.Push(std::make_unique<int>(50));
	EXPECT_EQ(pool.Count(), 1);
	auto obj = pool.Pop();
	obj->_name = 50;
	EXPECT_EQ(pool.Count(), 0);
	EXPECT_THROW(pool.Pop(), std::out_of_range);
	EXPECT_EQ(obj->_name, 50);
	obj->_name = 4;
	EXPECT_EQ(obj->_name, 4);
	obj.reset(nullptr);
	EXPECT_EQ(pool.Count(), 1);
}


TEST(ObjectPool, RAII)
{
	ObjectPool<TestPoolable> pool(10, 1);

	{
		auto obj = pool.Pop();
		EXPECT_EQ(pool.Count(), 9);
	}

	EXPECT_EQ(pool.Count(), 10);
}

TEST(ObjectPool, EqualityComparable)
{
	ObjectPool<TestPoolable> pool(5,1);
	ObjectPool<TestPoolable> pool1(5,1);

	EXPECT_TRUE(pool == pool1);
	{
		auto obj = pool.Pop();
	}
	EXPECT_TRUE(pool == pool1);

	auto obj = pool.Pop();
	EXPECT_FALSE(pool == pool1);
}




//this should be in user test
TEST(ObjectPool, KillsOldestPoolable)
{
	ObjectPool<TestPoolable> pool(5, 0);
	std::vector<std::unique_ptr<TestPoolable, ObjectPool<TestPoolable>::ReturnToPool>> activePool;

	for (int i = 0; i < 5; i++)
	{
		auto obj = pool.Pop();
		obj->_name = i;
		activePool.push_back(std::move(obj));
		DoSomethingExpensive();
	}


	EXPECT_EQ(pool.Count(), 0);

	for (int i = 0; i < 5; i++)
	{
		EXPECT_EQ(activePool[i]->_name, i);
	}


	try
	{
		activePool.push_back(pool.Pop());
	}
	catch (...) {
		std::chrono::milliseconds longestTime = std::chrono::milliseconds::min();
		unsigned int longest = activePool.size() + 1;
		for (unsigned int k = 0; k < activePool.size(); k++)
		{
			if (activePool[k]->TimeAlive() > longestTime)
			{
				longestTime = activePool[k]->TimeAlive();
				longest = k;
			}
		}
		if (longest < activePool.size()) {
			EXPECT_EQ(activePool[longest]->_name, 0);
			activePool.erase(activePool.begin() + longest);

		}
	}

	EXPECT_EQ(activePool.size(), 4);
	EXPECT_EQ(pool.Count(), 1);

	activePool.push_back(pool.Pop());
	EXPECT_EQ(activePool.size(), 5);
	try
	{
		activePool.push_back(pool.Pop());
	}
	catch (...) {
		std::chrono::milliseconds longestTime = std::chrono::milliseconds::min();
		unsigned int longest = activePool.size() + 1;
		for (unsigned int k = 0; k < activePool.size(); k++)
		{
			if (activePool[k]->TimeAlive() > longestTime)
			{
				longestTime = activePool[k]->TimeAlive();
				longest = k;
			}
		}
		if (longest < activePool.size()) {
			EXPECT_EQ(activePool[longest]->_name, 1);
			activePool.erase(activePool.begin() + longest);

		}
	}

	EXPECT_EQ(activePool.size(), 4);
	EXPECT_EQ(pool.Count(), 1);

	activePool.push_back(pool.Pop());
	EXPECT_EQ(activePool.size(), 5);
	try
	{
		activePool.push_back(pool.Pop());
	}
	catch (...) {
		std::chrono::milliseconds longestTime = std::chrono::milliseconds::min();
		unsigned int longest = activePool.size() + 1;
		for (unsigned int k = 0; k < activePool.size(); k++)
		{
			if (activePool[k]->TimeAlive() > longestTime)
			{
				longestTime = activePool[k]->TimeAlive();
				longest = k;
			}
		}
		if (longest < activePool.size()) {
			EXPECT_EQ(activePool[longest]->_name, 2);
			activePool.erase(activePool.begin() + longest);

		}
	}

	EXPECT_EQ(activePool.size(), 4);
	EXPECT_EQ(pool.Count(), 1);

	activePool.push_back(pool.Pop());
	EXPECT_EQ(activePool.size(), 5);
	try
	{
		activePool.push_back(pool.Pop());
	}
	catch (...) {
		
		std::chrono::milliseconds longestTime = std::chrono::milliseconds::min();
		unsigned int longest = activePool.size() + 1;
		for (unsigned int k = 0; k < activePool.size(); k++)
		{
			if (activePool[k]->TimeAlive() > longestTime)
			{
				longestTime = activePool[k]->TimeAlive();
				longest = k;
			}
		}
		if (longest < activePool.size()) {
			EXPECT_EQ(activePool[longest]->_name, 3);
			activePool.erase(activePool.begin() + longest);

		}
	}

	EXPECT_EQ(activePool.size(), 4);
	EXPECT_EQ(pool.Count(), 1);

	activePool.push_back(pool.Pop());
	EXPECT_EQ(activePool.size(), 5);
	try
	{
		activePool.push_back(pool.Pop());
	}
	catch (...) {
		std::chrono::milliseconds longestTime = std::chrono::milliseconds::min();
		unsigned int longest = activePool.size() + 1;
		for (unsigned int k = 0; k < activePool.size(); k++)
		{
			if (activePool[k]->TimeAlive() > longestTime)
			{
				longestTime = activePool[k]->TimeAlive();
				longest = k;
				std::cout << longestTime.count() << '\n';
			}
		}
		if (longest < activePool.size()) {
			EXPECT_EQ(activePool[longest]->_name, 4);
			activePool.erase(activePool.begin() + longest);

		}
	}

	EXPECT_EQ(activePool.size(), 4);
	EXPECT_EQ(pool.Count(), 1);

	activePool.push_back(pool.Pop());


}

TEST(Poolable, TimeAliveReturnsCorrect)
{
	TestPoolable t(1);

	auto time = t.TimeAlive();
	EXPECT_TRUE(time == t.TimeAlive());
	DoSomethingExpensive();
	EXPECT_FALSE(time == t.TimeAlive());
	EXPECT_TRUE(t.TimeAlive() > time);
}