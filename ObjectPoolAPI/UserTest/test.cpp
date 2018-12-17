#include "pch.h"
#include "../ObjectPoolAPI/ObjectPool.h"
#include "../ObjectPoolAPI/IPoolable.h"

class TestPoolable : public IPoolable
{
public:
	TestPoolable()
	{
		std::cout << "Constructor called\n";
	}

	int GetName() const {
		return _name;
	}
	void SetName(const int name)
	{
		std::cout << "Name changed to " << name<<"\n";
		_name = name;
	}

private:
	int _name = 0;
	
};

void DoSomethingExpensive() //approx 20ms 
{
	for (int i = 0; i < 10000; i++)
	{
		for (int j = 0; j < 1000; j++) {

		}
	}
}

TEST(ObjectPool, KillsOldestPoolable)
{
	ObjectPool<TestPoolable> pool(5);
	std::vector<std::unique_ptr<TestPoolable, ObjectPool<TestPoolable>::Deleter>> activePool;

	for (int i = 0; i < 5; i++)
	{
		activePool.push_back(pool.Pop());
		activePool.back()->SetName(i);
		DoSomethingExpensive();
	}

	EXPECT_EQ(pool.Count(), 0);

	for (int i = 0; i < 5; i++)
	{
		EXPECT_EQ(activePool[i]->GetName(), i);
	}
	for(int i=0; i < 10; i ++)
	{
		for (int j = 0; j < 5; j++){

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
						std::cout << longest << '\n';
					}
				}
				if (longest < activePool.size()) {
					EXPECT_EQ(activePool[longest]->GetName(), j);
					activePool[longest].reset(nullptr);
					activePool.erase(activePool.begin() + longest);

				}
			}

			EXPECT_EQ(activePool.size(), 4);
			EXPECT_EQ(pool.Count(), 1);

			activePool.push_back(pool.Pop());
			DoSomethingExpensive();

		}
	}
}