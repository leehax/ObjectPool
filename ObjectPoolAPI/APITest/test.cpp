#include "pch.h"
#include "../ObjectPoolAPI/ObjectPool.h"
#include "../ObjectPoolAPI/IPoolable.h"
#include <gtest/gtest.h>


class TestPoolable : public IPoolable
{
public:
	TestPoolable(){}

	TestPoolable(const int name)
	{
		_value = 5;
		_name = name;
	}
	TestPoolable(const int name, const int value)
	{
		_value = value;
		_name = name;
	}
	int _name = 0;
	int _value = 0;
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
	ObjectPool<TestPoolable> pool;
	EXPECT_EQ(pool.Count(),20);

}

TEST(ObjectPool, ConstructorSetsMaxSize)
{
	{
		ObjectPool<TestPoolable> pool(1);
		EXPECT_EQ(pool.Count(), 1);
	}
	{
		ObjectPool<TestPoolable> pool(10);
		EXPECT_EQ(pool.Count(), 10);
	}
	{
		ObjectPool<TestPoolable> pool(660);
		EXPECT_EQ(pool.Count(), 660);
	}
	{
		ObjectPool<TestPoolable> pool(-10);
		EXPECT_EQ(pool.Count(), 10);
	}
	{
		ObjectPool<TestPoolable> pool(100);
		EXPECT_EQ(pool.Count(), 100);
	}
	{
		ObjectPool<TestPoolable> pool(5);
		EXPECT_EQ(pool.Count(), 5);
	}

}

TEST(ObjectPool, InitializesPoolablesWithOneArg)
{
	ObjectPool<TestPoolable> pool(1, 1);
	auto obj = pool.Pop();
	EXPECT_TRUE(obj->_name == 1);
}

TEST(ObjectPool, InitializesPoolablesWithArgs)
{
	ObjectPool<TestPoolable> pool(1, 1, 2);
	auto obj = pool.Pop();
	EXPECT_TRUE(obj->_name == 1);
	EXPECT_TRUE(obj->_value == 2);
}

TEST(ObjectPool, ThrowsOnOutOfRange)
{
	ObjectPool<TestPoolable> pool(0);
	EXPECT_THROW(pool.Pop(), std::out_of_range);

	ObjectPool<TestPoolable> pool1(5);
	auto o = pool1.Pop();
	auto o1 = pool1.Pop();
	auto o2 = pool1.Pop();
	auto o3 = pool1.Pop();
	auto o4 = pool1.Pop();
	EXPECT_THROW(pool1.Pop(), std::out_of_range);
}


TEST(ObjectPool, CanGet)
{
	ObjectPool<TestPoolable> pool(10);

	EXPECT_EQ(pool.Count(), 10);
	auto obj = pool.Pop();
	EXPECT_EQ(pool.Count(), 9);
	EXPECT_FALSE(obj == nullptr);
}


TEST(ObjectPool, PointersReturnToPoolAfterReset)
{
	ObjectPool<TestPoolable> pool(1);
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

TEST(ObjectPool, PointerRetainsDataAfterReset)
{
	ObjectPool<TestPoolable> pool(1);
	EXPECT_EQ(pool.Count(), 1);
	auto obj = pool.Pop();
	obj->_name = 50;
	EXPECT_EQ(pool.Count(), 0);
	EXPECT_THROW(pool.Pop(), std::out_of_range);
	obj.reset(nullptr);
	auto obj1 = pool.Pop();
	EXPECT_EQ(obj1->_name, 50);
}

TEST(ObjectPool, RAII)
{
	ObjectPool<TestPoolable> pool(10);

	{
		auto obj = pool.Pop();
		EXPECT_EQ(pool.Count(), 9);
		{
			auto obj1 = pool.Pop();
			EXPECT_EQ(pool.Count(), 8);
		}
		EXPECT_EQ(pool.Count(), 9);
	}

	EXPECT_EQ(pool.Count(), 10);
}

TEST(ObjectPool, EqualityComparable)
{
	ObjectPool<TestPoolable> pool(5);
	ObjectPool<TestPoolable> pool1(5);

	EXPECT_FALSE(pool == pool1);
	{
		auto obj = pool.Pop();
		EXPECT_FALSE(pool == pool1);
	}
	EXPECT_FALSE(pool == pool1);

	auto obj = pool.Pop();
	EXPECT_FALSE(pool == pool1);
}


TEST(Poolable, DefaultConstructible)
{
	TestPoolable t;
	EXPECT_TRUE(t._name == 0);
	EXPECT_TRUE(t.TimeAlive() == t.TimeAlive().zero());;
}
TEST(Poolable, ConstructorInitializesData)
{
	TestPoolable t(10);
	EXPECT_TRUE(t._name == 10);
}

TEST(Poolable, TimeAliveReturnsCorrect)
{
	TestPoolable t(1);
	t.OnRemoveFromPool();
	auto time = t.TimeAlive();
	EXPECT_TRUE(time == t.TimeAlive());
	DoSomethingExpensive();
	EXPECT_FALSE(time == t.TimeAlive());
	EXPECT_TRUE(t.TimeAlive() > time);
}