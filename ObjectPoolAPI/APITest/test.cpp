#include "pch.h"
#include "../ObjectPoolAPI/ObjectPool.h"



TEST(ObjectPool, IsDefaultConstructible)
{
	auto pool = ObjectPool<int>();
	EXPECT_EQ(pool.Count(),0);
	EXPECT_EQ(pool.ActiveCount(),0);
	

}

TEST(ObjectPool, ThrowsOnOutOfRange)
{
	auto pool = ObjectPool<int>();
	EXPECT_THROW(pool.Pop(), std::out_of_range);
}

//TEST(ObjectPool, IsCopyable)
//{
//	//auto pool = ObjectPool<int>();
//	//auto pool1 = ObjectPool<int>();
//	//pool1 = pool;
//	//EXPECT_TRUE(pool == pool1);
//	//auto pool2(pool);
//	//EXPECT_TRUE(pool == pool2);
//
//}
//
//TEST(ObjectPool, IsMovable)
//{
//	auto pool = ObjectPool<int>();
//	pool.Push(std::make_unique<int>(4));
//	auto pool1(std::move(pool));
//	EXPECT_FALSE(pool == pool1);
//	auto pool2 = ObjectPool<int>();
//	auto defaultPool = ObjectPool<int>();
//	pool2 = std::move(pool1);
//	pool2.Push(std::make_unique<int>(5));
//	EXPECT_TRUE(defaultPool == pool1);
//	EXPECT_FALSE(pool2 == pool1);
//	auto pool3 = std::move(pool2);
//	auto obj = pool3.Pop();
//	EXPECT_EQ(*obj, 5);
//
//}


TEST(ObjectPool, CanGet)
{
	
	auto pool = ObjectPool<int>();

	for(int i = 0 ; i<10;i++)
	{
		pool.Push(std::make_unique<int>(i));
	}
	EXPECT_EQ(pool.Count(), 10);
	auto obj = pool.Pop();
	
	EXPECT_EQ(pool.Count(), 9);
	obj.reset(nullptr);
	EXPECT_EQ(pool.Count(), 10);
	//auto obj = pool.Pop();
	//std::unique_ptr<int> oldObj = std::make_unique<int>(*obj);
	//EXPECT_EQ(*obj, 9); //last in first out
	//EXPECT_EQ(pool.Count(), 9);
	//obj = pool.Pop();
	//EXPECT_EQ(*obj, 8);
	//EXPECT_FALSE(*obj == *oldObj);
	//EXPECT_FALSE(obj == oldObj);
	//EXPECT_EQ(pool.Count(), 8);
	//auto obj1= pool.Pop();
	//EXPECT_EQ(pool.Count(), 7);
	//auto obj2= pool.Pop();
	//EXPECT_EQ(pool.Count(), 6);
	//auto obj3= pool.Pop();
	//EXPECT_EQ(pool.Count(), 5);
	//auto obj4= pool.Pop();
	//EXPECT_EQ(pool.Count(), 4);
	//auto obj5= pool.Pop();
	//EXPECT_EQ(pool.Count(), 3);
	//auto obj6= pool.Pop();
	//EXPECT_EQ(pool.Count(), 2);
	//auto obj7= pool.Pop();
	//EXPECT_EQ(pool.Count(), 1);
	//auto obj8 =pool.Pop();
	//*obj = 5;
	//EXPECT_EQ(pool.Count(), 10);
	//EXPECT_THROW(pool.Pop(), std::out_of_range);
	//EXPECT_EQ(*obj, 5);

}


TEST(ObjectPool, CanRelease)
{

	auto pool = ObjectPool<int>();

	pool.Push(std::make_unique<int>(50));

	auto obj = pool.Pop();
	auto obj1 = pool.Pop();
	pool.Pop();
	pool.Pop();
	pool.Pop();
	pool.Pop();
	pool.Pop();
	pool.Pop();
	pool.Pop();
	pool.Pop();

	*obj = 5;


	EXPECT_EQ(*obj, 5);


}


TEST(ObjectPool, RAII)
{
	auto pool = ObjectPool<int>();
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
//
//TEST(ObjectPool, RAIIAfterSwapConstructor)
//{
//	auto pool = ObjectPool<int>();
//	for (int i = 0; i < 10; i++)
//	{
//		pool.Push(std::make_unique<int>(i));
//	}
//
//	auto pool1(std::move(pool));
//	{
//		auto obj = pool1.Pop();
//		EXPECT_EQ(pool1.ActiveCount(), 9);
//	}
//	EXPECT_EQ(pool1.ActiveCount(), 10);
//}
//
//TEST(ObjectPool, RAIIAfterSwapAssignment)
//{
//	auto pool = ObjectPool<int>();
//	pool.Push(std::make_unique<int>(6));
//	pool.Pop();
//	/*for (int i = 0; i < 10; i++)
//	{
//		pool.Push(std::make_unique<int>(i));
//	}
//*/
//	auto pool1 = ObjectPool<int>();
//	
//
//	pool1 = std::move(pool);
//	for (int i = 0; i < 10; i++)
//	{
//		pool1.Push(std::make_unique<int>(i));
//	}
//	{
//		auto obj = pool1.Pop();
//		EXPECT_EQ(pool1.ActiveCount(), 9);
//	
//	}
//	EXPECT_EQ(pool1.ActiveCount(), 10);
//
//}