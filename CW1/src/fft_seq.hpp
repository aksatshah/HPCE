#ifndef FFT_SEQ_HPP
#define FFT_SEQ_HPP

#include <complex>
#include <stdio.h>
#include <tbb/task.h>
#include <iostream>
#include <tbb/parallel_for.h>

#define THRESH 16

/* Does a recursive FFT
	n = Number of elements (must be a power of two)
	wn =  Complex root of unity of order n
	(pIn,sIn) = Base and stride of input data
	(pOut,Sout) = Base and stride of output data
*/


class fft_seq_impl {
	public:
		int n; 
		std::complex<double> wn;
		const std::complex<double> *pIn;
		int sIn;
		std::complex<double> *pOut; 
		int sOut;

		fft_seq_impl( int n, std::complex<double> wn, 
			const std::complex<double> *pIn, int sIn, 
			std::complex<double> *pOut, int sOut):

			n(n), wn(wn), pIn(pIn), sIn(sIn), pOut(pOut), sOut(sOut)
			{}


		void operate() {
			if (n == 1){
				pOut[0] = pIn[0];
	    	}else if (n == 2){
				pOut[0] = pIn[0]+pIn[sIn];
				pOut[sOut] = pIn[0]-pIn[sIn];
	    	}else{

				unsigned m = n/2;

				if (n < THRESH) { //used for agglomeration in optimized form
					//recursively split tree to remove overhead from parallel tasks
					fft_seq_impl fft_seq_impl_left(m,wn*wn,pIn,2*sIn,pOut,sOut);
					fft_seq_impl_left.serial();
					fft_seq_impl fft_seq_impl_right(m,wn*wn,pIn+sIn,2*sIn,pOut+sOut*m,sOut);
					fft_seq_impl_right.serial();
				}else{
					//serial implementation of fft - previously spawning fft
				 	fft_seq_impl fft_seq_impl_left(m,wn*wn,pIn,2*sIn,pOut,sOut);
					fft_seq_impl_left.operate();
					fft_seq_impl fft_seq_impl_right(m,wn*wn,pIn+sIn,2*sIn,pOut+sOut*m,sOut);
					fft_seq_impl_right.operate();
			 	}
			 	
				
				std::complex<double> w=std::complex<double>(1.0, 0.0);
					for (unsigned j=0;j<m;j++){
				  		std::complex<double> t1 = w*pOut[m+j];
				  		std::complex<double> t2 = pOut[j]-t1;
				  		pOut[j] = pOut[j]+t1;                 /*  pOut[j] = pOut[j] + w^i pOut[m+j] */
				  		pOut[j+m] = t2;                       /*  pOut[j] = pOut[j] - w^i pOut[m+j] */
				  		w = w*wn;
					}
				}
		}

		void serial() {
			if (n == 1){
				pOut[0] = pIn[0];
	    	}else if (n == 2){
				pOut[0] = pIn[0]+pIn[sIn];
				pOut[sOut] = pIn[0]-pIn[sIn];
	    	}else{
				unsigned m = n/2;

				//recursively split tree to remove overhead from parallel tasks
				fft_seq_impl fft_seq_impl_left(m,wn*wn,pIn,2*sIn,pOut,sOut);
				fft_seq_impl_left.serial();
				fft_seq_impl fft_seq_impl_right(m,wn*wn,pIn+sIn,2*sIn,pOut+sOut*m,sOut);
				fft_seq_impl_right.serial();
			 	
				std::complex<double> w=std::complex<double>(1.0, 0.0);
					for (unsigned j=0;j<m;j++){
				  		std::complex<double> t1 = w*pOut[m+j];
				  		std::complex<double> t2 = pOut[j]-t1;
				  		pOut[j] = pOut[j]+t1;                 /*  pOut[j] = pOut[j] + w^i pOut[m+j] */
				  		pOut[j+m] = t2;                       /*  pOut[j] = pOut[j] - w^i pOut[m+j] */
				  		w = w*wn;
					}
					
				}
			}
};
	
void fft_seq(int n, const std::complex<double> *pIn, std::complex<double> *pOut)
{
	const double pi2=6.283185307179586476925286766559;
	double angle = pi2/n;
	std::complex<double> wn(cos(angle), sin(angle));

	fft_seq_impl root(n, wn, pIn, 1, pOut, 1);
	root.operate();
	
}

#endif
