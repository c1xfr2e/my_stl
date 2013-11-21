#pragma once

#include "hash_function.h"
#include "vector.h"
#include "algorithm.h"

__NS_BEGIN

template <class Tp>
struct __hashtable_node
{
	__hashtable_node* m_next;
	Tp m_value;
};

template <class Key, class Value, class HashFun, 
		  class ExtractKey, class EqualKey, class Alloc = type_allocator<__hashtable_node<Value> > >
class hashtable;

template <class Key, class Value, class HashFun, 
		  class ExtractKey, class EqualKey, 
		  class Alloc>
struct __hashtable_iterator 
{
	typedef forward_iterator_tag iterator_category;
	typedef Value value_type;
	typedef ptrdiff_t difference_type;
	typedef size_t size_type;
	typedef Value& reference;
	typedef Value* pointer;
	typedef hashtable<Key,Value,HashFun,ExtractKey,EqualKey,Alloc> Hashtable;
	typedef __hashtable_iterator<Key,Value,HashFun,ExtractKey,EqualKey,Alloc> iterator;
	typedef __hashtable_node<Value> Node;

	Node* m_cur;
	Hashtable* m_ht;

	__hashtable_iterator(Node* node = 0, Hashtable* table = 0) : m_cur(node), m_ht(table) {}
	reference operator*() const { return m_cur->m_value; }
	pointer operator->() const { return &(operator*()); }
	
	iterator& operator++()
	{
		Node* old = m_cur;
		m_cur = m_cur->m_next;
		if (!m_cur)
		{
			size_type index = m_ht->__value_2_bkt_index(old->m_value);
			for (++index; !m_cur && index < m_ht->m_buckets.size(); ++index)
				m_cur = m_ht->m_buckets[index];
		}
		return *this;
	}

	iterator operator++(int) { iterator tmp = this; ++*this; return tmp; }
	bool operator==(const iterator& it) const { return m_cur == it.m_cur; }
	bool operator!=(const iterator& it) const { return m_cur != it.m_cur; }
};

enum { __NUM_PRIMES = 28 };
static const unsigned long __prime_list[__NUM_PRIMES] =
{
	53ul,         97ul,         193ul,       389ul,       769ul,
	1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
	49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
	1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
	50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul, 
	1610612741ul, 3221225473ul, 4294967291ul
};

unsigned long __stl_next_prime(unsigned long n)
{
	const unsigned long* first = __prime_list;
	const unsigned long* last = __prime_list + (int)__NUM_PRIMES;
	const unsigned long* pos = lower_bound(first, last, n);
	return pos == last ? *(last - 1) : *pos;
}

template <class Key, class Value, class HashFun, 
		  class ExtractKey, class EqualKey, 
		  class Alloc>
class hashtable 
{
public:
	typedef Key key_type;
	typedef Value value_type;
	typedef HashFun hasher;
	typedef EqualKey key_equal;

	typedef size_t            size_type;
	typedef ptrdiff_t         difference_type;
	typedef value_type*       pointer;
	typedef const value_type* const_pointer;
	typedef value_type&       reference;
	typedef const value_type& const_reference;

	typedef __hashtable_node<Value> Node;
	typedef __hashtable_iterator<Key,Value,HashFun,ExtractKey,EqualKey,Alloc> iterator;

	friend struct __hashtable_iterator<Key,Value,HashFun,ExtractKey,EqualKey,Alloc>;

private:
	hasher			m_hash;
	key_equal		m_equal;
	ExtractKey		m_get_key;
	vector<Node*>	m_buckets;
	size_type		m_num_elements;

	Node* __get_node() { return Alloc::allocate(1); }
	void __put_node(Node* p) { Alloc::deallocate(p, 1); }
	Node* __new_node(const value_type& obj)
	{
		Node* n = __get_node();
		n->m_next = 0;
		construct(&n->m_value, obj);
		return n;
	}
	void __delete_node(Node* n)
	{
		destruct(&n->m_value);
		__put_node(n);
	}

public:
	hashtable(size_type n)
		: m_hash(HashFun()), m_equal(EqualKey()), m_get_key(ExtractKey()), m_num_elements(0)
	{ __initialize_buckets(n); }

	hashtable(size_type n, const HashFun& hf, const EqualKey& eql, const ExtractKey& ext)
		: m_hash(hf), m_equal(eql), m_get_key(ext), m_num_elements(0)
	{ __initialize_buckets(n); }

	hashtable(size_type n, const HashFun& hf, const EqualKey& eql)
		: m_hash(hf), m_equal(eql), m_get_key(ExtractKey()), m_num_elements(0)
	{ __initialize_buckets(n); }

	hashtable(const hashtable& ht)
		: m_hash(ht.m_hash), m_equal(ht.m_equal), m_get_key(ht.m_get_key), m_num_elements(0)
	{ __copy_from(ht); }

	hashtable& operator= (const hashtable& ht)
	{
		if (&ht != this) 
		{
			clear();
			m_hash = ht.m_hash;
			m_equal = ht.m_equal;
			m_get_key = ht.m_get_key;
			__copy_from(ht);
		}
		return *this;
	}

	~hashtable() { clear(); }

	hasher hash_funct() const { return m_hash; }
	key_equal key_eq() const { return m_equal; }

	size_type size() const { return m_num_elements; }
	size_type max_size() const { return size_type(-1); }
	bool empty() const { return size() == 0; }

	void swap(hashtable& ht)
	{
		::swap(m_hash, ht.m_hash);
		::swap(m_equal, ht.m_equal);
		::swap(m_get_key, ht.m_get_key);
		m_buckets.swap(ht.m_buckets);
		::swap(m_num_elements, ht.m_num_elements);
	}

	iterator begin()
	{ 
		for (size_type n = 0; n < m_buckets.size(); ++n)
			if (m_buckets[n])
				return iterator(m_buckets[n], this);
		return end();
	}
	iterator end() { return iterator(0, this); }

	size_type bucket_count() const { return m_buckets.size(); }
	size_type max_bucket_count() const { return __prime_list[(int)__NUM_PRIMES - 1]; } 
	size_type elements_in_bucket(size_type bucket) const
	{
		size_type result = 0;
		for (Node* cur = m_buckets[bucket]; cur; cur = cur->m_next)
			result += 1;
		return result;
	}

	pair<iterator, bool> insert_unique_noresize(const value_type& obj)
	{
		size_type bkt_index = __value_2_bkt_index(obj);
		Node* first = m_buckets[bkt_index];
		for (Node* cur = first; cur; cur = cur->m_next)
		{
			if (m_equal(m_get_key(first->m_value), m_get_key(obj)))
				return pair<iterator, bool>(iterator(0,this), false);
		}
		Node* new_node = __new_node(obj);
		new_node->m_next = first;
		m_buckets[bkt_index] = new_node;
		++m_num_elements;
		return pair<iterator, bool>(iterator(new_node,this), true);	
	}

	iterator insert_equal_noresize(const value_type& obj)
	{
		size_type bkt_index = __value_2_bkt_index(obj);
		Node* first = m_buckets[bkt_index];
		for (Node* cur = first; cur; cur = cur->m_next)
		{
			if (m_equal(m_get_key(cur->m_value), m_get_key(obj)))
			{
				Node* new_node = __new_node(obj);
				new_node->m_next = cur->m_next;
				cur->m_next = new_node;
				++m_num_elements;
				return iterator(new_node, this);
			}
		}
		Node* new_node = __new_node(obj);
		new_node->m_next = first;
		m_buckets[bkt_index] = new_node;
		++m_num_elements;
		return iterator(new_node,this);	
	}

	pair<iterator, bool> insert_unique(const value_type& obj)
	{
		resize(m_num_elements + 1);
		return insert_unique_noresize(obj);
	}

	template <class InputIterator>
	void insert_unique(InputIterator first, InputIterator last)
	{
		insert_unique(first, last, iterator_category(first));
	}
	template <class InputIterator>
	void insert_unique(InputIterator first, InputIterator last, input_iterator_tag)
	{
		for ( ; first != last; ++first)
			insert_unique(*first);
	}
	template <class ForwardIterator>
	void insert_unique(ForwardIterator first, ForwardIterator last, forward_iterator_tag)
	{
		size_type n = 0;
		distance(first, last, n);
		resize(m_num_elements + n);
		for ( ; n > 0; --n, ++first)
			insert_unique_noresize(*first);
	}

	iterator insert_equal(const value_type& obj)
	{
		resize(m_num_elements + 1);
		return insert_equal_noresize(obj);
	}
	template <class InputIterator>
	void insert_equal(InputIterator first, InputIterator last)
	{
		insert_equal(first, last, iterator_category(first));
	}

	template <class InputIterator>
	void insert_equal(InputIterator first, InputIterator last, input_iterator_tag)
	{
		for ( ; first != last; ++first)
			insert_equal(*first);
	}
	template <class ForwardIterator>
	void insert_equal(ForwardIterator first, ForwardIterator last, forward_iterator_tag)
	{
		size_type n = 0;
		distance(first, last, n);
		resize(m_num_elements + n);
		for ( ; n > 0; --n, ++first)
			insert_equal_noresize(*first);
	}

	iterator find(const key_type& key) 
	{
		size_type bkt_index = __key_2_bkt_index(key);
		Node* first;
		for (first = m_buckets[bkt_index]; 
			 first && !m_equal(m_get_key(first->m_value), key);
			 first = first->m_next)
		{}
		return iterator(first, this);
	} 

	size_type count(const key_type& key) const
	{
		const size_type bkt_index = __key_2_bkt_index(key);
		size_type result = 0;
		for (Node* cur = m_buckets[bkt_index]; cur; cur = cur->m_next)
		{
			if (m_equal(m_get_key(cur->m_value), key))
				++result;
		}
		return result;
	}

	pair<iterator, iterator> equal_range(const key_type& key);

	size_type erase(const key_type& key)
	{
		size_type bkt_index = __key_2_bkt_index(key);
		Node* first = m_buckets[bkt_index];
		size_type erase_count = 0;
		if (!first) return;
		Node* cur = first;
		Node* next = cur->m_next;
		while (next)
		{
			if (m_equal(m_get_key(next->m_value), key))
			{
				cur->m_next = next->m_next;
				__delete_node(next);
				next = cur->m_next;
				--m_num_elements;
				++erase_count;
			}
			else
			{
				cur = next;
				next = cur->m_next;
			}
		}
		if (m_equal(m_get_key(first->m_value), key))
		{
			m_buckets[bkt_index] = first->m_next;
			__delete_node(first);	
			--m_num_elements;
			++erase_count;
		}
		return erase_count;
	}

	void erase(const iterator& it)
	{
		if (it.m_ht != this) return;
		Node* target = it.m_cur;
		if (!target) return;
		size_type bkt_index = __value_2_bkt_index(*it);
		Node* cur = m_buckets[bkt_index];
		if (cur == target)
		{
			m_buckets[bkt_index] = first->m_next;
			__delete_node(cur);
			--m_num_elements;
		}
		else
		{
			Node* next = cur->m_next;
			while (next)
			{
				if (next == target)
				{
					cur->m_next = next->m_next;
					__delete_node(it.m_cur);
					--m_num_elements;
					break;
				}
				cur = next;
				next = cur->m_next;
			}
		}
	}

	void erase(iterator first, iterator last)
	{
		size_type first_bucket = first.m_cur ? 
			__value_2_bkt_index(first.m_cur->m_value) : m_buckets.size();
		size_type last_bucket = last.m_cur ?  
			__value_2_bkt_index(last.m_cur->m_value) : m_buckets.size();

		if (first.m_cur == last.m_cur)
			return;
		else if (first_bucket == last_bucket)
			__erase_bucket(first_bucket, first.m_cur, last.m_cur);
		else 
		{
			__erase_bucket(first_bucket, first.m_cur, 0);
			for (size_type m = first_bucket + 1; n < last_bucket; ++n)
				__erase_bucket(n, 0);
			if (last_bucket != m_buckets.size())
				__erase_bucket(last_bucket, last.m_cur);
		}
	}

	void resize(size_type num_elements_hint)
	{
		size_type old_bucket_size = m_buckets.size();
		if (num_elements_hint > old_bucket_size)
		{
			size_type new_bkt_size = __next_size(num_elements_hint);
			if (new_bkt_size > old_bucket_size)
			{
				vector<Node*> tmp(new_bkt_size, (Node*)0);
				for (size_type bucket = 0; bucket < old_bucket_size; ++bucket)
				{
					Node* first = m_buckets[bucket];
					while (first)
					{
						size_type new_bucket_index = __value_2_bkt_index(first->m_value, new_bkt_size);
						m_buckets[bucket] = first->m_next;
						first->m_next = tmp[new_bucket_index];
						tmp[new_bucket_index] = first;
						first = m_buckets[bucket];
					}
				}
				m_buckets.swap(tmp);
			}
		}
	}

	void clear()
	{
		for (size_type i = 0; i < m_buckets.size(); ++i) 
		{
			Node* cur = m_buckets[i];
			while (cur != 0) 
			{
				Node* next = cur->m_next;
				__delete_node(cur);
				cur = next;
			}
			m_buckets[i] = 0;
		}
		m_num_elements = 0;
	}

private:
	size_type __next_size(size_type n) const
	{ return __stl_next_prime(n); }

	void __initialize_buckets(size_type n)
	{
		const size_type n_buckets = __next_size(n);
		m_buckets.reserve(n_buckets);
		m_buckets.insert(m_buckets.end(), n_buckets, (Node*)0);
		m_num_elements = 0;
	}

	size_type __key_2_bkt_index(const key_type& key, size_t n) const 
	{ 
		return m_hash(key) % n; 
	}
	size_type __key_2_bkt_index(const key_type& key) const 
	{
		return __key_2_bkt_index(key, m_buckets.size()); 
	}

	size_type __value_2_bkt_index(const value_type& obj, size_t n) const 
	{ 
		return __key_2_bkt_index(m_get_key(obj), n); 
	}
	size_type __value_2_bkt_index(const value_type& obj) const 
	{ 
		return __key_2_bkt_index(m_get_key(obj)); 
	}

	void __erase_bucket(const size_type n, Node* first, Node* last)
	{
		Node* cur = m_buckets[n];
		if (cur == first)
			__erase_bucket(n, last);
		else 
		{
			Node* next;
			for (next = cur->m_next; next != first; cur = next, next = cur->m_next)
				;
			while (next != last) 
			{
				cur->m_next = next->m_next;
				__delete_node(next);
				next = cur->m_next;
				--m_num_elements;
			}
		}
	}
	void __erase_bucket(const size_type n, Node* last)
	{
		Node* cur = m_buckets[n];
		while (cur != last) 
		{
			Node* next = cur->m_next;
			__delete_node(cur);
			cur = next;
			m_buckets[n] = cur;
			--m_num_elements;
		}
	}

	void __copy_from(const hashtable& ht)
	{
		clear();
		m_buckets.reserve(ht.m_buckets.size());
		m_buckets.insert(m_buckets.end(), m_buckets.size(), (Node*)0);
		for (size_type bucket = 0; bucket < ht.m_buckets.size(); ++bucket)
		{
			Node* first = ht.m_buckets[i];
			if (first)
			{
				Node* last = __new_node(first->m_value);
				m_buckets[i] = last;
				for (first = first->m_next; first; first = first->m_next;)
				{
					last->m_next = __new_node(first->m_value);
					last = last->m_next;
				}
			}
		}
		m_num_elements = ht.m_num_elements;
	}
};

__NS_END
