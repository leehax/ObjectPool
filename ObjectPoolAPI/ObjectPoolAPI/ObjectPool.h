#pragma once
#include <utility>
#include <memory>
#include <stack>
#include <stdexcept>
#include <iostream>
template<typename T, typename D = std::default_delete<T>>
class ObjectPool
{

private :
	template<typename T, typename D = std::default_delete<T>>
	friend void swap(ObjectPool<T, D>& a, ObjectPool<T, D>& b);

	friend struct ReturnToPool;
	
	struct ReturnToPool
	{
		explicit ReturnToPool(std::weak_ptr<ObjectPool<T,D>*> pool): _weakPool(pool){}
		void operator()(T* ptr) const
		{
			
			if(auto pool_ptr=_weakPool.lock()) 
			{
				std::cout << "Return to Pool\n";
				std::cout << ((ObjectPool*)*pool_ptr.get())->_sharedThis << '\n';
			
				(*pool_ptr.get())->Push(std::unique_ptr<T, D>{ptr});
				//--(*pool_ptr.get())->_activeCount;
			}
			else
			{
				std::cout << "Pool is Dead\n";
				D{}(ptr);  //if the pool doesnt exist anymore, delete using deleter D
			}
		}
		

	private:
		std::weak_ptr<ObjectPool<T, D>*> _weakPool;
	};


public:

	enum HandleOverflow
	{
		KILL_NEW, //don't create a new object if our pool has no free slots
		KILL_EXISTING, //kill an existing object to allow creation of new, this only works if the pooled type implements ipoolable
		INCREASE_SIZE //increase size of pool if needed
	};

	ObjectPool(size_t maxSize = 100) :_sharedThis(std::make_shared<ObjectPool<T,D>*>(this))
	{
		_pool = {};
		_maxSize = maxSize;
		std::cout << _sharedThis << '\n';
	}

	//disable copy and move semantics
	ObjectPool(const ObjectPool& other) = delete;
	ObjectPool(ObjectPool&& other) = delete;
	ObjectPool& operator = (const ObjectPool& other) = delete;
	ObjectPool& operator = (ObjectPool&& other) = delete;

	//disable copy constructor
	//ObjectPool(const ObjectPool& other) = delete;


	////move constructor
	//ObjectPool(ObjectPool&& other):_sharedThis(other._sharedThis)
	//{
	//	swap(*this, other);
	//}

	////disable copy assignment
	//ObjectPool& operator = (const ObjectPool& other) = delete;


	////move assignment
	//ObjectPool& operator = (ObjectPool&& other)
	//{
	//	//auto tmp = _sharedThis;
	//	_sharedThis = other._sharedThis;
	////	other._sharedThis = tmp;
	//	swap(*this, other);
	//	return *this;
	//}

	~ObjectPool() = default;

	size_t Count();
	unsigned int ActiveCount();
	std::unique_ptr<T, ReturnToPool> Pop();
	void Push(std::unique_ptr<T, D> ptr);


	bool IsFull();
	bool IsEmpty();
	bool IsEqualTo(const ObjectPool& other) const;
	std::shared_ptr<ObjectPool<T, D>*> _sharedThis;
private:
	
	size_t _maxSize = 100;
	std::stack <std::unique_ptr<T,D>> _pool;
	size_t _activeCount = 0;
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

	_pool.pop();
	return std::move(ptr);
}

template <typename T, typename D>
void ObjectPool<T, D>::Push(std::unique_ptr<T, D> ptr)
{
	if(IsFull()) //and not set to overflow = allow resize
	{
		throw std::length_error("Pool Is Full");
	}
	try
	{	_pool.push(std::move(ptr));
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
//	return std::count_if(_active.begin(), _active.end(), [](bool b) {return b; });
}

//template <typename T, typename D>
// T* const ObjectPool<T, D>::Pop()
//{
//	
//	for(int i = 0; i <_active.size(); i++)
//	{
//		if (!_active[i])
//		{			
//			_active[i] = true;
//			return &_pool[i];
//		}
//	}
//	return nullptr;
//	
//}
//


//template <typename T, typename D>
//bool ObjectPool<T,D>::Release(T* ptrToFree) //return true if the pointer was found and freed, else false
//{
//	for(int i = 0; i< _pool.size(); i++)
//	{
//		if(&_pool[i]==ptrToFree)
//		{
//			_active[i] = false;
//			return true;
//		}
//	}
//	throw std::invalid_argument("Couldn't release the pointer");
//	return false;
//}

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

	//size_t size = a._pool.size();
	//for ( int i=0; i<size; i++)
	//{
	//	a._pool.pop();
	//}
	////a._pool.swap(b._pool);

	//size_t sizeB = b._pool.size();
	//for(int i=0; i<size;i++)
	//{
	//	std::unique_ptr < T,D > ptr(b._pool.top().release());
	//	b._pool.pop();
	//	a._pool.push(std::move(ptr));
	//}

	
	//a._sharedThis.swap(b._sharedThis);
	
	//std::swap(a._active, b._active);

}
