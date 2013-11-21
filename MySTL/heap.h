#pragma once 

#include "algo_base.h"
#include "allocator.h"
#include "initialize.h"

__NS_BEGIN

template <class RAI, class DiffType, class Tp, class Compare>
void heap_sink(RAI start, DiffType hole, DiffType len, Tp new_value, Compare cmp)
{
	DiffType child = hole * 2 + 1;
	while (child < len)
	{
		if (child + 1 < len && cmp(*(start + child), *(start + child + 1)))
			++child;
		if (cmp(new_value, *(start + child)))
			*(start + hole) = *(start + child); 
		else
			break;
		hole = child;
		child = hole * 2 + 1;
	}
	*(start + hole) = new_value;
}

template <class RAI, class DiffType, class Tp, class Compare>
void heap_swim(RAI start, DiffType top, DiffType hole, Tp hole_value, Compare cmp)
{
	DiffType parent = (hole - 1) / 2;
	while (hole > top)
	{
		if (!cmp(*(start + parent), hole_value))
			break;
		*(start + hole) = *(start + parent);
		hole = parent;
		parent = (hole - 1) / 2;
	}
	*(start + hole) = hole_value;
}

template <class RAI, class Compare>
void pop_heap(RAI first, RAI last, Compare cmp)
{
	typedef typename iterator_traits<RAI>::value_type value_type;
	value_type tmp = *first;
	heap_sink(first, 0, last - first, *(last - 1), cmp);
	*(last - 1) = tmp;
}

template <class RAI, class Compare>
void push_heap(RAI first, RAI last, Compare cmp)
{
	heap_swim(first, 0, last - first - 1, *(last - 1), cmp);
}

template <class RAI, class Compare>
void make_heap(RAI first, RAI last, Compare cmp)
{
	if (last - first < 2)
		return;
	typedef typename iterator_traits<RAI>::difference_type DiffType;
	DiffType len = last - first;
	DiffType hole = (len - 2) / 2;
	while (hole >= 0)
	{
		heap_sink(first, hole, len, *(first + hole), cmp);
		--hole;
	}
}

template <class RAI, class Compare>
void heap_sort(RAI first, RAI last, Compare cmp)
{
	make_heap(first, last, cmp);
	while (last - first > 1)
	{
		pop_heap(first, last, cmp);
		--last;
	}
}

__NS_END
