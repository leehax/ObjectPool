#pragma once
#include <cstdint>
#include <chrono>
#include <iostream>
class IPoolable
{
public:
	
	virtual void OnRemoveFromPool();
	virtual void OnAddToPool();
	virtual std::chrono::milliseconds TimeAlive();
	IPoolable() = default;
	virtual ~IPoolable(){}

	bool IsEqualTo(const IPoolable& other) const;
protected:
	std::chrono::time_point<std::chrono::high_resolution_clock> _tickOnCreate;

};

inline void IPoolable::OnRemoveFromPool()
{
	std::cout << "Removed IPoolable from pool\n";
	_tickOnCreate = std::chrono::high_resolution_clock::now();
}

inline void IPoolable::OnAddToPool()
{
	 std::cout << "Added IPoolable to Pool\n";
	 _tickOnCreate = std::chrono::time_point<std::chrono::high_resolution_clock>::min();
}

inline std::chrono::milliseconds IPoolable::TimeAlive()
{
	const auto currentTime = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - _tickOnCreate);
}

inline bool IPoolable::IsEqualTo(const IPoolable& other) const
{
	return  _tickOnCreate == other._tickOnCreate;
}

inline bool operator == (const IPoolable& lhs, const IPoolable& rhs)
{
	return lhs.IsEqualTo(rhs);
}
