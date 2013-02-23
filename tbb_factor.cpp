#include <iostream>
#include <math.h>

#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>

typedef unsigned long long num_t;

struct FactorWorker
{
	num_t n;
	num_t *res;
	
	FactorWorker(num_t _n, num_t *_res)
	{
		n=_n;
		res=_res;
	}
	
	void operator()(num_t i) const
	{
		if( (n%i) ==0){ // found a factor
			*res=i; 	// record the factor
		}
	}
};

num_t ParallelFactor(num_t n)
{
	num_t mx=1+ceil(sqrt((double)n));
	
	num_t res=n;
	tbb::parallel_for((num_t)2, mx,
		// body of the for loop
		FactorWorker(n,&res)
	);
	
	return res;
}

int main(int argc, char *[])
{
	num_t numerator;
	
	while(true){
		std::cin>>numerator;
		if(std::cin.fail())
			break;

		num_t seqResult=ParallelFactor(numerator);
		
		std::cout<<"Numerator = "<<numerator<<"\n";
		std::cout<<"  ParallelFactor = "<<seqResult<<"\n";
	}
	
	return 0;
}
