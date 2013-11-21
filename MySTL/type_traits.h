#pragma once

#include "config.h"

__NS_BEGIN

struct true_type {};
struct false_type {};

template <typename T>
struct type_traits
{
	typedef false_type has_trivial_default_constructor;
	typedef false_type has_trivial_copy_constructor;
	typedef false_type has_trivial_assignment_operator;
	typedef false_type has_trivial_destructor;
	typedef false_type is_POD_type; // POD: Plain Old Data
};

template <class T>
struct type_traits<T*> 
{
	typedef true_type has_trivial_default_constructor;
	typedef true_type has_trivial_copy_constructor;
	typedef true_type has_trivial_assignment_operator;
	typedef true_type has_trivial_destructor;
	typedef true_type is_POD_type;
};

template <typename IsPod>
struct __type_traits_aux
{
	typedef false_type has_trivial_default_constructor;
	typedef false_type has_trivial_copy_constructor;
	typedef false_type has_trivial_assignment_operator;
	typedef false_type has_trivial_destructor;
	typedef false_type is_POD_type;
};

template <>
struct __type_traits_aux <false_type>
{
	typedef false_type has_trivial_default_constructor;
	typedef false_type has_trivial_copy_constructor;
	typedef false_type has_trivial_assignment_operator;
	typedef false_type has_trivial_destructor;
	typedef false_type is_POD_type;
};

template <>
struct __type_traits_aux <true_type>
{
	typedef true_type has_trivial_default_constructor;
	typedef true_type has_trivial_copy_constructor;
	typedef true_type has_trivial_assignment_operator;
	typedef true_type has_trivial_destructor;
	typedef true_type is_POD_type;
};

#define DEFINE_TYPE_TRAITS_FOR(T) \
	template <> struct type_traits<T> : __type_traits_aux<true_type> {}; \
	template <> struct type_traits<const T> : __type_traits_aux<true_type> {}; \
	template <> struct type_traits<volatile T> : __type_traits_aux<true_type> {}; \
	template <> struct type_traits<const volatile T> : __type_traits_aux<true_type> {}

DEFINE_TYPE_TRAITS_FOR(bool);
DEFINE_TYPE_TRAITS_FOR(char);
DEFINE_TYPE_TRAITS_FOR(signed char);
DEFINE_TYPE_TRAITS_FOR(unsigned char);
DEFINE_TYPE_TRAITS_FOR(wchar_t);
DEFINE_TYPE_TRAITS_FOR(short);
DEFINE_TYPE_TRAITS_FOR(unsigned short);
DEFINE_TYPE_TRAITS_FOR(int);
DEFINE_TYPE_TRAITS_FOR(unsigned int);
DEFINE_TYPE_TRAITS_FOR(long);
DEFINE_TYPE_TRAITS_FOR(unsigned long);
DEFINE_TYPE_TRAITS_FOR(float);
DEFINE_TYPE_TRAITS_FOR(double);
DEFINE_TYPE_TRAITS_FOR(long double);
DEFINE_TYPE_TRAITS_FOR(__int64);

// integer traits
template <class T> struct is_integer 
{
	typedef false_type integer;
};

template <class IsInteger>
struct __is_integer_aux
{
	typedef false_type integer;
};

template <>
struct __is_integer_aux <true_type>
{
	typedef true_type integer;
};

#define DEFINE_INTEGER_TYPE_FOR(T) \
	template <> struct is_integer<T> : __is_integer_aux<true_type> {}; \
	template <> struct is_integer<const T> : __is_integer_aux<true_type> {}; \
	template <> struct is_integer<volatile T> : __is_integer_aux<true_type> {}; \
	template <> struct is_integer<const volatile T> : __is_integer_aux<true_type> {}

DEFINE_INTEGER_TYPE_FOR(bool);
DEFINE_INTEGER_TYPE_FOR(char);
DEFINE_INTEGER_TYPE_FOR(signed char);
DEFINE_INTEGER_TYPE_FOR(unsigned char);
DEFINE_INTEGER_TYPE_FOR(wchar_t);
DEFINE_INTEGER_TYPE_FOR(short);
DEFINE_INTEGER_TYPE_FOR(unsigned short);
DEFINE_INTEGER_TYPE_FOR(int);
DEFINE_INTEGER_TYPE_FOR(unsigned int);
DEFINE_INTEGER_TYPE_FOR(long);
DEFINE_INTEGER_TYPE_FOR(unsigned long);

__NS_END
