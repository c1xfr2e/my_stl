#pragma once

#include "config.h"
#include "type_traits.h"
#include "iterator_base.h"
#include "algo_base.h"
#include "stack.h"

#include <string>

__NS_BEGIN

struct draw_edge
{
	int x0,y0;
	int x1,y1;
};

struct draw_node
{
	int x0,y0;
	std::string str;
	unsigned int color;
};

typedef bool __RB_tree_color;
const bool __RB_TREE_COLOR_RED = false;
const bool __RB_TREE_COLOR_BLACK = true;

template <class Tp>
struct __RB_tree_node
{
	__RB_tree_node* m_left;
	__RB_tree_node* m_right;
	__RB_tree_node* m_parent;
	__RB_tree_color m_color;
	Tp m_value;

	__RB_tree_node() 
		: m_left(0), m_right(0), m_parent(0), m_color(__RB_TREE_COLOR_BLACK)
	{}
};

template <class Tp>
__RB_tree_node<Tp>*& __left(__RB_tree_node<Tp>* x) { return x->m_left; }
template <class Tp>
__RB_tree_node<Tp>*& __right(__RB_tree_node<Tp>* x) { return x->m_right; }

template <class T>
bool __red(__RB_tree_node<T>* x) { return (x && x->m_color == __RB_TREE_COLOR_RED); }

template <class T>
__RB_tree_color& __color(__RB_tree_node<T>* x) { return x->m_color; }

template <class Tp>
__RB_tree_node<Tp>* __minimum(__RB_tree_node<Tp>* p)
{
	while (__left(p))
		p = __left(p);
	return p;
}

template <class Tp>
__RB_tree_node<Tp>* __maximum(__RB_tree_node<Tp>* p)
{
	while (__right(p))
		p = __right(p);
	return p;
}

template <class Tp>
__RB_tree_node<Tp>* __successor(__RB_tree_node<Tp>* p)
{
	if (__right(p))
	{
		p = __right(p);
		while (__left(p))
			p = __left(p);
		return p;
	}
	__RB_tree_node* parent = p->m_parent;
	while (parent && parent->m_right == p)
	{
		p = parent;
		parent = p->m_parent;
	}
	return parent;
}

template <class Tp>
__RB_tree_node<Tp>* __predecessor(__RB_tree_node<Tp>* p)
{
	if (__left(p))
	{
		p = __left(p);
		while (__right(p))
			p = __right(p);
		return p;
	}
	__RB_tree_node* parent = p->m_parent;
	while (parent && parent->m_left == p)
	{
		p = parent;
		parent = p->m_parent;
	}
	return parent;
}

template <class Tp>
void __left_rotate(__RB_tree_node<Tp>*& x)
{
	__RB_tree_node<Tp>* y = __right(x);
	__right(x) = __left(y);
	__left(y) = x;
	x = y;
}

template <class Tp>
void __right_rotate(__RB_tree_node<Tp>*& x)
{
	__RB_tree_node<Tp>* y = __left(x);
	__left(x) = __right(y);
	__right(y) = x;
	x = y;
}

template <class Tp, class Ref, class Ptr>
struct __RB_tree_iterator
{
	typedef bidirectional_iterator_tag category;
	typedef Tp value_type;
	typedef Ptr pointer;
	typedef Ref	reference;
	typedef ptrdiff_t difference_type;
	typedef __RB_tree_iterator<Tp,Ref,Ptr> Self;
	typedef __RB_tree_node<Tp> RB_tree_node;
	
	RB_tree_node* m_node;

	__RB_tree_iterator(RB_tree_node* node = 0) : m_node(node) {}
	void m_incr() { m_node = __successor(m_node); }
	void m_decr() { m_node = __predecessor(m_node); }

	reference operator*() const { return m_node->m_value; }
	pointer operator->() const { return &(operator*()); }
	bool operator==(const Self& x) { return m_node == x.m_node; }
	bool operator!=(const Self& x) { return m_node != x.m_node; }
	Self& operator++() { m_incr(); return *this; }
	Self operator++(int) { Self tmp = *this; m_incr(); return tmp; }
	Self& operator--() { m_decr(); return *this; }
	Self operator--(int) { Self tmp = *this; m_decr(); return tmp; }
};

template <class Key, class Value, 
          class ExtractKey, class KeyCompare,
		  class Alloc = type_allocator<__RB_tree_node<Value> > >
class RB_tree
{
public:
	typedef Key key_type;
	typedef Value value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef __RB_tree_node<Value> RB_tree_node;
	typedef __RB_tree_iterator<value_type,reference,pointer> iterator;

protected:
	RB_tree_node* m_root;
	KeyCompare m_key_compare;
	ExtractKey m_extract_key;

	RB_tree_node* __get_node() { return Alloc::allocate(1); }
	void __put_node(RB_tree_node* p) { return Alloc::deallocate(p, 1); }
	RB_tree_node* __create_node(const Value& x)
	{
		RB_tree_node* p = __get_node();
		__left(p) = 0;
		__right(p) = 0;
		p->m_parent = 0;
		p->m_color = __RB_TREE_COLOR_RED;
		construct(&p->m_value, x);
		return p;
	}
	RB_tree_node* __create_node() { return __create_node(Value()); }
	RB_tree_node* __clone_node(RB_tree_node* x)
	{
		RB_tree_node* p = __create_node(x->m_value);
		p->m_color = x->m_color;
		return p;
	}

	const key_type& __key(RB_tree_node* x) { return m_extract_key(x->m_value); }
	const key_type& __key(const value_type& x) { return m_extract_key(x); }

public:
	RB_tree(RB_tree_node* root = 0) : m_root(root) {}

	RB_tree_node* clone() { return clone(m_root); }	
	RB_tree_node* clone(RB_tree_node* p)
	{
		RB_tree_node* root = 0;
		RB_tree_node* parent = 0;
		while (p != 0)
		{
			RB_tree_node* node = __create_node(p->m_value);
			if (parent)
				parent->m_right = node;
			else
				root = node;
			node->m_left = clone(__left(p));
			parent = node;
			p = __right(p);
		}
		return root;
	}

	template <class Fun>
	void travel(Fun f) { return nonrecursive_travel(m_root, f); }
	template <class Fun>
	void travel(RB_tree_node* p, Fun f)
	{
		if (p)
		{
			f(*p);
			travel(__left(p), f);
			travel(__right(p), f);
		}
	}

	void draw_tree(int left, int right, int top, int height, 
		vector<draw_edge>& edges, vector<draw_node>& nodes)
	{
		draw_tree(m_root, left, right, top, height, edges, nodes, 0);
	}

	void draw_tree(RB_tree_node* x, 
		int left, int right, int top, int height, 
		vector<draw_edge>& edges, vector<draw_node>& nodes, draw_node* parent_dn)
	{
		if (!x) return;

		int mid = (left + right) / 2;
		draw_node dn;
		dn.x0 = mid;
		dn.y0 = top;
		char strbuf[32];
		sprintf_s(strbuf, 32, "%d", x->m_value);
		dn.str = strbuf;
		if (x->m_color == __RB_TREE_COLOR_RED)
		{
			dn.color = RGB(255,0,0);
		}
		else
		{	
			dn.color = RGB(0,0,0);
		}
		nodes.push_back(dn);
		if (parent_dn)
		{
			draw_edge de;
			de.x0 = dn.x0;
			de.y0 = dn.y0;
			de.x1 = parent_dn->x0;
			de.y1 = parent_dn->y0;
			edges.push_back(de);
		}
		
		draw_tree(x->m_left, left, mid, top + height, height, edges, nodes, &dn);
		draw_tree(x->m_right, mid, right, top + height, height, edges, nodes, &dn);
	}

	Value* find(const key_type& x) { return find(m_root, x); }
	Value* find(RB_tree_node* p, const key_type& x)
	{
		if (p == 0)
			return 0;
		else if (m_key_compare(__key(x), __key(p)))
			return find(__left(p), x);
		else if (m_key_compare(__key(p), __key(x)))
			return find(__right(p), x);
		else
			return &p->m_value;
	}

	void insert_unique(const value_type& x) 
	{ 
		insert_unique(m_root, x, 0); 
		m_root->m_color = __RB_TREE_COLOR_BLACK;
	}
	void insert_unique(RB_tree_node*& p, const value_type& x, bool from_left)
	{
		if (p == 0)
		{
			p = __create_node(x);
		}
		else if (m_key_compare(__key(x), __key(p)))
		{
			insert_unique(__left(p), x, true);
			if (__red(p) && __red(__left(p)) && !from_left)
				__right_rotate(p);
			if (__red(__left(p)) && __red(__left(__left(p))))
			{
				if (__red(__right(p)))
				{
					__color(p) = __RB_TREE_COLOR_RED;
					__color(__left(p)) = __RB_TREE_COLOR_BLACK;
					__color(__right(p)) = __RB_TREE_COLOR_BLACK;
				}
				else
				{
					__color(p) = __RB_TREE_COLOR_RED;
					__color(__left(p)) = __RB_TREE_COLOR_BLACK;
					__right_rotate(p);
				}
			}
		}
		else if (m_key_compare(__key(p), __key(x)))
		{
			insert_unique(__right(p), x, false);
			if (__red(p) && __red(__right(p)) && from_left)
				__left_rotate(p);
			if (__red(__right(p)) && __red(__right(__right(p))))
			{
				if (__red(__left(p)))
				{
					__color(p) = __RB_TREE_COLOR_RED;
					__color(__left(p)) = __RB_TREE_COLOR_BLACK;
					__color(__right(p)) = __RB_TREE_COLOR_BLACK;
				}
				else
				{
					__color(p) = __RB_TREE_COLOR_RED;
					__color(__right(p)) = __RB_TREE_COLOR_BLACK;
					__left_rotate(p);
				}
			}
		}
		else
		{
			p->m_value = x;
		}
	}

	bool remove_min(RB_tree_node*& p, RB_tree_node*& min)
	{
		if (!__left(p))
		{
			min = p;
			p = __right(p);
			return __color(min) == __RB_TREE_COLOR_BLACK;
		}
		else
		{ 
			bool need_rebalance = remove_min(__left(p), min);
			if (need_rebalance)
				return rebalance_at(p, true);
			else
				return false;
		}
	}

	bool rebalance_at(RB_tree_node*& x, bool left)
	{
		if (left)
		{
			if (__left(x) && __color(__left(x)) == __RB_TREE_COLOR_RED)
			{
				__color(__left(x)) = __RB_TREE_COLOR_BLACK;
				return false;
			}
			RB_tree_node*& y = __right(x);
			if (__red(y))
			{
				__color(x) = __RB_TREE_COLOR_RED;
				__color(y) = __RB_TREE_COLOR_BLACK;
				__left_rotate(x);
				if (!rebalance_at(__left(x), true))
					return false;
			}
			if (!__red(__left(y)) && !__red(__right(y)))
			{
				__color(y) = __RB_TREE_COLOR_RED;
				if (__color(x) == __RB_TREE_COLOR_RED)
				{
					__color(x) = __RB_TREE_COLOR_BLACK;
					return false;
				}
			}
			else
			{
				if (!__red(__right(y)))
				{
					__color(__left(y)) = __RB_TREE_COLOR_BLACK;
					__color(y) = __RB_TREE_COLOR_RED;
					__right_rotate(y);
				}
				__color(y) = __color(x);
				__color(x) = __RB_TREE_COLOR_BLACK;
				__color(__right(y)) = __RB_TREE_COLOR_BLACK;
				__left_rotate(x);
				return false;
			}
		}
		else
		{
			if (__right(x) && __color(__right(x)) == __RB_TREE_COLOR_RED)
			{
				__color(__right(x)) = __RB_TREE_COLOR_BLACK;
				return false;
			}
			RB_tree_node*& y = __left(x);
			if (__red(y))
			{
				__color(x) = __RB_TREE_COLOR_RED;
				__color(y) = __RB_TREE_COLOR_BLACK;
				__right_rotate(x);
				if (!rebalance_at(__right(x), false))
					return false;
			}
			if (!__red(__left(y)) && !__red(__right(y)))
			{
				__color(y) = __RB_TREE_COLOR_RED;
				if (__color(x) == __RB_TREE_COLOR_RED)
				{
					__color(x) = __RB_TREE_COLOR_BLACK;
					return false;
				}
			}
			else
			{
				if (!__red(__left(y)))
				{
					__color(__right(y)) = __RB_TREE_COLOR_BLACK;
					__color(y) = __RB_TREE_COLOR_RED;
					__left_rotate(y);
				}
				__color(y) = __color(x);
				__color(x) = __RB_TREE_COLOR_BLACK;
				__color(__left(y))  = __RB_TREE_COLOR_BLACK;
				__right_rotate(x);
				return false;
			}
		}
	} 

	void erase(const key_type& x) { erase(m_root, x); }
	bool erase(RB_tree_node*& p, const key_type& k)
	{
		if (p == 0) 
		{
			return false;
		} 
		else if (m_key_compare(__key(k), __key(p)))
		{
			bool need_rebalance = erase(__left(p), k);
			if (need_rebalance)
				return rebalance_at(p, true);
		}
		else if (m_key_compare(__key(p), __key(k)))
		{		
			bool need_rebalance = erase(__right(p), k);
			if (need_rebalance)
				return rebalance_at(p, false);
		}
		else
		{	
			if (__left(p) && __right(p))
			{
				RB_tree_node* successor = 0;
				bool need_rebalance = remove_min(__right(p), successor);
				__left(successor) = __left(p);
				__right(successor) = __right(p);
				__color(successor) = __color(p);
				destruct(&p->m_value);
				__put_node(p);
				p = successor;
				if (need_rebalance)
					return rebalance_at(p, false);
				return false;
			}	
			else
			{
				RB_tree_node* tmp = __left(p)? __left(p) : __right(p);
				__RB_tree_color color = __color(p);
				destruct(&p->m_value);
				__put_node(p);
				p = tmp;
				return color == __RB_TREE_COLOR_BLACK;
			}
		}
	}

};

__NS_END