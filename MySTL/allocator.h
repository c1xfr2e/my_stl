#pragma once

#include "config.h"

__NS_BEGIN

class allocator
{
public:
	static void* allocate(size_t n)
	{
		return n == 0? 0 : ::operator new(n);
	}

	static void deallocate(void* p)
	{
		::operator delete(p);
	}

	static void deallocate(void* p, size_t)
	{
		::operator delete(p);
	}
};


template <class T, class Alloc = allocator>
class type_allocator
{
public:
	static T* allocate(void)
	{
		return (T*)Alloc::allocate(sizeof(T));
	}
	
	static T* allocate(size_t n)
	{
		return n == 0? 0 : (T*)Alloc::allocate(n * sizeof(T));
	}

	static void deallocate(T* p)
	{
		Alloc::deallocate(p);
	}

	static void deallocate(T* p, size_t)
	{
		Alloc::deallocate(p);
	}
};

__NS_END
