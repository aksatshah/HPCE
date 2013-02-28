#include "fft.hpp"
#include "fft_tbb.hpp"
#include "fft_opt.hpp"
#include <vector>
#include <stdio.h>
#include <math.h>
#include <cstdlib>
#include <tbb/tick_count.h>
#include <iostream>

#define ITER 2
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
	srand(0);
	
	if(argc<2){
		fprintf(stderr, "Specify log2 of transform size.");
		exit(1);
	}
	
	int log2n=atoi(argv[1]);
	int n=1<<log2n;

	std::vector<std::complex<double> > in(n, 0.0), in_tbb(n, 0.0), in_opt(n, 0.0), out(n), out_tbb(n), out_opt(n);
	for(int j=0;j<n;j++){
		in[j]=std::complex<double>(rand()/(double)(RAND_MAX) - 0.5, rand()/(double)(RAND_MAX) - 0.5);
		in_tbb[j] = in[j];
		in_opt[j] = in[j];
	}
	
	tbb::tick_count serial_start = tbb::tick_count::now();
	for (int i = 0; i < ITER; ++i)
		fft(n, &in[0], &out[0]);
	tbb::tick_count serial_end = tbb::tick_count::now();
	std::cout << "Serial Time = " << (serial_end - serial_start).seconds()/ITER << std::endl;
	if (!(check(n, &in[0] ,&in[0]))) //check against original results for correct result
		std::cout << "Error in original code!" << std::endl;
	else
		std::cout << "Original code correct!" << std::endl;
/*	std::cout << "Serial Values:" << std::endl;
	for(int j=0;j<n;j++){
		fprintf(stdout, "%.16lg, %.16lg, %.16lg, %.16lg\n", real(in[j]), imag(in[j]), real(out[j]), imag(out[j]));
	}*/


	tbb::tick_count tbb_start = tbb::tick_count::now();
	for (int i = 0; i < ITER; ++i)
		fft_tbb(n, &in_tbb[0], &out_tbb[0]);
	tbb::tick_count tbb_end = tbb::tick_count::now();
	std::cout << "\nTBB Time = " << (tbb_end - tbb_start).seconds()/ITER << std::endl;
	if (!(check(n, &in[0] ,&in_tbb[0]))) //check against original results for correct result
		std::cout << "Error in original code!" << std::endl;
	else
		std::cout << "Original code correct!" << std::endl;
/*	std::cout << "TBB Values:" << std::endl;
	for(int j=0;j<n;j++){
		fprintf(stdout, "%.16lg, %.16lg, %.16lg, %.16lg\n", real(in_tbb[j]), imag(in_tbb[j]), real(out_tbb[j]), imag(out_tbb[j]));
	}*/

	tbb::tick_count opt_start = tbb::tick_count::now();
	for (int i = 0; i < ITER; ++i)
		fft_opt(n, &in_opt[0], &out_opt[0]);
	tbb::tick_count opt_end = tbb::tick_count::now();
	std::cout << "\nOPT Time = " << (opt_end - opt_start).seconds()/ITER << std::endl;
	if (!(check(n, &in[0] ,&in_opt[0]))) //check against original results for correct result
		std::cout << "Error in original code!" << std::endl;
	else
		std::cout << "Original code correct!" << std::endl;
/*	std::cout << "OPT Values:" << std::endl;
	for(int j=0;j<n;j++){
		fprintf(stdout, "%.16lg, %.16lg, %.16lg, %.16lg\n", real(in_opt[j]), imag(in_opt[j]), real(out_opt[j]), imag(out_opt[j]));
	}*/

	//std::cout << "Serial Time = " << (serial_end - serial_start).seconds() << std::endl;
	
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
