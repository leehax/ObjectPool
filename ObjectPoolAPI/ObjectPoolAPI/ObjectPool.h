#pragma once
#include <array>
#include <list>

template<typename T, unsigned int POOL_SIZE =16>
class ObjectPool
{
public:

	enum HandleOverflow
	{
		KILL_NEW, //don't create a new object if our pool has no free slots
		KILL_EXISTING, //kill an existing object to allow creation of new, this only works if the pooled type implements ipoolable
		INCREASE_SIZE //increase size of pool if needed
	};

	ObjectPool()
	{
		_pool = {};
		_active = {};
		_size = POOL_SIZE;

	}
	//copy constructor
	ObjectPool(const ObjectPool& other) : _size(other._size), _pool(other._pool)
	{
		std::copy(other._pool.begin(), other._pool.end(), _pool);
	}

	//move constructor
	ObjectPool(ObjectPool&& other)
	{
		swap(*this, other);
	}

	//copy assignment
	ObjectPool& operator = (const ObjectPool& other)
	{
		if (&other == this)
		{
			return *this;
		}
		_size = other._size;
		_pool = {};
		std::copy(other._pool.begin(), other._pool.end(), _pool);
		return *this;
		static_assert(std::is_base_of<int,T>::value);
	}

	//move assignment
	ObjectPool& operator = (ObjectPool&& other)
	{
		swap(*this, other);
		return *this;
	}

	~ObjectPool()
	{
	
	}

	bool operator ==( const ObjectPool& other)
	{
		return _pool == other._pool && _active == other._active && _size == other._size;
	}
	size_t Count();
	unsigned int ActiveCount();
	T* GetObject();
	bool Release(T ptrToFree);
	bool IsFull();

private:
	unsigned int _size;
	std::array<T, POOL_SIZE> _pool;
	std::array<bool, POOL_SIZE> _active;
	friend void swap(ObjectPool & a, ObjectPool& b);

	

};



template <typename T, unsigned int POOL_SIZE>
size_t ObjectPool<T, POOL_SIZE>::Count()
{
	return _pool.size();
}

template <typename T, unsigned int POOL_SIZE>
unsigned int ObjectPool<T, POOL_SIZE>::ActiveCount()
{
	return std::count_if(_active.begin(), _active.end(), [](bool b) {return b; });
}

template <typename T, unsigned int POOL_SIZE>
 T* ObjectPool<T, POOL_SIZE>::GetObject() 
{
	
	for(int i = 0; i <_active.size(); i++)
	{
		if (!_active[i])
		{			
			_active[i] = true;
			return &_pool[i];
		}
	}
	return nullptr;
	
}

template <typename T, unsigned int POOL_SIZE>
bool ObjectPool<T, POOL_SIZE>::Release(T ptrToFree) //return true if the pointer was found and freed, else false
{
	for(int i = 0; i< _pool.size(); i++)
	{
		if(_pool[i]==ptrToFree)
		{
			_active[i] = false;
			return true;
		}
	}
	return false;
}

template <typename T, unsigned int POOL_SIZE>
bool ObjectPool<T, POOL_SIZE>::IsFull()
{
	return ActiveCount() == Count();
}

template<typename T, unsigned int POOL_SIZE = 16>
void swap(ObjectPool<T,POOL_SIZE>& a, ObjectPool<T,POOL_SIZE> b)
{
	std::swap(a._size, b._size);
	std::swap(a._pool, b._pool);
	std::swap(a._active, b._active);

}

