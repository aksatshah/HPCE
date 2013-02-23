#include "graph_distance.hpp"

#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <iostream>

int main(int argc, char *argv[])
{
	if(argc!=2){
		std::cerr<<"Must specify n.\n";
		return 1;
	}
	int n=atoi(argv[1]);
	
	std::vector<node> graph=build_graph(n);
	
	dump_graph(graph);
		
	// The run-time can vary, depending on where you start from. How should you
	// take that into account when timing it?
	int start=rand()%n;
	// Note that it is only graph_distance that we care about
	std::vector<int> tmp=graph_distance(graph, start);
	for(int i=0;i<tmp.size();i++){
		fprintf(stdout, "dist(%d->%d) = %d\n", start, i, tmp[i]);
	}
	
	return 0;
}
