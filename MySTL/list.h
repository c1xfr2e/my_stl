#pragma once

#include "algo_base.h"
#include "allocator.h"
#include "initialize.h"

__NS_BEGIN

template <class Tp>
struct __list_node
{
	Tp m_data;
	__list_node* m_next;
	__list_node* m_prev;
};

template <class Tp, class Ref, class Ptr>
struct __list_iterator
{
	typedef bidirectional_iterator_tag category;
	typedef Tp value_type;
	typedef Ptr pointer;
	typedef Ref	reference;
	typedef ptrdiff_t difference_type;
	typedef __list_iterator<Tp,Ref,Ptr> Self;

	__list_node<Tp>* m_node;

	void m_incr() { m_node = m_node->m_next; }
	void m_decr() { m_node = m_node->m_prev; }

	__list_iterator(__list_node<Tp>* node) : m_node(node) {}
	Ref operator*() const { return m_node->m_data; }
	Ptr operator->() const { return &(operator*()); }
	bool operator==(const Self& x) const { return x.m_node == m_node; }
	bool operator!=(const Self& x) const { return x.m_node != m_node; }
	Self& operator++()
	{
		m_incr();
		return *this;
	}
	Self operator++(int)
	{
		Self tmp = *this;
		m_incr();
		return tmp;
	}
	Self& operator--()
	{
		m_decr();
		return *this;
	}
	Self operator--(int)
	{
		Self tmp = *this;
		m_decr();
		return tmp;
	}
};

template <class Tp, class Alloc = type_allocator<__list_node<Tp> > >
class list
{
public:
	typedef Tp value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef __list_node<Tp> Node;

	typedef __list_iterator<Tp,Tp&,Tp*>             iterator;
	typedef __list_iterator<Tp,const Tp&,const Tp*> const_iterator;

protected:
	Node* m_node;
	Node* __get_node() { return Alloc::allocate(1); }
	void __put_node(Node* p) { return Alloc::deallocate(p, 1); }
	Node* __create_node(const Tp& x)
	{
		Node* p = __get_node();
		construct(&p->m_data, x);
		return p;
	}
	Node* __create_node() { return __create_node(Tp()); }
	
	void __initialize_empty()
	{
		m_node = __get_node();
		m_node->m_next = m_node;
		m_node->m_prev = m_node;
	}
	void __fill_initialize(size_type n, const Tp& x);
	template <class Integer>
	void __initialize_aux(Integer n, Integer x, true_type);
	template <class InputIterator>
	void __initialize_aux(InputIterator first, InputIterator last, false_type);

	void __fill_insert(iterator pos, size_type n, const Tp& x)
	{
		for ( ; n > 0; --n)
			insert(pos, x);
	}
	template <class Integer>
	void __insert_aux(iterator pos, Integer n, Integer x, true_type)
	{
		__fill_insert(pos, n, x);
	}
	template <class InputIterator>
	void __insert_aux(iterator pos, InputIterator first, InputIterator last, false_type)
	{
		for ( ; first != last; ++first)
			insert(pos, *first);
	}

	void __fill_assign(size_type n, const Tp& x)
	{
		iterator p = begin();
		for ( ; p != end() && n > 0; ++p, --n)
			*p = x;
		if (n == 0)
			erase(p, end());
		else
			insert(end(), n, x);
	}
	template <class InputIterator>
	void __range_assign(InputIterator first, InputIterator last)
	{
		iterator p = begin();
		for ( ; p != end() && first != last; ++p, ++first)
			*p = *first;
		if (p != end())
			erase(p, end());
		else
			insert(end(), first, last);
	}	
	template <class Integer>
	void __assign_aux(Integer n, Integer x, true_type)
	{
		__fill_assign(n, x);
	}
	template <class InputIterator>
	void __assign_aux(InputIterator first, InputIterator last, false_type)
	{
		__range_assign(first, last);
	}

public:
	list() { __initialize_empty(); }
	list(size_type n, const Tp& x)
	{
		__initialize_empty();
		__fill_insert(n, x);
	}
	explicit list(size_type n)
	{
		__initialize_empty();
		__fill_insert(n, Tp());
	}
	template <class InputIterator>
	list(InputIterator first, InputIterator last)
	{
		__initialize_empty();
		insert(end(), first, last);
	}
	list(const list<Tp, Alloc>& other)
	{
		__initialize_empty();
		insert(end(), other.begin(), other.end());
	}

	~list() { clear(); __put_node(m_node); }

	iterator begin() { return iterator(m_node->m_next); }
	const_iterator begin() const { return const_iterator(m_node->m_next); }
	iterator end() { return iterator(m_node); }
	const_iterator end() const { return const_iterator(m_node); }
	bool empty() const { return m_node == m_node->m_next; }
	size_type size() const { return distance(begin(), end()); }
	reference front() { return *begin(); }
	const_reference front() const { return *begin(); }
	reference back() { return *--end(); }
	const_reference back() const { return *--end(); }

	void swap(list<Tp, Alloc>& x)
	{
		::swap(m_node, x.m_node);
	}

	iterator insert(iterator pos, const Tp& x)
	{
		Node* p = __create_node(x);
		p->m_next = pos.m_node;
		p->m_prev = pos.m_node->m_prev;
		pos.m_node->m_prev->m_next = p;
		pos.m_node->m_prev = p;
		return p;
	}
	void insert(iterator pos, size_type n, const Tp& x)
	{
		__fill_insert(pos, n, x);
	}
	template <class InputIterator>
	void insert(iterator pos, InputIterator first, InputIterator last)
	{
		typedef typename is_integer<InputIterator>::integer is_int;
		__insert_aux(pos, first, last, is_int());
	}
	void push_front(const Tp& x) { insert(begin(), x); }
	void push_back(const Tp& x) { insert(end(), x); }

	iterator erase(iterator pos)
	{
		Node* p = (pos++).m_node;
		if (p != m_node)
		{
			p->m_prev->m_next = p->m_next;
			p->m_next->m_prev = p->m_prev;
			destruct(&p->m_data);
			__put_node(p);
		}
		return pos;
	}
	iterator erase(iterator first, iterator last)
	{
		if (first == begin() && last == end())
		{
			clear();
			return end();
		}
		else
		{
			while (first != last)
				first = erase(first);
			return last;
		}
	}
	void pop_front() { erase(begin()); }
	void pop_back() { erase(--end()); }
	void clear()
	{
		Node* p = m_node->m_next;
		m_node->m_next = m_node;
		m_node->m_prev = m_node;
		Node* d = 0;
		while (p != m_node)
		{
			d = p;
			p = p->m_next;
			destruct(&d->m_data);
			__put_node(d);
		}
	}

	void resize(size_type new_size, const Tp& x)
	{
		size_type old_sz = distance(begin(), end());
		if (old_sz < new_size)
			for (size_type n = new_size - old_sz; n > 0; --n)
				push_back(x);
		else
			for (size_type n = old_sz - new_size; n > 0; --n)
				pop_back();
	}
	void resize(size_type new_size) { resize(new_size, Tp()); }

	void assign(iterator pos, size_type n, const Tp& x)
	{
		__fill_assign(pos, n, x);
	}
	template <class InputIterator>
	void assign(InputIterator first, InputIterator last)
	{
		typedef typename is_integer<InputIterator>::integer is_int;
		__assign_aux(first, last, is_int());
	}

	void transfer(iterator pos, iterator first, iterator last)
	{
		if (pos != last)
		{
			first.m_node->m_prev->m_next = last.m_node;
			Node* last_prev = last.m_node->m_prev;
			last.m_node->m_prev = first.m_node->m_prev;
			pos.m_node->m_prev->m_next = first.m_node;
			first.m_node->m_prev = pos.m_node->m_prev;
			last_prev->m_next = pos.m_node;
			pos.m_node->m_prev = last_prev;
		}
	}

	void splice(iterator pos, list& x)
	{
		if (!x.empty())
			transfer(pos, x.begin(), x.end());
	}

	void splice(iterator pos, list&, iterator i)
	{
		iterator j = i;
		++j;
		if (pos == i || pos == j) 
			return;
		transfer(pos, i, j);
	}

	void splice(iterator pos, list&, iterator first, iterator last)
	{
		if (first != last)
			this->transfer(pos, first, last);
	}

	void remove(const Tp& x)
	{
		iterator cur = begin();
		while (cur != end())
		{
			if (*cur == x)
				cur = erase(cur);
			else
				++cur;
		}
	}

	void unique()
	{
		Node* cur = m_node->m_next;
		Node* next = cur->m_next;
		while (next != m_node)
		{
			if (next->m_data == cur->m_data)
				erase(cur);
			cur = next;
			next = cur->m_next;
		}
	}

	void reverse()
	{
		Node* p = m_node;
		do
		{
			::swap(p->m_prev, p->m_next);
			p = p->m_prev;
		}
		while(p != m_node);
	}

	void merge(list& x)
	{
		iterator first1 = begin();
		iterator end1 = end();
		iterator first2 = x.begin();
		iterator end2 = x.end();
		while (first1 != end1 && first2 != end2)
		{
			if (*first2 < *first1)
			{
				iterator next = first2;
				++next;
				transfer(first1, first2, next);
				first2 = next;
			}
			else
				++first1;
		}
		if (first2 != end2)
			transfer(end1, first2, end2);
	}

	void sort()
	{
		if (m_node->m_next == m_node || m_node->m_next->m_next == m_node)
			return;
		const size_t MAX_BINS = 64;
		list<Tp, Alloc> tmp_list;
		list<Tp, Alloc> bin_list[MAX_BINS + 1];
		size_t current_max_bin = 0;

		while (!empty())
		{	
			tmp_list.splice(tmp_list.begin(), *this, begin(), ++begin());
			size_t bin = 0;
			for ( ; bin < current_max_bin && !bin_list[bin].empty(); ++bin)
			{
				// to keep stable, do not use tmp_list.merge(bin_list[bin]);
				bin_list[bin].merge(tmp_list);
				bin_list[bin].swap(tmp_list);
			}
			tmp_list.swap(bin_list[bin]);
			if (bin == current_max_bin)
				++current_max_bin;
		}

		for (size_t bin = 1; bin < current_max_bin; ++bin)
			bin_list[bin].merge(bin_list[bin - 1]);
		splice(begin(), bin_list[current_max_bin - 1]);
	}
};

__NS_END