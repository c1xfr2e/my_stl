#pragma once

#include "type_traits.h"
#include "iterator_base.h"
#include "string.h"

__NS_BEGIN

template <class T>
const T& max(const T& a, const T& b) 
{
	return  a < b ? b : a;
}

template <class T>
const T& min(const T& a, const T& b) 
{
	return  a < b ? a : b;
}

template <class T>
void swap(T& a, T& b)
{
	T tmp = a;
	a = b;
	b = tmp;
}

template <class T>
T* copy_trivial(const T* first, const T* last, T* result)
{
	memmove(result, first, sizeof(T) * (last - first));
	return result + (last - first);
}

template <class ForwardIterator, class InputIterator, class DiffType>
ForwardIterator copy_nontrivial(InputIterator first, InputIterator last, ForwardIterator result, 
								input_iterator_tag, DiffType*)
{
	while (first != last)
	{
		*result = *first;
		++result;
		++first;
	}
	return result;
}


template <class ForwardIterator, class RandomAccessIterator, class DiffType>
ForwardIterator copy_nontrivial(RandomAccessIterator first, RandomAccessIterator last, ForwardIterator result, 
								random_access_iterator_tag, DiffType*)
{
	DiffType n = last - first;
	while (n-- > 0)
	{
		*result = *first;
		++result;
		++first;
	}
	return result;
}

template <class ForwardIterator, class InputIterator>
ForwardIterator copy_aux(InputIterator first, InputIterator last, ForwardIterator result, false_type)
{
	typedef typename iterator_traits<InputIterator>::category iterator_category;
	typedef typename iterator_traits<InputIterator>::difference_type difference_type;
	return copy_nontrivial(first, last, result, iterator_category(), (difference_type*)0);
}

template <class ForwardIterator, class InputIterator>
ForwardIterator copy_aux(InputIterator first, InputIterator last, ForwardIterator result, true_type)
{
	typedef typename iterator_traits<InputIterator>::category iterator_category;
	typedef typename iterator_traits<InputIterator>::difference_type difference_type;
	return copy_nontrivial(first, last, result, iterator_category(), (difference_type*)0);
}

template <class T>
T* copy_aux(const T* first, const T* last, T* result, true_type)
{
	return copy_trivial(first, last, result);
};

template <class T>
T* copy_aux(T* first, T* last, T* result, true_type)
{
	return copy_trivial(first, last, result);
};

template <class ForwardIterator, class InputIterator>
ForwardIterator copy(InputIterator first, InputIterator last, ForwardIterator result)
{
	typedef typename iterator_traits<ForwardIterator>::value_type value_type;
	typedef typename type_traits<value_type>::has_trivial_assignment_operator has_trivial_assignment_operator;
	return copy_aux(first, last, result, has_trivial_assignment_operator());
};

template <class BI1, class BI2, class DiffType>
BI1 copy_backward_nontrivial(BI2 first, BI2 last, BI1 result, bidirectional_iterator_tag, DiffType*)
{
	while (last != first)
	{
		--last;
		--result;
		*result = *last;
	}
	return result;
}

template <class BI, class RAI, class DiffType>
BI copy_backward_nontrivial(RAI first, RAI last, BI result, random_access_iterator_tag, DiffType*)
{
	DiffType n = last - first;
	while (n-- > 0)
	{
		--last;
		--result;
		*result = *last;
	}
	return result;
}

template <class BI1, class BI2>
BI1 copy_backward_aux(BI2 first, BI2 last, BI1 result, false_type)
{
	typedef typename iterator_traits<BI2>::category iterator_category;
	typedef typename iterator_traits<BI2>::difference_type difference_type;
	return copy_backward_nontrivial(first, last, result, iterator_category(), (difference_type*)0);
}

template <class BI1, class BI2>
BI1 copy_backward_aux(BI2 first, BI2 last, BI1 result, true_type)
{
	typedef typename iterator_traits<BI2>::category iterator_category;
	typedef typename iterator_traits<BI2>::difference_type difference_type;
	return copy_backward_nontrivial(first, last, result, iterator_category(), (difference_type*)0);
}

template <class T>
T* copy_backward_aux(const T* first, const T* last, T* result, true_type)
{
	ptrdiff_t n = last - first;
	memmove(result - n, first, sizeof(T) * n);
	return result - n;
};

template <class T>
T* copy_backward_aux(T* first, T* last, T* result, true_type)
{
	ptrdiff_t n = last - first;
	memmove(result - n, first, sizeof(T) * n);
	return result - n;
};

template <class BI1, class BI2>
BI1 copy_backward(BI2 first, BI2 last, BI1 result)
{
	typedef typename iterator_traits<BI1>::value_type value_type;
	typedef typename type_traits<value_type>::has_trivial_assignment_operator has_trivial_assignment_operator;
	return copy_backward_aux(first, last, result, has_trivial_assignment_operator());
}

template <class ForwardIterator, class Tp>
void fill(ForwardIterator first, ForwardIterator last, const Tp& val)
{
	for ( ; first != last; ++first)
		*first = val;
}

template <class ForwardIterator, class Tp, class Size>
ForwardIterator fill_n(ForwardIterator result, Size n, const Tp& val)
{
	for ( ; n > 0; --n)
		*result++ = val;
	return result;
}

__NS_END