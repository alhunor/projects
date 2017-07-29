#ifndef _GRAPH_H_
#define _GRAPH_H_

//vocabulary: vertex (csomopont), edge (él)


#include "vector.h"
#include "matrix.h"
#include <fstream>
#include <queue>
#include <stack>
#include <list>
#include <utility>
#include "myStuff/heap.h"


//#include <limits.h>

//std::vector<int>::const_iterator ci;

class disjointSet
{ // follows: tarjan - Efficiency of a Good But Not Linear Set Union Algorithm (1975)
public:
	explicit disjointSet(int n) : nb(n), sets(n), rank(n)
	{
		for (int i = 0; i < nb; ++i)
		{
			sets[i] = i;
			rank[i] = 1;
		}
	}
	int find(int k)
	{	// implements path compression
		if (sets[k] == k) return k;
		sets[k] = find(sets[k]);
		return sets[k];
	}

	void merge(int x, int y)
	{
		// points the lowest lowest ranked tree into the highest rank
		int xx = find(x);
		int yy = find(y);
		
		// point xx to the highest rank
		if (rank[xx] < rank[yy]) { std::swap(xx, yy); }
		
		// increase rank if both have equal rank
		if (rank[xx] == rank[yy]) rank[xx]++;
		
		// point lowest rank tree to higher one
		sets[yy] = xx;
	}

protected:
	int nb;
	std::vector<int> sets;
	std::vector<int> rank;
};



template <class underlying>
class const_iterator
{
public:
	const_iterator(int vertex, int neighbour, const underlying& _und) : self(vertex), current(neighbour), und(_und) { }
	const_iterator<underlying>& operator++();
	bool operator!=(const_iterator<underlying>& lhs)
	{ 
		bool b = self != lhs.self || current != lhs.current;
		return b; 
	}
	int operator*() { return current; }
protected:
	int self, current;
	const underlying& und;
};

template <typename EdgeCost = double>
class adjacencyMatrix // implements directed graphs
{
public:
	adjacencyMatrix() = delete;
	adjacencyMatrix(unsigned int _nbVerices) : nbVerices(_nbVerices), mat(NULL)
	{
		init(_nbVerices);
	} // adjacencyMatrix(unsigned int _nbNodes) : nbNodes(_nbNodes), a(nbNodes, nbNodes) 
	void clear() 
	{
		if (mat) delete mat;
		nbVerices = 0;
		nbEdges = 0;
	}

	void init(unsigned int _nbVerices)
	{
		clear();
		nbVerices = _nbVerices;
		mat = new HuMatrix<int>(nbVerices, nbVerices);
		/*
		int* dataPtr = mat->dataptr();
		memset(dataPtr, 0, nbVerices* nbVerices);
		*/
		int i, j;
		for (i = 0; i < nbVerices; ++i)
		{
			for (j = 0; j < nbVerices; ++j)
			{
				(*mat)(i, j) = 0;
			}
		}
	} //void init(unsigned int _nbVerices)

	bool load(const char* fileName) // loads graph from adjacency matrix file
	{
		std::ifstream in;
		in.open(fileName);
		string filetype;
		in >> nbVerices >> filetype;
		init(nbVerices);
		if (filetype == "AdjacencyMatrix")
		{
			int i, j, cost;
			for (i = 0; i < nbVerices; ++i)
			{
				for (j = 0; j < nbVerices; ++j)
				{
					in >> cost;
					(*mat)(i, j) = cost;
					if (cost != 0)
					{
						++nbEdges;
					}
				}
			}
		} else if (filetype == "EdgeList")
		{
			int i, j, cost;
			while (!in.eof())
			{
				in >> i >> j >> cost;
				(*mat)(i, j) = cost;
				++nbEdges;
			}
		} else return false;
		in.close();
		return true;
	} //bool load(const char* fileName)


	int edge_cost(int source, int dest)
	{
		return (*mat)(source, dest);
	}

	int valence(int v) // returns valence of vertex v 
	{ 
		int val = 0;
		for (int i = 0; i < nbVerices; ++i)
		{
			if (*(mat)(v, i) != 0)
			{
				++val;
			}
		}
		return val;
	}


	void insertEdge(unsigned int i, unsigned int j, EdgeCost cost = 0) // inserts directed edge i->j
	{
		(*mat)(i, j) = cost;
	}

	const_iterator<adjacencyMatrix<EdgeCost> > end(int vertex)
	{
		const_iterator<adjacencyMatrix<EdgeCost> > ci(vertex, nbVerices, *this);
		return ci;
	}

	const_iterator<adjacencyMatrix<EdgeCost> > begin(int vertex)
	{
		int i;
		for (i = 0; i < nbVerices; ++i)
		{
			if ((*mat)(vertex, i) != 0) break;
		}
		const_iterator<adjacencyMatrix<EdgeCost> > ci(vertex, i, *this);
		return ci;
	}

// change it to protected later
	int nbVerices, nbEdges;
	HuMatrix<int>* mat;
protected:
};

//template <typename InfoType = double>
class adjacencyList
{
public:
	adjacencyList() = delete;
	adjacencyList(unsigned int _nbVerices) : nbVerices(_nbVerices)
	{
		connections = new LightVector<unsigned int>[nbVerices];
		for (int i = 0; i < nbVerices; ++i)
		{
			connections[i].setSize(1); // check if it reserves space or set size as well
			//connections[i].upsize(1);
		}
	}
	int valemnce(int v) { return connections[v].size(); }
	void clear()
	{
		if (nbVerices == 0) return;
		delete[] connections;
		connections = NULL;
		//delete[] EdgeInfo;
	}
	void insertEdge(unsigned int i, unsigned int j) // inserts directed edge i->j
	{
		//if (connections[i].contains(j) >= 0) return;
		connections[i].push_back(j);
		// save cost of the edge i->j
	}

protected:
	LightVector<unsigned int>* connections; // connections[a] contains all nodes b, such as (a, b) is an edge a->b
	//LightVector<double>* weight; // EdgeInfo[min(a,b)] contains the number of vertex connecting a to b.
	int nbVerices;
};



const_iterator<adjacencyMatrix<int> >& const_iterator<adjacencyMatrix<int> >::operator++()
{
	HuMatrix<int>& m = (*und.mat);
	for (++current; current < und.nbVerices; ++current)
	{
		if (m(self, current) != 0) break;
	}
	return *this;
}

struct EdgeT
{
	int start, end; // starting and final vertex
	int cost;
	bool operator<(const EdgeT& lhs) { return cost < lhs.cost; }
};

template <typename InfoType, class underlying = adjacencyMatrix<int> >
class Graph
{
public:
	Graph() : Graph(0) {}
	Graph(int _nbNodes) : und(_nbNodes), hasEdgeList(false) {}
	~Graph()
	{
		und.clear();
	}

	const_iterator<underlying> begin(int vertex)
	{
		return und.begin(vertex);
	}

	const_iterator<underlying> end(int vertex)
	{
		return und.end(vertex);
	}

	int edge_cost(int source, int dest)
	{
		return und.edge_cost(source, dest);
	}

	void insertEdge(unsigned int a, unsigned int b)
	{
		und->insertEdge(a, b);
		hasEdgeList = false; // change it to false as information is potentially outdated. Can be optimised tu update instead of total rebuild
	}

	void buildEdgeList()
	{
		edgeList.clear();
		// iterate through all vertices and edges
		int dest;// , i(0);
		unsigned int  nbVerices = getNbVerices();
		for (unsigned int v = 0; v < nbVerices; ++v)
		{
			for (const_iterator<adjacencyMatrix<int> > ci = begin(v); ci != end(v); ++ci)
			{
				dest = *ci;
				EdgeT edge;
				edge.start = v;
				edge.end = dest;
				edge.cost = edge_cost(v, dest);
				edgeList.push_back(edge);
			}
		}
		hasEdgeList = true;
	}

// basic package

	void bfs(int v) // breadth first search starting from vertex v
	{
		std::queue<int> q;
		vector<int> visited;
		int nbVerices = getNbVerices();
		visited.resize(nbVerices);
		int i;
		for (i = 0; i < nbVerices; ++i)
		{
			visited[i] = false;
		} 
		visited[v] = true;
		q.push(v);
		int inqueue = 1;
		while (! q.empty())
		{
			v = q.front(); q.pop();
			cout << v << "  ";
			// get all neighbours of "v";
			
			for (const_iterator<adjacencyMatrix<int> > ci = begin(v); ci !=end(v); ++ci)
			{
				i = *ci;
				if (!visited[i])
				{
					visited[i] = true;
					q.push(i);
					++inqueue;
				}
			}
			if (inqueue == nbVerices)
			{
				while (!q.empty())
				{
					v = q.front(); q.pop();
					cout << v << "  ";
				}
				break;
			}
		}
		cout << endl;
	} // void bfs(int v) 

	void dfs(int v) // depth first search starting from vertex v
	{
		std::stack<int> st;
		vector<int> visited;
		int nbVerices = getNbVerices();
		visited.resize(nbVerices);
		int i;
		for (i = 0; i < nbVerices; ++i)
		{
			visited[i] = false;
		}
		st.push(v);

		while (!st.empty())
		{
			v = st.top(); st.pop();
			if (visited[v]) continue;
			visited[v] = true;
			cout << v << "  ";
			// get all neighbours of "v";

			for (const_iterator<adjacencyMatrix<int> > ci = begin(v); ci != end(v); ++ci)
			{
				i = *ci;
				if (!visited[i])
				{
					st.push(i);
				}
			}
		} // while (!st.empty())
		cout << endl;
	} // void dfs(int v) 

	void prim(int v) // Prim'algorithm starting from vertex v
	{
		cout << "Prim (array)";
		int nbVerices = getNbVerices();
		int* prio = new int[nbVerices];
		int* parent = new int[nbVerices];
		bool* inMST = new bool[nbVerices];
		int i;
		int togo = nbVerices-1; // number of vertices to be still processed
		for (i = 0; i < nbVerices; ++i)
		{
			prio[i] = INT_MAX;
			inMST[i] = false; // NULL
			parent[i] = -1;  // none
		}
		prio[v] = 0;
		int minpos = v;
		inMST[v] = true;

		while (togo > 0)
		{
			// "v" is the vertex with lowest priority that has just been added to the MST

			// for all edges (v, dest) such as dest is not in the MST, set priority to edge_cost(v, dest) if it is smaller than prio[dest]
			int dest;
			for (const_iterator<adjacencyMatrix<int> > ci = begin(v); ci != end(v); ++ci)
			{
				dest = *ci;
				int ec = edge_cost(v, dest);
				if (ec < prio[dest] && !inMST[dest])
				{
					prio[dest] = ec;
					parent[dest] = v;
				}
			}

			// find lowest prio vertex in "dest"
			dest = 0; // find cheapest edge out of it
			while (inMST[dest]) ++dest;
			for (i = 0; i < nbVerices; ++i)
			{	// chose lowest priority node that has not yet been selected
				if ((prio[i] < prio[dest]) && (! inMST[i])) dest = i;
			}
			if (prio[dest] == INT_MAX) // found disconnected component
			{
				prio[dest]=0; // restart the algo from
			}
			else
			{// add edge (v, dest) to MST
				cout << "(" << parent[dest] << "," << dest << ") ";
			}
			--togo;
			v = dest;
			inMST[v] = true;
		}

		cout << endl;

		delete[] inMST;
		delete[] prio;
		delete[] parent;
	} // void prim(int v) 

	void prim2(int v) // Prim'algorithm starting from vertex v, uses priority queue
	{
		cout << "Prim (priority queue)";

		int nbVerices = getNbVerices();
		int* prio = new int[nbVerices];
		int* parent = new int[nbVerices];
		bool* inMST = new bool[nbVerices];
		int i;

		for (i = 0; i <nbVerices; ++i)
		{
			prio[i] = INT_MAX;
			inMST[i] = false; // NULL
			parent[i] = -1;  // none
		}

		prio[v] = 0;
		minHeap< std::pair<int, int> > pq(nbVerices);
		pq.push(std::make_pair(0, v)); // (priority, vertexNo)
		std::pair<int, int> top;

		while (! pq.empty())
			{
			// "v" is the vertex with lowest priority that has just been added to the MST
			top = pq.popTop();
			v = top.second;
			inMST[v] = true;

			// explore all (v, dest) outgoing edges from "v"
			int dest;
			for (const_iterator<adjacencyMatrix<int> > ci = begin(v); ci != end(v); ++ci)
			{
				dest = *ci;
				int ec = edge_cost(v, dest);
				// if dest is not already in the MST and we found a shorter path to it.
				if (ec < prio[dest] && !inMST[dest])
				{
					prio[dest] = ec; // Set priority od "dest" to edge_cost(v, dest)
					parent[dest] = v; // indicate that "dest" has been best reached from "v". This will be needed to geerate the edges later
					pq.push(std::make_pair(ec, dest)); // push the new vertex into the priority queue
				}
			}
		}
		
		// Output the edge list for the MST
		for (i = 0; i < nbVerices; ++i)
		{
			if (parent[i] != -1) // exclude parent[starting node] which is -1
			{
				cout << "(" << parent[i] << "," << i << ") ";
			}
		}
		
		cout << endl;

		delete[] inMST;
		delete[] prio;
		delete[] parent;
	} // void prim(int v) 

	void kruskal(int v)
	{
		cout << "Kruskal : ";

		// build and sort edge list
		if (!hasEdgeList)
		{
			buildEdgeList();
		}
		sort(edgeList.begin(), edgeList.end());
		/*for (vector<EdgeT>::const_iterator ci = edgeList.begin(); ci != edgeList.end(); ++ci)
		{
			cout << "(" << ci->start << "," << ci->end << ") "<< ci->cost<<"  "<<endl;
		}*/

		// initilaise every vertex in a different set, uses efficient disjoint set data structure
		int nbVerices = getNbVerices();
		int nbEdges = getNbEdges();
		disjointSet partitions(nbVerices);

		int inMST = 0; // number of vertices in the MST
		// we will iterate through all edges starting from the cheapest
	 
		for (int edge = 0; edge < nbEdges; ++edge )
		{
			// skip edges from the same partition
			if (partitions.find(edgeList[edge].start) == partitions.find(edgeList[edge].end)) continue;
			cout << "(" << edgeList[edge].start <<","<< edgeList[edge].end << ") ";
			partitions.merge(edgeList[edge].start, edgeList[edge].end);
			++inMST;
			if (inMST == nbVerices - 1) // first insert adds two vertices, but we only count 1
			{
				break;
			}
		}
		cout << endl;
	}

	void dijkstra(int v)
	{	// requires all edges to have positive cost
		// finds shortest paths from "v" to all other nodes
		cout << "Dijkstra : "<<endl;
		int nbVerices = getNbVerices();
		int nbEdges = getNbEdges();

		int* distance = new int[nbVerices];
		int* parent = new int[nbVerices];

		for (int i = 0; i <nbVerices; ++i)
		{
			distance[i] = INT_MAX;
			parent[i] = -1;  // none
		}

		distance[v] = 0;

		minHeap< std::pair<int, int> > pq(nbVerices);
		pq.push(std::make_pair(0, v)); // (priority, vertexNo)
		std::pair<int, int> top;

		while (!pq.empty())
		{
			// "v" is the vertex with lowest cost
			top = pq.popTop();
			v = top.second;

			int dest;
			for (const_iterator<adjacencyMatrix<int> > ci = begin(v); ci != end(v); ++ci)
			{
				dest = *ci;
				int ec = edge_cost(v, dest);
				// if we found a shorter path to it "dest"
				if (ec + distance[v] < distance[dest])
				{
					distance[dest] = ec  + distance[v]; // Set distance to new lower value
					parent[dest] = v; // indicate that "dest" has been best reached from "v"
					pq.push(std::make_pair(distance[dest], dest)); // push the new vertex into the priority queue
				}
			}
		}

		int j;
		for (int i = 0; i <nbVerices; ++i)
		{
			cout << i;
			j = parent[i];
			while (j != -1) 
			{
				cout << " <- "<< j;
				j = parent[j];
			}
			cout << " : " << distance[i] << endl;
		}

		delete[] distance;
		delete[] parent;
	}

	void floydWarshall() {}

	void bellmanFord(int v) {}

	void maximumFlow() {}

///Advanced package
	void borouvka(int v) {}

	// add near linear time MST algo by Tarjan

	void 

	unsigned int valence(unsigned int v) const { return und->valence(v); }
	bool load(const char* fileName) { return und.load(fileName); }

	//void completeVertexInfo();
	//int neighbour(unsigned int i, unsigned int j);
//	void initEdgeTraversee() {a=0; b=0; itt=0;}
//	Edge nextEdge();
	unsigned int getNbEdges() const { return und.nbEdges;}
	unsigned int getNbVerices() const { return und.nbVerices;}

private:
	underlying und;
	unsigned int a,b, itt;//used for vertex traversee
	bool hasEdgeList;
	std::vector<EdgeT> edgeList;
};


/*
template <typename InfoType>
int Graph<InfoType>::neighbour(unsigned int i, unsigned int j)
{
	if (i>nbNodes) throw "Bad index";
	if (j>VertexConnections[i].size()) "Bad index";
	return VertexConnections[i][j];
}


template <typename InfoType>
void Graph<InfoType>::completeVertexInfo()
{
	unsigned int i,j, node;
	for(i=0; i<nbNodes; ++i)
	{
		for(j=0; j<VertexConnections[i].size(); ++j)
		{
			node = VertexConnections[i][j];
			if (node<i) break;
			VertexConnections[node].pushback(i);
			EdgeInfo[node].pushback(EdgeInfo[i][j]);
		} // for(j=0; j<Edges[i].size(); ++j)
	} // for(i=0; i<nbNodes; ++i)
} // void Graph::completeVertexInfo()


/*
template <typename InfoType>
Edge Graph<InfoType>::nextEdge()
{
	Edge e;
	if (itt<nbEdges)
	{
again:
		if (Edges[a].size()>b)
		{
			e.a=a; e.b=Edges[a][b];
			b++;
		} else 
		{
			++a; 
			b=0;
			goto again;
		}
	}
	++itt;
	return e;
}
*/
/*
template <typename InfoType> 
void Graph<InfoType>::init(unsigned int _nbNodes) 
{
	nbEdges = 0;
	EdgeInfo = new LightVector<InfoType>[nbNodes];	
	for (unsigned int i=0; i<nbNodes; ++i) 
	{
		EdgeInfo[i].upsize(5);
	}
}*/


/*
template <typename InfoType>
unsigned int Graph<InfoType>::insertEdge(unsigned int a, unsigned int b)
{
// numbering of the vertices starts from 0;
	if (nbNodes==0) throw "Graph has not been initialised.";
	if (a>nbNodes || b>nbNodes) throw "Vertex endpoint exceeds Graph capacity.";

	int aux;

	if (a<b)
	{
		aux = VertexConnections[a].contains(b);
		if (aux==-1) 
		{
			VertexConnections[a].pushback(b);
			aux = nbEdges++;
			EdgeInfo[a].pushback(aux);
		} else aux = EdgeInfo[a][aux];
	} else
	{
		aux = VertexConnections[b].contains(a);
		if (aux==-1)
		{
			VertexConnections[b].pushback(a);
			aux = nbEdges++;
			EdgeInfo[b].pushback(aux);
		} else aux = EdgeInfo[b][aux];
	} // else branch of if (a<b)
	return aux;
} // unsignd int Graph::insertEdge(int a, int b)
*/

#endif