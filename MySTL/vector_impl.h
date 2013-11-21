#pragma once

template <class T, class Alloc>
vector<T, Alloc>::vector(void)
	:m_start(0), m_finish(0), m_end_of_storage(0)
{}

template <class T, class Alloc>
vector<T, Alloc>::vector(size_type n, const T& x)
	:m_start(0), m_finish(0), m_end_of_storage(0)
{
	__fill_initialize(n, x);
}

template <class T, class Alloc>
vector<T, Alloc>::vector(size_type n)
	:m_start(0), m_finish(0), m_end_of_storage(0)
{
	__fill_initialize(n, T());
}

template <class T, class Alloc>
template <class InputIterator>
vector<T, Alloc>::vector(InputIterator first, InputIterator last)
{
	typedef typename is_integer<InputIterator>::integer is_int;
	__initialize_aux(first, last, is_int());
}

template <class T, class Alloc>
vector<T, Alloc>::vector(const vector<T, Alloc>& other)
{
	__range_initialize(other.begin(), other.end(), forward_iterator_tag());
}
	
template <class T, class Alloc>
vector<T, Alloc>::~vector()
{
	erase(m_start, m_finish);
	Alloc::deallocate(m_start, m_end_of_storage - m_start);
}

template <class T, class Alloc>
typename vector<T, Alloc>::iterator vector<T, Alloc>::begin() 
{
	return m_start; 
}

template <class T, class Alloc>
typename vector<T, Alloc>::const_iterator vector<T, Alloc>::begin() const 
{
	return m_start; 
}

template <class T, class Alloc>
typename vector<T, Alloc>::iterator vector<T, Alloc>::end() 
{
	return m_finish; 
}

template <class T, class Alloc>
typename vector<T, Alloc>::const_iterator vector<T, Alloc>::end() const 
{
	return m_finish; 
}

template <class T, class Alloc>
typename vector<T, Alloc>::size_type vector<T, Alloc>::size() const 
{
	return m_finish - m_start; 
}

template <class T, class Alloc>
typename vector<T, Alloc>::size_type vector<T, Alloc>::capacity() const 
{
	return m_end_of_storage - m_start; 
}

template <class T, class Alloc>
bool vector<T, Alloc>::empty() const 
{
	return m_start == m_finish; 
}

template <class T, class Alloc>
typename vector<T, Alloc>::reference vector<T, Alloc>::front()
{
	return *begin();
}

template <class T, class Alloc>
typename vector<T, Alloc>::const_reference vector<T, Alloc>::front() const
{
	return *begin();
}


template <class T, class Alloc>
void vector<T, Alloc>::__fill_initialize(size_type n, const T& x)
{
	m_start = Alloc::allocate(n);
	m_end_of_storage = m_start + n;
	m_finish = fill_construct_n(m_start, n, x);
}

template <class T, class Alloc>
template <class ForwardIterator>
void vector<T, Alloc>::__range_initialize(ForwardIterator first, ForwardIterator last, input_iterator_tag)
{
	for ( ; first != last; ++first)
		push_back(*first);
}	

template <class T, class Alloc>
template <class ForwardIterator>
void vector<T, Alloc>::__range_initialize(ForwardIterator first, ForwardIterator last, forward_iterator_tag)
{
	size_type n = distance(first, last);
	m_start = Alloc::allocate(n);
	m_end_of_storage = m_start + n;
	m_finish = range_construct(first, last, m_start);
}

template <class T, class Alloc>
template <class InputIterator>
void vector<T, Alloc>::__initialize_aux(InputIterator first, InputIterator last, true_type)
{
	__fill_initialize(first, last);
}

template <class T, class Alloc>
template <class InputIterator>
void vector<T, Alloc>::__initialize_aux(InputIterator first, InputIterator last, false_type)
{
	__range_initialize(first, last, iterator_category(first));
}

template <class T, class Alloc>
void vector<T, Alloc>::insert(iterator pos, const T& x)
{
	if (m_finish != m_end_of_storage)
	{
		if (pos == m_finish)
		{
			construct(m_finish, x);
			++m_finish;
		}
		else
		{
			construct(m_finish , *(m_finish - 1));
			++m_finish;
			T x_copy = x;
			copy_backward(pos, m_finish - 2, m_finish - 1);
			*pos = x_copy;
		}
	}
	else
	{
		size_type new_sz = (size() == 0) ? 1 : 2 * size();
		iterator new_start = Alloc::allocate(new_sz);
		iterator new_finish = range_construct(m_start, pos, new_start);
		construct(new_finish, x);
		++new_finish;
		new_finish = range_construct(pos, m_finish, new_finish);
		range_destruct(m_start, m_finish);
		Alloc::deallocate(m_start, m_finish - m_start);
		m_start = new_start;
		m_finish = new_finish;
		m_end_of_storage = new_start + new_sz;
	}
}

template <class T, class Alloc>
void vector<T, Alloc>::push_back(const T& x)
{
	insert(end(), x);
}

template <class T, class Alloc>
void vector<T, Alloc>::push_front(const T& x)
{
	insert(begin(), x);
}

template <class T, class Alloc>
void vector<T, Alloc>::__fill_insert(iterator pos, size_type n, const T& x)
{
	if (n == 0)
		return;
	if (size_type(m_end_of_storage - m_finish) >= n)
	{
		T x_copy = x;
		size_type elems_after = m_finish - pos;
		if (elems_after >= n)
		{
			range_construct(m_finish - n, m_finish, m_finish);
			MySTL::copy_backward(pos, m_finish - n, m_finish);
			MySTL::fill_n(pos, n, x_copy);
			m_finish += n;	
		}
		else
		{
			iterator new_finish = fill_construct_n(m_finish, n - elems_after, x_copy);
			new_finish = range_construct(pos, m_finish, new_finish);
			MySTL::fill_n(pos, elems_after, x_copy);
			m_finish = new_finish;
		}
	}
	else
	{
		size_type old_sz = size();
		size_type new_sz = old_sz + max(old_sz, n);
		iterator new_start = Alloc::allocate(new_sz);
		iterator new_finish = range_construct(m_start, pos, new_start);
		new_finish = fill_construct_n(new_finish, n, x);
		new_finish = range_construct(pos, m_finish, new_finish);
		range_destruct(m_start, m_finish);
		Alloc::deallocate(m_start, m_end_of_storage - m_start);
		m_start = new_start;
		m_finish = new_finish;
		m_end_of_storage = m_start + new_sz;
	}
}

template <class T, class Alloc>
void vector<T, Alloc>::insert(iterator pos, size_type n, const T& x)
{
	__fill_insert(pos, n, x);
}

template <class T, class Alloc>
template <class ForwardIterator>
void vector<T, Alloc>::__range_insert(iterator pos, ForwardIterator first, ForwardIterator last, input_iterator_tag)
{
	for ( ; first != last; ++first)
	{
		insert(pos, *first);
		++pos;
	}
}	

template <class T, class Alloc>
template <class ForwardIterator>
void vector<T, Alloc>::__range_insert(iterator pos, ForwardIterator first, ForwardIterator last, forward_iterator_tag)
{
	if (first == last)
		return;
	size_type n = distance(first, last);
	if (size_type(m_end_of_storage - m_finish) >= n)
	{
		size_type elems_after = m_finish - pos;
		if (elems_after >= n)
		{
			range_construct(m_finish - n, m_finish, m_finish);
			copy_backward(pos, m_finish - n, m_finish);
			m_finish += n;	
			copy(first, last, pos);
		}
		else
		{
			ForwardIterator mid = first;
			advance(mid, elems_after);
			iterator new_finish = range_construct(mid, last, m_finish);
			new_finish = range_construct(pos, m_finish, new_finish);
			m_finish = new_finish;
			copy(first, mid, pos);
		}
	}
	else
	{
		size_type old_sz = size();
		size_type new_sz = old_sz + max(old_sz, n);
		iterator new_start = Alloc::allocate(new_sz);
		iterator new_finish = range_construct(m_start, pos, new_start);
		new_finish = range_construct(first, last, new_finish);
		new_finish = range_construct(pos, m_finish, new_finish);
		range_destruct(m_start, m_finish);
		Alloc::deallocate(m_start, m_end_of_storage - m_start);
		m_start = new_start;
		m_finish = new_finish;
		m_end_of_storage = m_start + new_sz;
	}
}

template <class T, class Alloc>
template <class InputIterator>
void vector<T, Alloc>::__insert_aux(iterator pos, InputIterator first, InputIterator last, true_type)
{
	__fill_insert(pos, first, last);
}

template <class T, class Alloc>
template <class InputIterator>
void vector<T, Alloc>::__insert_aux(iterator pos, InputIterator first, InputIterator last, false_type)
{
	__range_insert(pos, first, last, iterator_category(first));
}

template <class T, class Alloc>
template <class InputIterator>
void vector<T, Alloc>::insert(iterator pos, InputIterator first, InputIterator last)
{
	typedef typename is_integer<InputIterator>::integer is_int;
	__insert_aux(pos, first, last, is_int());
}

template <class T, class Alloc>
typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator pos)
{
	if (pos != m_finish - 1)
		copy(pos + 1, m_finish, pos);
	--m_finish;
	destruct(m_finish);
	return pos;
}

template <class T, class Alloc>
typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator first, iterator last)
{
	iterator new_finish = MySTL::copy(last, m_finish, first);
	range_destruct(new_finish, m_finish);
	m_finish = new_finish;
	return first;
}

template <class T, class Alloc>
void vector<T, Alloc>::pop_back()
{
	erase(m_finish - 1);
}

template <class T, class Alloc>
void vector<T, Alloc>::clear()
{
	erase(first, last);
}

template <class T, class Alloc>
void vector<T, Alloc>::resize(size_type new_sz, const T& x)
{
	if (new_sz > capacity())
	{
		iterator new_start = Alloc::allocate(new_sz);
		iterator i = range_construct(m_start, m_finish, new_start);
		fill_construct(i, new_start + new_sz, x);
		range_destruct(m_start, m_finish);
		Alloc::deallocate(m_start, m_end_of_storage - m_start);
		m_start = new_start;
		m_finish = new_start + new_sz;
		m_end_of_storage = m_finish;
	}
	else
	{
		if (new_sz > size())
		{
			fill_construct(m_finish, m_start + new_sz, x);
			m_finish = m_start + new_sz;
		}
		else
		{
			range_destruct(m_start + new_sz, m_finish);
			m_finish = m_start + new_sz;
		}
	}
}

template <class T, class Alloc>
void vector<T, Alloc>::resize(size_type new_sz)
{
	resize(new_sz, T());
}

template <class T, class Alloc>
void vector<T, Alloc>::reserve(size_type new_sz)
{
	if (capacity() >= new_sz)
		return;
	iterator new_start = Alloc::allocate(new_sz);
	range_construct(m_start, m_finish, new_start);
	range_destruct(m_start, m_finish);
	Alloc::deallocate(m_start, m_end_of_storage - m_start);
	size_type old_sz = size();
	m_start = new_start;
	m_finish = new_start + old_sz;
	m_end_of_storage = new_start + new_sz;
}

template <class T, class Alloc>
void vector<T, Alloc>::__fill_assign(size_type n, const T& x)
{
	if (n <= capacity())
	{
		if (n <= size())
		{
			iterator new_finish = fill_n(m_start, n, x);
			erase(new_finish, m_finish);
		}
		else
		{
			fill(m_start, m_finish, x);
			m_finish = fill_construct_n(m_finish, n - size(), x);
		}
	}
	else
	{
		vector<T, Alloc> tmp(n, x);
		tmp.swap(*this);
	}
}

template <class T, class Alloc>
template <class InputIterator>
void vector<T, Alloc>::__range_assign(InputIterator first, InputIterator last, input_iterator_tag)
{
	iterator i = begin();
	for (; i != end(); && first != last; ++i, ++first)
		*i = *first;
	if (i == end())
		insert(i, first, last);
	else
		erase(i, end());
}

template <class T, class Alloc>
template <class ForwardIterator>
void vector<T, Alloc>::__range_assign(ForwardIterator first, ForwardIterator last, forward_iterator_tag)
{
	size_type n = distance(first, last);
	if (n <= capacity())
	{
		if (n <= size())
		{
			iterator new_finish = copy(first, last, m_start);
			range_destruct(new_finish, m_finish);
			m_finish = new_finish;
		}
		else
		{
			ForwardIterator mid = first;
			advance(mid, size());
			copy(first, mid, m_start);
			m_finish = range_construct(mid, last, m_finish);
		}
	}
	else
	{
		vector<T, Alloc> tmp(first, last);
		tmp.swap(*this);
	}
}

template <class T, class Alloc>
template <class InputIterator>
void vector<T, Alloc>::__assign_aux(InputIterator first, InputIterator last, true_type)
{
	__fill_assign(first, last);
}

template <class T, class Alloc>
template <class InputIterator>
void vector<T, Alloc>::__assign_aux(InputIterator first, InputIterator last, false_type)
{
	__range_assign(first, last, iterator_category(first));
}

template <class T, class Alloc>
template <class InputIterator>
void vector<T, Alloc>::assign(InputIterator first, InputIterator last)
{
	typedef typename is_integer<InputIterator>::integer is_int;
	__assign_aux(first, last, is_int());
}

template <class T, class Alloc>
vector<T, Alloc>& vector<T, Alloc>::operator=(const vector<T, Alloc>& other)
{
	if (this != &other)
		__range_assign(other.begin(), other.end(), forward_iterator_tag());
	return *this;
}

template <class T, class Alloc>
void vector<T, Alloc>::swap(vector<T, Alloc>& other)
{
	MySTL::swap(m_start, other.m_start);
	MySTL::swap(m_finish, other.m_finish);
	MySTL::swap(m_end_of_storage, other.m_end_of_storage);
}

template <class T, class Alloc>
T& vector<T, Alloc>::operator[](size_type index) 
{
	return *(begin() + index); 
}

template <class T, class Alloc>
const T& vector<T, Alloc>::operator[](size_type index) const 
{
	return *(begin() + index); 
}