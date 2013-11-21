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
};

template <class Tp>
struct __AVL_tree_node
{
	__AVL_tree_node* m_left;
	__AVL_tree_node* m_right;
	__AVL_tree_node* m_parent;
	Tp m_value;
	int m_height;

	__AVL_tree_node() 
		: m_left(0), m_right(0), m_parent(0), m_height(0)
	{}
};

template <class Tp>
__AVL_tree_node<Tp>*& __left(__AVL_tree_node<Tp>* x) { return x->m_left; }
template <class Tp>
__AVL_tree_node<Tp>*& __right(__AVL_tree_node<Tp>* x) { return x->m_right; }
template <class Tp>
int __height(__AVL_tree_node<Tp>* x) { return x? x->m_height : -1; }


template <class Tp>
__AVL_tree_node<Tp>* __minimum(__AVL_tree_node<Tp>* p)
{
	while (__left(p))
		p = __left(p);
	return p;
}

template <class Tp>
__AVL_tree_node<Tp>* __maximum(__AVL_tree_node<Tp>* p)
{
	while (__right(p))
		p = __right(p);
	return p;
}

template <class Tp>
__AVL_tree_node<Tp>* __successor(__AVL_tree_node<Tp>* p)
{
	if (__right(p))
	{
		p = __right(p);
		while (__left(p))
			p = __left(p);
		return p;
	}
	__AVL_tree_node* parent = p->m_parent;
	while (parent && parent->m_right == p)
	{
		p = parent;
		parent = p->m_parent;
	}
	return parent;
}

template <class Tp>
__AVL_tree_node<Tp>* __predecessor(__AVL_tree_node<Tp>* p)
{
	if (__left(p))
	{
		p = __left(p);
		while (__right(p))
			p = __right(p);
		return p;
	}
	__AVL_tree_node* parent = p->m_parent;
	while (parent && parent->m_left == p)
	{
		p = parent;
		parent = p->m_parent;
	}
	return parent;
}

template <class Tp>
void __left_rotate(__AVL_tree_node<Tp>*& x)
{
	__AVL_tree_node<Tp>* y = __right(x);
	__right(x) = __left(y);
	__left(y) = x;
	x->m_height = max(__height(__left(x)), __height(__right(x))) + 1;
	y->m_height = max(__height(__left(y)), __height(__right(y))) + 1;
	x = y;
}

template <class Tp>
void __right_rotate(__AVL_tree_node<Tp>*& x)
{
	__AVL_tree_node<Tp>* y = __left(x);
	__left(x) = __right(y);
	__right(y) = x;
	x->m_height = max(__height(__left(x)), __height(__right(x))) + 1;
	y->m_height = max(__height(__left(y)), __height(__right(y))) + 1;
	x = y;
}

template <class Key, class Value, 
          class ExtractKey, class KeyCompare,
		  class Alloc = type_allocator<__AVL_tree_node<Value> > >
class AVL_tree
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
	typedef __AVL_tree_node<Value> AVL_tree_node;

protected:
	AVL_tree_node* m_root;
	KeyCompare m_key_compare;
	ExtractKey m_extract_key;

	AVL_tree_node* __get_node() { return Alloc::allocate(1); }
	void __put_node(AVL_tree_node* p) { return Alloc::deallocate(p, 1); }
	AVL_tree_node* __create_node(const Value& x)
	{
		AVL_tree_node* p = __get_node();
		__left(p) = 0;
		__right(p) = 0;
		p->m_parent = 0;
		p->m_height = 0;
		construct(&p->m_value, x);
		return p;
	}
	AVL_tree_node* __create_node() { return __create_node(Value()); }
	AVL_tree_node* __clone_node(AVL_tree_node* x)
	{
		AVL_tree_node* p = __create_node(x->m_value);
		p->m_color = x->m_color;
		return p;
	}

	const key_type& __key(AVL_tree_node* x) { return m_extract_key(x->m_value); }
	const key_type& __key(const value_type& x) { return m_extract_key(x); }

public:
	AVL_tree(AVL_tree_node* root = 0) : m_root(root) {}

	AVL_tree_node* clone() { return clone(m_root); }	
	AVL_tree_node* clone(AVL_tree_node* p)
	{
		AVL_tree_node* root = 0;
		AVL_tree_node* parent = 0;
		while (p != 0)
		{
			AVL_tree_node* node = __create_node(p->m_value);
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
	void travel(AVL_tree_node* p, Fun f)
	{
		if (p)
		{
			f(*p);
			travel(__left(p), f);
			travel(__right(p), f);
		}
	}

	template <class Fun>
	void nonrecursive_travel(AVL_tree_node* p, Fun f)
	{
		stack<AVL_tree_node*> s;
		while (p != 0 || !s.empty())
		{
			if (p != 0)
			{
				f(*p);			
				s.push(p);
				p = __left(p);
			}
			else
			{
				p = s.top();
				s.pop();
				p = __right(p);
			}
		}
	}

	void draw_tree(int left, int right, int top, int height, 
		vector<draw_edge>& edges, vector<draw_node>& nodes)
	{
		draw_tree(m_root, left, right, top, height, edges, nodes, 0);
	}

	void draw_tree(AVL_tree_node* x, 
		int left, int right, int top, int height, 
		vector<draw_edge>& edges, vector<draw_node>& nodes, draw_node* parent_dn)
	{
		if (!x) return;

		int mid = (left + right) / 2;
		draw_node dn;
		dn.x0 = mid;
		dn.y0 = top;
		char strbuf[32];
		sprintf_s(strbuf, 32, "%d:%d", x->m_value, x->m_height);
		dn.str = strbuf;
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
	Value* find(AVL_tree_node* p, const key_type& x)
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

	void insert_unique(const value_type& x) { insert_unique(m_root, x); }
	void insert_unique(AVL_tree_node*& p, const value_type& x)
	{
		if (p == 0)
		{
			p = __create_node(x);
		}
		else if (m_key_compare(__key(x), __key(p)))
		{
			insert_unique(__left(p), x);
		}
		else if (m_key_compare(__key(p), __key(x)))
		{
			insert_unique(__right(p), x);
		}
		else
		{
			p->m_value = x;
		}
		p->m_height = max(__height(__left(p)), __height(__right(p))) + 1;
		rebalance(p);
	}

	void remove_min(AVL_tree_node*& p, AVL_tree_node*& ret)
	{
		if (!__left(p))
		{
			ret = p;
			p = __right(p);
		}
		else
		{
			remove_min(__left(p), ret);
			p->m_height = max(__height(__left(p)), __height(__right(p))) + 1;
			rebalance(p);
		}
	}

	void erase(const key_type& x) { erase(m_root, x); }
	void erase(AVL_tree_node*& p, const key_type& k)
	{
		if (p == 0) 
		{
			return;
		} 
		else if (m_key_compare(__key(k), __key(p)))
		{
			erase(__left(p), k);
		}
		else if (m_key_compare(__key(p), __key(k)))
		{		
			erase(__right(p), k);
		}
		else
		{	
			if (__left(p) && __right(p))
			{
				AVL_tree_node* successor = 0;
				remove_min(__right(p), successor);
				__left(successor) = __left(p);
				__right(successor) = __right(p);
				p = successor;
			}	
			else
			{
				AVL_tree_node* tmp = __left(p)? __left(p) : __right(p);
				destruct(&p->m_value);
				__put_node(p);
				p = tmp;
			}
		}

		if (p)
		{
			p->m_height = max(__height(__left(p)), __height(__right(p))) + 1;
			rebalance(p);
		}
	}

	void rebalance(AVL_tree_node*& x)
	{
		if (__height(__left(x)) - __height(__right(x)) == 2)
		{
			AVL_tree_node* y = __left(x);
			if (__height(__left(y)) >= __height(__right(y)))
				__right_rotate(x);
			else
			{
				__left_rotate(y);
				__right_rotate(x);
			}
		}
		else if (__height(__right(x)) - __height(__left(x)) == 2)
		{
			AVL_tree_node* y = __right(x);
			if (__height(__right(y)) >= __height(__left(y)))
				__left_rotate(x);
			else
			{
				__right_rotate(y);
				__left_rotate(x);
			}
		}
	}

	size_type count(const key_type& x) const;

};

__NS_END