#include "graph_distance.hpp"
#include "graph_distance_tbb.hpp"
#include "graph_distance_opt.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

#include <tbb/tick_count.h>
#include "tbb/concurrent_vector.h"

#define ITER 100

int main(int argc, char *argv[])
{
	if(argc!=2){
		std::cerr<<"Must specify n.\n";
		return 1;
	}
	int n=atoi(argv[1]);
	
	std::vector<node> graph=build_graph(n);
	
	dump_graph(graph);

	std::vector<int> tmp, tmp2, tmp3;
	//tbb::concurrent_vector<int> tmp3;
		
	// The run-time can vary, depending on where you start from. How should you
	// take that into account when timing it?
	int start=rand()%n;
	// Note that it is only graph_distance that we care about

	tbb::tick_count serial_start = tbb::tick_count::now();
	for (int i = 0; i < ITER; ++i)
		tmp =graph_distance(graph, start);
	tbb::tick_count serial_end = tbb::tick_count::now();
	std::cout << "Serial time = " << (serial_end - serial_start).seconds()/ITER << std::endl;


	tbb::tick_count tbb_start = tbb::tick_count::now();
	for (int i = 0; i < ITER; ++i)
		tmp2=graph_distance_tbb(graph, start);
	tbb::tick_count tbb_end = tbb::tick_count::now();
	std::cout << "TBB time = " << (tbb_end - tbb_start).seconds()/ITER << std::endl;


	tbb::tick_count opt_start = tbb::tick_count::now();
	for (int i = 0; i < ITER; ++i)
		tmp3=graph_distance_opt(graph, start);
	tbb::tick_count opt_end = tbb::tick_count::now();
	std::cout << "Opt time = " << (opt_end - opt_start).seconds()/ITER << std::endl;


/*	for(int i=0;i<tmp.size();i++){
		fprintf(stdout, "dist(%d->%d) = %d\n", start, i, tmp[i]);
		fprintf(stdout, "dist(%d->%d) = %d\n", start, i, tmp2[i]);
		fprintf(stdout, "dist(%d->%d) = %d\n", start, i, tmp3[i]);
	}*/

	return 0;
}
