#include "mat_mat_mul.hpp"
#include "mat_mat_mul_tbb.hpp"
#include "mat_mat_mul_opt.hpp"
#include <tbb/tick_count.h>
#include <tbb/task.h>

#define ITER 5

int main(int argc, char *argv[])
{
	srand(0);
	
	int n=8;
	if(argc>1)
		n=atoi(argv[1]);
		
	local_mat_t A(n,n), B(n,n);
	local_mat_t R(n,n);
	
	A.randomise();
	//A.dump(std::cout);
	
	B.randomise();
	//B.dump(std::cout);

	tbb::tick_count serial_start = tbb::tick_count::now();
	for (int i = 0; i < ITER; ++i)
		mat_mat_mul(R, A, B);
	tbb::tick_count serial_end = tbb::tick_count::now();
	//R.dump(std::cout);
	std::cout << "Serial Time = " << (serial_end - serial_start).seconds()/ITER << std::endl;


	tbb::tick_count tbb_start = tbb::tick_count::now();
	for (int i = 0; i < ITER; ++i) 
		mat_mat_mul_tbb(R, A, B);
	tbb::tick_count tbb_end = tbb::tick_count::now();
	//R.dump(std::cout);
	std::cout << "TBB Time = " << (tbb_end - tbb_start).seconds()/ITER << std::endl;

	
	tbb::tick_count opt_start = tbb::tick_count::now();
	for (int i = 0; i < ITER; i++)
		mat_mat_mul_opt(R, A, B);
	tbb::tick_count opt_end = tbb::tick_count::now();
	std::cout << "OPT Time = " << (opt_end - opt_start).seconds()/ITER << std::endl;
	
	return 0;
}
