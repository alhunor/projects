#include "graph.h"
#include <iostream>
#include <conio.h>

using namespace std;

void main()
{
	Graph<int, adjacencyMatrix<int> > g;
	g.load("pentagon.txt");
	g.bfs(0);
	g.dfs(0);
	
	g.load("prim.txt");
	g.prim(7);
	g.prim2(7);
	g.kruskal(7);
	g.dijkstra(7);

	g.load("GeeksforGeeksPrim.txt");
	g.prim2(0);



	cout << "Press any key to continue..." << endl;
	_getch();
}