#pragma once
#include <cstdint>
#include <chrono>
class IPoolable
{
public:
	IPoolable()
	{
		_tickOnCreate = std::chrono::high_resolution_clock::now();
	};

	virtual std::chrono::milliseconds TimeAlive();

private:
	std::chrono::time_point<std::chrono::steady_clock> _tickOnCreate;

};

inline std::chrono::milliseconds IPoolable::TimeAlive()
{
	const auto currentTime = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - _tickOnCreate);
}
