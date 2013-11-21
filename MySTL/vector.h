#pragma once

#include <stddef.h>
#include "algo_base.h"
#include "allocator.h"
#include "initialize.h"

__NS_BEGIN

template <class T, class Alloc = type_allocator<T> >
class vector
{
public:
	// type defines
	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	
	typedef T* iterator;
	typedef const T* const_iterator;

	// initialize
	vector(void);
	vector(size_type n, const T& x);
	explicit vector(size_type n);
	vector(const vector<T,Alloc>& other);
	template <class InputIterator>
	vector(InputIterator first, InputIterator last);

	// destruct
	~vector();

	// query
	iterator begin();
	const_iterator begin() const;
	iterator end();
	const_iterator end() const;
	size_type size() const;
	size_type capacity() const;
	bool empty() const;
	reference front();
	const_reference front() const;

	// insert
	void insert(iterator pos, const T& x);
	void push_back(const T& x);
	void push_front(const T& x); 
	void insert(iterator pos, size_type n, const T& x);
	template <class InputIterator>
	void insert(iterator pos, InputIterator first, InputIterator last);

	// erase
	iterator erase(iterator pos);
	iterator erase(iterator first, iterator last);
	void pop_back();
	void clear();
	
	// resize
	void resize(size_type new_sz, const T& x);
	void resize(size_type new_sz);
	void reserve(size_type n);

	// assign
	void assign(size_type n, const T& x);
	template <class InputIterator>
	void assign(InputIterator first, InputIterator last);
	vector<T, Alloc>& operator=(const vector<T, Alloc>& other);

	// swap
	void swap(vector<T, Alloc>& other);

	T& operator[](size_type index);
	const T& operator[](size_type index) const;

private:
	T* m_start;
	T* m_finish;
	T* m_end_of_storage;

	// initialize ¸¨Öúº¯Êý
	void __fill_initialize(size_type n, const T& x);
	template <class InputIterator>
	void __range_initialize(InputIterator first, InputIterator last, input_iterator_tag);
	template <class ForwardIterator>
	void __range_initialize(ForwardIterator first, ForwardIterator last, forward_iterator_tag);
	template <class InputIterator>
	void __initialize_aux(InputIterator first, InputIterator last, true_type);
	template <class InputIterator>
	void __initialize_aux(InputIterator first, InputIterator last, false_type);

	// insert ¸¨Öúº¯Êý
	void __fill_insert(iterator pos, size_type n, const T& x);
	template <class InputIterator>
	void __range_insert(iterator pos, InputIterator first, InputIterator last, input_iterator_tag);
	template <class ForwardIterator>
	void __range_insert(iterator pos, ForwardIterator first, ForwardIterator last, forward_iterator_tag);
	template <class InputIterator>
	void __insert_aux(iterator pos, InputIterator first, InputIterator last, true_type);
	template <class InputIterator>
	void __insert_aux(iterator pos, InputIterator first, InputIterator last, false_type);

	// assign ¸¨Öúº¯Êý
	void __fill_assign(size_type n, const T& x);
	template <class InputIterator>
	void __range_assign(InputIterator first, InputIterator last, input_iterator_tag);
	template <class ForwardIterator>
	void __range_assign(ForwardIterator first, ForwardIterator last, forward_iterator_tag);
	template <class InputIterator>
	void __assign_aux(InputIterator first, InputIterator last, true_type);
	template <class InputIterator>
	void __assign_aux(InputIterator first, InputIterator last, false_type);
};

#include "vector_impl.h"

__NS_END