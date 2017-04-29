#ifndef _GRAPH_H_
#define _GRAPH_H_

//vocabulary: vertex (csomopont), edge (él)


#include "vector.h"

template <typename InfoType>
class Graph
{
public:
	Graph() : nbNodes(0) {}
	~Graph();

	unsigned int insertEdge(unsigned int a, unsigned int b); // return number associated to the vertex
	void init(unsigned int _nbNodes);
	inline int getNbEdges() {return nbEdges;}
	unsigned int valence(unsigned int v) {return VertexConnections[v].size();}
	void completeVertexInfo();
	int neighbour(unsigned int i, unsigned int j);
//	void initEdgeTraversee() {a=0; b=0; itt=0;}
//	Edge nextEdge();

protected:
	
	LightVector<unsigned int>* VertexConnections; // adjacency information VertexConnections[a] contains all nodes b that are connected to a
	LightVector<InfoType>* EdgeInfo; // EdgeInfo[min(a,b)] contains the number of vertex connecting a to b.
	unsigned int nbEdges, nbNodes;
private:
	unsigned int a,b, itt;//used for vertex traversee
};

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

template <typename InfoType> 
void Graph<InfoType>::init(unsigned int _nbNodes) 
{
	nbNodes = _nbNodes;
	nbEdges = 0;
	VertexConnections = new LightVector<unsigned int>[nbNodes];	
	EdgeInfo = new LightVector<InfoType>[nbNodes];	
	for (unsigned int i=0; i<nbNodes; ++i) 
	{
		VertexConnections[i].upsize(5);
		EdgeInfo[i].upsize(5);
	}
}


template <typename InfoType>
Graph<InfoType>::~Graph()
{
	if (nbNodes==0) return;
	delete[] VertexConnections;
	delete[] EdgeInfo;
}


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


#endif