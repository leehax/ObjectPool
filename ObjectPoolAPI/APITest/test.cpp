#include "pch.h"
#include "../ObjectPoolAPI/ObjectPool.h"
#include "../ObjectPoolAPI/IPoolable.h"


TEST(ObjectPool, IsDefaultConstructible)
{
	ObjectPool<int> pool;
	EXPECT_EQ(pool.Count(),0);

}

TEST(ObjectPool, ConstructorSetsMaxSize)
{
	ObjectPool<int> pool(1);
	EXPECT_EQ(pool.Count(), 0);
	pool.Push(std::make_unique<int>(5));

}

TEST(ObjectPool, ThrowsOnOutOfRange)
{
	ObjectPool<int> pool;
	EXPECT_THROW(pool.Pop(), std::out_of_range);
}


TEST(ObjectPool, CanGet)
{
	ObjectPool<int> pool;

	for(int i = 0 ; i<10;i++)
	{
		pool.Push(std::make_unique<int>(i));
	}
	EXPECT_EQ(pool.Count(), 10);
	auto obj = pool.Pop();
	EXPECT_EQ(pool.Count(), 9);
}


TEST(ObjectPool, PointersReturnToPoolAfterReset)
{
	ObjectPool<int> pool;
	pool.Push(std::make_unique<int>(50));
	EXPECT_EQ(pool.Count(), 1);
	auto obj = pool.Pop();
	EXPECT_EQ(pool.Count(), 0);
	EXPECT_THROW(pool.Pop(), std::out_of_range);
	EXPECT_EQ(*obj, 50);
	*obj = 4;
	EXPECT_EQ(*obj, 4);
	obj.reset(nullptr);
	EXPECT_EQ(pool.Count(), 1);
}


TEST(ObjectPool, RAII)
{
	ObjectPool<int> pool;
	for (int i = 0; i < 10; i++)
	{
		pool.Push(std::make_unique<int>(i));
	}

	{
		auto obj = pool.Pop();
		EXPECT_EQ(pool.Count(), 9);
	}

	EXPECT_EQ(pool.Count(), 10);
}

TEST(ObjectPool, EqualityComparable)
{
	ObjectPool<int> pool;
	ObjectPool<int> pool1;
	
	EXPECT_TRUE(pool == pool1);
	pool.Push(std::make_unique<int>(15));
	pool1.Push(std::make_unique<int>(15));

	EXPECT_FALSE(pool == pool1);
}


class TestPoolable : public IPoolable
{
public:
	TestPoolable(const std::string name)
	{
		test = 5;
		_name = name;
	}
	std::string _name = "";
private:
	int test = 0;
};

void DoSomethingExpensive() //approx 200ms 
{
	for (int i = 0; i < 10000; i++)
	{
		for (int j = 0; j < 10000; j++) {
		
		}
	}
}
TEST(Poolable, TimeAliveReturnsCorrect)
{
	TestPoolable t("t");
	auto time= t.TimeAlive();
	EXPECT_TRUE(time== t.TimeAlive());
	DoSomethingExpensive();
	EXPECT_FALSE(time == t.TimeAlive());
	EXPECT_TRUE(t.TimeAlive() > time);
}
//this should be in user test
TEST(ObjectPool, KillsOldestPoolable)
{
	ObjectPool<TestPoolable> pool(5);
	std::vector<std::unique_ptr<TestPoolable,ObjectPool<TestPoolable>::ReturnToPool>> activePoolables;

	pool.Push(std::make_unique<TestPoolable>("first"));
	DoSomethingExpensive();
	pool.Push(std::make_unique<TestPoolable>("second"));
	DoSomethingExpensive();
	pool.Push(std::make_unique<TestPoolable>("third"));
	DoSomethingExpensive();
	pool.Push(std::make_unique<TestPoolable>("fourth"));
	DoSomethingExpensive();
	pool.Push(std::make_unique<TestPoolable>("fifth"));

	const size_t poolSize = pool.Count();
	for(unsigned int i = 0; i < poolSize; i++)
	{
		activePoolables.push_back(pool.Pop());
		
	}


	EXPECT_TRUE(pool.Count()== 0);
	EXPECT_TRUE(activePoolables.size() == 5);

	try
	{
		activePoolables.push_back(pool.Pop());
	}
	catch(...)
	{
		//pool was empty

		std::chrono::milliseconds longestTime = std::chrono::milliseconds::min();
		unsigned int longest = activePoolables.size() + 1;
		for(unsigned int i = 0; i < activePoolables.size(); i++)
		{
			if(activePoolables[i]->TimeAlive() > longestTime)
			{
				longestTime = activePoolables[i]->TimeAlive();
				longest = i;
			}
		}

		if(longest < activePoolables.size())
		{
			EXPECT_EQ(activePoolables[longest]->_name, "first");
			activePoolables[longest].reset(nullptr);
			activePoolables.erase(activePoolables.begin() + longest);
			EXPECT_TRUE(activePoolables.size() ==4);
			EXPECT_TRUE(pool.Count() == 1);
			activePoolables.push_back(pool.Pop());
			EXPECT_TRUE(activePoolables.size() == 5);
			EXPECT_TRUE(pool.Count() == 0);
		}
	}
}