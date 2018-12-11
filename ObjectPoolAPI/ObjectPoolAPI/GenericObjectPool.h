#pragma once

template <typename T>
class GenericObjectPool
{
public:
	GenericObjectPool();
	~GenericObjectPool();

	void PopulatePool(T);

private:

	size_t _size;

};

