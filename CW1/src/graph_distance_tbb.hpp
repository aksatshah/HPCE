#ifndef graph_distance_tbb_hpp
#define graph_distance_tbb_hpp

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


class graph_distance_class{
	public:
		tbb::concurrent_queue<std::pair<int,int> > *todo;
		const std::vector<node> nodes;
		std::pair<int,int> curr;

		graph_distance_class( tbb::concurrent_queue<std::pair<int,int> > *todo, const std::vector<node> nodes, std::pair<int,int> curr):
			todo(todo), nodes(nodes), curr(curr)
			{}

		void operator() (const tbb::blocked_range<int>& range) const {
			for(int i = range.begin();i < range.end(); i++)
				todo->push(std::make_pair(nodes[curr.first].edges[i],curr.second+1));
		}
};

/*! This is the function we are interested in the execution time of. */
// Here n is the number of nodes (nodes.size())
std::vector<int> graph_distance_tbb(const std::vector<node> &nodes, int start)
{
	std::vector<int> distance(nodes.size(), INT_MAX); // all nodes start at infinite distance
	
	// a list of (id,dist)
	tbb::concurrent_queue<std::pair<int,int> > todo;
	todo.push(std::make_pair(start,0));
	
	while(!todo.empty()){
		std::pair<int,int> curr;
		todo.try_pop(curr);
		
		if(distance[curr.first]==INT_MAX){
			distance[curr.first]=curr.second;
			//parallel for to split for loop into smaller sizes
			tbb::parallel_for(tbb::blocked_range<int>(0,nodes[curr.first].edges.size()),graph_distance_class(&todo, nodes, curr));
		}
	}
	
	return distance;
}

#endif
