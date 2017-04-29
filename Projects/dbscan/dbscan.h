#ifndef _DBSCAN_H__
#define _DBSCAN_H__

#define SUCCESS 0
#define FAILURE -3


struct node_s
{
	int index;
	node_s *next;
};

struct epsilon_neighbours_s
{
	int num_members;
	node_s *head, *tail;
};


#endif