#pragma once
#include <utility>
#include <memory>
#include <stack>
#include <stdexcept>
#include <iostream>
#include <complex>
#include <vector>
#include "IPoolable.h"

template<typename T, typename D = std::default_delete<T>>
class ObjectPool
{
	static_assert(std::is_base_of<IPoolable, T>::value, "Poolable type must implement IPoolable");
	template<typename T, typename D = std::default_delete<T>>
	friend void swap(ObjectPool<T, D>& a, ObjectPool<T, D>& b);
	struct ReturnToPool
	{
		explicit ReturnToPool(std::weak_ptr<ObjectPool<T, D>*> pool) : _weakPool(pool) {}
		void operator()(T* ptr) const
		{
			if (auto pool_ptr = _weakPool.lock())
			{
				//(*pool_ptr.get())->Push(*ptr);
				if ((*pool_ptr.get())->IsFull()) //and not set to overflow = allow resize
				{
					throw std::length_error("Pool Is Full");
				}
				try
				{
					ptr->OnAddToPool();
					(*pool_ptr.get())->_pool.push(std::unique_ptr<T>{ptr});
				}
				catch (const std::bad_alloc&) {}
			}
			else
			{
				D{}(ptr);  //if the pool doesn't exist anymore, delete using deleter D
			}
		}
	private:
		std::weak_ptr<ObjectPool<T, D>*> _weakPool;
	};
public:
	using Deleter = ReturnToPool;
	template <class... Args>
	ObjectPool(int maxSize = 20, Args const&...args) :_sharedThis(std::make_shared<ObjectPool<T,D>*>(this))
	{
		
		_pool = {};
		_maxSize = std::abs(maxSize);
		std::cout << _sharedThis << '\n';
		for(unsigned int i = 0; i<_maxSize; i++)
		{
			_pool.push(std::make_unique<T>(args...));
		}
	}

	//disable copy and move semantics as they will break the return to pool deleter
	ObjectPool(const ObjectPool& other) = delete;
	ObjectPool(ObjectPool&& other) = delete;
	ObjectPool& operator = (const ObjectPool& other) = delete;
	ObjectPool& operator = (ObjectPool&& other) = delete;
	~ObjectPool() = default;
		
	size_t Count();
	std::unique_ptr<T, ReturnToPool> Pop();
	bool IsEqualTo(const ObjectPool& other) const;
private:
	bool IsFull();
	bool IsEmpty();

	unsigned int  _maxSize = 100;
	std::stack <std::unique_ptr<T>> _pool;
	std::shared_ptr<ObjectPool<T, D>*> _sharedThis;
};

template <typename T, typename D>
std::unique_ptr<T, typename ObjectPool<T, D>::ReturnToPool> ObjectPool<T, D>::Pop()
{
	if(IsEmpty())
	{
		throw std::out_of_range("Pool Is Empty");
	}	
	std::unique_ptr<T, ReturnToPool> ptr(_pool.top().release(),
									ReturnToPool(
									std::weak_ptr<ObjectPool<T,D>*>(_sharedThis)));

	ptr->OnRemoveFromPool();
	_pool.pop();
	return std::move(ptr);
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
