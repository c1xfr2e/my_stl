#pragma once

#include "vector.h"
#include "functor.h"
#include "heap.h"
#include "pair.h"

__NS_BEGIN

template <class Tp, 
		  class Compare = less<Tp> >
class index_priority_queue
{
private:		
	Compare m_cmp;
	vector<size_t> m_id_seq; // heap storage
	vector<Tp> m_values;    // id to value
	vector<size_t> m_index; // id to m_id_seq index

	void __heap_swim(size_t hole)
	{
		size_t tmp = m_id_seq[hole];
		size_t parent = (hole - 1) / 2;
		while (hole > 0)
		{
			size_t id_parent = m_id_seq[parent];
			size_t id_hole = m_id_seq[hole];
			if(!m_cmp(m_values[id_parent], m_values[tmp]))
				break;
			m_id_seq[hole] = id_parent;
			m_index[id_parent] = hole;
			hole = parent;
			parent = (hole - 1) / 2;
		}
		m_id_seq[hole] = tmp;
		m_index[tmp] = hole;
	}

	void __heap_sink(size_t hole)
	{
		size_t len = m_id_seq.size();
		size_t tmp = m_id_seq[hole];
		size_t child = hole * 2 + 1; // left child
		while (child < len)
		{
			if (child + 1 < len && m_cmp(m_values[m_id_seq[child]], m_values[m_id_seq[child + 1]]))
				child += 1;
			size_t id_hole = m_id_seq[hole];
			size_t id_child = m_id_seq[child];
			if (!m_cmp(m_values[tmp], m_values[id_child]))
				break;
			m_id_seq[hole] = id_child;
			m_index[id_child] = hole; 
			hole = child;
			child = hole * 2 + 1;
		}
		m_id_seq[hole] = tmp;
		m_index[tmp] = hole;
	}

public:
	index_priority_queue(size_t max)
		: m_values(max), m_index(max, -1)
	{}

	void insert(size_t id, const Tp& val) 
	{
		size_t index = m_id_seq.size();
		m_id_seq.push_back(id);
		m_values[id] = val;
		m_index[id] = index;
		__heap_swim(index);                                                             
	}

	size_t pop_min() 
	{
		size_t id = m_id_seq[0];
		m_index[id] = -1;
		m_id_seq[0] = m_id_seq[m_id_seq.size() - 1];
		m_id_seq.pop_back();
		__heap_sink(0);
		return id;
	}

	Tp& get_min()
	{
		return m_values[m_id_seq[0]];
	}

	bool empty() const 
	{ 
		return m_id_seq.empty(); 
	}	

	void change(size_t id, const Tp& x)
	{
		if (m_index[id] == -1)
			return;
		m_values[id] = x;
		__heap_swim(m_index[id]);
		__heap_sink(m_index[id]);
	}

	bool contain(size_t id) 
	{ 
		return m_index[id] != -1; 
	}

	void remove(size_t id)
	{

	}

};

__NS_END