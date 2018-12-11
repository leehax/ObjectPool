class ObjectPool
{
	ObjectPool() {}

public:
	void PopulatePool(int val, size_t amount);
	int* GetInactive();
};


void SpawnEnemies()
{
	
}

void main(int* argc, int** argv)
{
	ObjectPool* _pool =  new ObjectPool();

	_pool->PopulatePool(2, 15);

	_pool->GetInactive();
	
}