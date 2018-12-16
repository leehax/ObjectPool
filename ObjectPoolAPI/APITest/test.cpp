#include "pch.h"
#include "../ObjectPoolAPI/ObjectPool.h"



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