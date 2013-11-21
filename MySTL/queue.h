#pragma once

#include "vector.h"
#include "deque.h"
#include "functor.h"
#include "heap.h"

__NS_BEGIN

template <class Tp, class Sequence = deque<Tp> >
class queue 
{
public:
	typedef typename Sequence::value_type      value_type;
	typedef typename Sequence::reference       reference;
	typedef typename Sequence::const_reference const_reference;
	typedef typename Sequence::size_type       size_type;
	typedef          Sequence                  container_type;

	queue() : m_container() {}
	explicit queue(const container_type& seq_c) : m_container(seq_c) {}

	bool empty() const { return m_container.empty(); }
	size_type size() const { return m_container.size(); }
	reference front() { return m_container.front(); }
	const_reference front() const { return m_container.front(); }
	reference back() { return m_container.back(); }
	const_reference back() const { return m_container.back(); }
	void push(const value_type& x) { m_container.push_back(x); }
	void pop() { m_container.pop_front(); }

private:
	container_type m_container;
};

template <class Tp, 
		  class Sequence = vector<Tp>, 
		  class Compare = less<typename Sequence::value_type> >
class priority_queue 
{
public:
	typedef typename Sequence::value_type      value_type;
	typedef typename Sequence::reference       reference;
	typedef typename Sequence::const_reference const_reference;
	typedef typename Sequence::size_type       size_type;
	typedef          Sequence                  container_type;

	priority_queue() : m_container() {}
	explicit priority_queue(const Compare& x) :  m_container(), m_cmp(x) {}
	priority_queue(const container_type& seq) 
		: m_container(seq)
	{ make_heap(m_container.begin(), m_container.end(), m_cmp); }

	template <class InputIterator>
	priority_queue(InputIterator first, InputIterator last) 
		: m_container(first, last) 
	{ make_heap(m_container.begin(), m_container.end(), m_cmp); }

	template <class InputIterator>
	priority_queue(InputIterator first, InputIterator last, const Compare& x)
		: m_container(first, last), m_cmp(x) 
	{ make_heap(m_container.begin(), m_container.end(), m_cmp); }

	bool empty() const { return m_container.empty(); }
	size_type size() const { return m_container.size(); }
	const_reference top() const { return m_container.front(); }
	void push(const value_type& x) 
	{
		m_container.push_back(x); 
		push_heap(m_container.begin(), m_container.end(), m_cmp);
	}
	void pop() 
	{
		pop_heap(m_container.begin(), m_container.end(), m_cmp);
		m_container.pop_back();
	}

private:
	container_type m_container;
	Compare m_cmp;
};

__NS_END