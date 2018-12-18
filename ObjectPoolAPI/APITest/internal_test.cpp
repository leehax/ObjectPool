#include "pch.h"
#include "../ObjectPoolAPI/ObjectPool.h"
#include "../ObjectPoolAPI/IPoolable.h"


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
	EXPECT_EQ(pool.Size(),20);

}

TEST(ObjectPool, ConstructorSetsMaxSize)
{
	{
		ObjectPool<TestPoolable> pool(1);
		EXPECT_EQ(pool.Size(), 1);
	}
	{
		ObjectPool<TestPoolable> pool(10);
		EXPECT_EQ(pool.Size(), 10);
	}
	{
		ObjectPool<TestPoolable> pool(660);
		EXPECT_EQ(pool.Size(), 660);
	}
	{
		ObjectPool<TestPoolable> pool(-10);
		EXPECT_EQ(pool.Size(), 10);
	}
	{
		ObjectPool<TestPoolable> pool(100);
		EXPECT_EQ(pool.Size(), 100);
	}
	{
		ObjectPool<TestPoolable> pool(5);
		EXPECT_EQ(pool.Size(), 5);
	}
	{
		ObjectPool<TestPoolable> pool('a');
		EXPECT_EQ(pool.Size(), 97);
	}
	{
		ObjectPool<TestPoolable> pool('\n');
		EXPECT_EQ(pool.Size(), 10);
	}
	{
		ObjectPool<TestPoolable> pool('5');
		EXPECT_EQ(pool.Size(), 53);
	}
	{
		ObjectPool<TestPoolable> pool(1.f);
		EXPECT_EQ(pool.Size(), 1);
	}
	{
		ObjectPool<TestPoolable> pool(1.69f);
		EXPECT_EQ(pool.Size(), 1);
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


TEST(ObjectPool, CanGetObject)
{
	ObjectPool<TestPoolable> pool(10);

	EXPECT_EQ(pool.Size(), 10);
	auto obj = pool.Pop();
	EXPECT_EQ(pool.Size(), 9);
	EXPECT_FALSE(obj == nullptr);
}


TEST(ObjectPool, PointersReturnToPoolAfterReset)
{
	ObjectPool<TestPoolable> pool(1);
	EXPECT_EQ(pool.Size(), 1);
	auto obj = pool.Pop();
	EXPECT_EQ(pool.Size(), 0);
	obj.reset(nullptr);
	EXPECT_EQ(pool.Size(), 1);
}

TEST(ObjectPool, PointerRetainsDataAfterReset)
{
	ObjectPool<TestPoolable> pool(1);
	auto obj = pool.Pop();
	obj->_name = 50;
	obj.reset(nullptr);
	auto obj1 = pool.Pop();
	EXPECT_EQ(obj1->_name, 50);
}

TEST(ObjectPool, PointerRetainsAddressAfterReset)
{
	ObjectPool<TestPoolable> pool(1);
	EXPECT_EQ(pool.Size(), 1);
	auto obj = pool.Pop();
	TestPoolable* objptr = obj.get();
	EXPECT_EQ(pool.Size(), 0);
	obj.reset(nullptr);
	auto obj1 = pool.Pop();
	EXPECT_EQ(obj1.get(), objptr);
	obj1.reset(nullptr);
	auto obj2 = pool.Pop();
	EXPECT_EQ(obj2.get(), objptr);
}

TEST(ObjectPool, PointerReturnsToPoolWhenOutOfScope)
{
	ObjectPool<TestPoolable> pool(10);

	{
		auto obj = pool.Pop();
		EXPECT_EQ(pool.Size(), 9);
		{
			auto obj1 = pool.Pop();
			EXPECT_EQ(pool.Size(), 8);
		}
		EXPECT_EQ(pool.Size(), 9);
	}

	EXPECT_EQ(pool.Size(), 10);
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

	TestPoolable t1(6, 19);
	EXPECT_TRUE(t1._name == 6);
	EXPECT_TRUE(t1._value == 19);
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