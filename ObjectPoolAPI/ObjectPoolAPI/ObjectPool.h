#pragma once

#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H
#include <memory>
#include <stack>
#include <stdexcept>
#include <complex>
#include "IPoolable.h"

//Pool holding pointers to type T with the deleter D
//type D defaults to the default deleter of type T
//this can be overwritten with another custom deleter if wished
template<typename T, typename D = std::default_delete<T>>
class ObjectPool
{
	static_assert(std::is_base_of<IPoolable, T>::value, "Poolable type must implement IPoolable");

	//Custom deleter that returns the objects to the pool 
	struct ReturnToPool
	{
		explicit ReturnToPool(std::weak_ptr<ObjectPool<T, D>*> weakPool) : _weakPool(std::move(weakPool)) {}
		void operator()(T* ptr) const;
	private:
		std::weak_ptr<ObjectPool<T, D>*> _weakPool;
	};
public:
	using Deleter = ReturnToPool;

	//Constructs an object pool of type T with size specified in arguments
	//Will fill the pool with pointers of type T
	//type T is constructed with args passed into this constructor
	template <class... Args>
	ObjectPool(int maxSize = 20, Args const&...args) :_sharedThis(std::make_shared<ObjectPool<T,D>*>(this))
	{		
		_pool = {};
		_maxSize = std::abs(maxSize);
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

	//Returns the amount of free objects in the pool
	size_t Size();
	//Returns the pointer on top of the pool, throws std::out_of_range is pool is empty
	std::unique_ptr<T, ReturnToPool> Pop();
	//Returns true if equal, due to pointer equality and copying of object pools being disabled this will never return true
	bool IsEqualTo(const ObjectPool& other) const;
	//Returns true if the pool size equals its max size
	bool IsFull();
	//Returns true if the pool has no free objects
	bool IsEmpty();
private:
	
	unsigned int  _maxSize = 100;
	std::stack <std::unique_ptr<T>> _pool;
	std::shared_ptr<ObjectPool<T, D>*> _sharedThis;
};

#include "ObjectPool_priv.h"
#endif