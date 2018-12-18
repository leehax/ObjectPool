#pragma once
#ifndef OBJECTPOOL_PRIV_H
#define OBJECTPOOL_PRIV_H
template <typename T, typename D>
std::unique_ptr<T, typename ObjectPool<T, D>::ReturnToPool> ObjectPool<T, D>::Pop()
{
	if (IsEmpty())
	{
		throw std::out_of_range("Pool Is Empty");
	}
	std::unique_ptr<T, ReturnToPool> ptr(_pool.top().release(),
		ReturnToPool(
			std::weak_ptr<ObjectPool<T, D>*>(_sharedThis)));

	ptr->OnRemoveFromPool();
	_pool.pop();
	return std::move(ptr);
}

template<typename T, typename D>
bool operator == (const ObjectPool<T, D>& lhs, const ObjectPool<T, D>& rhs)
{
	return lhs.IsEqualTo(rhs);
}

template <typename T, typename D>
void ObjectPool<T, D>::ReturnToPool::operator()(T* ptr) const
{
	if (auto pool_ptr = _weakPool.lock())
	{
		if ((*pool_ptr.get())->IsFull())
		{
			throw std::length_error("Pool Is Full");
		}
		try
		{
			ptr->OnAddToPool();
			(*pool_ptr.get())->_pool.push(std::unique_ptr<T>{ptr});
		}
		catch (const std::bad_alloc&){}
	}
	else
	{
		D{}(ptr); //if the pool doesn't exist anymore, delete using deleter D
	}
}

template <typename T, typename D>
size_t ObjectPool<T, D>::Size()
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

#endif