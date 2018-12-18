#include "pch.h"
#include "../ObjectPoolAPI/ObjectPool.h"
#include "../ObjectPoolAPI/IPoolable.h"

class TestPoolable : public IPoolable
{
public:
	TestPoolable()
	{
	}

	int GetName() const {
		return _name;
	}
	void SetName(const int name)
	{
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

	EXPECT_EQ(pool.Size(), 0);

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
					}
				}
				if (longest < activePool.size()) {
					EXPECT_EQ(activePool[longest]->GetName(), j);
					activePool.erase(activePool.begin() + longest);
				}
			}

			EXPECT_EQ(activePool.size(), 4);
			EXPECT_EQ(pool.Size(), 1);

			activePool.push_back(pool.Pop());
			DoSomethingExpensive();

		}
	}
}

class Enemy : public IPoolable
{
public:
	Enemy()
	{
		_type = "Enemy";
	}
	Enemy(const std::string type)
	{
		_type = type;
	}

	void OnRemoveFromPool() override
	{
		_hp = 100;
	}
	void DecreaseHP(int amount)
	{
		_hp -= amount;
	}
	const int GetHP() const
	{
		return _hp;
	}

private:
	int _hp = 0;
	std::string _type = "";

};


TEST(EnemyPool, EnemiesReturnToPoolWhenDead)
{
	ObjectPool<Enemy> enemyPool(10);
	std::vector < std::unique_ptr<Enemy, ObjectPool<Enemy>::Deleter>> enemies;

	//void spawn enemies
	for(int i = 0; i<10; i++ )
	{
		enemies.push_back(enemyPool.Pop());
	}

	int gameRunning = 50;
	while (gameRunning > 0) {
		//void update 
		gameRunning--;
		int expectedPoolSize = enemyPool.Size();
		
		for (int i = 0; i < enemies.size(); i++)
		{
			enemies[i]->DecreaseHP(5);
			if (enemies[i]->GetHP() <= 0)
			{
				enemies.erase(enemies.begin() + i);
				expectedPoolSize++;
				EXPECT_EQ(enemyPool.Size(), expectedPoolSize);
			}
		}
		EXPECT_EQ(enemyPool.Size(), expectedPoolSize);

		if (enemies.size() == 0)
		{
			enemies.push_back(enemyPool.Pop());
		}
	}
	EXPECT_EQ(enemies.size(), 1);
	EXPECT_EQ(enemyPool.Size(), 9);
}


static int idAccum = 0;
class Bullet : public IPoolable
{
public:
	Bullet()
	{
		_id = idAccum++;
	};

	int GetID()
	{
		return _id;
	}
private:

	int _id;
};

class Player : public IPoolable
{
public:
	Player()
	{
		_maxBullets = 10;
		_bullets = new ObjectPool<Bullet>(_maxBullets);
	}
	~Player()
	{
		delete _bullets;
		_bullets = nullptr;
	}
	
	int Shoot()
	{
		DoSomethingExpensive(); //shoot cooldown
		if (_bullets->IsEmpty())
		{
			RecycleBullet();
		}
		_activeBullets.push_back(_bullets->Pop());
		return _activeBullets.back()->GetID();
	}
	void RecycleBullet()
	{
		std::chrono::milliseconds longestTime = std::chrono::milliseconds::min();
		unsigned int longest = _activeBullets.size() + 1;
		for (unsigned int i = 0; i < _activeBullets.size(); i++)
		{
			if (_activeBullets[i]->TimeAlive() > longestTime)
			{
				longestTime = _activeBullets[i]->TimeAlive();
				longest = i;
			}
		}
		if (longest < _activeBullets.size()) {
			_activeBullets.erase(_activeBullets.begin() + longest);
		}

	}
	ObjectPool<Bullet>* _bullets;
	std::vector< std::unique_ptr<Bullet, ObjectPool<Bullet>::Deleter>> _activeBullets;
private:
	int _maxBullets;
};

TEST(Player, OldestBulletIsRecycled)
{
	Player player; //has 10 bullets

	EXPECT_EQ(player.Shoot(), 9);
	EXPECT_EQ(player.Shoot(), 8);
	EXPECT_EQ(player.Shoot(), 7);
	EXPECT_EQ(player.Shoot(), 6);
	EXPECT_EQ(player.Shoot(), 5);
	EXPECT_EQ(player.Shoot(), 4);
	EXPECT_EQ(player.Shoot(), 3);
	EXPECT_EQ(player.Shoot(), 2);
	EXPECT_EQ(player.Shoot(), 1);
	EXPECT_EQ(player.Shoot(), 0);

	//10 bullets shot, next one should recycle the oldest one, aka 9
	EXPECT_EQ(player.Shoot(), 9);
	EXPECT_EQ(player.Shoot(), 8);
	EXPECT_EQ(player.Shoot(), 7);
	EXPECT_EQ(player.Shoot(), 6);
	EXPECT_EQ(player.Shoot(), 5);
	EXPECT_EQ(player.Shoot(), 4);
	EXPECT_EQ(player.Shoot(), 3);
	EXPECT_EQ(player.Shoot(), 2);
	EXPECT_EQ(player.Shoot(), 1);
	EXPECT_EQ(player.Shoot(), 0);

	//10 bullets shot, next one should recycle the oldest one, aka 9
	EXPECT_EQ(player.Shoot(), 9);
	EXPECT_EQ(player.Shoot(), 8);
	EXPECT_EQ(player.Shoot(), 7);
	EXPECT_EQ(player.Shoot(), 6);
	EXPECT_EQ(player.Shoot(), 5);
	EXPECT_EQ(player.Shoot(), 4);
	EXPECT_EQ(player.Shoot(), 3);
	EXPECT_EQ(player.Shoot(), 2);
	EXPECT_EQ(player.Shoot(), 1);
	EXPECT_EQ(player.Shoot(), 0);

}