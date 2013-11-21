// MySTL.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "list.h"
#include "SeqST.h"
#include "pair.h"
#include "avltree.h"
#include "functor.h"
#include "hash_function.h"
#include "hash_table.h"
#include <string>
#include <fstream>
#include <iostream>
#include <hash_map>
#include <map>
#include "slist.h"
#include "graph.h"
#include "deque.h"
#include "queue.h"
#include "index_priority_queue.h"
#include "quick_sort_nonrecursive.h"
#include <assert.h>
#include <set>

using MySTL::hashtable;
using MySTL::hash;
using MySTL::identity;
using MySTL::less;
using MySTL::pair;
using MySTL::equal;
using MySTL::select1st;
using MySTL::slist;
using MySTL::deque;
using MySTL::queue;
using MySTL::priority_queue;
using MySTL::index_priority_queue;

typedef MySTL::AVL_tree<int,int,identity<int>,less<int> > tree;

void print_node(tree::AVL_tree_node& x)
{
	printf("%d ", x.m_value);
}

template <> 
struct hash<std::string>
{
	size_t operator()(const std::string& s) const { return __stl_hash_string(s.c_str()); }
};

int power(int x, int n)
{
	int r = (n & 1)? x : 1;
	n >>= 1;
	while (n != 0) 
	{
		x = x * x;
		if ((n & 1) != 0)
			r = r * x;
		n >>= 1;
	}
	return r;
}




int main(int argc, char* argv[])
{
  std::set<int> si;
  si.insert(1);
  si.insert(2);
  si.insert(3);
  si.insert(4);
  si.insert(5);

  std::set<int>::iterator setit = si.find(3);
  int sample = *setit;
  *setit = 100;

	int kkkk = power(2, 1);
	int k222 = power(2, 4);
	int k333 = power(2, 5);
	int i = 1;
	if (i == 0)
	{
		printf("i==0\n");
	}

// 
// 	int N = 10000000;
// 	int *a = new int[10000000];
// 	srand((unsigned int)time(NULL));
// 	for (int i = 0; i < N; ++i)
// 	{
// 		int n = rand() % N + 1;
// 		a[i] = n;
// 	}
// 
// 	MySTL::quick_sort_nonrecursive(a, 0, N - 1);
// 	for (int i = 1; i < N; ++i)
// 	{
// 		assert(a[i - 1] <= a[i]);
// 	}
// 	delete[] a;
// 

	std::ifstream tinyG;
	tinyG.open("../data/tinyEWD.txt");  
	int E, V;
	tinyG >> V;
	tinyG >> E;
	graph g(V, true);
	int v, w;
	float weight;
	while (tinyG >> v >> w >> weight)
	{
		printf("%f\n", weight);
		g.add_edge(v, w, weight);
	}
	
	//PrimMST prim(g);
	//KruskalMST kruskal(g);
	DijkstraSP Dijkstra(g);

	Search search(g);
	search.BFS(0);
	for (int i = 0; i < g.num_vertices(); ++i)
	{
		if (search.marked(i))
			printf("%d ", i);
	}
	printf("\n");
	if (search.count() != g.num_vertices())
		printf("NOT ");
	printf("connected\n");

	for (int i = 0; i < g.num_vertices(); ++i)
	{
		vector<int> path = search.path_to(0, i);
		printf("%d to %d: ", 0, i);
		for (size_t j = 0; j < path.size(); ++j)
		{
			printf("%d",path[j]);
			if (j < path.size() - 1)
				printf("-");
		}
		printf("\n");
	}

	tree t1;
	t1.insert_unique(9);
	t1.insert_unique(3);
	t1.insert_unique(18);
	t1.insert_unique(0);
	t1.insert_unique(6);
	t1.insert_unique(10);
	t1.insert_unique(12);
	t1.insert_unique(15);
	t1.insert_unique(20);
	t1.insert_unique(2);
	t1.insert_unique(5);
	t1.insert_unique(11);                              
	t1.insert_unique(8);
	t1.insert_unique(13);

	t1.travel(print_node);

	typedef pair<std::string,int> ValueType;
	typedef hashtable<std::string,ValueType,hash<std::string>,
					  select1st<ValueType>,equal<std::string> > 
					  Hashtable;
	Hashtable ht(10000);

	typedef stdext::hash_map<std::string,int> std_hash_map;
	std_hash_map ht2;

	std::ifstream input;
	input.open("../data/tale.txt");
	std::string word;
	int max_count = 0;
	std::string max_word;
	int word_count = 0;
 
 	while (input)
 	{
 		input >> word;
 		Hashtable::iterator it = ht.find(word);
 		if (it == ht.end())
 		{
 			ht.insert_unique(ValueType(word, 1));
 		}
 		else
 		{
 			int count = (*it).second + 1;
 			(*it).second = count;
 			if (count > max_count)
 			{
 				max_count = count;
 				max_word = (*it).first;
 			}
 		}
		++word_count;
 	}

	printf("words count : %d\n", word_count);
	printf("unique words : %d\n", ht.size());
	printf("%s %d\n", max_word.c_str(), max_count);
	Hashtable::iterator it = ht.find("executive");
	if (it != ht.end())
		printf("%s %d\n", (*it).first.c_str(), (*it).second);

	
	typedef std::map<size_t,size_t> int_map;
	int_map bkt_length;
		
	float everage_length = (float)ht.size() / (float)ht.bucket_count();
	printf("bucket count : %d\n", ht.bucket_count());
	printf("everage length : %f\n", everage_length);

	size_t max_buckets = 0;
	size_t used_buckets = 0;
	for (size_t i = 0; i < ht.bucket_count(); ++i)
	{
		size_t c = ht.elements_in_bucket(i);
		if (c != 0)
			++used_buckets;
		if (c > max_buckets)
			max_buckets = c;
		int_map::iterator it = bkt_length.find(c);
		if (it == bkt_length.end())
		{
			bkt_length.insert(std::make_pair(c, 1));
		}
		else
		{
			(*it).second = (*it).second + 1;
		}
		
	}
	printf("max bucket : %d\n", max_buckets);
	printf("used bucket : %d\n", used_buckets);
	for (int_map::iterator it = bkt_length.begin();
		 it != bkt_length.end();
		 ++it)
	{
		printf("%d : %d\n", (*it).first, (*it).second);
	}


// 
// 	int unique_count = ht.size();
// 	Hashtable::iterator it = ht.begin();
// 	for ( ; it != ht.end(); ++it)
// 	{
// 		
// 		printf("%s %d\n", (*it).first.c_str(), (*it).second);
// 	}
// 	printf("\n");

	MySTL::SeqST<int,int> seq;
	seq.put(7, 7);
	seq.put(5, 5);
	seq.put(3, 3);
	seq.put(3, 3);
	seq.put(2, 2);
	seq.put(4, 4);
	seq.put(40, 40);


	printf("%d\n", seq.size(30,50));

	return 0;
}