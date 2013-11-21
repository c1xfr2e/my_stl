#pragma once

#include <stddef.h>

__NS_BEGIN

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

template <typename T, typename Category, 
typename Pointer = T*, typename Reference = T&, typename Distance = ptrdiff_t>
struct iterator 
{
	typedef Category	category;
	typedef T			value_type;
	typedef Distance	difference_type;
	typedef Pointer		pointer;
	typedef Reference	reference;
};

template <typename Iterator>
struct iterator_traits 
{
	typedef typename Iterator::category			  category;
	typedef typename Iterator::value_type		  value_type;
	typedef typename Iterator::difference_type    difference_type;
	typedef typename Iterator::pointer			  pointer;
	typedef typename Iterator::reference		  reference;
};

template <typename T>
struct iterator_traits<T*> 
{
	typedef random_access_iterator_tag category;
	typedef T                          value_type;
	typedef ptrdiff_t                  difference_type;
	typedef T*                         pointer;
	typedef T&						   reference;
};

template <typename T>
struct iterator_traits<const T*> 
{
	typedef random_access_iterator_tag category;
	typedef T                          value_type;
	typedef ptrdiff_t                  difference_type;
	typedef const T*                   pointer;
	typedef const T&                   reference;
};


template <typename I>
inline typename iterator_traits<I>::category 
iterator_category(const I&)
{
	typedef typename iterator_traits<I>::category c;
	return c();
}

template <typename I>
inline typename iterator_traits<I>::value_type* 
value_type(const I&)
{
	return static_cast<typename iterator_traits<I>::value_type*>(0);
}

template <typename I>
inline typename iterator_traits<I>::difference_type* 
distance_type(const I&)
{
	return static_cast<typename iterator_traits<I>::difference_type*>(0);
}

//! 以下是整组 distance 函数实现
//
template <typename II>
inline typename iterator_traits<II>::difference_type
distance_aux(II first, II last, input_iterator_tag)
{
	typename iterator_traits<II>::difference_type n = 0;
	for ( ; first != last; ++first) 
		++n;
	return n;
}

template <typename RAI>
inline typename iterator_traits<RAI>::difference_type
distance_aux(RAI first, RAI last, random_access_iterator_tag) 
{
	return last - first;
}

template <typename II>
inline typename iterator_traits<II>::difference_type 
distance(II first, II last) 
{
	typedef typename iterator_traits<II>::category c;
	return distance_aux(first, last, c());
}
//~

//! 以下是整组 advance 函数实现
//
template <typename II, typename Distance>
inline void advance_aux(II& i, Distance n, input_iterator_tag) 
{
	while (n--)
		++i;
}

template <typename BI, typename Distance>
inline void advance_aux(BI& i, Distance n, bidirectional_iterator_tag) 
{
	if (n >= 0)
		while (n--) ++i;
	else
		while (n++) --i;
}

template <typename RAI, typename Distance>
inline void advance_aux(RAI& i, Distance n, random_access_iterator_tag) 
{
	i += n;
}

template <typename II, typename Distance>
inline void advance(II& i, Distance n) 
{
	advance_aux(i, n, iterator_category(i));
}

//

template <class InputIterator, class Distance>
inline void __distance(InputIterator first, InputIterator last, Distance& n, input_iterator_tag)
{
	while (first != last)
	{
		++first;
		++n; 
	}
}

template <class RandomAccessIterator, class Distance>
inline void __distance(RandomAccessIterator first, RandomAccessIterator last, 
					   Distance& n, random_access_iterator_tag)
{
	n += last - first;
}

template <class InputIterator, class Distance>
inline void distance(InputIterator first, InputIterator last, Distance& n)
{
	__distance(first, last, n, iterator_category(first));
}

__NS_END