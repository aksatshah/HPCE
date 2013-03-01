#ifndef mat_mat_mul_hpp
#define mat_mat_mul_hpp

#include "mat_t.hpp"

void mat_mat_mul(
	mat_t dst,			// This is non-const, so we are able to write into it
	const mat_t a,		// These two are const, so we can only read from them
	const mat_t b
){
	if((dst.rows==1) || (dst.cols==1)){
		/* Once we get down to a matrix with one column or one row, we stop trying to
			create more parallel work.  This is just a standard three loop matrix matrix multiply. */
		for(unsigned row=0;row<dst.rows;row++){
			for(unsigned col=0;col<dst.cols;col++){
				// This is a dot-product across one row from a and one column from b.
				double acc=0.0;
				for(unsigned i=0;i<a.cols;i++){
					acc += a.at(row,i) * b.at(i,col);
				}
				dst.at(row,col) = acc;
			}
		}
	}else{
		// This is creating a temporary matrix of the same size as the destination.
		// This is seperate from dst (i.e. they don't share storage), so the two can
		// be modified safely in parallel.
		local_mat_t right(dst.rows, dst.cols);
		
		// We perform one set of quadrant calculations into the destination memory
		// we have been given.
		// This calculation is multiplying the top-left quadrants of a and b, and storing
		// into the top-left quadrant of the destination.
		mat_mat_mul(dst.quad(0,0), a.quad(0,0), b.quad(0,0));
		mat_mat_mul(dst.quad(0,1), a.quad(0,0), b.quad(0,1));
		mat_mat_mul(dst.quad(1,0), a.quad(1,0), b.quad(0,0));
		// This is multiplying the bottom-right of a and the top-right of b, and storing into the bottom-right of dst.
		mat_mat_mul(dst.quad(1,1), a.quad(1,0), b.quad(0,1));
		
		// And another set into the temporary matrix that we have created.
		// Note that the second and third arguments are passed as const (see prototype),
		// so we know that the called function won't modify them.
		mat_mat_mul(right.quad(0,0), a.quad(0,1), b.quad(1,0));
		mat_mat_mul(right.quad(0,1), a.quad(0,1), b.quad(1,1));
		mat_mat_mul(right.quad(1,0), a.quad(1,1), b.quad(1,0));
		mat_mat_mul(right.quad(1,1), a.quad(1,1), b.quad(1,1));
		
		// Finally we combine the two parts together
		for(unsigned row=0;row<dst.rows;row++){
			for(unsigned col=0;col<dst.cols;col++){
				// Note that this is +=, so we are keeping the destination matrix data,
				// and adding on the results in the right matrix
				dst.at(row,col) += right.at(row,col);
			}
		}
		
		// At this point we are finished with the right matrix,  so it will go out of
		// scope and be deallocated. The destination matrix will contain the matrix
		// seen by the caller.
	}
}

#endif