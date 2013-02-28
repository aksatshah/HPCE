#ifndef FFT_OPT_HPP
#define FFT_OPT_HPP

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

class fft_par_for {
	public:

		std::complex<double> *pOut;
		std::complex<double> *w;
		std::complex<double> wn;
		unsigned m;

		fft_par_for(std::complex<double> *pOut, std::complex<double> *w, std::complex<double> wn, unsigned m):
		pOut(pOut), w(w), wn(wn), m(m)
		{}

		void operator() (const tbb::blocked_range<int>& range) const {
			for(unsigned j=range.begin();j<range.end();j++) {
				std::complex<double> t1 = (*w)*pOut[m+j];
		  		std::complex<double> t2 = pOut[j]-t1;
		  		pOut[j] = pOut[j]+t1;                 /*  pOut[j] = pOut[j] + w^i pOut[m+j] */
		  		pOut[j+m] = t2;                          /*  pOut[j] = pOut[j] - w^i pOut[m+j] */
		  		*w = (*w)*wn;
			}
		}
};


class fft_opt_impl : public tbb::task {
	public:
		int n; 
		std::complex<double> wn;
		const std::complex<double> *pIn;
		int sIn;
		std::complex<double> *pOut; 
		int sOut;

		fft_opt_impl( int n, std::complex<double> wn, 
			const std::complex<double> *pIn, int sIn, 
			std::complex<double> *pOut, int sOut):

			n(n), wn(wn), pIn(pIn), sIn(sIn), pOut(pOut), sOut(sOut)
			{}

		tbb::task* execute() {
			if (n == 1){
				pOut[0] = pIn[0];
	    	}else if (n == 2){
				pOut[0] = pIn[0]+pIn[sIn];
				pOut[sOut] = pIn[0]-pIn[sIn];
	    	}else{
				unsigned m = n/2;

				if (n < THRESH) {
					fft_opt_impl fft_opt_impl_left(m,wn*wn,pIn,2*sIn,pOut,sOut);
					fft_opt_impl_left.serial();
					fft_opt_impl fft_opt_impl_right(m,wn*wn,pIn+sIn,2*sIn,pOut+sOut*m,sOut);
					fft_opt_impl_right.serial();
				}else{
					set_ref_count(3);
					fft_opt_impl &split_left = *new(allocate_child()) fft_opt_impl(m,wn*wn,pIn,2*sIn,pOut,sOut);
					fft_opt_impl &split_right = *new(allocate_child()) fft_opt_impl(m,wn*wn,pIn+sIn,2*sIn,pOut+sOut*m,sOut);
				 	spawn(split_left);
				 	spawn(split_right);
				 	wait_for_all();
			 	}
			 	
				
				std::complex<double> w=std::complex<double>(1.0, 0.0);
					for (unsigned j=0;j<m;j++){
				  		std::complex<double> t1 = w*pOut[m+j];
				  		std::complex<double> t2 = pOut[j]-t1;
				  		pOut[j] = pOut[j]+t1;                 /*  pOut[j] = pOut[j] + w^i pOut[m+j] */
				  		pOut[j+m] = t2;                          /*  pOut[j] = pOut[j] - w^i pOut[m+j] */
				  		w = w*wn;
					}
				}
			return NULL;
		}

		void serial() {
			if (n == 1){
				pOut[0] = pIn[0];
	    	}else if (n == 2){
				pOut[0] = pIn[0]+pIn[sIn];
				pOut[sOut] = pIn[0]-pIn[sIn];
	    	}else{
				unsigned m = n/2;

				if (n < THRESH) {
					fft_opt_impl fft_opt_impl_left(m,wn*wn,pIn,2*sIn,pOut,sOut);
					fft_opt_impl_left.serial();
					fft_opt_impl fft_opt_impl_right(m,wn*wn,pIn+sIn,2*sIn,pOut+sOut*m,sOut);
					fft_opt_impl_right.serial();
				}else{
					set_ref_count(3);
					fft_opt_impl &split_left = *new(allocate_child()) fft_opt_impl(m,wn*wn,pIn,2*sIn,pOut,sOut);
					fft_opt_impl &split_right = *new(allocate_child()) fft_opt_impl(m,wn*wn,pIn+sIn,2*sIn,pOut+sOut*m,sOut);
				 	spawn(split_left);
				 	spawn(split_right);
				 	wait_for_all();
			 	}
			 	
				std::complex<double> w=std::complex<double>(1.0, 0.0);
					for (unsigned j=0;j<m;j++){
				  		std::complex<double> t1 = w*pOut[m+j];
				  		std::complex<double> t2 = pOut[j]-t1;
				  		pOut[j] = pOut[j]+t1;                 /*  pOut[j] = pOut[j] + w^i pOut[m+j] */
				  		pOut[j+m] = t2;                          /*  pOut[j] = pOut[j] - w^i pOut[m+j] */
				  		w = w*wn;
					}
					
				}
			}
};
	
void fft_opt(int n, const std::complex<double> *pIn, std::complex<double> *pOut)
{
	const double pi2=6.283185307179586476925286766559;
	double angle = pi2/n;
	std::complex<double> wn(cos(angle), sin(angle));

	fft_opt_impl &root = *new(tbb::task::allocate_root()) fft_opt_impl(n, wn, pIn, 1, pOut, 1);
	tbb::task::spawn_root_and_wait(root);
}

#endif
