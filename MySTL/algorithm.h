#pragma once

#include "algo_base.h"

__NS_BEGIN

template <class ForwardIter, class Tp, class Distance>
ForwardIter __lower_bound(ForwardIter first, ForwardIter last, const Tp& val, Distance*) 
{
	Distance len = distance(first, last);
	Distance half;
	ForwardIter middle;
	while (len > 0)
	{
		half = len >> 1;
		middle = first;
		advance(middle, half);
		if (*middle < val) 
		{
			first = middle;
			++first;
			len = len - half - 1;
		}
		else
			len = half;
	}
	return first;
}

template <class ForwardIter, class Tp>
inline ForwardIter lower_bound(ForwardIter first, ForwardIter last, const Tp& val) 
{
	return __lower_bound(first, last, val, distance_type(first));
}

template <class ForwardIter, class Tp, class Cmp, class Distance>
ForwardIter __lower_bound(ForwardIter first, ForwardIter last,
						   const Tp& val, Cmp cmp, Distance*)
{
	Distance len = 0;
	distance(first, last, len);
	Distance half;
	ForwardIter middle;

	while (len > 0) 
	{
		half = len >> 1;
		middle = first;
		advance(middle, half);
		if (cmp(*middle, val)) 
		{
			first = middle;
			++first;
			len = len - half - 1;
		}
		else
			len = half;
	}
	return first;
}

template <class ForwardIter, class Tp, class Cmp>
inline ForwardIter lower_bound(ForwardIter first, ForwardIter last, const Tp& val, Cmp cmp) 
{
	return __lower_bound(first, last, val, cmp, distance_type(first));
}

template <class RAI, class Tp>
void unguarded_linear_insert(RAI last, Tp value)
{
    RAI next = last;
    --next;
    while (value < *next)
    {
        *last = *next;
        last = next;
        --next;
    }
    *last = value;
}

template <class RAI>
void linear_insert(RAI first, RAI cur)
{
    if (*cur < *first)
    {
        typename iterator_traits<RAI>::value_type tmp = *cur;
        copy_backward(cur + 1, first, cur);
        *first = tmp;
    }
    else
    {
        unguarded_linear_insert(cur, *cur);
    }
}

template <class RAI>
void insert_sort(RAI first, RAI last)
{
    if (!(first < last))
        return;

    for (RAI cur = first + 1; cur != last; ++cur)
    {
        linear_insert(first, cur);
    }
}

__NS_END
