#pragma once

/* Class invariants:
 *	For any nonsingular iterator i:
 *      i.node is the address of an element in the map array.  The
 *        contents of i.node is a pointer to the beginning of a node.
 *      i.first == *(i.node) 
 *      i.last  == i.first + node_size
 *      i.cur is a pointer in the range [i.first, i.last).  NOTE:
 *        the implication of this is that i.cur is always a dereferenceable
 *        pointer, even if i is a past-the-end iterator.
 *  Start and Finish are always nonsingular iterators.  NOTE: this means
 *      that an empty deque must have one node, and that a deque
 *      with N elements, where N is the buffer size, must have two nodes.
 *  For every node other than start.node and finish.node, every element
 *      in the node is an initialized object.  If start.node == finish.node,
 *      then [start.cur, finish.cur) are initialized objects, and
 *      the elements outside that range are uninitialized storage.  Otherwise,
 *      [start.cur, start.last) and [finish.first, finish.cur) are initialized
 *      objects, and [start.first, start.cur) and [finish.cur, finish.last)
 *      are uninitialized storage.
 *  [map, map + map_size) is a valid, non-empty range.  
 *  [start.node, finish.node] is a valid range contained within [map, map + map_size).  
 *  A pointer in the range [map, map + map_size) points to an allocated node
 *      if and only if the pointer is in the range [start.node, finish.node].
 */

__NS_BEGIN

#include "algo_base.h"
#include "allocator.h"
#include "initialize.h"

size_t __deque_buf_size(size_t __size)
{
	return __size < 512 ? size_t(512 / __size) : size_t(1);
}

template <class Tp, class Ref, class Ptr>
struct __deque_iterator
{
	typedef random_access_iterator_tag category;
	typedef Tp value_type;
	typedef Ptr pointer;
	typedef Ref reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef Tp** Map_pointer;
	typedef __deque_iterator Self;
	typedef __deque_iterator<Tp, Tp&, Tp*>             iterator;
	typedef __deque_iterator<Tp, const Tp&, const Tp*> const_iterator;

	Tp* m_cur;
	Tp* m_first;
	Tp* m_last;
	Map_pointer m_node;

	static size_type __buffer_size() { return __deque_buf_size(sizeof(Tp)); }

	void __set_node(Map_pointer new_node)
	{
		m_node = new_node;
		m_first = *new_node;
		m_last = m_first + difference_type(__buffer_size());
	}

	__deque_iterator(Tp* x, Map_pointer y)
		: m_cur(x), m_first(*y), m_last(*y + __buffer_size()), m_node(y) {}
	__deque_iterator()
		: m_cur(0), m_first(0), m_last(0), m_node(0) {}
	__deque_iterator(const iterator& x)
		: m_cur(x.m_cur), m_first(x.m_first), m_last(x.m_last), m_node(x.m_node) {}

	reference operator*() const { return *m_cur; }
	pointer operator->() const { return m_cur; }

	difference_type operator-(const Self& x) const
	{
		return difference_type(__buffer_size()) * (m_node - x.m_node - 1) 
			+ (m_cur - m_first) + (x.m_last - x.m_cur);
	}

	Self& operator++()
	{
		++m_cur;
		if (m_cur == m_last)
		{
			__set_node(m_node + 1);
			m_cur = m_first;
		}
		return *this;
	}
	Self operator++(int) { Self tmp = *this; ++*this; return tmp; }
	Self& operator--()
	{
		if (m_cur == m_first)
		{
			__set_node(m_node - 1);
			m_cur = m_last;
		}
		--m_cur;
		return *this;
	}
	Self operator--(int) { Self tmp = *this; --*this; return tmp; }

	Self& operator+=(difference_type n)
	{
		difference_type buffer_size = difference_type(__buffer_size());
		difference_type offset = n + (m_cur - m_first);
		if (offset >= 0 && offset < buffer_size)
			m_cur += n;
		else
		{
			difference_type node_offset = offset > 0 ? 
				offset / buffer_size : -((-offset - 1) / buffer_size) - 1;
			__set_node(m_node + node_offset);
			m_cur = m_first + (offset - node_offset * buffer_size);
		}
		return *this;
	}

	Self operator+(difference_type n) const
	{
		Self tmp = *this;
		return tmp += n;
	}

	Self& operator-=(difference_type n) { return *this += -n; }
	Self operator-(difference_type n) const { Self tmp = *this; return tmp -= n; }

	reference operator[](difference_type n) const { return *(*this + n); }

	bool operator==(const Self& x) const { return m_cur == x.m_cur; }
	bool operator!=(const Self& x) const { return !(*this == x); }
	bool operator<(const Self& x) const
	{
		return (m_node == x.m_node) ?
			(m_cur < x.m_cur) : (m_node < x.m_node);
	}
	bool operator>(const Self& x) const { return x < *this; }
	bool operator<=(const Self& x) const { return !(x < *this); }
	bool operator>=(const Self& x) const { return !(*this < x); }
};

template <class Tp, class Ref, class Ptr>
__deque_iterator<Tp, Ref, Ptr> operator+(ptrdiff_t n, const __deque_iterator<Tp, Ref, Ptr>& x)
{
	return x + n;
}

template <class Tp, class Alloc = allocator>
class deque
{
public:
	typedef Tp value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef __deque_iterator<Tp,Tp&,Tp*> iterator;
	typedef __deque_iterator<Tp,const Tp&,const Tp*> const_iterator;

	typedef pointer* Map_pointer;

	typedef type_allocator<Tp, Alloc>  NodeAllocator;
	typedef type_allocator<Tp*, Alloc> MapAllocator;

private:
	Map_pointer m_map;
	size_type m_map_size;
	iterator m_start;
	iterator m_finish;

	enum { __initial_map_size = 8 };

	static size_t __buffer_size() { return __deque_buf_size(sizeof(Tp)); }

	pointer __allocate_node() { return NodeAllocator::allocate(__deque_buf_size(sizeof(Tp))); }
	void __deallocate_node(pointer p) { NodeAllocator::deallocate(p, __deque_buf_size(sizeof(Tp))); }
	Map_pointer __allocate_map(size_t n) { return MapAllocator::allocate(n); }
	void __deallocate_map(Map_pointer p, size_t n) { MapAllocator::deallocate(p, n); }

	void __create_nodes(Map_pointer nstart, Map_pointer nfinish)
	{
		for (Map_pointer cur = nstart; cur < nfinish; ++cur)
			*cur = __allocate_node();
	}

	void __destroy_nodes(Map_pointer nstart, Map_pointer nfinish)
	{
		for (Map_pointer cur = nstart; cur < nfinish; ++cur)
			__deallocate_node(*cur);
	}

	void __initialize_map(size_t num_elements)
	{
		size_t num_nodes = num_elements / __deque_buf_size(sizeof(Tp)) + 1;
		m_map_size = max((size_t) __initial_map_size, num_nodes + 2);
		m_map = __allocate_map(m_map_size);

		Map_pointer nstart = m_map + (m_map_size - num_nodes) / 2;
		Map_pointer nfinish = nstart + num_nodes;
		__create_nodes(nstart, nfinish);

		m_start.__set_node(nstart);
		m_start.m_cur = m_start.m_first;

		m_finish.__set_node(nfinish - 1);
		m_finish.m_cur = m_finish.m_first + num_elements % __deque_buf_size(sizeof(Tp));
	}

	void __fill_initialize(const value_type& x)
	{
		for (Map_pointer cur = m_start.m_node; cur < m_finish.m_node; ++cur)
			fill_construct(*cur, *cur + __buffer_size(), x);
		fill_construct(m_finish.m_first, m_finish.m_cur, x);
	}

	template <class InputIterator>
	void __range_initialize(InputIterator first, InputIterator last, input_iterator_tag)
	{
		__initialize_map(0);
		for ( ; first != last; ++first)
			push_back(*first);
	}

	template <class ForwardIterator>
	void __range_initialize(ForwardIterator first, ForwardIterator last, forward_iterator_tag)
	{
		size_type n = 0;
		distance(first, last, n);
		__initialize_map(n);

		Map_pointer cur_node;
		for (cur_node = m_start.m_node; cur_node < m_finish.m_node; ++cur_node)
		{
			ForwardIterator mid = first;
			advance(mid, __buffer_size());
			range_construct(first, mid, *cur_node);
			first = mid;
		}
		range_construct(first, last, m_finish.m_first);
	}

	template <class Integer>
	void __initialize_aux(Integer n, Integer x, true_type)
	{
		__initialize_map(n);
		__fill_initialize(x);
	}

	template <class _InputIter>
	void __initialize_aux(_InputIter first, _InputIter last, false_type)
	{
		__range_initialize(first, last, iterator_category(first));
	}

	void __push_back_aux(const value_type& t)
	{
		value_type t_copy = t;
		__reserve_map_at_back();
		*(m_finish.m_node + 1) = __allocate_node();
		construct(m_finish.m_cur, t_copy);
		m_finish.__set_node(m_finish.m_node + 1);
		m_finish.m_cur = m_finish.m_first;
	}

	void __push_front_aux(const value_type& t)
	{
		value_type t_copy = t;
		__reserve_map_at_front();
		*(m_start.m_node - 1) = __allocate_node();
		m_start.__set_node(m_start.m_node - 1);
		m_start.m_cur = m_start.m_last - 1;
		construct(m_start.m_cur, t_copy);
	}

	void __pop_back_aux()
	{
		__deallocate_node(m_finish.m_first);
		m_finish.__set_node(m_finish.m_node - 1);
		m_finish.m_cur = m_finish.m_last - 1;
		destruct(m_finish.m_cur);
	}

	void __pop_front_aux()
	{
		destruct(m_start.m_cur);
		__deallocate_node(m_start.m_first);
		m_start.__set_node(m_start.m_node + 1);
		m_start.m_cur = m_start.m_first;
	}

	void __reserve_map_at_back(size_type nodes_to_add = 1)
	{
		if (nodes_to_add + 1 > m_map_size - (m_finish.m_node - m_map))
			__reallocate_map(nodes_to_add, false);
	}

	void __reserve_map_at_front(size_type nodes_to_add = 1)
	{
		if (nodes_to_add > size_type(m_start.m_node - m_map))
			__reallocate_map(nodes_to_add, true);
	}

	void __reallocate_map(size_type nodes_to_add, bool add_at_front)
	{
		size_type old_num_nodes = m_finish.m_node - m_start.m_node + 1;
		size_type new_num_nodes = old_num_nodes + nodes_to_add;

		Map_pointer new_nstart;
		if (m_map_size > 2 * new_num_nodes)
		{
			new_nstart = m_map + (m_map_size - new_num_nodes) / 2
				+ (add_at_front ? nodes_to_add : 0);
			if (new_nstart < m_start.m_node)
				copy(m_start.m_node, m_finish.m_node + 1, new_nstart);
			else
				copy_backward(m_start.m_node, m_finish.m_node + 1,
				new_nstart + old_num_nodes);
		}
		else
		{
			size_type new_map_size =
				m_map_size + max(m_map_size, nodes_to_add) + 2;

			Map_pointer __new_map = __allocate_map(new_map_size);
			new_nstart = __new_map + (new_map_size - new_num_nodes) / 2
				+ (add_at_front ? nodes_to_add : 0);
			copy(m_start.m_node, m_finish.m_node + 1, new_nstart);
			__deallocate_map(m_map, m_map_size);

			m_map = __new_map;
			m_map_size = new_map_size;
		}

		m_start.__set_node(new_nstart);
		m_finish.__set_node(new_nstart + old_num_nodes - 1);
	}

	iterator __insert_aux(iterator pos, const value_type& x)
	{
		difference_type index = pos - m_start;
		value_type __x_copy = x;
		if (size_type(index) < this->size() / 2)
		{
			push_front(front());
			iterator __front1 = m_start;
			++__front1;
			iterator __front2 = __front1;
			++__front2;
			pos = m_start + index;
			iterator __pos1 = pos;
			++__pos1;
			copy(__front2, __pos1, __front1);
		}
		else
		{
			push_back(back());
			iterator __back1 = m_finish;
			--__back1;
			iterator __back2 = __back1;
			--__back2;
			pos = m_start + index;
			copy_backward(pos, __back2, __back1);
		}
		*pos = __x_copy;
		return pos;
	}

public:
	deque() 
		: m_map(0), m_map_size(0),  m_start(), m_finish() 
	{ __initialize_map(0); }
	explicit deque(size_type num_elements) 
		: m_map(0), m_map_size(0),  m_start(), m_finish()
	{ 
		__initialize_map(num_elements);
		__fill_initialize(value_type());
	}
	deque(size_type n, const value_type& value) 
	{ 
		__initialize_map(num_elements);
		__fill_initialize(value); 
	}
	deque(const deque& x)
	{
		__initialize_map(x.size());
		range_construct(x.begin(), x.end(), m_start);
	}
	template <class InputIterator>
	deque(InputIterator first, InputIterator last)
	{
		typedef typename is_integer<InputIterator>::integer integer;
		__initialize_aux(first, last, integer());
	}

	~deque()
	{
		range_destruct(m_start, m_finish);
		if (m_map)
		{
			__destroy_nodes(m_start.m_node, m_finish.m_node + 1);
			__deallocate_map(m_map, m_map_size);
		}
	}

	iterator begin() { return m_start; }
	iterator end() { return m_finish; }
	const_iterator begin() const { return m_start; }
	const_iterator end() const { return m_finish; }

	reference operator[](size_type n) { return m_start[difference_type(n)]; }
	const_reference operator[](size_type n) const { return m_start[difference_type(n)]; }

	reference front() { return *m_start; }
	reference back() { iterator tmp = m_finish; --tmp; return *tmp; }
	const_reference front() const { return *m_start; }
	const_reference back() const { const_iterator tmp = m_finish; --tmp; return *tmp; }
	size_type size() const { return m_finish - m_start; }
	size_type max_size() const { return size_type(-1); }
	bool empty() const { return m_finish == m_start; }

	void swap(deque& x)
	{
		::swap(m_start, x.m_start);
		::swap(m_finish, x.m_finish);
		::swap(m_map, x.m_map);
		::swap(m_map_size, x.m_map_size);
	}

	void push_back(const value_type& t)
	{
		if (m_finish.m_cur != m_finish.m_last - 1)
		{
			construct(m_finish.m_cur, t);
			++m_finish.m_cur;
		}
		else
			__push_back_aux(t);
	}

	void push_front(const value_type& t)
	{
		if (m_start.m_cur != m_start.m_first)
		{
			construct(m_start.m_cur - 1, t);
			--m_start.m_cur;
		}
		else
			__push_front_aux(t);
	}

	void pop_back()
	{
		if (m_finish.m_cur != m_finish.m_first)
		{
			--m_finish.m_cur;
			destruct(m_finish.m_cur);
		}
		else
			__pop_back_aux();
	}

	void pop_front()
	{
		if (m_start.m_cur != m_start.m_last - 1)
		{
			destruct(m_start.m_cur);
			++m_start.m_cur;
		}
		else
			__pop_front_aux();
	}

	iterator insert(iterator position, const value_type& x)
	{
		if (position.m_cur == m_start.m_cur)
		{
			push_front(x);
			return m_start;
		}
		else if (position.m_cur == m_finish.m_cur)
		{
			push_back(x);
			iterator tmp = m_finish;
			--tmp;
			return tmp;
		}
		else
		{
			return __insert_aux(position, x);
		}
	}

	iterator erase(iterator pos)
	{
		iterator len = pos;
		++len;
		difference_type index = pos - m_start;
		if (size_type(index) < (this->size() >> 1))
		{
			copy_backward(m_start, pos, len);
			pop_front();
		}
		else
		{
			copy(len, m_finish, pos);
			pop_back();
		}
		return m_start + index;
	}

	void resize(size_type new_size, const value_type& x)
	{
		const size_type len = size();
		if (new_size < len)
			erase(m_start + new_size, m_finish);
		else
			insert(m_finish, new_size - len, x);
	}

	void resize(size_type new_size) { resize(new_size, value_type()); }

	void clear()
	{
		for (Map_pointer node = m_start.m_node + 1; node < m_finish.m_node; ++node)
		{
			range_destruct(*node, *node + __buffer_size());
			__deallocate_node(*node);
		}

		if (m_start.m_node != m_finish.m_node)
		{
			range_destruct(m_start.m_cur, m_start.m_last);
			range_destruct(m_finish.m_first, m_finish.m_cur);
			__deallocate_node(m_finish.m_first);
		}
		else
			range_destruct(m_start.m_cur, m_finish.m_cur);

		m_finish = m_start;
	}
};

__NS_END