#include "pch.h"

TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}


TEST(ObjectPool, PoolSize)
{

	_pool.PopulatePool(v, 59);
	EXPECT_EQ(_pool.Size(), 59);

}

TEST(ObjectPool, IsCopyable)
{
	
}