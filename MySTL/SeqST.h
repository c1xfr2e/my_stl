
#include "list.h"
#include "pair.h"

__NS_BEGIN

template <class Key, class Value>
class SeqST : public list<pair<Key,Value> >
{
private:
	typedef pair<Key,Value> ElemType;

public:
	void put(const Key& key, const Value& value)
	{
		iterator p = begin();
		while (p != end() && (*p).first < key)
			++p;
		insert(p, ElemType(key, value));		
	}
	iterator get(const Key& key)
	{
		iterator p = begin();
		while (p != end() && (*p).first != key)
			++p;
		return p;
	}
	iterator floor(const Key& key)
	{
		iterator ret = end();
		iterator p = begin();
		while (p != end() && !(key < (*p).first))
		{
			ret = p;
			p++;
		}
		return ret;
	}
	iterator ceiling(const Key& key)
	{
		iterator p = begin();
		while (p != end() && (*p).first < key)
			p++;
		return p;
	}
	size_type rank(const Key& key)
	{
		size_type r = 0;
		iterator p = begin();
		while (p != end() && (*p).first < key)
		{
			++r;
			++p;
		}
		return r;
	}
	iterator select(size_type r)
	{
		iterator p = begin();
		while (p != end() && r > 0)
		{
			++p;
			--r;
		}
		return p;
	}
	size_type size(const Key& lo, const Key& hi)
	{
		if (hi < lo)
			return 0;
		iterator left = begin();
		while (left != end() && (*left).first < lo)
			++left;
		if (left == end())
			return 0;
		iterator right = --end();
		while (right != end() && hi < (*right).first)
			--right;
		if (right == end())
			return 0;
		return distance(left, right) + 1;
	}
};

__NS_END