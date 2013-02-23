#ifndef mat_mat_mul_hpp
#define mat_mat_mul_hpp

#include "mat_t.hpp"

void mat_mat_mul(
	mat_t dst,
	const mat_t a,
	const mat_t b
){
	if((dst.rows==1) || (dst.cols==1)){
		for(unsigned row=0;row<dst.rows;row++){
			for(unsigned col=0;col<dst.cols;col++){
				double acc=0.0;
				for(unsigned i=0;i<a.cols;i++){
					acc += a.at(row,i) * b.at(i,col);
				}
				dst.at(row,col) = acc;
			}
		}
	}else{
		local_mat_t right(dst.rows, dst.cols);
		
		mat_mat_mul(dst.quad(0,0), a.quad(0,0), b.quad(0,0));
		mat_mat_mul(dst.quad(0,1), a.quad(0,0), b.quad(0,1));
		mat_mat_mul(dst.quad(1,0), a.quad(1,0), b.quad(0,0));
		mat_mat_mul(dst.quad(1,1), a.quad(1,0), b.quad(0,1));
		
		mat_mat_mul(right.quad(0,0), a.quad(0,1), b.quad(1,0));
		mat_mat_mul(right.quad(0,1), a.quad(0,1), b.quad(1,1));
		mat_mat_mul(right.quad(1,0), a.quad(1,1), b.quad(1,0));
		mat_mat_mul(right.quad(1,1), a.quad(1,1), b.quad(1,1));
		
		for(unsigned row=0;row<dst.rows;row++){
			for(unsigned col=0;col<dst.cols;col++){
				dst.at(row,col) += right.at(row,col);
			}
		}
	}
}

#endif
