#pragma once

#include <new.h>
#include "type_traits.h"
#include "iterator_base.h"
#include "algo_base.h"

__NS_BEGIN

template <class T1, class T2>
void construct(T1* p, const T2& v)
{
	new (p) T1(v); 
}

template <class T>
void construct(T* p) 
{
	new (p) T();
}

template <class ForwardIterator, class InputIterator>
ForwardIterator range_construct_aux(InputIterator first, InputIterator last, ForwardIterator result, false_type)
{
	while (first != last)
	{
		construct(&*result, *first);
		++result;
		++first;
	}
	return result;
}

template <class ForwardIterator, class InputIterator>
ForwardIterator range_construct_aux(InputIterator first, InputIterator last, ForwardIterator result, true_type)
{
	return MySTL::copy(first, last, result);
}

// construct uninitialized elements starting at 'result' with the value in range ['first','last')
template <class ForwardIterator, class InputIterator>
ForwardIterator range_construct(InputIterator first, InputIterator last, ForwardIterator result)
{
	typedef typename iterator_traits<ForwardIterator>::value_type value_type;
	typedef typename type_traits<value_type>::is_POD_type is_POD_type;
	return range_construct_aux(first, last, result, is_POD_type());		
}

template <class ForwardIterator, class Tp>
void fill_construct_aux(ForwardIterator first, ForwardIterator last, const Tp& value, false_type)
{
	while (first != last)
	{
		construct(&*first, value);
		++first;
	}
}

template <class ForwardIterator, class Tp>
void fill_construct_aux(ForwardIterator first, ForwardIterator last, const Tp& value, true_type)
{
	fill(first, last, value);
}

// construct uninitialized elements in range ['first','last') with 'value'
template <class ForwardIterator, class Tp>
void fill_construct(ForwardIterator first, ForwardIterator last, const Tp& value)
{
	typedef typename iterator_traits<ForwardIterator>::value_type value_type;
	typedef typename type_traits<value_type>::is_POD_type is_POD_type;
	fill_construct_aux(first, last, value, is_POD_type());
}

template <class ForwardIterator, class Tp, class Size>
ForwardIterator fill_construct_n_aux(ForwardIterator result, Size n, const Tp& value, false_type)
{
	while (n-- > 0)
	{
		construct(&*result, value);
		++result;
	}
	return result;
}

template <class ForwardIterator, class Tp, class Size>
ForwardIterator fill_construct_n_aux(ForwardIterator result, Size n, const Tp& value, true_type)
{
	return MySTL::fill_n(result, n, value);
}

// construct uninitialized elements starting at 'result' with 'n' 'value's
template <class ForwardIterator, class Tp, class Size>
ForwardIterator fill_construct_n(ForwardIterator result, Size n, const Tp& value)
{
	typedef typename iterator_traits<ForwardIterator>::value_type value_type;
	typedef typename type_traits<value_type>::is_POD_type is_POD_type;
	return fill_construct_n_aux(result, n, value, is_POD_type());
}

template <class T>
void destruct(T* p)
{
	p->~T();
}

template <class ForwardIterator>
void range_destruct_aux(ForwardIterator first, ForwardIterator last, false_type)
{	
	while (first != last)
	{
		destruct(&*first);
		++first;
	}
}

template <class ForwardIterator>
void range_destruct_aux(ForwardIterator first, ForwardIterator last, true_type)
{}

template <class ForwardIterator>
void range_destruct(ForwardIterator first, ForwardIterator last)
{
	typedef typename iterator_traits<ForwardIterator>::value_type value_type;
	typedef typename type_traits<value_type>::has_trivial_destructor has_trivial_destructor;
	range_destruct_aux(first, last, has_trivial_destructor());
}

__NS_END
