#include "pch.h"
#include "../ObjectPoolAPI/ObjectPool.h"



TEST(ObjectPool, Constructor)
{
	auto pool = ObjectPool<int, 10>();
	int* obj = pool.GetObject();
	*obj = 5;
	EXPECT_EQ(pool.Count(), 10);
	EXPECT_EQ(nullptr, pool.GetObject());
	//EXPECT_EQ(nullptr, pool.GetObject());
	//EXPECT_EQ(*obj, 5);

}

TEST(ObjectPool, IsCopyable)
{
	auto pool = ObjectPool<int, 10>();
	auto pool1(pool);
	EXPECT_TRUE(pool == pool1);

}

TEST(ObjectPool, ActiveCount)
{

}