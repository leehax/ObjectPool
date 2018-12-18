#pragma once
#include <chrono>
class IPoolable
{
public:
	IPoolable() = default;
	virtual ~IPoolable() = default;
	virtual void OnRemoveFromPool();
	virtual void OnAddToPool();
	virtual std::chrono::milliseconds TimeAlive();
	bool IsEqualTo(const IPoolable& other) const;
protected:
	std::chrono::time_point<std::chrono::high_resolution_clock> _tickOnCreate=std::chrono::time_point<std::chrono::high_resolution_clock>::min();;

};
inline void IPoolable::OnRemoveFromPool()
{
	_tickOnCreate = std::chrono::high_resolution_clock::now();
}

inline void IPoolable::OnAddToPool()
{
	_tickOnCreate = std::chrono::time_point<std::chrono::high_resolution_clock>::min();
}

inline std::chrono::milliseconds IPoolable::TimeAlive()
{
	const auto currentTime = std::chrono::high_resolution_clock::now();
	if (_tickOnCreate > std::chrono::time_point<std::chrono::high_resolution_clock>::min()) {
		return std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - _tickOnCreate);
	}
	return std::chrono::duration_cast<std::chrono::milliseconds>(_tickOnCreate - _tickOnCreate);
}

inline bool IPoolable::IsEqualTo(const IPoolable& other) const
{
	return  _tickOnCreate == other._tickOnCreate;
}

inline bool operator == (const IPoolable& lhs, const IPoolable& rhs)
{
	return lhs.IsEqualTo(rhs);
}

