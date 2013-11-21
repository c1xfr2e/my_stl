#pragma once

#include "vector.h"
#include "slist.h"
#include "queue.h"
#include "index_priority_queue.h"
#include "float.h"
#include "union_find.h"

using MySTL::slist;
using MySTL::vector;
using MySTL::queue;
using MySTL::index_priority_queue;
using MySTL::greater;
using MySTL::priority_queue;

struct edge
{
	edge(int v = -1, int w = -1, float wei = 0.0f)
		: from(v), to(w), weight(wei) 
	{}
	int from;
	int to;
	float weight;
	bool operator> (const edge& x) const
	{
		return weight > x.weight;
	}
};

class graph
{
private:
	typedef slist<edge> AdjacencyList;
	typedef vector<AdjacencyList*> AdjacencyListArray;
	int m_num_vertices;
	int m_num_edges;
	AdjacencyListArray m_adjacency_lists;
	bool m_directed;

public:
	graph(int v, bool directed = false)
		: m_num_vertices(v),
		  m_num_edges(0),
		  m_adjacency_lists(v),
		  m_directed(directed)
	{
		for (int i = 0; i < v; ++i)
			m_adjacency_lists[i] = new AdjacencyList;
	}
	~graph()
	{
		for (int i = 0; i < m_num_vertices; ++i)
			delete m_adjacency_lists[i];
	}	

	int num_vertices() const { return m_num_vertices; }
	int num_edges() const { return m_num_edges; }
	void add_edge(int v, int w, float weight = 0.0f)
	{
		m_adjacency_lists[v]->push_front(edge(v, w, weight));
		if (!m_directed)
			m_adjacency_lists[w]->push_front(edge(w, v, weight));
		m_num_edges++;
	}

	typedef AdjacencyList::iterator AdjacencyIterator;
	AdjacencyIterator adjacency_begin(int v)
	{
		return m_adjacency_lists[v]->begin();
	}
	AdjacencyIterator adjacency_end(int v)
	{
		return m_adjacency_lists[v]->end();
	}

	vector<edge> get_all_edge()
	{
		vector<edge> edges;
		for (int i = 0; i < m_num_vertices; i++)
		{
			AdjacencyIterator it = m_adjacency_lists[i]->begin();
			for ( ; it != m_adjacency_lists[i]->end(); ++it)
			{
				if (m_directed || (*it).to > (*it).from)
					edges.push_back(*it);
			}
		}
		return edges;
	}
};

class Search
{
private:
	bool* m_marked;
	int m_count;
	int *edge_to;
	graph& m_graph;
public:
	Search(graph& m_graph) 
		: m_graph(m_graph), m_count(0), m_marked(0)
	{
		m_marked = new bool[m_graph.num_vertices()];
		for (int i = 0; i < m_graph.num_vertices(); ++i)
			m_marked[i] = false;
		edge_to = new int[m_graph.num_vertices()];
		for (int i = 0; i < m_graph.num_vertices(); ++i)
			edge_to[i] = -1;
	}
	~Search() { delete[] m_marked; delete[] edge_to;}

	void DFS(int v)
	{
		m_marked[v] = true;
		m_count++;
		graph::AdjacencyIterator adj_iter = m_graph.adjacency_begin(v);
		for ( ; adj_iter != m_graph.adjacency_end(v); ++adj_iter)
		{
			if (!m_marked[(*adj_iter).to])
			{
				edge_to[(*adj_iter).to] = v;
				DFS((*adj_iter).to);
			}
		}
	}	

	void BFS(int s)
	{
		queue<int> q;
		m_marked[s] = true;
		q.push(s);
		while (!q.empty())
		{
			int v = q.front();
			q.pop();
			graph::AdjacencyIterator adj_iter = m_graph.adjacency_begin(v);
			for ( ; adj_iter != m_graph.adjacency_end(v); ++adj_iter)
			{
				if (!m_marked[(*adj_iter).to])
				{
					int w = (*adj_iter).to;
					edge_to[w] = v;
					m_marked[w] = true; 
					q.push(w);
				}
			}
		}
	}

	bool marked(int v) { return m_marked[v]; }
	int count() { return m_count; }
	vector<int> path_to(int s, int v)
	{
		vector<int> path;
		if (marked(v))
		{
			for (int i = v; i != s; i = edge_to[i])
				path.push_front(i);
			path.push_front(s);
		}
		return path;	
	}
};

class PrimMST
{
private:
	vector<bool> m_marked;
	vector<float> m_dist_to;
	vector<int> m_edge_to;
	index_priority_queue<float, greater<float> > m_crossing_edge;
	graph& m_graph;

	void __visit(int v)
	{
		m_marked[v] = true;
		for (graph::AdjacencyIterator it = m_graph.adjacency_begin(v);
			 it != m_graph.adjacency_end(v);
			 ++it)
		{
			int w = (*it).to;
			if (m_marked[w])
				continue;
			if ((*it).weight < m_dist_to[w])
			{
				m_edge_to[w] = v;
				m_dist_to[w] = (*it).weight;
				if (m_crossing_edge.contain(w))
					m_crossing_edge.change(w, (*it).weight);
				else
					m_crossing_edge.insert(w, (*it).weight);
			}
		}
	}

public:
	PrimMST(graph& g)
		: m_graph(g),
		  m_marked(g.num_vertices(), false),
		  m_dist_to(g.num_vertices(), FLT_MAX),
		  m_edge_to(g.num_vertices(), -1),
		  m_crossing_edge(g.num_vertices())
	{
		m_crossing_edge.insert(0, 0.0f);
		while (!m_crossing_edge.empty())
			__visit(m_crossing_edge.pop_min());

		for (size_t i = 1; i < m_edge_to.size(); i++)
		{
			printf("%d %d\n", i, m_edge_to[i]);
		}
	}
};

class KruskalMST
{
private:
	vector<bool> m_marked;
	vector<edge> m_mst_edge;
	priority_queue<edge, vector<edge>, greater<edge> > m_edge_queue;
	graph& m_graph;
	UF m_uf;
public:
	KruskalMST(graph& g) 
	: m_graph(g), m_marked(g.num_vertices(), false), m_uf(g.num_vertices()),
	  m_edge_queue(g.get_all_edge())
	{
		while (!m_edge_queue.empty() && m_mst_edge.size() < (size_t)g.num_vertices())
		{
			edge e = m_edge_queue.top();
			m_edge_queue.pop();
			if (m_uf.is_connected(e.from, e.to))
				continue;
			m_uf.connect(e.from, e.to);
			m_mst_edge.push_back(e);
		}

		for (size_t i = 0; i < m_mst_edge.size(); i++)
		{
			printf("%d %d %f\n", m_mst_edge[i].from, m_mst_edge[i].to,
				m_mst_edge[i].weight);
		}
	}
	
};

class DijkstraSP
{
private:
	vector<float> m_dist_to; // 当前找到的最短距离
	vector<edge> m_edge_to; // 最短路径上的最后一条边
	graph& m_graph;
	index_priority_queue<float, greater<float> > m_edge_queue;

	void __relax(int v)
	{
		for (graph::AdjacencyIterator it = m_graph.adjacency_begin(v);
			 it != m_graph.adjacency_end(v);
			 ++it)
		{
			int w = (*it).to;
			if (m_dist_to[w] > m_dist_to[v] + (*it).weight)
			{
				m_dist_to[w] = m_dist_to[v] + (*it).weight;
				m_edge_to[w] = *it;
				if (m_edge_queue.contain(w))
					m_edge_queue.change(w, m_dist_to[w]);
				else 
					m_edge_queue.insert(w, m_dist_to[w]);
			}
		}
	}

public:
	DijkstraSP(graph& g) 
		: m_graph(g), m_dist_to(g.num_vertices(), FLT_MAX), m_edge_to(g.num_vertices()),
		  m_edge_queue(g.num_vertices())
	{
		m_dist_to[0] = 0.0f;
		m_edge_queue.insert(0, 0.0f);
		while (!m_edge_queue.empty())
		{
			__relax(m_edge_queue.pop_min());
		}
	}

};