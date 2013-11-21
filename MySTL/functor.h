#pragma once

#include "pair.h"

__NS_BEGIN

template <class Arg, class Result>
struct unary_function 
{
	typedef Arg argument_type;
	typedef Result result_type;
};

template <class Arg1, class Arg2, class Result>
struct binary_function 
{
	typedef Arg1 first_argument_type;
	typedef Arg2 second_argument_type;
	typedef Result result_type;
};      

template <class Tp>
struct identity : public unary_function<Tp,Tp> 
{
	const Tp& operator()(const Tp& x) const { return x; }
};

template <class Pair>
struct select1st : public unary_function<Pair, typename Pair::first_type> 
{
	const typename Pair::first_type& operator()(const Pair& x) const 
	{
		return x.first;
	}
};

template <class Pair>
struct select2nd : public unary_function<Pair, typename Pair::second_type>
{
	const typename Pair::second_type& operator()(const Pair& x) const 
	{
		return x.second;
	}
};

template <class Tp>
struct less : public binary_function<Tp,Tp,bool> 
{
	bool operator()(const Tp& x, const Tp& y) const { return x < y; }
};

template <class Tp>
struct greater : public binary_function<Tp,Tp,bool> 
{
	bool operator()(const Tp& x, const Tp& y) const { return x > y; }
};

template <class Tp>
struct equal : public binary_function<Tp,Tp,bool> 
{
	bool operator()(const Tp& x, const Tp& y) const { return x == y; }
};

__NS_END