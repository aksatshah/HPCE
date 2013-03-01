#ifndef mat_mat_mul_opt_hpp
#define mat_mat_mul_opt_hpp

#include "mat_t.hpp"
#include <tbb/task.h>

#define THRESH 8

class mat_par_for4 {
	public:
		mat_t *dst;
		const mat_t a;
		const mat_t b;

		mat_par_for4(mat_t *dst, const mat_t a, const mat_t b):
			dst(dst), a(a), b(b)
			{}

		void operator() (const tbb::blocked_range2d<int>& range) const {
			for(unsigned row=range.rows().begin();row<range.rows().end();row++){
				for(unsigned col=range.cols().begin();col<range.cols().end();col++){
					double acc=0.0;
					for(unsigned i=0;i<a.cols;i++){
						acc += a.at(row,i) * b.at(i,col);
					}
					dst->at(row,col) = acc;
				}
			}
		}
};


class mat_mat_mul_opt4_class : public tbb::task {
	public:
		mat_t dst;
		const mat_t a;
		const mat_t b;

		mat_mat_mul_opt4_class( mat_t dst, const mat_t a, const mat_t b):
			dst(dst), a(a), b(b)
			{}

		void operate() {
//			if((dst.rows==THRESH) || (dst.cols==THRESH)){
				tbb::parallel_for(tbb::blocked_range2d<int>(0,dst.rows,0,dst.cols),mat_par_for(&dst, a, b));
/*			}else{
				local_mat_t right(dst.rows, dst.cols);

				mat_mat_mul_opt4_class dst_ul(dst.quad(0,0), a.quad(0,0), b.quad(0,0));
				mat_mat_mul_opt4_class dst_ur(dst.quad(0,1), a.quad(0,0), b.quad(0,1));
				mat_mat_mul_opt4_class dst_dl(dst.quad(1,0), a.quad(1,0), b.quad(0,0));
				mat_mat_mul_opt4_class dst_dr(dst.quad(1,1), a.quad(1,0), b.quad(0,1));
				
				mat_mat_mul_opt4_class right_ul(right.quad(0,0), a.quad(0,1), b.quad(1,0));
				mat_mat_mul_opt4_class right_ur(right.quad(0,1), a.quad(0,1), b.quad(1,1));
				mat_mat_mul_opt4_class right_dl(right.quad(1,0), a.quad(1,1), b.quad(1,0));
				mat_mat_mul_opt4_class right_dr(right.quad(1,1), a.quad(1,1), b.quad(1,1));
				
				dst_ul.operate();
				dst_ur.operate();
				dst_dl.operate();
				dst_dr.operate();

				right_ul.operate();
				right_ur.operate();
				right_dl.operate();
				right_dr.operate();

				for(unsigned row=0;row<dst.rows;row++){
					for(unsigned col=0;col<dst.cols;col++){
						dst.at(row,col) += right.at(row,col);
					}
				}
			}*/
			return;
		}
};

void mat_mat_mul_opt(mat_t dst, const mat_t a, const mat_t b) {
	mat_mat_mul_opt4_class &root = *new(tbb::task::allocate_root()) mat_mat_mul_opt4_class(dst, a, b);
	tbb::task::spawn_root_and_wait(root);
}

#endif
