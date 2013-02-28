#ifndef fft_hpp
#define fft_hpp

#include <complex>

/* Does a recursive FFT
	n = Number of elements (must be a power of two)
	wn =  Complex root of unity of order n
	(pIn,sIn) = Base and stride of input data
	(pOut,Sout) = Base and stride of output data
*/
void fft_impl(
	int n, std::complex<double> wn,
	const std::complex<double> *pIn, int sIn,
	std::complex<double> *pOut, int sOut
){
	if (n == 1){
		pOut[0] = pIn[0];
    }else if (n == 2){
		pOut[0] = pIn[0]+pIn[sIn];
		pOut[sOut] = pIn[0]-pIn[sIn];
    }else{
		unsigned m = n/2;

		fft_impl(m,wn*wn,pIn,2*sIn,pOut,sOut);
		fft_impl(m,wn*wn,pIn+sIn,2*sIn,pOut+sOut*m,sOut);
		 
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
	
void fft(int n, const std::complex<double> *pIn, std::complex<double> *pOut)
{
	const double pi2=6.283185307179586476925286766559;
	double angle = pi2/n;
	std::complex<double> wn(cos(angle), sin(angle));
	fft_impl(n, wn, pIn, 1, pOut, 1);
}

#endif
