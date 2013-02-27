#include "graph_distance.hpp"
//#include "graph_distance_check.hpp"
#include "graph_distance_tbb.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

#include <tbb/tick_count.h>

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
	tbb::tick_count serial_start = tbb::tick_count::now();
	std::vector<int> tmp=graph_distance(graph, start);
	tbb::tick_count serial_end = tbb::tick_count::now();

	tbb::tick_count check_start = tbb::tick_count::now();
	std::vector<int> tmp2=graph_distance_tbb(graph, start);
	tbb::tick_count check_end = tbb::tick_count::now();

	for(int i=0;i<tmp.size();i++){
		fprintf(stdout, "dist(%d->%d) = %d\n", start, i, tmp[i]);
		fprintf(stdout, "dist(%d->%d) = %d\n", start, i, tmp2[i]);
	}
	std::cout << "Serial time = " << (serial_end - serial_start).seconds() << std::endl;
	std::cout << "TBB time = " << (check_end - check_start).seconds() << std::endl;

	return 0;
}
