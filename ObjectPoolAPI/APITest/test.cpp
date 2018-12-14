#include "pch.h"
#include "../ObjectPoolAPI/ObjectPool.h"



TEST(ObjectPool, Constructor)
{
	auto pool = ObjectPool<int, 10>();
	int* obj = pool.GetObject();
	EXPECT_EQ(pool.Count(), 10);
	EXPECT_FALSE(obj == nullptr);
}

TEST(ObjectPool, IsCopyable)
{
	auto pool = ObjectPool<int, 10>();
	auto pool1 = ObjectPool<int, 10 >();
	pool1 = pool;
	EXPECT_TRUE(pool == pool1);
	auto pool2(pool);
	EXPECT_TRUE(pool == pool2);

}

TEST(ObjectPool, IsMovable)
{
	auto pool = ObjectPool<int, 10>();
	auto pool1(std::move(pool));
	EXPECT_FALSE(pool == pool1);
	auto pool2 = ObjectPool<int, 10>();
	auto oldPool2 = pool2;
	pool2 = std::move(pool1);
	*pool2.GetObject() = 5;
	EXPECT_TRUE(oldPool2 == pool1);
	EXPECT_FALSE(pool2 == pool1);
}


TEST(ObjectPool, CanGet)
{
	
	auto pool = ObjectPool<int, 10>();

	
	int* obj = pool.GetObject();
	EXPECT_EQ(pool.ActiveCount(), 1);
	pool.GetObject();
	EXPECT_EQ(pool.ActiveCount(), 2);
	pool.GetObject();
	EXPECT_EQ(pool.ActiveCount(), 3);
	pool.GetObject();
	EXPECT_EQ(pool.ActiveCount(), 4);
	pool.GetObject();
	EXPECT_EQ(pool.ActiveCount(), 5);
	pool.GetObject();
	EXPECT_EQ(pool.ActiveCount(), 6);
	pool.GetObject();
	EXPECT_EQ(pool.ActiveCount(), 7);
	pool.GetObject();
	EXPECT_EQ(pool.ActiveCount(), 8);
	pool.GetObject();
	EXPECT_EQ(pool.ActiveCount(), 9);
	pool.GetObject();
	*obj = 5;
	EXPECT_EQ(pool.ActiveCount(), 10);
	EXPECT_EQ(pool.GetObject(), nullptr);
	EXPECT_EQ(*obj, 5);

}


TEST(ObjectPool, CanRelease)
{

	auto pool = ObjectPool<int, 10>();

	int* nObj = new int(5);
	EXPECT_FALSE(pool.Release(nObj));
	int* const obj = pool.GetObject();
	int* const obj1 = pool.GetObject();
	pool.GetObject();
	pool.GetObject();
	pool.GetObject();
	pool.GetObject();
	pool.GetObject();
	pool.GetObject();
	pool.GetObject();

	*obj = 5;
	std::unique_ptr<int> ptr = std::make_unique<int>(43);
	std::unique_ptr<int> ptr2;
	ptr.reset(new int);

	EXPECT_EQ(*obj, 5);
	EXPECT_EQ(pool.ActiveCount(), 9);
	EXPECT_TRUE(pool.Release(obj));
	EXPECT_EQ(pool.ActiveCount(), 8);
	EXPECT_TRUE(pool.Release(obj1));

}