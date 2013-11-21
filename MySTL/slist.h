#pragma once

#include "algo_base.h"
#include "allocator.h"
#include "initialize.h"

__NS_BEGIN

template <class Tp>
struct __slist_node
{
	Tp m_data;
	__slist_node* m_next;
};

template <class T>
__slist_node<T>* __slist_make_link(__slist_node<T>* prev_node, __slist_node<T>* new_node)
{
	new_node->m_next = prev_node->m_next;
	prev_node->m_next = new_node;
	return new_node;
}

template <class T>
__slist_node<T>* __slist_previous(__slist_node<T>* head, const __slist_node<T>* node)
{
	while (head && head->m_next != node)
		head = head->m_next;
	return head;
}

template <class T>
const __slist_node<T>* __slist_previous(const __slist_node<T>* head, const __slist_node<T>* node)
{
	while (head && head->m_next != node)
		head = head->m_next;
	return head;
}

template <class T>
void __slist_splice_after(__slist_node<T>* pos, __slist_node<T>* before_first, __slist_node<T>* before_last)
{
	if (pos != before_first && pos != before_last) 
	{
		__slist_node<T>* first = before_first->m_next;
		before_first->m_next = before_last->m_next;
		before_last->m_next = pos->m_next;
		pos->m_next = first;
	}
}

template <class T>
void __slist_splice_after(__slist_node<T>* pos, __slist_node<T>* head)
{
	__slist_node<T>* before_last = __slist_previous(head, (__slist_node<T>*)0);
	if (before_last != head) 
	{
		before_last->m_next = pos->m_next;
		pos->m_next = head->m_next;
		head->m_next = 0;
	}
}

template <class T>
__slist_node<T>* __slist_reverse(__slist_node<T>* node)
{
	__slist_node<T>* result = 0;
	while(node) 
	{
		__slist_node<T>* next = node->m_next;
		node->m_next = result;
		result = node;
		node = next;
	}
	return result;
}

template <class T>
size_t __slist_size(__slist_node<T>* node)
{
	size_t result = 0;
	for ( ; node != 0; node = node->m_next)
		++result;
	return result;
}

template <class Tp, class Ref, class Ptr>
struct __slist_iterator
{
	typedef Tp						value_type;
	typedef Ptr						pointer;
	typedef Ref						reference;
	typedef ptrdiff_t				difference_type;
	typedef forward_iterator_tag    category;
	typedef size_t					size_type;
	typedef __slist_node<Tp>							Node;
	typedef __slist_iterator<Tp, Tp&, Tp*>				iterator;
	typedef __slist_iterator<Tp, const Tp&, const Tp*>	const_iterator;
	typedef __slist_iterator<Tp, Ref, Ptr>				Self;

	Node* m_node;

	void __incr() { m_node = m_node->m_next; }

	__slist_iterator(Node* x = 0) : m_node(x) {}
	__slist_iterator(const iterator& x) : m_node(x.m_node) {}
	bool operator==(const __slist_iterator& x) const { return m_node == x.m_node; }
	bool operator!=(const __slist_iterator& x) const { return m_node != x.m_node; }
	reference operator*() const { return m_node->m_data; }
	pointer operator->() const { return &(operator*()); }
	Self& operator++() { __incr(); return *this; }
	Self operator++(int) { Self tmp = *this; __incr(); return tmp; }
};


template <class Tp, class Alloc = type_allocator<__slist_node<Tp> > >
class slist
{
public:
	typedef Tp                value_type;
	typedef value_type*       pointer;
	typedef const value_type* const_pointer;
	typedef value_type&       reference;
	typedef const value_type& const_reference;
	typedef size_t            size_type;
	typedef ptrdiff_t         difference_type;

	typedef __slist_iterator<Tp, Tp&, Tp*>				iterator;
	typedef __slist_iterator<Tp, const Tp&, const Tp*>	const_iterator;

	typedef __slist_node<Tp> Node;

private:
	Node m_head;

	Node* __get_node() { return Alloc::allocate(1); }
	void __put_node(Node* p) { Alloc::deallocate(p, 1); }

	Node* __create_node(const value_type& x) 
	{
		Node* node = __get_node();
		construct(&node->m_data, x);
		node->m_next = 0;
		return node;
	}

	Node* __erase_after(Node* pos)
	{
		Node* next = pos->m_next;
		Node* next_next = next->m_next;
		pos->m_next = next_next;
		destruct(&next->m_data);
		__put_node(next);
		return next_next;
	}
	Node* __erase_after(Node* before_first, Node* last_node)
	{
		Node* cur = before_first->m_next;
		before_first->m_next = last_node;
		while (cur != last_node) 
		{
			Node* tmp = cur;
			cur = cur->m_next;
			destruct(&tmp->m_data);
			__put_node(tmp);
		}
		return last_node;
	}

	void __fill_assign(size_type n, const Tp& val)
	{
		Node* prev = &m_head;
		Node* node = m_head.m_next;
		for ( ; node != 0 && n > 0 ; --n) 
		{
			node->m_data = val;
			prev = node;
			node = node->m_next;
		}
		if (n > 0)
			__insert_after_fill(prev, n, val);
		else
			__erase_after(prev, 0);
	}

	template <class Integer>
	void __assign_aux(Integer n, Integer val, true_type) 
	{ __fill_assign((size_type) n, (Tp)val); }

	template <class InputIterator>
	void __assign_aux(InputIterator first, InputIterator last, false_type)
	{
		Node* prev = &this->m_head;
		Node* node = m_head.m_next;
		while (node != 0 && first != last) 
		{
			node->m_data = *first;
			prev = node;
			node = node->m_next;
			++first;
		}
		if (first != last)
			__insert_after_range(prev, first, last);
		else
			__erase_after(prev, 0);
	}


	Node* __insert_after(Node* pos, const value_type& x) 
	{ return __slist_make_link(pos, __create_node(x)); }

	void __insert_after_fill(Node* pos, size_type n, const value_type& x) 
	{
		for (size_type i = 0; i < n; ++i)
			pos = __slist_make_link(pos, __create_node(x));
	}

	template <class Integer>
	void __insert_after_range_aux(Node* pos, Integer n, Integer x, true_type) 
	{ __insert_after_fill(pos, n, x); }

	template <class InputIterator>
	void __insert_after_range_aux(Node* pos, InputIterator first, InputIterator last, false_type) 
	{
		while (first != last) 
		{
			pos = __slist_make_link(pos, __create_node(*first));
			++first;
		}
	}

	template <class InputIterator>
	void __insert_after_range(Node* pos, InputIterator first, InputIterator last) 
	{
		typedef typename is_integer<InputIterator>::integer integer;
		__insert_after_range_aux(pos, first, last, integer());
	}

public:
	slist() { m_head.m_next = 0; }
	slist(size_type n, const value_type& x) { __insert_after_fill(&this->m_head, n, x); }
	explicit slist(size_type n) { __insert_after_fill(&this->m_head, n, value_type()); }
	template <class InputIterator>
	slist(InputIterator first, InputIterator last) { __insert_after_range(&this->m_head, first, last); }
	slist(const slist& x) { __insert_after_range(&this->m_head, x.begin(), x.end()); }
	~slist() { __erase_after(&m_head, 0); }

	slist& operator= (const slist& x)
	{
		if (&x != this) 
		{
			Node* p1 = m_head;
			Node* n1 = m_head.m_next;
			Node* n2 = x.m_head.m_next;
			while (n1 && n2) 
			{
				n1->m_data = n2->m_data;
				p1 = n1;
				n1 = n1->m_next;
				n2 = n2->m_next;
			}
			if (n2 == 0)
				__erase_after(p1, 0);
			else
				__insert_after_range(p1, n2, 0);
		}
		return *this;
	}


	void assign(size_type n, const Tp& val)
	{ __fill_assign(n, val); }

	template <class InputIterator>
	void assign(InputIterator first, InputIterator last) 
	{
		typedef typename is_integer<InputIterator>::integer integer;
		__assign_aux(first, last, integer());
	}

	iterator begin() { return iterator((Node*)this->m_head.m_next); }
	iterator end() { return iterator(0); }
	const_iterator begin() const { return const_iterator(this->m_head.m_next);}
	const_iterator end() const { return const_iterator(0); }
	iterator before_begin() { return iterator(&m_head); }
	const_iterator before_begin() const { return const_iterator(&m_head); }
	size_type size() const { return __slist_size(m_head.m_next); }
	size_type max_size() const { return size_type(-1); }
	bool empty() const { return m_head.m_next == 0; }
	void swap(slist& x) { ::swap(m_head.m_next, x.m_head.m_next); }

	reference front() { return m_head.m_next->m_data; }
	const_reference front() const { return m_head.m_next->m_data; }
	void push_front(const value_type& x) { __slist_make_link(&m_head, __create_node(x)); }
	void pop_front() 
	{
		Node* node = m_head.m_next;
		m_head.m_next = node->m_next;
		destruct(&node->m_data);
		__put_node(node);
	}

	iterator previous(const_iterator pos) 
	{ return iterator(__slist_previous(&m_head, pos.m_node)); }
	const_iterator previous(const_iterator pos) const 
	{ return const_iterator(__slist_previous(&m_head,pos.m_node)); }


	iterator insert_after(iterator pos, const value_type& x) 
	{
		return iterator(__insert_after(pos.m_node, x));
	}

	void insert_after(iterator pos, size_type n, const value_type& x) 
	{
		__insert_after_fill(pos.m_node, n, x);
	}

	template <class InputIterator>
	void insert_after(iterator pos, InputIterator first, InputIterator last) 
	{
		__insert_after_range(pos.m_node, first, last);
	}

	iterator insert(iterator pos, const value_type& x) 
	{
		return iterator(__insert_after(__slist_previous(&m_head, pos.m_node), x));
	}

	void insert(iterator pos, size_type n, const value_type& x) 
	{
		__insert_after_fill(__slist_previous(&m_head, pos.m_node), n, x);
	} 

	template <class InputIterator>
	void insert(iterator pos, InputIterator first, InputIterator last) 
	{
		__insert_after_range(__slist_previous(&m_head, pos.m_node), first, last);
	}

	iterator erase_after(iterator pos) 
	{
		return iterator(__erase_after(pos.m_node));
	}
	iterator erase_after(iterator before_first, iterator last) 
	{
		return iterator(__erase_after(before_first.m_node, last.m_node));
	} 

	iterator erase(iterator pos) 
	{
		return __erase_after(__slist_previous(&m_head, pos.m_node));
	}
	iterator erase(iterator first, iterator last) 
	{
		return __erase_after(__slist_previous(&m_head, first.m_node), last.m_node);
	}

	void resize(size_type new_size, const Tp& x)
	{
		Node* cur = &m_head;
		while (cur->m_next != 0 && len > 0) 
		{
			--len;
			cur = cur->m_next;
		}
		if (cur->m_next) 
			this->__erase_after(cur, 0);
		else
			__insert_after_fill(cur, len, x);
	}

	void resize(size_type new_size) { resize(new_size, Tp()); }
	void clear() { this->__erase_after(&m_head, 0); }

	void splice_after(iterator pos, iterator before_first, iterator before_last)
	{
		if (before_first != before_last) 
			__slist_splice_after(pos.m_node, before_first.m_node, before_last.m_node);
	}
	void splice_after(iterator pos, iterator prev)
	{
		__slist_splice_after(pos.m_node, prev.m_node, prev.m_node->m_next);
	}
	void splice_after(iterator pos, slist& x)
	{
		__slist_splice_after(pos.m_node, &x.m_head);
	}

	void splice(iterator pos, slist& x) 
	{
		if (x.m_head.m_next)
			__slist_splice_after(
				__slist_previous(&m_head, pos.m_node), 
				&x.m_head, 
				__slist_previous(&x.m_head, (Node*)0)
				);
	}
	void splice(iterator pos, slist& x, iterator i) 
	{
		__slist_splice_after(
			__slist_previous(&this->m_head, pos.m_node),
			__slist_previous(&x.m_head, i.m_node),
			i.m_node
			);
	}
	void splice(iterator pos, slist& x, iterator first, iterator last)
	{
		if (first != last)
			__slist_splice_after(
			__slist_previous(&this->m_head, pos.m_node),
			__slist_previous(&x.m_head, first.m_node),
			__slist_previous(first.m_node, last.m_node)
			);
	}

	void reverse() 
	{ 
		if (m_head.m_next)
			m_head.m_next = __slist_reverse(m_head.m_next);
	}

	void remove(const Tp& val)
	{
		Node* cur = &m_head;
		while (cur && cur->m_next) 
		{
			if (cur->m_next->m_data == val)
				this->__erase_after(cur);
			else
				cur = cur->m_next;
		}
	}

	void unique()
	{
		Node* cur = m_head.m_next;
		if (cur) 
		{
			while (cur->m_next) 
			{
				if (cur->m_data == cur->m_next->m_data)
					__erase_after(cur);
				else
					cur = cur->m_next;
			}
		}
	}

	void merge(slist& x)
	{
		Node* n1 = &m_head;
		while (n1->m_next && x.m_head.m_next) 
		{
			if (x.m_head.m_next->m_data < n1->m_next->m_data) 
				__slist_splice_after(n1, &x.m_head, x.m_head.m_next);
			n1 = n1->m_next;
		}
		if (x.m_head.m_next) 
		{
			n1->m_next = x.m_head.m_next;
			x.m_head.m_next = 0;
		}
	}

	void sort()
	{
		if (this->m_head.m_next && this->m_head.m_next->m_next) 
		{
			slist carry;
			slist counter[64];
			int fill = 0;
			while (!empty()) 
			{
				__slist_splice_after(&carry.m_head, &m_head, m_head.m_next);
				int i = 0;
				while (i < fill && !counter[i].empty()) 
				{
					counter[i].merge(carry);
					carry.swap(counter[i]);
					++i;
				}
				carry.swap(counter[i]);
				if (i == fill)
					++fill;
			}

			for (int i = 1; i < fill; ++i)
				counter[i].merge(counter[i-1]);
			this->swap(counter[fill-1]);
		}
	}
};

template <class Tp, class Alloc>
inline void swap(slist<Tp,Alloc>& x, slist<Tp,Alloc>& y) 
{
	x.swap(y);
}

__NS_END