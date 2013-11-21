#pragma once

#include "algo_base.h"
#include "stack.h"

__NS_BEGIN

template<class Iterator>
int partition(Iterator a, int lo, int hi)
{
	int i = lo;
	int j = hi + 1;
	while (true)
	{
		while (i < hi && a[++i] < a[lo]) {}
		while (j > lo && a[--j] > a[lo]) {}
		if (!(i < j))
			break;
		swap(a[i], a[j]);
	}
	swap(a[lo], a[j]);
	return j;
}

template<class Iterator>
void quick_sort_recursive(Iterator a, int lo, int hi)
{
	if (hi <= lo) return;
	int mid = partition(a, lo, hi);
	quick_sort_recursive(a, lo, mid - 1);
	quick_sort_recursive(a, mid + 1, hi);
}

template<class Iterator>
void quick_sort_nonrecursive(Iterator a, int lo, int hi)
{
	stack<pair<int, int> > s;
	int mid = 0;
	while (lo < hi || !s.empty())
	{
		if (lo < hi)
		{
			int mid = partition(a, lo, hi);
			s.push(make_pair(mid, hi));
			hi = mid - 1;
		}
		else
		{
			pair<int, int> d = s.top();
			s.pop();
			lo = d.first + 1;
			hi = d.second;
		}
	}
}

__NS_END