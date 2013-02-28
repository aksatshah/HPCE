#include "mat_mat_mul.hpp"
#include "mat_mat_mul_tbb.hpp"
#include "mat_mat_mul_opt.hpp"
#include "mat_mat_mul_opt2.hpp"
#include "mat_mat_mul_opt3.hpp"
#include "mat_t.hpp"
#include <tbb/tick_count.h>
#include <tbb/task.h>
#include "tbb/task_scheduler_init.h"

#define ITER 20
#define MAX_ERROR 0.00000001

bool check(
	mat_t R_orig,
	mat_t dst,
	int n
){
	local_mat_t check_R(n,n);
	for (int row = 0; row < R_orig.rows; row++) {
		for (int col = 0; col < R_orig.cols; col++) {
			check_R.at(row,col) = R_orig.at(row,col)-dst.at(row,col);
			if (check_R.at(row,col) > MAX_ERROR) {
				std::cout << "Error in Destination Matrix in cell (" << row << "," << col << ")" << std::endl;
				return 0;
			}
		}
	}
	return 1;
}

int main(int argc, char *argv[])
{
	srand(0);
	
	int n=8;
	int x=-1;

	if(argc>1)
		n=atoi(argv[1]);

	if(argc>2)
		x=atoi(argv[2]);
		
	local_mat_t A(n,n), B(n,n);
	local_mat_t R_orig(n,n), R_tbb(n,n), R_opt(n,n), R_opt2(n,n), R_opt3(n,n);
	
	//randomly generate input matrices
	A.randomise();
	B.randomise();
	//A.dump(std::cout);
	//B.dump(std::cout);
	tbb::task_scheduler_init init(x);

	//run serial code ITER number of times and get average time
	tbb::tick_count serial_start = tbb::tick_count::now();
	for (int i = 0; i < ITER; ++i)
		mat_mat_mul(R_orig, A, B);
	tbb::tick_count serial_end = tbb::tick_count::now();
	//R_orig.dump(std::cout);
	std::cout << "Serial Time = " << (serial_end - serial_start).seconds()/ITER << std::endl;
	if (!(check(R_orig,R_orig,n))) //check against original results for correct result
		std::cout << "Error in original code!" << std::endl;
	else
		std::cout << "Original code correct!" << std::endl;



	//run tbb code ITER number of times and get average time
	tbb::tick_count tbb_start = tbb::tick_count::now();
	for (int i = 0; i < ITER; ++i) 
		mat_mat_mul_tbb(R_tbb, A, B);
	tbb::tick_count tbb_end = tbb::tick_count::now();
	//R.dump(std::cout);
	std::cout << "TBB Time = " << (tbb_end - tbb_start).seconds()/ITER << std::endl;
	if (!(check(R_orig,R_tbb,n))) //check against original results for correct result
		std::cout << "Error in tbb code!" << std::endl;
	else
		std::cout << "TBB code correct!" << std::endl;
	


	//run opt code ITER number of times and get average time
	tbb::tick_count opt_start = tbb::tick_count::now();
	//for (int i = 0; i < ITER; i++)
		mat_mat_mul_opt(R_opt, A, B);
	tbb::tick_count opt_end = tbb::tick_count::now();
	std::cout << "OPT Time = " << (opt_end - opt_start).seconds() << std::endl;
	//R_orig.dump(std::cout);
	//R_opt.dump(std::cout);
	if (!(check(R_orig,R_opt,n))) //check against original results for correct result
		std::cout << "Error in optimized code!" << std::endl;
	else
		std::cout << "Optimized code correct!" << std::endl;

}
