#include "graph.h"
#include <iostream>
#include <conio.h>

using namespace std;

void main()
{
	Graph<int, adjacencyMatrix<int> > g;
	g.load("pentagon.txt");
	bfs(g, 0);
	dfs(g, 0);
	
	g.load("prim.txt");
	prim(g, 7);
	prim2(g, 7);
	kruskal(g, 7);
	dijkstra(g,7);

	g.load("GeeksforGeeksPrim.txt");
	prim2(g, 0);

	g.load("khan.txt");
	khan(g);

	cout << "Press any key to continue..." << endl;
	_getch();
}