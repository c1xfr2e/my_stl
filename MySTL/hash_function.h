#pragma once

#include <stddef.h>

__NS_BEGIN

template <class Key> struct hash {};

size_t __stl_hash_string(const char* s)
{
	size_t h = 0; 
	for ( ; *s; ++s)
		h = 5 * h + *s;
	return h;
}

template<> struct hash<char*>
{
	size_t operator()(const char* s) const { return __stl_hash_string(s); }
};

template<> struct hash<const char*>
{
	size_t operator()(const char* s) const { return __stl_hash_string(s); }
};

template <class Tp>
struct __default_hash
{
	size_t operator()(Tp x) const { return x; }
};

#define DEFINE_DEFAULT_HASH_FOR(T) \
	template <> struct hash<T> : __default_hash<T> {}; \
	template <> struct hash<const T> : __default_hash<T> {}; \
	template <> struct hash<volatile T> :__default_hash<T> {}; \
	template <> struct hash<const volatile T> : __default_hash<T> {}

DEFINE_DEFAULT_HASH_FOR(char);
DEFINE_DEFAULT_HASH_FOR(signed char);
DEFINE_DEFAULT_HASH_FOR(unsigned char);
DEFINE_DEFAULT_HASH_FOR(wchar_t);
DEFINE_DEFAULT_HASH_FOR(short);
DEFINE_DEFAULT_HASH_FOR(unsigned short);
DEFINE_DEFAULT_HASH_FOR(int);
DEFINE_DEFAULT_HASH_FOR(unsigned int);
DEFINE_DEFAULT_HASH_FOR(long);
DEFINE_DEFAULT_HASH_FOR(unsigned long);

__NS_END
