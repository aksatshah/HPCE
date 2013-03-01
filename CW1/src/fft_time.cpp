#include "fft.hpp"
#include "fft_tbb.hpp"
#include "fft_opt.hpp"
#include "fft_seq.hpp"
#include <vector>
#include <stdio.h>
#include <math.h>
#include <cstdlib>
#include <tbb/tick_count.h>
#include "tbb/task_scheduler_init.h"


#define ITER 3
#define MAX_ERROR 0.00000001

bool check(
	int n,
	std::complex<double> *pOut_orig,
	std::complex<double> *pOut_test
){
	std::vector<std::complex<double> > out_check(n);
	for(int j=0;j<n;j++){
		out_check[j] = pOut_orig[j] - pOut_test[j];
		if (real(out_check[j]) > MAX_ERROR) {
			std::cout << "Error in real output in cell " << j << std::endl;
			return 0;
		}
		if (imag(out_check[j]) > MAX_ERROR) {
			std::cout << "Error in imag output in cell " << j << std::endl;
			return 0;
		}
	}
	return 1;
}

int main(int argc, char *argv[])
{
	int x=-1;
	srand(0);
	
	if(argc<2){
		fprintf(stderr, "Specify log2 of transform size.");
		exit(1);
	}

	if(argc>2)
		x=atoi(argv[2]);

	tbb::task_scheduler_init init(x);
	
	int log2n=atoi(argv[1]);
	int n=1<<log2n;

	std::vector<std::complex<double> > in(n, 0.0), in_tbb(n, 0.0), in_opt(n, 0.0), in_seq(n, 0.0), out(n), out_tbb(n), out_opt(n), out_seq(n);
	for(int j=0;j<n;j++){
		in[j]=std::complex<double>(rand()/(double)(RAND_MAX) - 0.5, rand()/(double)(RAND_MAX) - 0.5);
	}

	fft(n, &in[0], &out[0]); //warmup run	
	tbb::tick_count serial_start = tbb::tick_count::now();
	for (int i = 0; i < ITER; ++i)
		fft(n, &in[0], &out[0]);
	tbb::tick_count serial_end = tbb::tick_count::now();
	//std::cout << "Serial Time = " << (serial_end - serial_start).seconds()/ITER << std::endl;
	if (!(check(n, &out[0] ,&out[0]))) //check against original results for correct result
		std::cout << "Error in original code!" << std::endl;


	fft_tbb(n, &in[0], &out_tbb[0]); //warmup run
	tbb::tick_count tbb_start = tbb::tick_count::now();
	for (int i = 0; i < ITER; ++i)
		fft_tbb(n, &in[0], &out_tbb[0]);
	tbb::tick_count tbb_end = tbb::tick_count::now();
	//std::cout << "TBB Time = " << (tbb_end - tbb_start).seconds()/ITER << std::endl;
	if (!(check(n, &out[0] ,&out_tbb[0]))) //check against original results for correct result
		std::cout << "Error in original code!" << std::endl;


	fft_opt(n, &in[0], &out_opt[0]); //warmup run
	tbb::tick_count opt_start = tbb::tick_count::now();
	for (int i = 0; i < ITER; ++i)
		fft_opt(n, &in[0], &out_opt[0]);
	tbb::tick_count opt_end = tbb::tick_count::now();
	//std::cout << "OPT Time = " << (opt_end - opt_start).seconds()/ITER << std::endl;
	if (!(check(n, &out[0] ,&out_opt[0]))) //check against original results for correct result
		std::cout << "Error in original code!" << std::endl;


	fft_seq(n, &in[0], &out_seq[0]); //warmup run
	tbb::tick_count seq_start = tbb::tick_count::now();
	for (int i = 0; i < ITER; ++i)
		fft_seq(n, &in[0], &out_seq[0]);
	tbb::tick_count seq_end = tbb::tick_count::now();
	//std::cout << "SEQ Time = " << (seq_end - seq_start).seconds()/ITER << std::endl;
	if (!(check(n, &out[0] ,&out_seq[0]))) //check against original results for correct result
		std::cout << "Error in original code!" << std::endl;

	std::cout << log2n << ", " << (serial_end - serial_start).seconds()/ITER << ", " << (tbb_end - tbb_start).seconds()/ITER << ", " << (opt_end - opt_start).seconds()/ITER << ", " << (seq_end - seq_start).seconds()/ITER << std::endl;

	
	/* To test this, you can try loading the output into matlab. Load
		the output as a four column matrix x. Then the input is:
			in=x(:,1)+x(:,2)*i
		and the output is:
			out=x(:,3)+x(:,4)*i
		You can then do fft(in) to check what matlab thinks it should be.
		Note that this fft and matlab fft produce outputs in different
		orders. How could you "sort" (hint-hing) that out in matlab so that
		you can check you have the same spectrum.
	
		You can also try putting in various types of sine wave, and checking
		whether you get the right frequency peaks in the output. You're
		the signal processing experts, I assume you know what a fourier
		transform does.
	
		Note that this not a very accurate fft, so you should expect
		the error to grow as the transform size grows.
	*/
	
	return 0;
}
