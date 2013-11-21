#pragma once

#include "deque.h"

__NS_BEGIN

template <class Tp, class Sequence = deque<Tp> >
class stack 
{
public:
	typedef typename Sequence::value_type      value_type;
	typedef typename Sequence::reference       reference;
	typedef typename Sequence::const_reference const_reference;
	typedef typename Sequence::size_type       size_type;
	typedef          Sequence                  container_type;

	stack() : m_container() {}
	explicit stack(const container_type& s) : m_container(s) {}

	bool empty() const { return m_container.empty(); }
	size_type size() const { return m_container.size(); }
	reference top() { return m_container.back(); }
	const_reference top() const { return m_container.back(); }
	void push(const value_type& x) { m_container.push_back(x); }
	void pop() { m_container.pop_back(); }

private:
	container_type m_container;
};

__NS_END