#ifndef graph_distance_opt_hpp
#define graph_distance_opt_hpp

#include <vector>
#include <deque>
#include <utility>

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <tbb/task.h>
#include <tbb/parallel_for.h>
#include "tbb/concurrent_queue.h"
#include "tbb/concurrent_vector.h"
#include "tbb/atomic.h"

#include "graph_distance.hpp"


bool finish_check(std::vector<int> distance, const std::vector<node> nodes)
{
	for (int i=0; i<nodes.size(); i++)
	{
		if(distance[i]==INT_MAX)
			return 0;
	}
	return 1;
}


/*! This is the function we are interested in the execution time of. */
// Here n is the number of nodes (nodes.size())
std::vector< <tbb::atomic> int > graph_distance_opt(const std::vector<node> &nodes, int start)
{
	std::vector< <tbb::atomic> int> distance(nodes.size(), INT_MAX); // all nodes start at infinite distance
	
	// a list of (id,dist)
	tbb::concurrent_queue<std::pair<int,int> > todo;
	todo.push(std::make_pair(start,0));
	//while(!finish_check(distance, nodes)){
	while(!todo.empty()){
		std::pair<int,int> curr;
		todo.try_pop(curr);
		
		if(distance[curr.first]==INT_MAX){
			distance[curr.first]=curr.second;
			tbb::parallel_for(tbb::blocked_range<int>(0,nodes[curr.first].edges.size()),graph_distance_class(&todo, nodes, curr));
		}
	}
	
	return distance;
}

#endif
