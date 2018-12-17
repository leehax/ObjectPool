#pragma once
#include <utility>
#include <memory>
#include <stack>
#include <stdexcept>
#include <iostream>
#include <complex>
#include <vector>
#include "IPoolable.h"
enum HandleOverflow
{
	KILL_NEW, //don't create a new object if our pool has no free slots
	KILL_EXISTING, //kill an existing object to allow creation of new, this only works if the pooled type implements ipoolable
	INCREASE_SIZE //increase size of pool if needed
};

template<typename T, typename D = std::default_delete<T>>
class ObjectPool
{

	static_assert(std::is_base_of<IPoolable, T>::value, "Must implement IPoolable");
	template<typename T, typename D = std::default_delete<T>>
	friend void swap(ObjectPool<T, D>& a, ObjectPool<T, D>& b);

public:
	
	template <class... Args>
	ObjectPool(int maxSize = 20, Args const&...args) :_sharedThis(std::make_shared<ObjectPool<T,D>*>(this))
	{
		
		_pool = {};
		_maxSize = std::abs(maxSize);
		std::cout << _sharedThis << '\n';
		for(unsigned int i = 0; i<_maxSize; i++)
		{
			_pool.push(T(args...));
		}
	}

	//disable copy and move semantics as they will break the return to pool deleter
	ObjectPool(const ObjectPool& other) = delete;
	ObjectPool(ObjectPool&& other) = delete;
	ObjectPool& operator = (const ObjectPool& other) = delete;
	ObjectPool& operator = (ObjectPool&& other) = delete;

	struct ReturnToPool
	{
		explicit ReturnToPool(std::weak_ptr<ObjectPool<T, D>*> pool) : _weakPool(pool) {}
		void operator()(T* ptr) const
		{
			if (auto pool_ptr = _weakPool.lock())
			{
				(*pool_ptr.get())->Push(*ptr);
			}
			else
			{
				D{}(ptr);  //if the pool doesn't exist anymore, delete using deleter D
			}
		}
	private:
		std::weak_ptr<ObjectPool<T, D>*> _weakPool;
	};

	~ObjectPool() = default;

	size_t Count();
	unsigned int ActiveCount();
	std::unique_ptr<T, ReturnToPool> Pop();

	void Push(T& obj);
	//void Push(std::unique_ptr<T, D> ptr);


	bool IsFull();
	bool IsEmpty();
	bool IsEqualTo(const ObjectPool& other) const;
	std::shared_ptr<ObjectPool<T, D>*> _sharedThis;
private:
	
	unsigned int  _maxSize = 100;
	std::stack <T> _pool;
	unsigned int _activeCount = 0;

};

template <typename T, typename D>
std::unique_ptr<T, typename ObjectPool<T, D>::ReturnToPool> ObjectPool<T, D>::Pop()
{
	if(IsEmpty())
	{
		throw std::out_of_range("Pool Is Empty");
	}
	
	
	std::unique_ptr<T, ReturnToPool> ptr(&_pool.top(),
									ReturnToPool(
									std::weak_ptr<ObjectPool<T,D>*>(_sharedThis)));

	ptr->OnRemoveFromPool();

	_pool.pop();
	
	return std::move(ptr);
}

template <typename T, typename D>
void ObjectPool<T, D>::Push(T& obj)
{
	if(IsFull()) //and not set to overflow = allow resize
	{
		throw std::length_error("Pool Is Full");
		
	}
	try
	{
		obj.OnAddToPool();
		_pool.push(obj);
	}
	catch (const std::bad_alloc&) {}
}


template<typename T, typename D>
bool operator == (const ObjectPool<T, D>& lhs, const ObjectPool<T, D>& rhs)
{
	return lhs.IsEqualTo(rhs);
}

template <typename T,  typename D>
size_t ObjectPool<T, D>::Count()
{
	return _pool.size();
}

template <typename T, typename D>
unsigned int ObjectPool<T, D>::ActiveCount()
{
	return _activeCount;
}

template <typename T, typename D>
bool ObjectPool<T, D>::IsFull()
{
	return _pool.size() == _maxSize;
}

template <typename T, typename D>
bool ObjectPool<T, D>::IsEmpty()
{
	return _pool.empty();
}

template <typename T, typename D>
bool ObjectPool<T, D>::IsEqualTo(const ObjectPool& other) const
{
	return _pool == other._pool && _maxSize == other._maxSize;
}


template<typename T, typename D>
void swap(ObjectPool<T, D>& a, ObjectPool<T, D>& b)
{
	std::swap(a._maxSize, b._maxSize);
	std::swap(a._pool, b._pool);
	std::swap(a._sharedThis, b._sharedThis);
	std::swap(a._activeCount, b._activeCount);
}
