#ifndef mat_t_hpp
#define mat_t_hpp

#include <vector>
#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>

struct mat_t
{
	unsigned rows, cols;
	unsigned stride;
	double *mat;

protected:
	//! Used by local_mat_t only
	mat_t()
	{}
public:
	
	/*! Create a view onto a square matrix with height rows and width columns. The
		stride between the start of each row is _stride, and the data is held
		in some externally managed memory _mat. */
	mat_t(unsigned height, unsigned width, unsigned _stride, double *_mat)
		: rows(height), cols(width), stride(_stride), mat(_mat)
	{}
		
	/*! Returns reference to element at specified row and column.*/
	double &at(unsigned r, unsigned c)
	{ return mat[r*stride+c]; }
	
	/*! Const version of at. */
	double at(unsigned r, unsigned c) const
	{ return mat[r*stride+c]; }
	
	/*! Create a view onto one quadrant of this matrix.
		\param ud If 0 then select upper rows, else lower rows
		\param lr If 0 then select left columns, else right columns */
	mat_t quad(unsigned ud, unsigned lr)
	{
		return mat_t(
			ud ? (rows/2) : (rows-rows/2),
			lr ? (cols/2) : (cols-cols/2),
			stride,
			mat+ud*(stride*rows/2)+lr*(cols/2)
		);
	}
	
	/*! Const correct version of quad.  Views returned from this can't be modified.*/
	const mat_t quad(unsigned ud, unsigned lr) const
	{
		// This temporarily un-consts the current object, but is safe because there are no side effects
		return const_cast<mat_t&>(*this).quad(ud,lr);
	}
	
	/*! Randomise (poorly) the matrix to uniform values in [0,1] */
	void randomise()
	{
		for(unsigned r=0;r<rows;r++){
			for(unsigned c=0;c<cols;c++){
				// Note: don't use rand() in real code, this is for compactness.
				at(r,c) = rand() / (double)RAND_MAX - 0.5;
			}
		}
	}
	
	void dump(std::ostream &dst) const
	{
		for(unsigned r=0;r<rows;r++){
			dst<<"[";
			for(unsigned c=0;c<cols;c++){
				if(c!=0)
					dst<<" , ";
				dst<<at(r,c);
			}
			dst<<"]\n";
		}
		dst<<"\n";
	}
};

/*! This is a matrix which allocates storage as well (not just a view) */
struct local_mat_t
	: public mat_t
{
	std::vector<double> storage;
	
	local_mat_t(unsigned height, unsigned width)
		: storage(width*height, 0.0)
	{
		rows=height;
		cols=width;
		stride=width;
		mat=&storage[0];
	}
};

#endif
