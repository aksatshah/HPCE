#ifndef graph_distance_check_hpp
#define graph_distance_check_hpp

#include <vector>
#include <deque>
#include <utility>

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <iostream>
#include <stdio.h>
#include <tbb/tick_count.h>

struct node
{
	int id;
	std::vector<int> edges;
};

// This is just a simple platform independent random number generator, returns an integer in [0,maxn]
unsigned rng(unsigned &seed, unsigned maxn)
{
	seed=(seed*1664525UL*+1013904223UL)&0xFFFFFFFF;
	double u=ldexp((double)seed, -32);	// Convert to [0,1)
	return (unsigned)floor(u*(maxn+1));
}

std::vector<node> build_graph(int n)
{
	std::vector<node> res(n);
	for(int i=0;i<n;i++){
		res[i].id=i;
		res[i].edges.reserve(15);
	}
	
	unsigned seed=1;
	
	// randomly assign edge to random node
	for(int i=0;i<10*n;i++){
		int a=rng(seed,n-1); // Randomly select which node to add edge to
		int b=rng(seed,n-1); // Randomly select destination node
		
		res[a].edges.push_back(b);
	}
	return res;
}

void dump_graph(const std::vector<node> &nodes)
{
	for(unsigned i=0;i<nodes.size();i++){
		fprintf(stdout, "%d -> ", i);
		for(unsigned j=0;j<nodes[i].edges.size();j++){
			fprintf(stdout, " %d", nodes[i].edges[j]);
		}
		fprintf(stdout, "\n");
	}
}

/*! This is the function we are interested in the execution time of. */
// Here n is the number of nodes (nodes.size())
// Each edge is of unit distance
std::vector<int> graph_distance_check(const std::vector<node> &nodes, int start)
{
	tbb::tick_count graph_start = tbb::tick_count::now();
	std::vector<int> distance(nodes.size(), INT_MAX); // all nodes start at infinite distance
	
	// a list of (id,dist)
	std::deque<std::pair<int,int> > todo; // double ended queue - efficient access but not contiguous memory
	todo.push_back(std::make_pair(start,0));
/*	std::deque<int>::size_type sz = todo.size();

	for (int i=0; i<sz; i++) {
		std::cout << "todo = " << todo[i].first << "		" << todo[i].second << std::endl;
	}*/

	while(!todo.empty()){
		//std::cout << "todo front = first:" << todo.front().first << "		second:" << todo.front().second << std::endl;
		std::pair<int,int> curr=todo.front();
		todo.pop_front();
		
		// if its the first time hitting this node then check
		if(distance[curr.first]==INT_MAX){
			//std::cout << "in if" << std::endl;
			distance[curr.first]=curr.second; // distance from first node is curr.second
			for(int i=0;i<nodes[curr.first].edges.size();i++){
				todo.push_back(std::make_pair(nodes[curr.first].edges[i],curr.second+1));
			}
			for(int i=0;i<distance.size();i++){
				if (distance[i] == INT_MAX)
					break;
			}
			//std::cout << "time taken = " << ((tbb::tick_count::now()) - graph_start).seconds() << std::endl;
		}
	}
	//tbb::tick_count graph_end = tbb::tick_count::now();
	//std::cout << "time ended = " << (graph_end - graph_start).seconds() << std::endl;
 	return distance;
}

#endif
