#pragma once

class UF
{
public:
	UF(int N)
	{
		m_count = N;
		m_id = new int[N];
		m_sz = new int[N];
		for (int i = 0; i < N; ++i)
		{
			m_id[i] = i;
			m_sz[i] = 1;
		}
	}
	~UF()
	{
		delete[] m_id;
		delete[] m_sz;
	}

	int get_count() { return m_count; }

	int find(int p)
	{
		while (m_id[p] != p)
			p = m_id[p];
		return p;
	}

	bool is_connected(int p, int q)
	{
		return (find(p) == find(q));
	}

	void connect(int p, int q)
	{
		p = find(p);
		q = find(q);
		if (p == q)
			return;
		if (m_sz[p] < m_sz[q])
		{
			m_id[p] = q;
			m_sz[q] += m_sz[p];
		}
		else
		{
			m_id[q] = p;
			m_sz[p] += m_sz[q];
		}
		m_count--;
	}

private:
	int* m_id;
	int* m_sz;
	int m_count;
};
