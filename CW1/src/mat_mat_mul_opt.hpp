#ifndef mat_mat_mul_opt_hpp
#define mat_mat_mul_opt_hpp

#include "mat_t.hpp"
#include <tbb/task.h>

class mat_mat_mul_opt_class : public tbb::task {
	public:
		mat_t dst;
		const mat_t a;
		const mat_t b;

		mat_mat_mul_opt_class( mat_t dst, const mat_t a, const mat_t b):
			dst(dst), a(a), b(b)
			{}

		tbb::task* execute() {
/*			if((dst.rows==1) || (dst.cols==1)){
				for(unsigned row=0;row<dst.rows;row++){						//row = i
					for(unsigned col=0;col<dst.cols;col++){					//col = j
						double acc=0.0;
						for(unsigned i=0;i<a.cols;i++){						//i = k
							acc += a.at(row,i) * b.at(i,col);
						}
						dst.at(row,col) = acc;
					}
				}*/
			if((dst.rows==4) || (dst.cols==4)){
				for(unsigned row=0;row<dst.rows;row++){
					for(unsigned k=0;k<b.rows;k++){
						for(unsigned col=0;col<dst.cols;col++){
							dst.at(row,col) += a.at(row,k) * b.at(k,col);
						}
					}
				}
			}else{
				local_mat_t right(dst.rows, dst.cols);
				
				set_ref_count(9);

				mat_mat_mul_opt_class &dst_ul = *new(allocate_child()) mat_mat_mul_opt_class(dst.quad(0,0), a.quad(0,0), b.quad(0,0));
				mat_mat_mul_opt_class &dst_ur = *new(allocate_child()) mat_mat_mul_opt_class(dst.quad(0,1), a.quad(0,0), b.quad(0,1));
				mat_mat_mul_opt_class &dst_dl = *new(allocate_child()) mat_mat_mul_opt_class(dst.quad(1,0), a.quad(1,0), b.quad(0,0));
				mat_mat_mul_opt_class &dst_dr = *new(allocate_child()) mat_mat_mul_opt_class(dst.quad(1,1), a.quad(1,0), b.quad(0,1));
				
				mat_mat_mul_opt_class &right_ul = *new(allocate_child()) mat_mat_mul_opt_class(right.quad(0,0), a.quad(0,1), b.quad(1,0));
				mat_mat_mul_opt_class &right_ur = *new(allocate_child()) mat_mat_mul_opt_class(right.quad(0,1), a.quad(0,1), b.quad(1,1));
				mat_mat_mul_opt_class &right_dl = *new(allocate_child()) mat_mat_mul_opt_class(right.quad(1,0), a.quad(1,1), b.quad(1,0));
				mat_mat_mul_opt_class &right_dr = *new(allocate_child()) mat_mat_mul_opt_class(right.quad(1,1), a.quad(1,1), b.quad(1,1));
				
				spawn(dst_ul);
				spawn(dst_ur);
				spawn(dst_dl);
				spawn(dst_dr);

				spawn(right_ul);
				spawn(right_ur);
				spawn(right_dl);
				spawn(right_dr);

				wait_for_all();

				for(unsigned row=0;row<dst.rows;row++){
					for(unsigned col=0;col<dst.cols;col++){
						dst.at(row,col) += right.at(row,col);
					}
				}
			}
			return NULL;
		}
};

void mat_mat_mul_opt(mat_t dst, const mat_t a, const mat_t b) {
	mat_mat_mul_opt_class &root = *new(tbb::task::allocate_root()) mat_mat_mul_opt_class(dst, a, b);
	tbb::task::spawn_root_and_wait(root);
}

#endif