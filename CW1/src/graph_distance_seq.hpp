#ifndef graph_distance_seq_hpp
#define graph_distance_seq_hpp

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

#include "graph_distance.hpp"


/*! This is the function we are interested in the execution time of. */
// Here n is the number of nodes (nodes.size())
std::vector<int> graph_distance_seq(const std::vector<node> &nodes, int start)
{
	std::vector<int> distance(nodes.size(), INT_MAX); // all nodes start at infinite distance
	
	// a list of (id,dist)
	std::deque<std::pair<int,int> > todo;
	todo.push_back(std::make_pair(start,0));
	
	while(!todo.empty()){
		std::pair<int,int> curr=todo.front();
		todo.pop_front();
		
		if(distance[curr.first]==INT_MAX){
			distance[curr.first]=curr.second;
			for(int i=0;i<nodes[curr.first].edges.size();i++){
				todo.push_back(std::make_pair(nodes[curr.first].edges[i],curr.second+1));
			}
		}
	}
	
	return distance;

}

#endif
