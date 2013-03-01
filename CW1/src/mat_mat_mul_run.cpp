#include "mat_mat_mul.hpp"

int main(int argc, char *argv[])
{
	srand(0);
	
	int n=8;
	if(argc>1)
		n=atoi(argv[1]);
		
	local_mat_t A(n,n), B(n,n);
	local_mat_t R(n,n);
	
	A.randomise();
	A.dump(std::cout);
	
	B.randomise();
	B.dump(std::cout);
	
	mat_mat_mul(R, A, B);
	
	R.dump(std::cout);
	
	return 0;
}