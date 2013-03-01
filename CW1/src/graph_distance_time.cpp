#include "graph_distance.hpp"
#include "graph_distance_tbb.hpp"
#include "graph_distance_opt.hpp"
#include "graph_distance_seq.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

#include <tbb/tick_count.h>
#include "tbb/concurrent_vector.h"
#include "tbb/task_scheduler_init.h"

#define ITER 3

bool check(
	std::vector<int> orig_result,
	std::vector<int> test_result,
	int n
){
	for(int j=0;j<n;j++){
		if (orig_result != test_result) {
			std::cout << "Distance calculated wrong for node " << j << std::endl;
			return 0;
		}
	}
	return 1;
}

int main(int argc, char *argv[])
{
	if(argc<2){
		std::cerr<<"Must specify n.\n";
		return 1;
	}
	int n=atoi(argv[1]);
	int x=-1;

	if(argc>2)
		x=atoi(argv[2]);

	tbb::task_scheduler_init init(x);

	std::vector<node> graph=build_graph(n);
	
	//dump_graph(graph);

	std::vector<int> tmp_orig, tmp_tbb, tmp_opt, tmp_seq;
		
	// The run-time can vary, depending on where you start from. How should you
	// take that into account when timing it?
	int start=rand()%n;
	// Note that it is only graph_distance that we care about


	tmp_orig =graph_distance(graph, start); //warmup run
	tbb::tick_count serial_start = tbb::tick_count::now();
	for (int i = 0; i < ITER; ++i)
		tmp_orig =graph_distance(graph, start);
	tbb::tick_count serial_end = tbb::tick_count::now();
	std::cout << (serial_end - serial_start).seconds()/ITER << std::endl;


	tmp_tbb=graph_distance_tbb(graph, start); //warmup run
	tbb::tick_count tbb_start = tbb::tick_count::now();
	for (int i = 0; i < ITER; ++i)
		tmp_tbb=graph_distance_tbb(graph, start);
	tbb::tick_count tbb_end = tbb::tick_count::now();
	std::cout << (tbb_end - tbb_start).seconds()/ITER << std::endl;

	if (!(check(tmp_orig,tmp_tbb,n))) //check against original results for correct result
		std::cout << "Error in tbb code!" << std::endl;


	tmp_opt=graph_distance_opt(graph, start); //warmup run
	tbb::tick_count opt_start = tbb::tick_count::now();
	for (int i = 0; i < ITER; ++i)
		tmp_opt=graph_distance_opt(graph, start);
	tbb::tick_count opt_end = tbb::tick_count::now();
	std::cout << (opt_end - opt_start).seconds()/ITER << std::endl;

	if (!(check(tmp_orig,tmp_opt,n))) //check against original results for correct result
		std::cout << "Error in opt code!" << std::endl;


	tmp_seq=graph_distance_seq(graph, start); //warmup run
	tbb::tick_count seq_start = tbb::tick_count::now();
	for (int i = 0; i < ITER; ++i)
		tmp_seq=graph_distance_seq(graph, start);
	tbb::tick_count seq_end = tbb::tick_count::now();
	std::cout << (seq_end - seq_start).seconds()/ITER << std::endl;

	if (!(check(tmp_orig,tmp_seq,n))) //check against original results for correct result
		std::cout << "Error in opt code!" << std::endl;

	
	return 0;
}
