#ifndef mat_mat_mul_opt2_hpp
#define mat_mat_mul_opt2_hpp

#include "mat_t.hpp"
#include <tbb/task.h>
#include <tbb/parallel_for.h>
#include "tbb/blocked_range2d.h"

class mat_mat_mul_opt2_class : public tbb::task {
	public:
		mat_t dst;
		const mat_t a;
		const mat_t b;

		mat_mat_mul_opt2_class( mat_t dst, const mat_t a, const mat_t b):
			dst(dst), a(a), b(b)
			{}

		tbb::task* execute() {#include "tbb/blocked_range2d.h"
			if((dst.rows==8) || (dst.cols==8)){
				tbb::parallel_for(tbb::blocked_range<int>(0,))
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

				mat_mat_mul_opt2_class &dst_ul = *new(allocate_child()) mat_mat_mul_opt2_class(dst.quad(0,0), a.quad(0,0), b.quad(0,0));
				mat_mat_mul_opt2_class &dst_ur = *new(allocate_child()) mat_mat_mul_opt2_class(dst.quad(0,1), a.quad(0,0), b.quad(0,1));
				mat_mat_mul_opt2_class &dst_dl = *new(allocate_child()) mat_mat_mul_opt2_class(dst.quad(1,0), a.quad(1,0), b.quad(0,0));
				mat_mat_mul_opt2_class &dst_dr = *new(allocate_child()) mat_mat_mul_opt2_class(dst.quad(1,1), a.quad(1,0), b.quad(0,1));
				
				mat_mat_mul_opt2_class &right_ul = *new(allocate_child()) mat_mat_mul_opt2_class(right.quad(0,0), a.quad(0,1), b.quad(1,0));
				mat_mat_mul_opt2_class &right_ur = *new(allocate_child()) mat_mat_mul_opt2_class(right.quad(0,1), a.quad(0,1), b.quad(1,1));
				mat_mat_mul_opt2_class &right_dl = *new(allocate_child()) mat_mat_mul_opt2_class(right.quad(1,0), a.quad(1,1), b.quad(1,0));
				mat_mat_mul_opt2_class &right_dr = *new(allocate_child()) mat_mat_mul_opt2_class(right.quad(1,1), a.quad(1,1), b.quad(1,1));
				
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

void mat_mat_mul_opt2(mat_t dst, const mat_t a, const mat_t b) {
	mat_mat_mul_opt2_class &root = *new(tbb::task::allocate_root()) mat_mat_mul_opt2_class(dst, a, b);
	tbb::task::spawn_root_and_wait(root);
}

#endif
