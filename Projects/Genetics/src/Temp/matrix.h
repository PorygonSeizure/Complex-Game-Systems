#ifndef MATRIX_H
#define MATRIX_H

//
//Version 2.17b October 2013
//2.17  Added dual determination of useable rank to restore historical performance. 
//2.17a Changed a parameter in tracking of small sing. values in splits2 in rmatrix.h. 
//2.17b Some minor code refactoring and/or recoding. 
//      Some rare problems may experience a small change to solutions.
//      It is arguable which method may be best.
//
//matrix.h contains a library of matrix creation and manipulation 
//routines for any shape matrix, and a suite of standard solvers
//based on a pseudo-inverse (a.k.a. truncated SVD) treatment when needed.
//matrix.h also contains slightly modified versions of decomposition routines provided
//by Mathworks, NIST, and the Template Numerical Toolkit web site.
//
//rejtrix.h contains a suite of manually- and automatically-regularized solvers
//plus code to support solution of three-way systems:
//least squares          (A*x=b); 
//equality constraints   (E*x==f); and
//inequality constraints (G*x>=h). 
//
//sparse.h contains the Node and Sparse classes that support creation and manipulation
//of sparse matrices, and the code for the GMRES solver.  Sparse.h requires matrix.h.
//Recently we added eigenvalue/eigenvector routines to sparse.h.
//
//All these packages are freely distributed from the author's web site at
//www.rejonesconsulting.com.
//See that site for usage instructions and tutorials.
//
//Class diagram...
//
//    [-----Matrix------]  [---Diagonal---]     (base classes)
//       ^      ^    ^          ^    ^
//       |      .    |          .    .           dashed lines mean "derives from"
//       |      .    |          .    .           dotted lines mean "references" 
//       |      .    |          .    .
//    [-Row-]   .  [---Vector-----]  .          (limited to one row or column)
//              .         ^          .
//              .         .          .
//    [-Node-]  .         .          .          (a Node is a row of a Sparse matrix)
//       ^      .         .          .
//       .      .         .          .
//    [-----------------Sparse-----------]      (sparse matrix class)
//                        ^ 
//                        .
//                        .
//         [---------SparseEig--------]         (for sparse eigenvalues & vectors)
//
//
//
//------Licensing Notice---------------------------------------
//Copyright (c) 2006, 2008, 2011 Rondall E. Jones, Albuquerque NM, USA.
//As of January 1, 2012, we grant completely free usage of all these
//software packages to any users and all purposes.
//(The included NIST decomposition routines are unlimited release.)
//We do ask that you not remove the author's identification and notes above,
//and when appropriate, reference the author's web site in publications.
//
//------Warning Notice-----------------------------------------
//Warning: This software is provided AS IS!  
//THERE IS NO WARRANTY OF ANY SORT WHATSOEVER!
//The algorithms contained herein are partly heuristic and/or experimental.
//Inevitably this package contains bugs/errors which are as yet unknown,
//as well as known deficiencies.
//The user must determine if the results of any calculation using this package
//are appropriate for the user's purposes.
//
//------Technical note on coding style------------------------
//In matrix.h and rejtrix.h we have emphasized code re-use over efficiency 
//in lower level utility routines.  
//This is an attempt to minimize errors,
//maximize the utility of code testing, 
//and improve reliability of the code.
//  
//Specifically, Row and Vector inherit many operations from Matrix,
//at less than optimum efficiency, in order to reuse that one implementation.
//Or, see how Matrix::operator+() uses Matrix::operator+=()
//or how Matrix::PrependColumns() leverages off Matrix::AppendColumns()
//to make a trivial implementation of PrependColumns at a cost in efficiency.
//
//In sparse.h, efficiency is paramount and key algorithms are 
//coded with that in mind.
//
//Efficiency of the higher level algorithm usually far
//exceeds in impact any such minor issue of efficiency in low level routines.

//amatrix.h-----------------------------------------------------------
#include <stdlib.h>
#include <math.h>

//Recent compilers may need the .h removed in the following three includes.
//The form with .h is deprecated, but needed by older compilers.
#include <iostream>
#include <fstream> 
#include <iomanip>
#include <string.h>

//typedef int I;
//typedef double T;
static const double big = 1.0E30;
static const double big2 = 2.0E30;

double Abs(double x)
{
	if (x < 0.0)
		return (-x);
	else
		return x;
}
int Abs(int x)
{
	if (x < 0)
		return (-x);
	else
		return x;
}

int Min(int x, int y) { return x < y ? x : y; }
double Min(double x, double y) { return x < y ? x : y; }

int Max(int x, int y) { return x > y ? x : y; }
double Max(double x, double y) { return x > y ? x : y; }

int Min(int x, int y, int z)
{
	int a = x;
	if (y < a)
		a = y;
	if (z < a)
		a = z;
	return a;
}
double Min(double x, double y, double z)
{
	double a = x;
	if (y < a)
		a = y;
	if (z < a)
		a = z;
	return a;
}

int Max(int x, int y, int z)
{
	int a = x;
	if (y > a)
		a = y;
	if (z > a)
		a = z;
	return a;
}
double Max(double x, double y, double z)
{
	double a = x;
	if (y > a)
		a = y;
	if (z > a)
		a = z;
	return a;
}

int Min(int x, int y, int z, int p)
{
	int a = x;
	if (y < a)
		a = y;
	if (z < a)
		a = z;
	if (p < a)
		a = p;
	return a;
}

double LimitAbs(double x, double limit)
{
	double a = Abs(limit);
	if (x > a)
		return a;
	if (x < -a)
		return -a;
	return x;
}

double Square(double x) { return x * x; }
double PI() { return 3.14159265358979; }
double SignNum(double x)
{
	if (x >= 0.0)
		return 1.0;
	else
		return -1.0;
}

void Prompt()
{
	std::cout << "Ready? ";
	char str[99];
	std::cin.getline(str, 99);
}
void Separate()
{
	for (int z = 0; z < 10; z++)
		std::cout << "-------";
	std::cout << std::endl;
}
void SeparateX()
{
	for (int z = 0; z < 10; z++)
		std::cout << "---X---";
	std::cout << std::endl;
}
void Skipline()
{
	char str[99];
	std::cin.getline(str, 99);
}

bool AskTheUser(const char* msg)
{
	std::cout << msg << std::endl;
	char answer;
	std::cin >> answer;
	//Skipline;
	if (answer == 'y' || answer == 'Y')
		return true;
	else
		return false;
}

//******************************************************************
//The Matrix class.
//******************************************************************

class Matrix
{
private:
	int m_m;
	int m_n;
	double* m_data;
	double** m_v;

	friend class Row;
	friend class Vector;

	//internal use only
	void CheckDim(int m, int n)
	{
		if (m < 0 || n < 0)
			Matrix::XError(4, "Matrix::CheckDim");
		return;
	}
	void Setup2(int m, int n);
	void Setup() { Setup2(m_m, m_n); }

	void NormalizeThem(Matrix& b, Matrix& e, int i, double rowNorm);

public:

	//static utilities

	//error reporting routine
	static void XError(int m, const char* who)
	{
		std::cerr << "Error in routine " << who << std::endl;
		if (m == 1)  std::cerr << "Reference Out-of-Bounds!" << std::endl;
		else if (m == 2) std::cerr << "Dimensions do not match!" << std::endl;
		else if (m == 3) std::cerr << "Operation on an empty matrix!" << std::endl;
		else if (m == 4) std::cerr << "Invalid dimensions!" << std::endl;
		else if (m == 5) std::cerr << "Taking vector norm of non-vector! Use matrix norm?" << std::endl;
		else if (m == 6) std::cerr << "Divide by zero!" << std::endl;
		else if (m == 7) std::cerr << "Invalid input parameter" << std::endl;
		else if (m == 8) std::cerr << "Algorithm error" << std::endl;
		else if (m == 9) std::cerr << "Prohibited operation for Rows and Vectors!" << std::endl;
		else if (m == 10) std::cerr << "Given row is too long for matrix!" << std::endl;
		else if (m == 11) std::cerr << "Invalid argument vector!" << std::endl;
		else if (m == 12) std::cerr << "Problem is too large for current limits!" << std::endl;
		else std::cerr << "Miscellaneous error: " << m << std::endl;
		Prompt();
		exit(1);
	}

	//find the value of roundoff versus 1.0
	static double RoundOff()
	{
		static bool ok = false;
		static double round = 1.0e-9;
		if (ok)
			return round;
		int j = 0;
		double one = 1.0;
		double two = 1.0;
		double three;
		for (int i = 0; i <= 100; i++)
		{
			j = i;
			three = one + two;
			if (three == one)
				break;
			two /= 2.0;
		}
		round = two * 2.0;	//went one too far
		if (j >= 100)
			round = 1.0E-9;
		ok = true;
		return round;
	}

	//generate a random value between zero and one
	static double MyRandom(int reset = 0)
	{
		static int seed = 13 * 13 * 13;
		if (reset != 0)
			seed = reset % 16384;
		seed = seed * 13;	//scramble
		seed = seed % 16384;	//chop to 16 bits
		return (double(seed) / 16384.0);
	}

	//generate an approximately Gaussian random value, mean 0, sigma 1
	static double MyGauss()
	{
		double sum = 0.0;
		//for (int i = 0; i < 9; i++)
		//	sum += (MyRandom() - 0.5) * 2.0 * 1.732;
		//RMS is about 7% too large often... why???  so reduce it...
		for (int i = 0; i < 9; i++)
			sum += (MyRandom() - 0.5) * 2.0 * 1.62;
		return (sum / 3.0);
	}

	//count the number of arguments less than big
	static int CountArgs(double t1, double t2, double t3, double t4, double t5, double t6, double t7, double t8, double t9, double t10)
	{
		if (t2 > big) return 1;
		if (t3 > big) return 2;
		if (t4 > big) return 3;
		if (t5 > big) return 4;
		if (t6 > big) return 5;
		if (t7 > big) return 6;
		if (t8 > big) return 7;
		if (t9 > big) return 8;
		if (t10 > big) return 9;
		return 10;
	}

	//constructors---------------------------------------------------

	//default constructor: 0 by 0 matrix
	Matrix() : m_m(0), m_n(0) {}

	//construct an m by n matrix (zero filled)
	explicit Matrix(int m, int n) { Setup2(m, n); }

	//construct an m by n matrix filled with the value x
	explicit Matrix(int m, int n, double x);

	//construct an m by n matrix; copy the contents from the array a[m][ndim] 
	Matrix(int m, int n, int ndim, const double* a);

	//copy constructor
	Matrix(const Matrix& a);

	//destructors----------------------------------------------------

	//delete all data and set size to 0 by 0
	void Clear()
	{
		if (m_m > 0 && m_n > 0)
		{
			delete[] m_data;
			delete[] m_v;
		}
		m_m = 0;
		m_n = 0;
	}

	~Matrix() { Clear(); }

	//assignment-----------------------------------------------------

	//supports for example B = 3.14;
	Matrix operator=(const double x);

	//supports for example B = A;
	Matrix operator=(const Matrix& a);

	//accessors------------------------------------------------------

	//get the row dimension
	int Dim1() const { return m_m; }

	//get the column dimension
	int Dim2() const { return m_n; }

	//get the smaller dimension
	int DMin() const { return m_m < m_n ? m_m : m_n; }

	//get the larger dimension
	int DMax() const { return m_m > m_n ? m_m : m_n; }

	//get the 2-D size
	int DSize() const { return m_m * m_n; }

	//get the 2-dimensional array representing the matrix
	void GetArray(double* a)
	{
		int sz = m_m * m_n;
		for (int i = 0; i < sz; i++)
			a[i] = m_data[i];
	}

	//see if the two matrices have matching dimensions ... A.matching(B)
	bool Matches(const Matrix& b) const { return m_m == b.m_m && m_n == b.m_n; }

	//index----------------------------------------------------------

	inline double* operator[](int i)
	{
		if (i < 0 || i >= m_m) { Matrix::XError(1, "operator[]"); };	//DELETE for no debug
		return m_v[i];
	}

	inline const double* operator[](int i) const
	{
		if (i < 0 || i >= m_m) { Matrix::XError(1, "operator[]"); };	//DELETE for no debug
		return m_v[i];
	}

	//Alternative index form... A(i,j) rather than A[i][j].
	//This checks both the indices for proper range.
	//(The A[i][j] form can only check the first index.)

	double& operator()(int i, int j)
	{
		if (i < 0 || i >= m_m || j < 0 || j >= m_n)
			Matrix::XError(1, "operator(,)");
		return m_v[i][j];
	}

	const double& operator()(int i, int j) const
	{
		if (i < 0 || i >= m_m || j < 0 || j >= m_n)
			Matrix::XError(1, "operator(,)");
		return m_v[i][j];
	}

	//equivalence operations-----------------------------------------

	//supports A==B
	bool operator==(const Matrix& b) const;

	//supports A!=B
	bool operator!=(const Matrix& b) const;

	//approximately equal test.
	//Two values are considered approx. equal if they differ by
	//less than tolerance.
	//So this is an absolute test, not relative.
	bool Approximate(const Matrix& b, double tolerance) const;

	//element-wise Matrix operations---------------------------------

	//these operations support for example, a += 2.0, a -= 2.0, a *= 2.0, a /= 2.0  
	Matrix operator+=(double x)
	{
		int sz = m_m * m_n;
		for (int i = 0; i < sz; i++)
			m_data[i] += x;
		return *this;
	}
	Matrix operator-=(double x)
	{
		int sz = m_m * m_n;
		for (int i = 0; i < sz; i++)
			m_data[i] -= x;
		return *this;
	}
	Matrix operator*=(double x)
	{
		int sz = m_m * m_n;
		for (int i = 0; i < sz; i++)
			m_data[i] *= x;
		return *this;
	}
	Matrix operator/=(double x)
	{
		int sz = m_m * m_n;
		for (int i = 0; i < sz; i++)
			m_data[i] /= x;
		return *this;
	}

	//these operations support for example, a + 2.0, a - 2.0, a * 2.0, a / 2.0  
	Matrix operator+(double x) const
	{
		Matrix c(*this);
		c += x;
		return c;
	}
	Matrix operator-(double x) const
	{
		Matrix c(*this);
		c -= x;
		return c;
	}
	Matrix operator*(double x) const
	{
		Matrix c(*this);
		c *= x;
		return c;
	}
	Matrix operator/(double x) const
	{
		Matrix c(*this);
		c /= x;
		return c;
	}

	//unary minus--- for b = -a; for example
	Matrix operator-();

	//these operations support a += b, a -= b, a *= b, a /= b, which are all element-wise.
	//A and B must have exactly the same shape.
	Matrix operator+=(const Matrix& b);
	Matrix operator-=(const Matrix& b);
	Matrix operator*=(const Matrix& b);

	//these operations support a + b and a - b, which are all element-wise.
	//A and B must have exactly the same shape.
	Matrix operator+(const Matrix& b) const;
	Matrix operator-(const Matrix& b) const;

	//the following provides the matrix product a * b, 
	//where a's second dimension must equal b's first dimension
	Matrix operator*(const Matrix& b) const;

	//--------

	//the following scales each row of the matrix to unit norm,
	//and carries along b and e. (b usually the RHS; e the error est.)
	void NormalizeRows(Matrix& b, Matrix& e);

	//the following scales each row of the matrix to unit norm,
	//and carries along b
	void NormalizeRows(Matrix& b)
	{
		Matrix e(m_m, 1);
		NormalizeRows(b, e);
	}

	//the following scales each row of the matrix to unit norm
	void NormalizeRows()
	{
		Matrix b(m_m, 1);
		Matrix e(m_m, 1);
		NormalizeRows(b, e);
	}

	//--------

	//the following scales each row of the matrix to max element of 1.0
	//and carries along b and e. (b usually the RHS; e the error est.)
	void NormalizeRowsMax1(Matrix& b, Matrix& e);

	//the following scales each row of the matrix to max element of 1.0,
	//and carries along b
	void NormalizeRowsMax1(Matrix& b)
	{
		Matrix e(m_m, 1);
		NormalizeRowsMax1(b, e);
	}

	//the following scales each row of the matrix to max element of 1.0
	void NormalizeRowsMax1()
	{
		Matrix b(m_m, 1);
		Matrix e(m_m, 1);
		NormalizeRowsMax1(b, e);
	}

	//element-wise operations----------------------------------------

	//replaces each element with its absolute value
	void MAbs();

	//replaces each element with the square root of its absolute value
	void MSqrt();

	//replaces each element with its square
	void MSquare();

	//Replaces each element with the base 10 log of its absolute value
	//log(A.MaxAbs())-30.0 is used for zero elements.
	void MLog10();

	//Replaces each element a with 10^a.
	//That is, with the antilog10 of a.
	void MPow10();

	//makes each element at least x
	void AtLeast(double x);

	//truncates each number to n digits
	void KeepDigits(int n);

	//In the following integer utilities, the values are computed as integers,
	//but the TYPE remains floating point.

	//truncates each element to integer                  -2.6 --> -2.0    2.6 --> 2.0
	void Trunc();

	//rounds each element to the nearest integer         -2.6 --> -3.0    2.6 --> 3.0
	void Round();

	//rounds each element toward +infinity               -2.6 --> -2.0    2.6 --> 3.0
	void Ceil();

	//rounds each element to +1 or -1; zero goes to +1   -2.6 --> -1.0    2.6 --> 1.0
	void Signum();

	//rounds each element to +1 or -1; zero stays as 0   -2.6 --> -1.0    2.6 --> 1.0
	void Trinity();

	//convert each column to percentages based on the sum of the
	//(absolute values of the) elements of the column
	void ToPercentages();

	//min/max/sum functions------------------------------------------------

	//returns the element which is algebraically largest
	double MaxVal() const;

	//returns the element which is algebraically smallest
	double MinVal() const;

	//returns the (absolute value of the) element which is largest in absolute value
	double MaxAbs() const;

	//returns the (absolute value of the) element which is smallest in absolute value
	double MinAbs() const;

	//returns the smallest element greater than zero
	double MinPos() const;

	//returns (imax,jmax) position of element with largest abs value
	void IJMaxAbs(int& iMax, int& jMax) const;

	//returns the sum of the elements
	double Sum() const;

	//returns the sum of the absolute values of the elements
	double SumAbs() const;

	//returns the average of the values of the elements
	double Average() const { return (Sum() / (m_m * m_n)); }

	//returns the average of the absolute values of the elements
	double Averageabs() const { return (SumAbs() / (m_m * m_n)); }

	//find a neglible value for *this
	double Epsilon() const { return (MaxAbs() * 8.0 * Matrix::RoundOff()); }

	//count the number of non-zero elements
	int NumNonZero() const;

	//cout the number of non-negative elements
	int NumNonNegative() const;

	//1-D norms------------------------------------------------------
	//These methods require that the object be 1-dimensional.
	//That is, a Row, a Vector, or a Matrix of size 1 by n, or m by 1.
	//For a row v, Norm(v) = Sqrt(v * v').

	//returns Square(Norm(*this)) 
	double Norm2() const;

	//returns Norm(*this)
	double Norm() const { return sqrt(Norm2()); }

	//returns RootMeanSquare(*this)
	double RMS() const { return sqrt(Norm2() / double(Max(m_m, m_n, 1))); }

	//returns the population standard deviation
	double PopStdDev() const
	{
		double a = Average();
		Matrix d = *this - a;
		return d.RMS();
	}

	//returns the sample standard deviation
	double SamStdDev() const
	{
		if (m_m < 2) return 0.0;
		return PopStdDev() * sqrt(double(m_n) / double(m_n - 1));
	}

	//norms of the elements of the matrix as if it were 1-D ---------
	//These methods NO NOT require that the object be 1-dimensional.

	//returns the sum of the squares of all the elements
	double Norm2AsVector() const;

	//returns the square root of the sum of the squares of the elements
	double NormAsVector() const { return sqrt(Norm2AsVector()); }

	//Frobenius norm is another name for our NormAsVector 
	double Frobenius() const { return sqrt(Norm2AsVector()); }

	//returns root-mean-square of the matrix elements
	double RMSAsVector() const { return sqrt(Norm2AsVector() / double(m_m * m_n)); }

	//row/column operations-------------------------------------------

	//returns the dot product of two rows of *this
	double RowDot(int i, int k) const;

	//dot product of two equal-length 1-dimensional matrices.
	//dot tolerates any two equal length 1-D matrices: row.row, row.col, col.col
	double Dot(Matrix& b) const;

	//returns a row of *this
	Matrix GetRow(int i) const;

	//returns a column of *this
	Matrix GetColumn(int j) const;

	//sets all values in row i to val
	void SetRow(int i, double val);

	//sets all values in column j to val
	void SetColumn(int j, double val);

	//sets row i from a row matrix.  The row sizes must match.
	void SetRow(int i, Matrix a);

	//sets column i from a column matrix.  The columns sizes must match.
	void SetColumn(int j, Matrix a);

	//sets all values in row i to zero
	void SetRowZero(int i);

	//sets all values in column j to zero
	void SetColumnZero(int j);

	//matrix shape operations----------------------------------------

	//transposes *this
	Matrix T();

	//resize to smaller or larger
	//keeps upper left content as far as possible; fills with zero
	void Resize(int m, int n);

	//deletes row r; decreases matrix size!                     
	void DelRow(int r);

	//deletes column c; decreases matrix size!      
	void DelColumn(int c);

	//add m new rows at the bottom of *this; zero filled
	void AddRows(int m) { Resize(m_m + m, m_n); }

	//append Matrix b to bottom of *this
	void AppendRows(const Matrix& b);

	//prepend the Matrix b to the top of *this
	void PrependRows(const Matrix& b);

	//add n new columns at the right side of *this
	void AddColumns(int n) { Resize(m_m, m_n + n); };

	//append Matrix b to right side of *this
	void AppendColumns(const Matrix& b);

	//prepend the Matrix B to the left of *this
	void PrependColumns(const Matrix& b);

	//common matrices------------------------------------------------

	void Zeros();	//set *this to all zeros     
	void Ones();	//set *this to all ones   
	void Identity();	//set *this to identity matrix     
	void Iota();	//set *this[i][j] = i + j + 1.  In a row that's 1, 2, 3, ...
	void IotaZero();	//set *this[i][j] = i + j.  In a row that's 0, 1, 2, ...
	void Random();	//set *this to random values in (0, 1)
	void Gauss();	//set *this to random Gaussian, mean 0, standard deviation 1  
	void Hilbert();	//set *this[i][j] = 1 / (i + j + 1)
	void Heat();	//set to an example heat equation style kernel
	void Laplace();	//set to an example inverse Laplace style kernel
	void Cusp();	//set to one positive cusp of a sine function

	//following static methods create a Matrix of the given size and
	//call the appropriate routine above to define the elements of the Matrix.

	static Matrix Zeros(int m, int n)
	{
		Matrix a(m, n);
		a.Zeros();
		return a;
	}
	static Matrix Ones(int m, int n)
	{
		Matrix a(m, n);
		a.Ones();
		return a;
	}
	static Matrix Identity(int m, int n)
	{
		Matrix a(m, n);
		a.Identity();
		return a;
	}
	static Matrix Iota(int m, int n)
	{
		Matrix a(m, n);
		a.Iota();
		return a;
	}
	static Matrix IotaZero(int m, int n)
	{
		Matrix a(m, n);
		a.IotaZero();
		return a;
	}
	static Matrix Random(int m, int n)
	{
		Matrix a(m, n);
		a.Random();
		return a;
	}
	static Matrix Gauss(int m, int n)
	{
		Matrix a(m, n);
		a.Gauss();
		return a;
	}
	static Matrix Hilbert(int m, int n)
	{
		Matrix a(m, n);
		a.Hilbert();
		return a;
	}
	static Matrix Heat(int m, int n)
	{
		Matrix a(m, n);
		a.Heat();
		return a;
	}
	static Matrix Laplace(int m, int n)
	{
		Matrix a(m, n);
		a.Laplace();
		return a;
	}

	//displays-------------------------------------------------------

	//print a rectangular layout with default width
	void Print() const;

	//print by row in narrow (less than 80 column) format
	void PrintByRow() const;

	//print by column in narrow format
	void PrintByColumn() const;

	//print a glimpse of the matrix, in an 80-column wide format        
	void PrintA() const;

	//print a glimpse of the matrix and the right hand side vector, b, 
	//in an 80-column wide format
	void PrintAB(const Matrix& b) const;

	//print a glimpse of the matrix and the right hand side vector, b, 
	//and an error estimate vector e, in an 80-column wide format
	void PrintABE(const Matrix& b, const Matrix& e) const;

	//print a glimpse of the matrix, the solution, x, and the right hand side vector, b,
	//in an 80-column wide format.
	//x and b must be single column or single row matrices, or a Row or Vector.
	//By default, up to 25 rows will be printed.
	void PrintAXB(const Matrix& x, const Matrix& b, int maxRows = 25) const;

	//Compute for each element an Order of Magnitude of 1 to 16 or so.
	//This arrangement follows the way stars are classified: 
	//magnitude 1 is from largest to about 1/10th that;
	//magnitude 2 is from 1/10th of the largest to about 1/100th of the largest; 
	//etc
	Matrix ComputeStarMagnitudes() const;

	//show each element as Order of Magnitude 1 to 9 or blank for smaller than 9th magnitude
	void PrintStarMagnitudes() const;

};	//end class Matrix
       
//implementations for Matrix-----------------------------------------

void Matrix::Setup2(int m, int n)
{
	CheckDim(m, n);
	m_m = m;
	m_n = n;
	if (m_m == 0 || m_n == 0)
		return;
	int sz = m_m * m_n;
	m_data = new double[sz];
	for (int i = 0; i < sz; i++)
		m_data[i] = 0.0;

	m_v = new double*[m_m];
	double* p = &(m_data[0]);
	for (int i = 0; i < m_m; i++)
	{
		m_v[i] = p;
		p += m_n;
	}
}

Matrix::Matrix(int m, int n, double x) : m_m(m), m_n(n)
{
	Setup();
	for (int i = 0; i < m_m; i++)
	{
		for (int j = 0; j < m_n; j++)
			m_v[i][j] = x;
	}
}

Matrix::Matrix(int m, int n, int ndim, const double* a) : m_m(m), m_n(n)
{
	Setup();
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
			m_v[i][j] = a[i * ndim + j];
	}
}
    
Matrix::Matrix(const Matrix& a) : m_m(a.m_m), m_n(a.m_n)
{
	Setup();
	for (int i = 0; i < m_m; i++)
	{
		for (int j = 0; j < m_n; j++)
			m_v[i][j] = a.m_v[i][j];
	}
} 

Matrix Matrix::operator=(const double x)
{
	for (int i = 0; i < m_m; i++)
	{
		for (int j = 0; j < m_n; j++)
			m_v[i][j] = x;
	}
	return *this;
}

Matrix Matrix::operator=(const Matrix& a)
{
	Clear();
	Setup2(a.m_m , a.m_n);
	for (int i = 0; i < m_m; i++)
	{
		for (int j = 0; j < m_n; j++)
			m_v[i][j] = a[i][j];
	}
	return *this;
} 

bool Matrix::operator==(const Matrix& b) const
{
	if (m_m != b.m_m || m_n != b.m_n)
		return false;
	if (m_m == 0 || m_n == 0)
		return true;
	int sz = m_m * m_n;
	for (int i = 0; i < sz; i++)
	{
		if (m_data[i] != b.m_data[i])
			return false;
	}
	return true;
}

bool Matrix::Approximate(const Matrix& b, double tolerance) const
{
	if (m_m != b.m_m || m_n != b.m_n)
		return false;
	if (m_m == 0 || m_n == 0)
		return true;
	int sz = m_m * m_n;
	for (int i = 0; i < sz; i++)
	{
		if (Abs(m_data[i] - b.m_data[i]) > tolerance)
			return false;
	}
	return true;
}

bool Matrix::operator!=(const Matrix& b) const { return !((*this) == b); }  

Matrix Matrix::operator-()
{ 
	Matrix c(m_m, m_n);
	int sz = m_m * m_n; 
	for (int i = 0; i < sz; i++)
		c.m_data[i] = -m_data[i];
	return c;
}

Matrix Matrix::operator+=(const Matrix& b) 
{
	if (m_m != b.m_m || m_n != b.m_n)
		Matrix::XError(2, "Matrix+=Matrix");
	int sz = m_m * m_n;
	for (int i = 0; i < sz; i++)
		m_data[i] += b.m_data[i];
	return *this;
}

Matrix Matrix::operator-=(const Matrix& b)  
{
	if (m_m != b.m_m || m_n != b.m_n)
		Matrix::XError(2, "Matrix-=Matrix");
	int sz = m_m * m_n;
	for (int i = 0; i < sz; i++)
		m_data[i] -= b.m_data[i];
	return *this;
}

Matrix Matrix::operator*=(const Matrix& b)  
{
	if (m_m != b.m_m || m_n != b.m_n)
		Matrix::XError(2, "Matrix*=Matrix");
	int sz = m_m * m_n;
	for (int i = 0; i < sz; i++)
		m_data[i] *= b.m_data[i];
	return *this;
}

Matrix Matrix::operator+(const Matrix& b) const
{ 
	if (m_m != b.m_m || m_n != b.m_n)
		Matrix::XError(2, "Matrix+Matrix");
	Matrix c(*this);
	return c += b;
}

Matrix Matrix::operator-(const Matrix& b) const
{ 
	if (m_m != b.m_m || m_n != b.m_n)
		Matrix::XError(2, "Matrix-Matrix");
	Matrix c(*this);
	return c -= b;
}

Matrix Matrix::operator*(const Matrix& b) const
{
	if (m_n != b.m_m)
		Matrix::XError(2, "Matrix*Matrix");
	Matrix c(m_m, b.m_n);
	if (m_m == 0 || m_n == 0 || b.m_m == 0 || b.m_n == 0)
		return c;
	double sum;
	for (int i = 0; i < m_m; i++)
	{
		for (int j = 0; j < b.m_n; j++)
		{
			sum = 0.0;
			for (int k = 0; k < m_n; k++)
				sum = sum + m_v[i][k] * b[k][j];
			c[i][j] = sum;
		}
	}
	return c;
}

void Matrix::NormalizeThem(Matrix& b, Matrix& e, int i, double rowNorm)  //2011
{
	if (rowNorm > 0.0) //some rows may be zero
	{
		double scale = 1 / rowNorm;
		for (int j = 0; j < m_n; j++) 
		  m_v[i][j] = m_v[i][j] * scale;
		for (int j = 0; j < b.m_n; j++) 
		  b(i, j) = b(i, j) * scale;
		for (int j = 0; j < e.m_n; j++) 
		  e(i, j) = e(i, j) * scale;
	}
	//else what to do with B and E if row is zero??
}

void Matrix::NormalizeRows(Matrix& b, Matrix& e)
{
	if (m_m != b.m_m || m_m != e.m_m)
		Matrix::XError(2, "NormalizeRows(...)");
	for (int i = 0; i < m_m; i++)
	{
		Matrix r = this->GetRow(i);
		NormalizeThem(b, e, i, r.Norm());
	}
} 

void Matrix::NormalizeRowsMax1(Matrix& b, Matrix& e)
{
	if (m_m != b.m_m || m_m != e.m_m)
		Matrix::XError(2, "NormalizeRowsMax1(...)");
	for (int i = 0; i < m_m; i++)
	{
		Matrix r = this->GetRow(i);
		NormalizeThem(b, e, i, r.MaxAbs());
	}
} 

void Matrix::MAbs()
{ 
	int sz = m_m * m_n;
	for (int i = 0; i < sz; i++)
		m_data[i] = Abs(m_data[i]);
}

void Matrix::MSqrt()
{ 
	int sz = m_m * m_n;
	for (int i = 0; i < sz; i++)
		m_data[i] = sqrt(Abs(m_data[i]));
}

void Matrix::MSquare()
{ 
	int sz = m_m * m_n;
	for (int i = 0; i < sz; i++)
		m_data[i] = m_data[i] * m_data[i];
}

void Matrix::MLog10()
{ 
	int sz = m_m * m_n;
	double tiny = log10((*this).MaxAbs()) - 30.0;
	for (int i = 0; i < sz; i++)
	{
		if (m_data[i] != 0.0)
			m_data[i] = log10(Abs(m_data[i]));
		else
			m_data[i] = tiny;
	}
}

void Matrix::MPow10()
{ 
	int sz = m_m * m_n;
	double scale = log(10.0);
	for (int i = 0; i < sz; i++)
		m_data[i] = exp(scale * m_data[i]);
}

void Matrix::AtLeast(double x)
{ 
	int sz = m_m * m_n;
	for (int i = 0; i < sz; i++)
	{
		if (m_data[i] < x)
			m_data[i] = x;
	}
}

void Matrix::KeepDigits(int n)
{
	int p = 1;
	for (int i = 1; i < n; i++)
		p *= 10;
	int sz = m_m * m_n;
	for (int i = 0; i < sz; i++)
	{
		if (m_data[i] == 0.0)
			continue;
		double x = Abs(m_data[i]);
		double q = 1;
		while (x > (10 * p))
		{
			x /= 10;
			q *= 10;
		}
		while (x < p)
		{
			x *= 10;
			q /= 10;
		}
		int val = (int)(x + 0.5);
		x = val;
		x *= q;
		if (m_data[i] > 0.0)
			m_data[i] = x;
		else
			m_data[i] = -x;
	}
}

void Matrix::Trunc() 
{ 
	int sz = m_m * m_n;
	for (int i = 0; i < sz; i++) 
	{
		if (m_data[i] < 0.0)
			m_data[i] = -int(Abs(m_data[i]));
		else
			m_data[i] = int(m_data[i]);
	}
}
          
void Matrix::Round()  
{
	int sz = m_m * m_n;
	for (int i = 0; i < sz; i++)
	{
		if (m_data[i] < 0.0)
			m_data[i] = -int(Abs(m_data[i]) + 0.5);
		else
			m_data[i] = int(m_data[i] + 0.5);
	}
}
      
void Matrix::Ceil()
{ 
	int sz = m_m * m_n;
	int k;
	double d;
	for (int i = 0; i < sz; i++)
	{
		k = int(m_data[i]);
		d = double(k);
		m_data[i] = d < m_data[i] ? d + 1 : d;
	}
}
         
void Matrix::Signum()
{
	int sz = m_m * m_n;
	for (int i = 0; i < sz; i++)
	{
		if (m_data[i] < 0.0)
			m_data[i] = -1.0;
		else
			m_data[i] = 1.0;
	}  
}

void Matrix::Trinity()
{ 
	int sz = m_m * m_n;
	for (int i = 0; i < sz; i++) 
	{ 
		if (m_data[i] < 0.0)
			m_data[i] = -1.0;
		if (m_data[i] > 0.0)
			m_data[i] = 1.0;
	}  
} 

void Matrix::ToPercentages()
{
	for (int k = 0; k < m_n; k++) 
	{
		Matrix obs = GetColumn(k);
		double sum = obs.SumAbs();
		if (sum > 0.0)
			obs *= 100.0 / sum;
		SetColumn(k, obs);
	}
}

double Matrix::MaxVal() const
{    
	int sz = m_m * m_n;
	if (sz < 1)
		Matrix::XError(3, "Matrix::max()");
	double t = m_data[0];
	for (int i = 1; i < sz; i++)
	{
		if (m_data[i] > t)
			t = m_data[i];
	}
	return t;
}

double Matrix::MinVal() const 
{
	int sz = m_m * m_n;
	if (sz < 1)
		return 0.0;
	double t = m_data[0];
	for (int i = 1; i < sz; i++)
	{
		if (m_data[i] < t)
			t = m_data[i];
	}
	return t;
}

double Matrix::MaxAbs() const 
{    
	int sz = m_m * m_n;
	if (sz < 1)
		Matrix::XError(3, "Matrix::MaxAbs()");
	double t = Abs(m_data[0]);
	for (int i = 1; i < sz; i++)
	{
		if (Abs(m_data[i]) > t)
			t = Abs(m_data[i]);
	}
	return t;
}

double Matrix::MinAbs() const 
{
	int sz = m_m * m_n;
	if (sz < 1)
		return 0.0;
	double t = Abs(m_data[0]);
	for (int i = 1; i < sz; i++)
	{
		if (Abs(m_data[i]) < t)
			t = Abs(m_data[i]);
	}
	return t;
}

double Matrix::MinPos() const 
{
	int sz = m_m * m_n;
	if (sz < 1)
		return 0.0;
	double t = MaxAbs();
	for (int i = 0; i < sz; i++)
	{
		if (m_data[i] > 0.0 && m_data[i] < t)
			t = m_data[i];
	}
	return t;
}

void Matrix::IJMaxAbs(int& iMax, int& jMax) const
{    
	int sz = m_m * m_n;
	if (sz < 1)
		Matrix::XError(3, "Matrix::IJMaxAbs()");
	double t = Abs(m_data[0]);
	iMax = 0;
	jMax = 0;
	for (int i = 0; i < m_m; i++)
	{
		for (int j = 0; j < m_n; j++)
		{
			if (Abs(m_v[i][j]) > t)
			{
				t = Abs(m_v[i][j]);
				iMax = i;
				jMax = j;
			}
		}
	}
}

double Matrix::Sum() const
{
	int sz = m_m * m_n;
	if (sz < 1)
		return 0.0;
	double t = 0.0;
	for (int i = 0; i < sz; i++)
		t = t + m_data[i];
	return t;
}

double Matrix::SumAbs() const
{
	int sz = m_m * m_n;
	if (sz < 1)
		return 0.0;
	double t = 0.0;
	for (int i = 0; i < sz; i++)
		t = t + Abs(m_data[i]);
	return t;
}

int Matrix::NumNonZero() const
{
	int sz = m_m * m_n;
	if (sz < 1)
		return 0;
	int k = 0;
	for (int i = 0; i < sz; i++)
	{
		if (m_data[i] != 0.0)
			k++;
	}
	return k;
}

int Matrix::NumNonNegative() const
{
	int sz = m_m * m_n;
	if (sz < 1)
		return 0;
	int k = 0;
	for (int i = 0; i < sz; i++)
	{
		if (m_data[i] >= 0.0)
			k++;
	}
	return k;
}

double Matrix::Norm2AsVector() const
{ 
	int mn = m_m * m_n;
	double sum = 0.0;
	for (int i = 0; i < mn; i++)
		sum += m_data[i] * m_data[i];
	return sum;
}

double Matrix::Norm2() const
{ 
	if (m_m != 1 && m_n != 1)
		Matrix::XError(5, "Matrix::Norm2()");
	return Norm2AsVector();
}

double Matrix::RowDot(int i,int k) const
{
	double sum = 0.0;
	for (int j = 0; j < m_n; j++)
		sum += m_v[i][j] * m_v[k][j];
	return sum;
}

Matrix Matrix::T()              
{
	Matrix b(*this);
	Clear();
	Setup2(b.m_n, b.m_m);
	for (int i = 0; i < m_m; i++)
	{
		for (int j = 0; j < m_n; j++)
			m_v[i][j] = b[j][i];
	}
	return *this;
}

double Matrix::Dot(Matrix& b) const
{ 
	if ((m_m != 1 && m_n != 1) || (b.m_m != 1 && b.m_n != 1)) 
		Matrix::XError(2, "Matrix::Dot(Matrix)");
	int amn = m_m * m_n;
	int bmn = b.m_m * b.m_n;
	if (amn != bmn)
		Matrix::XError(2, "Matrix.Dot(Matrix)");
	double sum = 0.0;
	for (int i = 0; i < amn; i++)
		sum += m_data[i] * b.m_data[i];
	return sum;
}

Matrix Matrix::GetRow(int i) const
{
	Matrix r(1, m_n);
	for (int j = 0; j < m_n; j++)
		r[0][j] = m_data[i * m_n + j];
	return r;
}

Matrix Matrix::GetColumn(int j) const
{
	Matrix c(m_m, 1);
	for (int i = 0; i < m_m; i++)
		c[i][0] = m_data[i * m_n + j];
	return c;
}

void Matrix::SetRow(int i, double val)
{ 
	if (i < 0 || i >= m_m)
		Matrix::XError(1, "Matrix::SetRow(i,val)");
	for (int j = 0; j < m_n; j++)
		m_data[i * m_n + j] = val;
}

void Matrix::SetColumn(int j, double val)  
{ 
	if (j < 0 || j >= m_n)
		Matrix::XError(1, "Matrix::SetColumn(j,val)");
	for (int i = 0; i < m_m; i++)
		m_data[i * m_n + j] = val;
}

void Matrix::SetRow(int i, Matrix a)    
{ 
	if (m_n != a.m_n || a.m_m != 1)
		Matrix::XError(1, "Matrix::SetRow(i,a)");
	if (i < 0 || i >= m_m)
		Matrix::XError(1, "SetRow");
	for (int j = 0; j < m_n; j++)
		m_data[i * m_n + j] = a[0][j];
}

void Matrix::SetColumn (int j, Matrix a)    
{ 
	if (m_m != a.m_m || a.m_n != 1)
		Matrix::XError(1, "Matrix::SetColumn(i,a)");
	if (j < 0 || j >= m_n)
		Matrix::XError(1, "SetColumn");
	for (int i = 0; i < m_m; i++)
		m_data[i * m_n + j] = a[i][0];
}

void Matrix::SetRowZero(int i)
{ 
	if (i < 0 || i >= m_m)
		Matrix::XError(1, "Matrix::SetRowZero");
	for (int j = 0; j < m_n; j++)
		m_data[i * m_n + j] = 0.0;
}

void Matrix::SetColumnZero(int j)
{ 
	if (j < 0 || j >= m_n)
		Matrix::XError(1, "Matrix::SetColumnZero");
	for (int i = 0; i < m_m; i++)
		m_data[i * m_n + j] = 0.0;
}

void Matrix::Resize(int m, int n)
{ 
	if (m == m_m && n == m_n)
		return;	//nothing to do
	CheckDim(m, n);
	Matrix b(*this);	//save a copy
	Clear();	//discard current contents
	Setup2(m, n);	//reconfigure
	if (m_m > 0 && m_n > 0)
	{
		//copy whatever can be copied
		int mm = Min(m_m, b.m_m);
		int nn = Min(m_n, b.m_n);
		for (int i = 0; i < mm; i++)
		{
			for (int j = 0; j < nn; j++)
				m_v[i][j] = b[i][j];
		}
	}  
}
                     
void Matrix::DelRow(int r)
{
	//shift later rows up one
	for (int i = r; i < m_m - 1; i++)
	{
		for (int j = 0; j < m_n; j++)
			m_v[i][j] = m_v[i + 1][j];
	}
	Resize(m_m-1, m_n);
}
      
void Matrix::DelColumn(int c)
{
	//shift later columns left one
	for (int i = 0; i < m_m; i++)
	{
		for (int j = c; j < m_n - 1; j++)
			m_v[i][j] = m_v[i][j + 1];
	}
	Resize(m_m, m_n-1);
}            

void Matrix::AppendRows(const Matrix& b)
{
	if (m_m == 0 || m_n == 0)
	{
		*this = b;
		return;
	}  //append to null matrix
	if (m_n != b.m_n)
		Matrix::XError(2, "Matrix::AppendRows");
	int mm = m_m;	//save current row size of *this
	Resize(m_m + b.m_m, m_n);
	for (int i = 0; i < b.m_m; i++)
	{
		for (int j = 0; j < m_n; j++)
			m_v[mm + i][j] = b[i][j];
	}
}

void Matrix::PrependRows(const Matrix& b)
{ 
	if (m_m == 0 || m_n == 0)
	{
		*this = b;
		return;
	} 
	if (m_n != b.m_n)
		Matrix::XError(2, "Matrix::PrependRows");
	Matrix c = b;
	c.AppendRows(*this);
	*this = c;
} 

void Matrix::AppendColumns(const Matrix& b)
{
	if (m_m == 0 || m_n == 0)
	{
		*this = b;
		return;
	}
	if (m_m != b.m_m)
		Matrix::XError(2, "Matrix::AppendColumns");
	int nn = m_n;	//save current column size of *this
	Resize(m_m, m_n + b.m_n);
	for (int i = 0; i < m_m; i++)
	{
		for (int j = 0; j < b.m_n; j++)
			m_v[i][nn + j] = b[i][j];
	} 
}

void Matrix::PrependColumns(const Matrix& b)
{
	if (m_m == 0 || m_n == 0)
	{
		*this = b;
		return;
	}
	if (m_m != b.m_m)
		Matrix::XError(2, "Matrix::PrependColumns");
	Matrix c = b;
	c.AppendColumns(*this);
	*this = c;
}

void Matrix::Zeros()
{
	for (int i = 0; i < m_m; i++)
	{
		for (int j = 0; j < m_n; j++)
			m_v[i][j] = 0.0;
	}
}

void Matrix::Ones()  
{
	for (int i = 0; i < m_m; i++)
	{
		for (int j = 0; j < m_n; j++)
			m_v[i][j] = 1.0;
	}
}

void Matrix::Identity()
{ 
	Matrix::Zeros();
	int mn = Min(m_m, m_n);
	for (int i = 0; i < mn; i++)
		m_v[i][i] = 1.0;
} 

void Matrix::Iota()
{ 
	for (int i = 0; i < m_m; i++)
	{
		for (int j = 0; j < m_n; j++)
			m_v[i][j] = i + j + 1;
	}
}

void Matrix::IotaZero()
{ 
	for (int i = 0; i < m_m; i++)
	{
		for (int j = 0; j < m_n; j++)
			m_v[i][j] = i + j;
	}
}


void Matrix::Random() 
{
	for (int i = 0; i < m_m; i++)
	{
		for (int j = 0; j < m_n; j++)
			m_v[i][j] = Matrix::MyRandom();
	}
}

void Matrix::Gauss()
{
	for (int i = 0; i < m_m; i++)
	{
		for (int j = 0; j < m_n; j++)
			m_v[i][j] = Matrix::MyGauss();
	}
}

void Matrix::Hilbert()
{ 
	for (int i = 0; i < m_m; i++)
	{
		for (int j = 0; j < m_n; j++)
			m_v[i][j] = 1.0 / double(i + j + 1);
	}
}

void Matrix::Heat()
{ 
	//a suggested problem size is 11 x 51
	if (m_m == 0 || m_n == 0)
		return;
	double xx;
	double tt;
	for (int i = 0; i < m_m; i++)
	{
		xx = 1.5 * double(i) / double(m_m - 1);
		for (int j = 0; j < m_n; j++)
		{
			tt = 1.5 * double(j) / double(m_n - 1);
			m_v[i][j] = exp(-(xx - tt) * (xx - tt));
		}
	}
}

void Matrix::Laplace()
{
	//a suggested problem size is 19 x 21 
	if (m_m == 0 || m_n == 0)
		return;
	double s;
	double t;
	for (int i = 0; i < m_m; i++)
	{
		s = 0.5 + 5.0 * double(i) / double(m_m);
		for (int j = 0; j < m_n; j++)
		{
			t = 5.0 * double(j) / double(m_n);
			m_v[i][j] = exp(-s * t);
		}
	}
}

void Matrix::Cusp()
{
	if (m_m == 0 || m_n == 0)
		return;
	int mn = m_m * m_n;	//normally should be a Row or Vector... otherwise punt
	for (int i = 0; i < mn; i++)
		m_data[i] = sin(PI() * double(i) / double(m_m - 1));
} 

//supports cout << a; rather than a.Print()
std::ostream& operator<<(std::ostream& os, const Matrix& a)
{
	//a.Print();
	int m = a.Dim1();
	int n = a.Dim2();
	os << "Matrix is " << m << " rows by " << n << " columns:" << std::endl;
	for (int i = 0; i < m; i++)
	{ 
		for (int j = 0; j < n; j++) os << std::setw(12) << std::setprecision(6) << a(i, j) << " ";
		os << std::endl;
	}
	os << std::endl;
	return os;
}

void Matrix::Print() const { std::cout << *this; }

void Matrix::PrintByRow() const
{
	std::cout << "Matrix is " << m_m << " rows by " << m_n << " columns:" << std::endl;
	for (int i = 0; i < m_m; i++)
	{ 
		for (int j = 0; j < m_n; j++) 
		{
			if (j % 5 == 0)
			{
				if (j == 0)
					std::cout << "Row " << std::setw(4) << i;
				else
					std::cout << std::endl << "        ";
			}
			std::cout << std::setw(12) << std::setprecision(6) << m_data[i * m_n + j] << " ";
		}  
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void Matrix::PrintByColumn() const      
{ 
	std::cout << "Matrix is " << m_m << " rows by " << m_n << " columns:" << std::endl;
	for (int j = 0; j < m_n; j++)
	{ 
		for (int i = 0; i < m_m; i++) 
		{ 
			if (i % 5 == 0)
			{
				if (i == 0)
					std::cout << "Col " << std::setw(4) << j;
				else
					std::cout << std::endl << "        ";
			}
			std::cout << std::setw(12) << std::setprecision(6) << m_data[i * m_n + j] << " ";
		} 
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void Matrix::PrintA() const
{
	using namespace std;
	if (DMin() == 0)
	{
		cout << "(matrix is empty)" << endl;
		return;
	}
	double t;
	double small = 0.00001;
	int m = Dim1();
	int n = Dim2();
	cout << "Matrix is " << m << " rows by " << n << " columns:" << endl;
	
	int mx = m;
	if (mx > 15)
		mx = 15;
	int nn = n;
	if (nn > 6)
		nn = 6;
	for (int i = 0; i < mx; i++)
	{
		cout << setw(2) << i << ":";
		for (int j = 0; j < nn; j++)
		{
			t = m_v[i][j];
			if (Abs(t) < small)
				t = 0.0;
			cout << setw(10) << setprecision(4) << t << " ";
		}
		if (n > nn)
			cout << "...";
		cout << endl;
	}
	if (mx < m)
		cout << "          ..." << endl;
	cout << endl;
}

void Matrix::PrintAB(const Matrix& b) const	//revise like following routine???
{
	using namespace std;
	if (DMin() == 0)
	{
		cout << "(matrix is empty)" << endl;
		return;
	}
	double t;
	double small = 0.00001;
	int m = Dim1();
	int n = Dim2();
	if (Dim1() != b.Dim1())
		Matrix::XError(2, "Matrix::PrintAB");
	
	cout << "Matrix is " << m << " rows by " << n << " columns:" << endl;
	
	int mx = m;
	if (mx > 15)
		mx = 15;
	int nn = n;
	if (nn > 5)
		nn = 5;
	for (int i = 0; i < mx; i++)
	{
		cout << setw(2) << i << ":";
		for (int j = 0; j < nn; j++)
		{
			t = m_v[i][j];
			if (Abs(t) < small)
				t = 0.0;
			cout << setw(10) << setprecision(4) <<  t  << " ";
		}
		if (n > nn) cout << "...";
		
		double bb = b[i][0];
		if (Abs(bb) < small)
			bb = 0.0;
		cout << " = " << setw(10) << setprecision(4) << bb;
		cout << endl;
	}
	
	if (mx < m)
		cout << "          ..." << setw(58) << " " << "..." << endl;
	cout << setprecision(0) << endl;
}

void Matrix::PrintABE(const Matrix& b, const Matrix& e) const
{
	using namespace std;
	if (DMin() == 0)
	{
		cout << "(matrix is empty)" << endl;
		return;
	}
	double t;
	int m = Dim1();
	int n = Dim2();
	if (Dim1() != b.Dim1())
		Matrix::XError(2, "Matrix::PrintABE");
	if (Dim1() != e.Dim1())
		Matrix::XError(2, "Matrix::PrintABE");
	
	cout << "Matrix is " << m << " rows by " << n << " columns:" << endl;
	
//format is... (i is in first 2 columns)
//0000000111111111122222222223333333333444444444455555555556666666666777777777
//3456789012345678901234567890123456789012345678901234567890123456789012345678
//:-1.234E-15 -2.345E-14 xxxxxxxxxx yyyyyyyyyyy ... = rrrrrrrrrr +- eeeeeeeeee
	
	int mx = m;
	if (mx > 15)
		mx = 15;
	int nn = n;
	if (nn > 4)
		nn = 4;
	for (int i = 0; i < mx; i++)
	{
		cout << setw(2) << i << ":";
		for (int j = 0; j < nn; j++)
		{
			t = m_v[i][j];
			cout << setw(10) << setprecision(4) << t << " ";
		}
		if (n > nn) cout << "... ";
		
		double bb = b[i][0];
		cout << "= " << setw(10) << setprecision(4) << bb << " ";
		
		double ee = e[i][0];
		cout << "+- " << setw(10) << setprecision(4) << ee;
		cout << endl;
	}
	
	if (mx < m) 
	{
		for (int j = 0; j < nn; j++)
			cout << "    ...    ";
		if (n > nn)
			cout << "... ";
		cout << "=    ...     ";
		cout << "+-    ...";
		cout << endl;
	}
	cout << setprecision(0) << endl;
}

void Matrix::PrintAXB(const Matrix &x, const Matrix &b, int maxRows) const
{
	using namespace std;
	if (DMin() == 0)
	{
		cout << "(matrix is empty)" << endl;
		return;
	}
	double t;
	double small = 0.00001;
	int m = Dim1();
	int n = Dim2();
	if (Dim2() != x.Dim1())
		Matrix::XError(2, "Matrix::PrintAXB");
	if (Dim1() != b.Dim1())
		Matrix::XError(2, "Matrix::PrintAXB");
	
	cout << "Matrix is " << m << " rows by " << n << " columns:" << endl;
	
	int mx = m;
	if (n > m)
		mx = n;
	if (mx > maxRows)
		mx = maxRows;
	int nn = n;
	if (nn > 4)
		nn = 4;
	for (int i = 0; i < mx; i++)
	{
		cout << setw(2) << i << ":";
		if (i < m)
		{
			for (int j = 0; j < nn; j++)
			{
				t = m_v[i][j];
				if (Abs(t) < small)
					t = 0.0;
				cout << setw(10) << setprecision(4) <<  t    << " ";
			}
			if (n > nn) cout << "...";
		}
		else
		{
			for (int k = 0; k < nn; k++)
				cout << setw(10) << "  "  << " ";
			if (n > nn) cout << "   ";
		}
		
		double xx = i < n ? x[i][0] : 0.0;
		if (Abs(xx) < small)
			xx = 0.0;
		if (i < n)
			cout << " x" << setw(10) << setprecision(4) << xx;
		else
			cout << "  " << setw(10) << " ";
		
		double bb = i < m ? b[i][0] : 0.0;
		if (Abs(bb) < small)
			bb = 0.0;
		if (i < m)
			cout << " = " << setw(10) << setprecision(4) << bb;
		cout << endl;
	}
	
	if (mx < m)
		cout << "          ...";
	else
		cout << "             ";
	if (mx < n)
		cout << setw(46) << " " << "...      ";
	else
		cout << setw(55) << " ";
	if (mx < m)
		cout << "    ... ";
	cout << endl;
	
	cout << setprecision(0) << endl;
	//int junk; cout << "OK? "; cin >> junk;
}

Matrix Matrix::ComputeStarMagnitudes() const
{
	Matrix mag(*this);
	if (DMin() == 0)
		return mag;
	mag.MAbs();	//absolute values
	double mx = mag.MaxAbs();	//largest
	if (mx == 0.0) 
		mx = 1.0;	//handle all-zeroes case
	mag = mag / mx;	//largest is now 1.0
	mag.MLog10();	//logs range from  0.0 to -15 or so
	mag -= 1.0;	//logs range from -1.0 to -16 or so
	mag.MAbs();	//now all are flipped positive...ranging from 1.0 to 16
	return mag;
}

void Matrix::PrintStarMagnitudes() const
{
	using namespace std;
	if (DMin() == 0)
	{
		cout << "(matrix is empty)" << endl;
		return;
	}
	int m = Dim1();
	int n = Dim2();
	cout << "Printing star magnitudes with rows= " << m << "  columns= " << n << endl;
	double scale = this->MaxAbs();
	if (scale == 0.0)
		scale = 1.0;
	cout << "Magnitude 1 is " << scale << " to >" << scale/10.0 << endl;
	Matrix b = this->ComputeStarMagnitudes();
	int k;
	
	cout << " ";
	for (int j = 0; j < n; j++)
		cout << "-" ;
	cout << endl;
	for (int i = 0; i < m; i++)
	{
		cout << "|";
		for (int j = 0; j < n; j++)
		{
			k = int(b[i][j]);
			if (k <= 9)
				cout << setw(1) << k;
			else cout << " ";
		}
		cout << "|" << endl;
	}
	cout << " ";
	for (int j = 0; j < n; j++)
		cout << "-" ;
	cout << endl;
};

//Free functions for Matrix------------------------------------------

//supports 2.0+a for example...returns element[i][j] = 2.0+a[i][j]
Matrix operator+(double x, const Matrix& a)
{
	Matrix b(a);
	b += x;
	return b;
}

//supports 2.0*a for example...returns element[i][j] = 2.0*a[i][j]
Matrix operator*(double x, const Matrix& a)
{
	Matrix b(a);
	b *= x;
	return b;
}

//supports 2.0-a for example...returns element[i][j] = 2.0-a[i][j]
Matrix operator-(double x, const Matrix& a)
{ 
	Matrix b(a.Dim1(), a.Dim2(), x);
	b -= a;
	return b;
}

//returns the transpose of the object.
//Note that the object itself is NOT modified.  To modify the object, use a.T()
Matrix Transpose(const Matrix& a)
{
	int mm = a.Dim1();
	int nn = a.Dim2();
	Matrix b(nn, mm);
	for (int i = 0; i < nn; i++)
	{
		for (int j = 0; j < mm; j++)
			b[i][j] = a[j][i];
	}
	return b;
}

double Pilof() { return 3.14159265358979 - 2.0 * Matrix::RoundOff(); }
double Pihif() { return 3.14159265358980 + 2.0 * Matrix::RoundOff(); }

//******************************************************************
//The Diagonal Matrix class.
//This is coded completely separately to avoid any possible
//mix of Matrix functionality with Diagonal functionality.
//******************************************************************
class Diagonal 
{ 
private:
	
	int m_m;
	int m_n;
	int m_mm;
	double* m_data;
	
	void CheckDim(int m, int n)
	{
		if (m < 0 || n < 0)
			Matrix::XError(4, "Diagonal::CheckDim");
		return;
	}
	void SetupD(int m, int n);
	
public:
	
	//constructors---------------------------------------------------
	
	//default constructor: 0 length diagonal
	Diagonal() { SetupD(0, 0); }
	
	//construct a Diagonal of size m by m, zero filled
	explicit Diagonal(int m) { SetupD(m, m); } 
	 
	//construct a Diagonal of size m by n, zero filled
	explicit Diagonal(int m, int n) { SetupD(m, n); } 
	
	//construct a Diagonal of size m by n, with every diagonal element set to x
	Diagonal(int m, int n, double x);
	
	//construct a Diagonal of size m by n, with diagonal from array a
	Diagonal (int m, int n, const double* a);
	
	//copy constructor
	Diagonal(const Diagonal& d);
	
	//copy constructor from d with shape change to smaller or larger
	Diagonal(int m, int n, const Diagonal& d);
	
	//construct a Diagonal:
	// 1. from a row, with d[i] = r[i]
	// 2. from a column, with d[i] = c[i]
	// 3. of the same shape as matrix a, with d[i] = a[i][i]
	explicit Diagonal(const Matrix& a);
	
	//destructors
	
	//delete all data and set size to 0 by 0
	void Clear()
	{
		if (m_m > 0 && m_n > 0) { delete [] m_data; }           
		m_m = m_n = m_mm = 0;
	}
	
	~Diagonal() { Clear(); } 
	
	//assignment-----------------------------------------------------
	
	//supports d = x
	Diagonal operator=(const double x);
	
	//supports d = d
	Diagonal operator=(const Diagonal& d);
	
	//resize to smaller or larger
	//keeps upper left content as far as possible; fills with zero
	void Resize(int m, int n);
	
	//create a Diagonal of the same shape as a, 
	//with diagonal elements taken from a's diagonal: d[i] = a[i][i]
	Diagonal operator=(const Matrix& a);
	
	//accessors------------------------------------------------------
	
	//get the row dimension
	inline int Dim1() const { return m_m; }
	
	//get the column dimension
	inline int Dim2() const { return m_n; }
	
	//get the smaller dimension
	inline int DMin() const { return m_m < m_n ? m_m : m_n; }
	
	//get the larger dimension
	inline int DMax() const { return m_m > m_n ? m_m : m_n; }
	
	//index----------------------------------------------------------
	inline double& operator[](int i)
	{ 
		if (i < 0 || i >= m_mm)
			Matrix::XError(1, "Diagonal::operator[]");
		return m_data[i];
	}
	
	inline const double& operator[](int i) const
	{ 
		if (i < 0 || i >= m_mm)
			Matrix::XError(1, "Diagonal::operator[]");
		return m_data[i];
	}
	
	//alternative index form... D(i,i) rather than D[i].
	//This checks the indices for proper range
	inline double& operator()(int i, int j)
	{ 
		if (i < 0 || i >= m_mm || i != j )
			Matrix::XError(1, "Diagonal::operator(,)");
		return m_data[i];
	}
	
	//equivalence operations-----------------------------------------
	
	//supports d1==d2
	bool operator==(const Diagonal& d) const;
	
	//supports d1!=d2
	bool operator!=(const Diagonal& d) const { return !((*this) == d); }  
	
	//approximate equality, called as d.Approximate(d2,0.00000001);
	//Any absolute difference greater than the given scalar causes a return of false.
	bool Approximate(const Diagonal& d, double tolerance) const;
	
	//element-wise operations----------------------------------------
	
	//these operations support d+=2.0  d-=2.0   d*=2.0  d/=2.0  for example
	Diagonal operator+=(double x)
	{
		for (int i = 0; i <m_mm; i++)
			m_data[i] += x;
		return *this;
	}
	Diagonal operator-=(double x)
	{
		for (int i = 0; i <m_mm; i++)
			m_data[i] -= x;
		return *this;
	}
	Diagonal operator*=(double x)
	{
		for (int i = 0; i <m_mm; i++)
			m_data[i] *= x;
		return *this;
	}
	Diagonal operator/=(double x)
	{
		for (int i = 0; i <m_mm; i++)
			m_data[i] /= x;
		return *this;
	}
	
	//these operations support d+2.0  d-2.0   d*2.0  d/2.0  for example
	Diagonal operator+(double x) const
	{
		Diagonal c(*this);
		c += x;
		return c;
	}
	Diagonal operator-(double x) const
	{
		Diagonal c(*this);
		c -= x;
		return c;
	}
	Diagonal operator*(double x) const
	{
		Diagonal c(*this);
		c *= x;
		return c;
	}
	Diagonal operator/(double x) const
	{
		Diagonal c(*this);
		c /= x;
		return c;
	}
	
	//operate and assign
	Diagonal operator+=(const Diagonal& d);
	Diagonal operator-=(const Diagonal& d);
	
	//unary minus--- for b = -a; for example
	Diagonal operator-() const;
	
	//these operations support a+b  and a-b.  a and b must have exactly the same shape
	Diagonal operator+(const Diagonal& d) const;
	Diagonal operator-(const Diagonal& d) const;
	
	//the following allows a*b, where a's second dimension must equal b's first dimension
	Diagonal operator*(const Diagonal& d) const;
	
	//replaces each element with its absolute value
	void MAbs();
	
	//replaces each element with the square root of its absolute value
	void MSqrt();
	
	//replaces each element with its square
	void MSquare();
	
	//Replaces each element with the base 10 log of its absolute value.
	//log(A.MaxAbs())-30.0 is used for zero elements.
	void MLog10();
	
	//Replaces each element a with 10^a.
	//That is, with the antilog10 of a.
	void MPow10();
	
	//makes each element at least x
	void AtLeast(double x);
	
	//min / max -----------------------------------------------------
	
	//returns the (absolute value of the) element which is largest in absolute value
	double MaxAbs() const;
	
	//returns the (absolute value of the) element which is smallest in absolute value
	double MinAbs() const;
	
	//returns the index of the maximum absolute value in the Diagonal
	int IMaxAbs() const;
	
	//returns the index of the minimum absolute value in the Diagonal
	int IMinAbs() const;
	
	//returns the index of the last non-zero in the Diagonal
	int ILastNZ() const;
	
	//returns the trace of the matrix, which is the sum of the diagonal elements.
	double Trace() const;
	
	//find a neglible value for *this
	double Epsilon() const;
	
	//Determine the number of approximately equal singular
	//values at the end of the list of singular values,
	//not counting the first one at that level.
	//In other words, compute p in Algorithm 12.3.1 See Golub and Van Loan, 2nd Ed.
	//The point is that S(k-p-1,k-p-1) should be signifcantly larger than S(k-p,k-p)
	//where k=number of singular values in S.
	//Ideally, p should be 0.
	int Plateau() const;
	
	//transposes *this
	void T()
	{
		int t = m_n;
		m_n = m_m;
		m_m = t;
	}
	
	//builders
	void Zeros();	//set *this to all zeros     
	void Ones();	//set *this to all ones   
	void Identity();	//set *this to identity matrix     
	void Iota();	//set *this[i][j] = i + j + 1.  In a row that's 1, 2, 3, ...
	void Random();	//set *this to random values in (0,1)
	void Gauss();	//set *this to random Gaussian, mean 0, standard deviation 1 
	
	void Print() const;

};	//class Diagonal 

//implementations for Diagonal 

void Diagonal::SetupD(int m, int n)
{
	CheckDim(m ,n);
	m_m = m;
	m_n = n;
	m_mm = m_m;
	if (m_n < m_m)
		m_mm = m_n;
	if (m_m == 0 || m_n == 0)
		return;
	
	m_data = new double[m_mm];
	for (int i = 0; i < m_mm; i++)
		m_data[i] = 0.0;
}
   
Diagonal::Diagonal(int m, int n, double x)
{
	SetupD(m, n);
	for (int i = 0; i < m_mm; i++)
		m_data[i] = x;
}

Diagonal::Diagonal(int m, int n, const double* a)
{
	SetupD(m, n);
	for (int i = 0; i < m_mm; i++)
		m_data[i] = a[i];
}

Diagonal::Diagonal(const Diagonal& d)
{
	int m = d.Dim1();
	int n = d.Dim2();
	SetupD(m, n);
	for (int i = 0; i < m_mm; i++)
		m_data[i] = d[i];
}

Diagonal::Diagonal(int m, int n, const Diagonal& d)
{
	SetupD(m, n);
	int mm = Min(m_mm , d.m_mm);
	for (int i = 0; i < mm; i++)
		m_data[i] = d[i];
}

Diagonal::Diagonal(const Matrix& a)
{
	int m = a.Dim1();
	int n = a.Dim2();
	if (m == 1)
	{
		SetupD(n, n);
		for (int j = 0; j < n; j++)
			m_data[j] = a[0][j];
	}
	else if (n == 1)
	{
		SetupD(m, m);
		for (int i = 0; i < m; i++)
			m_data[i] = a[i][0];
	}
	else
	{
		SetupD(m, n);
		for (int i = 0; i < m_mm; i++)
			m_data[i] = a[i][i];
	}
};

Diagonal Diagonal::operator=(const double x)
{
	for (int i = 0; i < m_mm; i++)
		m_data[i] = x;
	return *this;
}

Diagonal Diagonal::operator=(const Diagonal& d)
{ 
	Clear();
	int m = d.Dim1();
	int n = d.Dim2();
	SetupD(m, n);
	for (int i = 0; i < m_mm; i++)
		m_data[i] = d[i];
	return *this;
} 

void Diagonal::Resize(int m, int n)
{
	Diagonal p = *this;
	SetupD(m, n);
	int mm = Min(m_mm, p.m_mm);
	for (int i = 0; i < mm; i++)
		m_data[i] = p[i];
}

Diagonal Diagonal::operator=(const Matrix& a)
{
	Diagonal e(a);
	*this = e;
	return *this;
}

bool Diagonal::operator==(const Diagonal& d) const
{
	if (m_m != d.m_m || m_n != d.m_n)
		return false;
	for (int i = 0; i < m_mm; i++)
	{
		if (m_data[i] != d.m_data[i])
			return false;
	}
	return true;
}

bool Diagonal::Approximate(const Diagonal& d, double tolerance) const
{
	if (m_m != d.m_m || m_n != d.m_n)
		return false;
	for (int i = 0; i < m_mm; i++)
	{
		if (Abs(m_data[i] - d.m_data[i]) > tolerance)
			return false;
	}
	return true;
}

Diagonal Diagonal::operator+=(const Diagonal& d)
{
	if (m_m != d.m_m || m_n != d.m_n)
		Matrix::XError(2, "Diagonal+=Diagonal");
	for (int i = 0; i < m_mm; i++)
		m_data[i] += d[i];
	return *this;
}

Diagonal Diagonal::operator-=(const Diagonal& d)
{
	if (m_m != d.m_m || m_n != d.m_n)
		Matrix::XError(2, "Diagonal-=Diagonal");
	for (int i = 0; i < m_mm; i++)
		m_data[i] -= d[i];
	return *this;
}

Diagonal Diagonal::operator-() const
{
	Diagonal d(*this);
	for (int i = 0; i < m_mm; i++)
		d[i] = -d[i];
	return d;
}

Diagonal Diagonal::operator+(const Diagonal& d) const
{
	Diagonal e(*this);
	e += d;
	return e;
}

Diagonal Diagonal::operator-(const Diagonal& d) const
{
	Diagonal e(*this);
	e -= d;
	return e;
}

Diagonal Diagonal::operator*(const Diagonal& d) const
{
	if (m_n != d.m_m)
		Matrix::XError(2, "Diagonal*Diagonal");
	Diagonal e(m_m, d.m_n);
	if (m_m == 0 || m_n == 0 || d.m_m == 0 || d.m_n == 0)
		return e;

	int m = Min(m_mm, d.m_mm);	//smallest of all 4 dimensions! 2011
	for (int i = 0; i < m; i++)
		e[i] = m_data[i] * d[i];
	return e;
}

void Diagonal::MAbs()
{
	for (int i = 0; i < m_mm; i++)
		m_data[i] = Abs(m_data[i]);
}

void Diagonal::MSqrt()
{
	for (int i = 0; i < m_mm; i++)
		m_data[i] = sqrt(Abs(m_data[i]));
}

void Diagonal::MSquare()
{
	for (int i = 0; i < m_mm; i++)
		m_data[i] = m_data[i] * m_data[i];
}   

void Diagonal::MLog10()
{
	double tiny = log10((*this).MaxAbs()) - 30.0;
	for (int i = 0; i < m_mm; i++)
	{
		if (m_data[i] != 0.0)
			m_data[i] = log10(Abs(m_data[i]));
		else
			m_data[i] = tiny;
	}
}

void Diagonal::MPow10()
{
	double scale = log(10.0);
	for (int i = 0; i < m_mm; i++)
		m_data[i] = exp(scale * m_data[i]);
}

void Diagonal::AtLeast(double x)
{
	for (int i = 0; i < m_mm; i++)
	{
		if (m_data[i] < x)
			m_data[i] = x;
	}
}

double Diagonal::MaxAbs() const
{
	if (m_mm < 1)
		return 0.0;
	double t = Abs(m_data[0]);
	for (int i = 1; i<m_mm; i++)
	{
		if (Abs(m_data[i]) > t)
			t = Abs(m_data[i]);
	}
	return t;
}

double Diagonal::MinAbs() const
{
	if (m_mm < 1)
		return 0.0;
	double t = Abs(m_data[0]);
	for (int i = 1; i < m_mm; i++)
	{
		if (Abs(m_data[i]) < t)
			t = Abs(m_data[i]);
	}
	return t;
}

int Diagonal::IMaxAbs() const
{
	if (m_mm < 1)
		Matrix::XError(3, "Diagonal::IMaxAbs");
	double t = Abs(m_data[0]);
	int k = 0;
	for (int i = 1; i < m_mm; i++)
	{
		if (Abs(m_data[i]) > t)
		{
			t = Abs(m_data[i]);
			k = i;
		}
	}
	return k;
}

int Diagonal::IMinAbs() const
{
	if (m_mm < 1)
		Matrix::XError(3, "Diagonal::IMinAbs");
	double t = Abs(m_data[0]);
	int k = 0;
	for (int i = 1; i < m_mm; i++)
	{
		if (Abs(m_data[i]) < t)
		{
			t = Abs(m_data[i]);
			k = i;
		}
	}
	return k;
}

int Diagonal::ILastNZ() const
{
	if (m_mm < 1)
		Matrix::XError(3, "Diagonal::IMinAbs");
	int k = -1;
	for (int i = 0; i < m_mm; i++)
	{
		if (m_data[i] != 0.0)
			k = i;
	}
	return k;
}

double Diagonal::Trace() const
{
	double t = 0.0;
	for (int i = 0; i < m_mm; i++)
		t += m_data[i];
	return t;
}

double Diagonal::Epsilon() const { return MaxAbs() * 8.0 * Matrix::RoundOff(); }

int Diagonal::Plateau() const
{
	double eps = 10.0 * Epsilon();
	double seps = sqrt(eps);

	int p = 0;
	for (int i = m_mm - 1; i > 0; i--)
	{
		if (m_data[i - 1] > eps && m_data[i - 1] > (1.0 + seps) * m_data[i])
			break;
		p++;
	}
	if (p >= m_mm - 1)
		p = 0;	//for a constant set of singular values. 
	return p;
}

void Diagonal::Zeros()
{
	for (int i = 0; i < m_mm; i++)
		m_data[i] = 0.0;
}

void Diagonal::Ones()
{
	for (int i = 0; i < m_mm; i++)
		m_data[i] = 1.0;
}

void Diagonal::Identity()
{
	for (int i = 0; i < m_mm; i++)
		m_data[i] = 1.0;
}

void Diagonal::Iota()
{
	for (int i = 0; i < m_mm; i++)
		m_data[i] = i + 1;
}

void Diagonal::Random()
{
	for (int i = 0; i < m_mm; i++)
		m_data[i] = Matrix::MyRandom();
}

void Diagonal::Gauss()
{
	for (int i = 0; i < m_mm; i++)
		m_data[i] = Matrix::MyGauss();
}

std::ostream& operator<<(std::ostream& os, const Diagonal& d)
{
	using namespace std;
	int m = d.Dim1();
	int n = d.Dim2();
	int mm = d.DMin();
	os << "Matrix is Diagonal of size " << m << " rows by " << n << " columns:" << endl;
	for (int i = 0; i < mm; i++)
	{
		for (int j = 0; j < i; j++)
			os << " ";
		os << d[i] << " " << endl;
	}
	for (int i = mm; i < m; i++)
	{
		for (int j = 0; j < i; j++)
			os << " ";
		os << "..." << endl;
	}
	os << endl;
	return os;
}

void Diagonal::Print() const
{
	std::cout << *this;
}

//Free functions for Diagonal-------------------------------

//supports 2.0+d for example...returns element[i][j] = 2.0+d[i][j]
Diagonal operator+(double x, const Diagonal& d)
{
	Diagonal b(d);
	b += x;
	return b;
}

//supports 2.0*d for example...returns element[i][j] = 2.0*d[i][j]
Diagonal operator*(double x, const Diagonal& d)
{
	Diagonal b(d);
	b *= x;
	return b;
}

//supports 2.0-d for example...returns element[i][j] = 2.0-d[i][j]
Diagonal operator-(double x, const Diagonal& d)
{
	Diagonal b(d.Dim1(), d.Dim2(), x);
	b -= d;
	return b;
}

//returns a Diagonal which is the transpose of the argument.  
//The argument is not changed.
Diagonal Transpose(const Diagonal& d)
{
	Diagonal b(d);
	b.T();
	return b;
}

//returns the pseudo-inverse of a Diagonal matrix d.
//Values below or near roundoff times the largest (magnitude) diagonal
//element are considered to be zero
Diagonal PseudoInverse(const Diagonal& d)
{
	int mm = d.DMin();
	if (mm < 1)
		Matrix::XError(3, "PseudoInverse(diagonal)");
	Diagonal s = Transpose(d);
	double eps = d.Epsilon();
	for (int i = 0; i < mm; i++)
	{
		if (s[i] > eps)
			s[i] = 1.0 / s[i];
		else
			s[i] = 0.0;
	}
	return s;
}

//returns the smoothed (regularized) pseudo-inverse of a Diagonal matrix D.
//Values below or near roundoff times the largest (magnitude) diagonal
//element are considered to be zero
Diagonal SmoothInverse(const Diagonal& s, double lambda)
{
	int m = s.DMin();
	if (m < 1)
		Matrix::XError(3, "SmoothInverse(diagonal)");

	if (lambda == 0.0)
		return PseudoInverse(s);
	double lambda2 = lambda * lambda;
	Diagonal p = Transpose(s);
	for (int i = 0; i < m; i++)
		p[i] = s[i] / (s[i] * s[i] + lambda2);

	double eps = s.Epsilon();
	double large = 1.0 / eps;
	for (int i = 0; i < m; i++)
	{
		if (p[i] > large)
			p[i] = 0.0;
	}

	return p;
}

//Tikhonov regularize the Matrix A with given lambda.
//Seldom necessary. Use the above when possible.
Diagonal Regularize(const Diagonal& s, double lambda)
{
	int m = s.Dim1();
	int n = s.Dim2();
	if (m == 0 || n == 0)
		return Diagonal(m, n);
	if (lambda <= 0.0)
		return s;

	double lambda2 = lambda * lambda;
	double eps = s.Epsilon();
	Diagonal p = s;
	for (int i = 0; i < p.DMin(); i++)
	{
		if (s[i] > eps)
			p[i] = (s[i] * s[i] + lambda2) / s[i];
		else
			p[i] = 0.0;
	}
	return p;
}

//returns the condition number of a Diagonal matrix d.
double ConditionNumber(const Diagonal& d)
{
	int mm = d.DMin();
	if (mm < 1)
		Matrix::XError(3, "ConditionNumber(diagonal)");
	double a = Abs(d[0]);
	double big = a;
	double small = a;
	for (int i = 1; i < mm; i++)
	{
		a = Abs(d[i]);
		if (a > big)
			big = a;
		if (a < small)
			small = a;
	}
	if (big == 0.0)
		return 1.0 / Matrix::RoundOff();
	if (small == 0.0)
		return 1.0 / d.Epsilon();
	return big / small;
}

//returns the condition number of a Diagonal matrix ignoring zero or near-zero values
double ConditionNumberNonZero(const Diagonal& d)
{
	int mm = d.DMin();
	if (mm < 1)
		Matrix::XError(3, "ConditionNumber(diagonal)");
	double eps = d.Epsilon();
	double a = Abs(d[0]);
	double big = a;
	double small = a;
	for (int i = 1; i < mm; i++)
	{
		a = Abs(d[i]);
		if (a <= eps)
			continue;
		if (a > big)
			big = a;
		if (a < small)
			small = a;
	}
	if (big == 0.0)
		return 1.0 / Matrix::RoundOff();
	if (small == 0.0)
		return 1.0 / d.Epsilon();
	return big / small;
}

//Matrix * Diagonal
Matrix operator*(const Matrix& a, const Diagonal& d)
{
	int am = a.Dim1();
	int an = a.Dim2();

	int mm = d.DMin();
	int bm = d.Dim1();
	int bn = d.Dim2();
	if (an != bm)
		Matrix::XError(2, "Matrix*Diagonal");

	Matrix c(am, bn);
	for (int j = 0; j < mm; j++)
	{
		for (int i = 0; i < am; i++)
			c[i][j] = a[i][j] * d[j];
	}
	//any extra columns remain at 0.0
	return c;
}

//Diagonal * Matrix
Matrix operator*(const Diagonal& d, const Matrix& b)
{
	int am = d.Dim1();
	int an = d.Dim2();
	int mm = d.DMin();

	int bm = b.Dim1();
	int bn = b.Dim2();
	if (an != bm)
		Matrix::XError(2, "Diagonal*Matrix");

	Matrix c(am, bn);
	for (int i = 0; i < mm; i++)
	{
		for (int j = 0; j < bn; j++)
			c[i][j] = d[i] * b[i][j];
	}
	//any extra rows remain at 0.0
	return c;
}

  //create a full Matrix of the same shape as D
  //with diagonal elements taken from D and zeros otherwise
Matrix Full(const Diagonal& d)
{
	int m = d.Dim1();
	int n = d.Dim2();
	Matrix a(m, n);
	int mm = d.DMin();
	for (int i = 0; i < mm; i++)
		a[i][i] = d[i];
	return a;
}

//******************************************************************
//Row and Vector classes.
//******************************************************************

class Row : public Matrix
{
public:
	//constructors

	//default constructor: 0 length row
	Row() : Matrix(0, 0) {}

	//construct a row of length m, zero filled
	explicit Row(int m) : Matrix(1, m) {}

	//construct a row of length m, with data from array a
	explicit Row(int m, double x) : Matrix(1, m, x) {}

	//construct a row of length m, with data from 1D array a
	explicit Row(int m, const double* a);

	//copy constructor
	Row(const Row& r) : Matrix(r) {}

	//construct a Row from a 1-row Matrix 
	Row(const Matrix& a) : Matrix(a)
	{
		if (a.Dim1() > 1)
			Matrix::XError(2, "Row(Matrix)");
	}

	//construct a Row from a Diagonal
	explicit Row(const Diagonal& d);

	//construct a row from a list of at least 2 values (must be less than big)
	explicit Row(double t1, double t2, double t3 = big2, double t4 = big2, double t5 = big2, double t6 = big2, double t7 = big2, double t8 = big2, double t9 = big2, double t10 = big2);

	//supports for example r = 3.14;
	Row operator=(double x);

	//assignment ... r = a, where a is a 1-row matrix
	Row operator=(const Matrix& a);

	//get the primary dimension
	int Dim() const { return m_n; }
	int Size() const { return m_n; }

	//index
	double& operator[](int i)
	{
		if (i < 0 || i >= m_n)
			Matrix::XError(1, "Row::operator[]");	//DELETE for no debug
		return m_data[i];
	}

	inline const double& operator[](int i) const
	{
		if (i < 0 || i >= m_n)
			Matrix::XError(1, "Row::operator[]");	//DELETE for no debug
		return m_data[i];
	}

	//limitations
	void Resize(int n) { Matrix::Resize(1, n); }
	void Resize(int m, int n)
	{
		if (m == 1)
			Matrix::Resize(m, n);
		else
			Matrix::XError(9, "Row::Resize to Matrix");
	}
	void AppendColumns(const Matrix& b)
	{
		if (b.Dim1() == 1)
			Matrix::AppendColumns(b);
		else
			Matrix::XError(2, "Vector::AppendColumns()");
	}
	void PrependColumns(const Matrix& b)
	{
		if (b.Dim1() == 1)
			Matrix::PrependColumns(b);
		else
			Matrix::XError(2, "Vector::PrependColumns()");
	}

	//prohibitions
private:
	void T() { Matrix::XError(9, "Row::T()"); }
	void DelRow(int r) { Matrix::XError(9, "Row::DelRow()"); }
	void AddRows(int m) { Matrix::XError(9, "Row::add_row()"); }
	void AppendRows(const Matrix& b) { Matrix::XError(9, "Row::AppendRows()"); }
	void PrependRows(const Matrix& b) { Matrix::XError(9, "Row::PrependRows()"); }

};	//end class Row

Row::Row(int m, const double* a)
{
	Setup2(1, m);
	for (int i = 0; i < m; i++)
		m_data[i] = a[i];
}

Row::Row(double t1, double t2, double t3, double t4, double t5, double t6, double t7, double t8, double t9, double t10)
{
	int k = Matrix::CountArgs(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10);
	Setup2(1, k);
	m_data[0] = t1;
	if (k >= 2)
		m_data[1] = t2;
	if (k >= 3)
		m_data[2] = t3;
	if (k >= 4)
		m_data[3] = t4;
	if (k >= 5)
		m_data[4] = t5;
	if (k >= 6)
		m_data[5] = t6;
	if (k >= 7)
		m_data[6] = t7;
	if (k >= 8)
		m_data[7] = t8;
	if (k >= 9)
		m_data[8] = t9;
	if (k >= 10)
		m_data[9] = t10;
}

Row::Row(const Diagonal& d)
{
	int mm = d.DMin();
	Setup2(1, mm);
	for (int i = 0; i < mm; i++)
		m_data[i] = d[i];
}

Row Row::operator=(double x)
{
	for (int j = 0; j < m_n; j++)
		m_data[j] = x;
	return *this;
}

Row Row::operator=(const Matrix& a)
{
	Clear();
	if (a.Dim1() > 1)
		Matrix::XError(2, "Row=Matrix");
	int n = a.Dim2();
	Setup2(1, n);
	for (int j = 0; j < n; j++)
		m_data[j] = a[0][j];
	return *this;
}

//-------------------------------------------------------------------
class Vector : public Matrix
{
public:
	//constructors

	//default constructor: 0 length
	Vector() : Matrix(0, 0) {}

	//construct a Vector of length m, zero filled
	explicit Vector(int m) : Matrix(m, 1) {}

	//construct a Vector of length m, filled with the value x
	explicit Vector(int m, double x) : Matrix(m, 1, x) {}

	//construct a Vector of length m, with data from 1D array a
	explicit Vector(int m, const double* a);

	//copy constructor
	Vector(const Vector& v) : Matrix(v) {}

	//construct a Vector from a 1-column Matrix 
	Vector(const Matrix& a) : Matrix(a)
	{
		if (a.Dim2() > 1)
			Matrix::XError(2, "Vector(Matrix)");
	}

	//construct a Vector from a Diagonal
	explicit Vector(const Diagonal& d);

	//construct a Vector from a list of values (each less than big)
	explicit Vector(double t1, double t2, double t3 = big2, double t4 = big2, double t5 = big2, double t6 = big2, double t7 = big2, double t8 = big2, double t9 = big2, double t10 = big2);

	//supports for example r = 3.14;
	Vector operator=(double x);

	//supports v = a, where a is a 1-column matrix
	Vector operator=(const Matrix& a);

	//copy a vector from the diagonal elements of a Diagonal matrix
	Vector operator=(const Diagonal &D);

	//get the primary dimension
	int Dim() const { return m_m; }
	int Size() const { return m_m; }

	//index
	double& operator[](int i)
	{
		if (i < 0 || i >= m_m)
		Matrix::XError(1, "Vector::operator[]");	//DELETE for no debug
		return m_data[i];
	}

	inline const double& operator[](int i) const
	{
		if (i < 0 || i >= m_m)
			Matrix::XError(1, "Vector::operator[]");	//DELETE for no debug
		return m_data[i];
	}

	//normalize this vector to unit norm, if possible
	void Normalize()
	{
		double a = Norm();
		if (a > 0.0)
			operator*=(1.0 / a);
	}

	//returns the index of the algebraically maximum value in the Vector.
	int IMax() const;

	//returns the index of the algebraically minimum value in the Vector.
	int IMin() const;

	//returns the index of the maximum absolute value in the Vector.
	int IMaxAbs() const;

	//returns the index of the minimum absolute value in the Vector.
	int IMinAbs() const;

	//create a vector from elements i through j of *this: Vector v2=v1.Range(3:9);
	Vector Range(int i, int j);

	//stack operations
	//Note that these operations are intended as a convenience,
	//not an optimally efficient mechanism.
	//Every push and pop requires a resize.

	//add a value to the front of the vector (becomes the new (*this)[0])
	void PushFront(double value);

	//remove the first value in the vector (remove (*this)[0])
	void PopFront();

	//add a value to the end of the vector
	void PushEnd(double value);

	//remove the fast value in the vector
	void PopEnd();

	//sort the vector into increasing order
	void Sort();

	//sort the Vector into increasing order and carry along the integer array p
	void Sort(int* p);

	//return the median value; for even lengths, return average of the two in the middle 
	double Median() const;

	//return the moving average of the elements of the vector (slow/accurate method)
	Vector MovingAverage(int w);

	//return the moving average of the elements of the vector (fast/inaccurate method)
	Vector MovingAverageFast(int w);

	//limitations
	void Resize(int m) { Matrix::Resize(m, 1); }
	void Resize(int m, int n)
	{
		if (n == 1)
			Matrix::Resize(m, n);
		else
			Matrix::XError(9, "Vector::Resize to Matrix");
	}
	void AppendRows(const Matrix& b)
	{
		if (b.Dim2() == 1)
			Matrix::AppendRows(b);
		else
			Matrix::XError(2, "Vector::AppendRows()");
	}
	void PrependRows(const Matrix& b)
	{
		if (b.Dim2() == 1)
			Matrix::PrependRows(b);
		else
			Matrix::XError(2, "Vector::PrependRows()");
	}

	//prohibitions  
private:
	void T() { Matrix::XError(9, "Vector::T()"); }
	void DelColumn(int r) { Matrix::XError(9, "Vector::del_rcolumn()"); }
	void AddColumns(int m) { Matrix::XError(9, "Vector::AddColumns()"); }
	void AppendColumns(const Matrix& b) { Matrix::XError(9, "Vector::AppendColumns()"); }
	void PrependColumns(const Matrix& b) { Matrix::XError(9, "Vector::PrependColumns()"); }

private:
	//sort elements v[m] to v[n] in increasing order.
	//w must be a work array at least as large as v.
	//If carry is true then the integer array p is carried along.
	//The array p must be the same length as v, and 
	//the integer work array q must be at least as large as p.
	static void Sort(double* v, double* w, bool carry, int* p, int* q, int m, int n);
};

Vector::Vector(int m, const double* a)
{
	Setup2(m, 1);
	if (m > 0)
	{
		for (int i = 0; i < m; i++)
			m_data[i] = a[i];
	}
}

Vector::Vector(const Diagonal& d)
{
	int mm = d.DMin();
	Setup2(mm, 1);
	for (int i = 0; i < mm; i++)
		m_data[i] = d[i];
}

Vector::Vector(double t1, double t2, double t3, double t4, double t5, double t6, double t7, double t8, double t9, double t10)
{
	int k = Matrix::CountArgs(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10);
	Setup2(k, 1);
	m_data[0] = t1;
	if (k >= 2)
		m_data[1] = t2;
	if (k >= 3)
		m_data[2] = t3;
	if (k >= 4)
		m_data[3] = t4;
	if (k >= 5)
		m_data[4] = t5;
	if (k >= 6)
		m_data[5] = t6;
	if (k >= 7)
		m_data[6] = t7;
	if (k >= 8)
		m_data[7] = t8;
	if (k >= 9)
		m_data[8] = t9;
	if (k >= 10)
		m_data[9] = t10;
}

Vector Vector::operator=(double x)
{
	for (int i = 0; i < m_m; i++)
		m_data[i] = x;
	return *this;
}

Vector Vector::operator=(const Matrix& a)
{
	Clear();
	if (a.Dim2() > 1)
		Matrix::XError(2, "Vector=Matrix");
	int m = a.Dim1();
	Setup2(m, 1);
	for (int i = 0; i < m; i++)
		m_data[i] = a[i][0];
	return *this;
}

Vector Vector::operator=(const Diagonal& d)
{
	Clear();
	int m = d.DMin();
	Setup2(m, 1);
	for (int i = 0; i < m; i++)
		m_data[i] = d[i];
	return *this;
}

int Vector::IMax() const
{
	int sz = m_m * m_n;
	if (sz < 1)
		Matrix::XError(3, "Matrix::IMaxAbs");
	double t = m_data[0];
	int k = 0;
	for (int i = 1; i < sz; i++)
	{
		if (m_data[i] > t)
		{
			t = m_data[i];
			k = i;
		}
	}
	return k;
}

int Vector::IMin() const
{
	int sz = m_m * m_n;
	if (sz < 1)
		Matrix::XError(3, "Matrix::IMinAbs");
	double t = m_data[0];
	int k = 0;
	for (int i = 1; i < sz; i++)
	{
		if (m_data[i] < t)
		{
			t = m_data[i];
			k = i;
		}
	}
	return k;
}

int Vector::IMaxAbs() const
{
	int sz = m_m * m_n;
	if (sz < 1)
		Matrix::XError(3, "Matrix::IMaxAbs");
	double t = Abs(m_data[0]);
	int k = 0;
	for (int i = 1; i < sz; i++)
	{
		if (Abs(m_data[i]) > t)
		{
			t = Abs(m_data[i]);
			k = i;
		}
	}
	return k;
}

int Vector::IMinAbs() const
{
	int sz = m_m * m_n;
	if (sz < 1)
		Matrix::XError(3, "Matrix::IMinAbs");
	double t = Abs(m_data[0]);
	int k = 0;
	for (int i = 1; i < sz; i++)
	{
		if (Abs(m_data[i]) < t)
		{
			t = Abs(m_data[i]);
			k = i;
		}
	}
	return k;
}

Vector Vector::Range(int i, int j)
{
	if (i < 0 || j < 0 || i >= Dim1() || j >= Dim1() || j < i)
		Matrix::XError(4, "Vector::Range(,)");
	Vector x(j - i + 1);
	for (int k = i; k <= j; k++)
		x[k - i] = m_data[k];
	return x;
}

void Vector::PushFront(double value)
{
	int m = m_m + 1;
	Vector b(*this);
	Clear();
	Setup2(m, 1);
	for (int i = 1; i < m; i++)
		m_data[i] = b[i - 1];
	m_data[0] = value;
}

void Vector::PopFront()
{
	int m = m_m - 1;
	Vector b(*this);
	Clear();
	Setup2(m, 1);
	for (int i = 0; i < m; i++)
		m_data[i] = b[i + 1];
}

void Vector::PushEnd(double value)
{
	int m = m_m + 1;
	Vector b(*this);
	Clear();
	Setup2(m, 1);
	for (int i = 0; i < m - 1; i++)
		m_data[i] = b[i];
	m_data[m - 1] = value;
}
    
void Vector::PopEnd()
{
	int m = m_m - 1;
	Vector b(*this);
	Clear();
	Setup2(m, 1);
	for (int i = 0; i < m; i++)
		m_data[i] = b[i];
}

void Vector::Sort(double* v, double* w, bool carry, int* p, int* q, int m, int n)
{
	int i;
	int j;
	int k;
	double temp;
	int temp2;

	//note that if m==n or the arguments are misordered nothing happens.
	if (n < m + 10)
	{
		//do a bubble sort for short sequences
		for (i = m + 1; i <= n; i++)
		{
			for (j = i; j > m; j--)
			{
				if (v[j] > v[j - 1])
					break;
				temp = v[j];
				v[j] = v[j - 1];
				v[j - 1] = temp;
				if (carry)
				{
					temp2 = p[j];
					p[j] = p[j - 1];
					p[j - 1] = temp2;
				}
			}
		}
		return;
	}

	//for long sequences split them into two and sort each...
	int mid = (m + n) / 2;
	Sort(v, w, carry, p, q, m, mid);
	Sort(v, w, carry, p, q, mid + 1, n);

	//then merge the two parts...
	i = m;
	j = mid + 1;
	k = m - 1;
	while (i <= mid || j <= n)
	{
		k++;
		if (i > mid)
		{
			w[k] = v[j];
			if (carry)
				q[k] = p[j];
			j++;
		}
		else if (j > n)
		{
			w[k] = v[i];
			if (carry)
				q[k] = p[i];
			i++;
		}
		else if (v[i] <= v[j])
		{
			w[k] = v[i];
			if (carry)
				q[k] = p[i];
			i++;
		}
		else
		{
			w[k] = v[j];
			if (carry)
				q[k] = p[j];
			j++;
		}
	}

	//and copy the result back into original arrays
	for (i = m; i <= n; i++)
		v[i] = w[i];
	if (carry)
	{
		for (i = m; i <= n; i++)
			p[i] = q[i];
	}
}

void Vector::Sort()
{
	int m = (*this).Dim1();
	Vector w(m);
	int* p;
	p = new int;
	int* q;
	q = new int;
	Sort(m_data, w.m_data, false, p, q, 0, m - 1);
}

void Vector::Sort(int* p)
{
	int m = (*this).Dim1();
	Vector w(m);
	int *q;
	q = new int[m];
	Sort(m_data, w.m_data, true, p, q, 0, m - 1);
}

double Vector::Median() const
{
	Vector b = *this;
	b.Sort();
	int m = b.Dim1();
	if (m < 1)
		return 0.0;
	int h = m / 2;
	if (2 * h == m)
		return 0.5 * (b[h - 1] + b[h]);
	else
		return b[h];
}

Vector Vector::MovingAverage(int w)
{
	//compute moving average accurately, but sloe
	if (w <= 0 || w > m_m)
		Matrix::XError(7, "Vector::MovingAverage()");
	int ln = m_m - w + 1;
	Vector avg(ln);
	double sum;
	for (int i = 0; i < ln; i++)
	{
		sum = 0.0;
		for (int j = i; j < i + w; j++)
			sum += m_data[j];
		avg[i] = sum;
	}
	return avg;
}

Vector Vector::MovingAverageFast(int w)
{
	//compute fast moving average which will have cumulative round-off error
	//cost is about 1 addition per element of the input vector
	if (w <= 0 || w > m_m)
		Matrix::XError(7, "Vector::MovingAverage()");
	int ln = m_m - w + 1;
	Vector avg = Vector(ln);

	//do first sum
	double sum = 0.0;
	for (int j = 0; j < w; j++)
		sum += m_data[j];
	avg[0] = sum;

	//do subsequent sum adjustments
	for (int i = 1; i < ln; i++)
	{
		sum = sum - m_data[i - 1] + m_data[i + w - 1];
		avg[i] = sum;
	}
	return avg;
}

//end class Vector

//******************************************************************
//Column is typedef'd to "Vector"
//******************************************************************
typedef Vector Column;

//******************************************************************
//Miscellaneous Free functions 
//******************************************************************

//Householder operations -- See Golub and Van Loan, 2nd Ed, Sec 5.1.3

//Create a Householder matrix of size m by m that zeroes all but
//the first element of the subcolumn or subrow given in vector x.
//NOTE: this is a quick-and-dirty way to implement Householder,
//and is not appropriate to use for matrix decompositions.
//Its purpose in rejtrix.h is for special situations such as T.L.S.
//See Golub and Van Loan for proper way to implement Householder
//for use in decompositions.

Matrix Householder(int m, const Vector& x)
{
	int k = x.Dim();
	if (m < k + 1)
		Matrix::XError(11, "Matrix::Householder");
	double mu = x.Norm();
	Vector v = x;
	if (mu != 0.0)
	{
		double beta = x[0] + SignNum(x[0]) * mu;
		for (int i = 1; i < k; i++)
			v[i] /= beta;
	}
	v[0] = 1.0;

	//now build the whole matrix
	Matrix h = Matrix::Identity(k, k) - (2.0 / v.Norm2()) * v * Transpose(v);
	int d = m - k;
	Matrix h2 = Matrix::Identity(m, m);
	for (int i = 0; i < k; i++)
	{
		for (int j = 0; j < k; j++)
			h2(i + d, j + d) = h(i, j);
	}
	return h2;
}

//******************************************************************
//Simple "printer plot" routines for demos.
//******************************************************************

//plot yy versus xx; xx and yy must be columns or rows
void Plot(const Matrix& xx, const Matrix& yy)
{
	using namespace std;
	if (xx.Dim1() * xx.Dim2() < 2)
	{
		cout << "Can't plot one value -- skipping plot" << endl;
		return;
	}
	if (xx.Dim1() == 0 || yy.Dim1() == 0)
		return;
	if (xx.Dim1() != 1 && xx.Dim2() != 1)
		Matrix::XError(4, "Plot(,)");
	if (yy.Dim1() != 1 && yy.Dim2() != 1)
		Matrix::XError(4, "Plot(,)");
	cout << "Plotting " << xx.Dim1() * xx.Dim2() << " points." << endl;
	Diagonal x(xx);
	Diagonal y(yy);
	if (x.Dim1() != y.Dim1())
		Matrix::XError(2, "Plot(,)");
	int n = x.Dim1();
	if (n < 1)
		return;

	int width = n;
	if (n < 30)
		width = 2 * n - 1;
	if (width > 60)
		width = 60;
	int height = 21;
	if (n < height)
		height = n;
	char p[61];
	int i;
	int j;
	int k;

	double top = y[0];
	double bot = y[0];
	for (i = 0; i < n; i++)
	{
		if (y[i] > top)
			top = y[i];
	}
	for (i = 0; i < n; i++) if (y[i] < bot) bot = y[i];
	if (bot > 0.0 && bot < 1.0 && top - bot > 0.1) bot = 0.0;	//cool
	if ((top - bot) < 1.0 && Abs(top) > 0.1) top = bot + 1.0;
	if (top == bot) { top = bot + 1.0; };
	double yinc = (top - bot) / (height - 1);
	double ytop = top + 0.5 * yinc;
	double ybot = ytop - yinc;

	double xleft = x[0];
	double xrite = x[0];
	for (i = 0; i < n; i++) if (x[i] < xleft) xleft = x[i];
	for (i = 0; i < n; i++) if (x[i] > xrite) xrite = x[i];
	double xlo = xleft;
	double xhi = xrite;
	if (xrite <= xleft) xrite = xleft + 1.0;
	double xinc = (xrite - xleft) / double(width - 1);
	xleft = xleft - 0.5 * xinc;

	for (k = 0; k < height; k++) //lines
	{
		for (j = 0; j < width; j++) p[j] = ' ';

		//axes
		if (xleft <= 0.0 && xrite >= 0.0)
		{
			j = int((0.0 - xleft) / xinc + 0.001); p[j] = '|';
		}
		if (0.0 < ytop && 0.0 >= ybot)
		{
			for (int a = 0; a < width; a++) p[a] = '-';
			if (xleft <= 0.0 && xrite >= 0.0)
			{
				j = int((0.0 - xleft) / xinc + 0.001); p[j] = '+';
			}
		}

		//data points
		for (i = 0; i < n; i++)
		{
			if (y[i] >= ytop) continue; //belongs above
			if (y[i] < ybot) continue; //belongs below
			j = int((x[i] - xleft) / xinc + 0.001); p[j] = '*';
		}

		double axis = 0.5 * (ytop + ybot);
		if (Abs(axis) < (yinc / 100.0)) axis = 0.0;
		cout << setw(14) << axis << " ";
		for (j = 0; j < width; j++) cout << p[j];  cout << endl;
		ytop = ybot;
		ybot = ytop - yinc;
	}

	for (i = 1; i <= 15; i++) cout << ' '; cout << '^';
	for (i = 2; i < width; i++) cout << ' '; cout << '^' << endl;

	cout << setw(16) << xlo;
	for (k = 0; k < width - 12; k++) cout << " ";
	cout << setw(12) << xhi << endl;
	Prompt();
}

//plot yy versus the indices from 0 to the size of yy
void Plot(const Matrix yy)
{
	int m = Max(yy.Dim1(), yy.Dim2());
	Vector xx(m);
	xx.Iota();
	Plot(xx, yy);
}

//plot a one-dimensional array of integers
void Plot(int* yy, int n)
{
	Vector y(n);
	for (int i = 0; i < n; i++) y[i] = yy[i];
	Plot(y);
}

//dmatrix.h-----------------------------------------------------------
//***********************************************************************
//Decomposition routines adapted from Template Numerical Toolkit.
//***********************************************************************
//Following matrix decomposition routines for SVD, QR, and Eigenvalue
//are adapted from http://math.nist.gov/tnt/index.html.
//These are public domain routines.
//My thanks to the authors for this nice public domain package.
  
double Hypot2(double a, double b)
{
	if (Abs(a) < Abs(b))
	{
		double c = a / b;
		return Abs(b) * sqrt(1.0 + c * c);
	}
	else
	{
		if (a == 0.0) return 0.0;
		double c = b / a;
		return Abs(a) * sqrt(1.0 + c * c);
	}
}

//the SVD decomposition----------------------------------------------
static void SVD(const Matrix& arg, Matrix& u, Diagonal& s, Matrix& v)
{
	//Singular Value Decomposition.
	//For an m-by-n matrix A with m >= n, the singular value decomposition is
	//an m-by-n orthogonal matrix u, an n-by-n diagonal matrix s, and
	//an n-by-n orthogonal matrix v so that A = u*s*v'.

	//The singular values, sigma[k] = s[k][k], are ordered so that
	//sigma[0] >= sigma[1] >= ... >= sigma[n-1].

	//The singular value decompostion always exists, so the constructor will
	//never fail.  The matrix condition number and the effective numerical
	//rank can be computed from this decomposition.
	//(Adapted from JAMA, a Java Matrix Library, developed by jointly 
	//by the Mathworks and NIST; see  http://math.nist.gov/javanumerics/jama).

	//Must have m>=n
	//Sizes must be arg(m,n)  u(m,n)   s(m+1)   v(n,n)

	int m = arg.Dim1();
	int n = arg.Dim2();
	if (m < n) Matrix::XError(4, "SVD()");
	if (m < n) return;

	Matrix a;
	a = arg;
	u = Matrix(m, n, 0.0);
	v = Matrix(n, n, 0.0);
	s = Diagonal(n);

	double* e; e = new double[n];
	double* work; work = new double[m];
	int nu = Min(m, n);

	//---end of interface changes

	int wantu = 1;  					// boolean 
	int wantv = 1;  					// boolean 
	int i = 0;
	int j = 0;
	int k = 0;

	// Reduce a to bidiagonal form, storing the diagonal elements
	// in s and the super-diagonal elements in e.
	int nct = Min(m - 1, n);
	int nrt = Max(0, Min(n - 2, m));
	for (k = 0; k < Max(nct, nrt); k++)
	{
		if (k < nct)
		{
			// Compute the transformation for the k-th column and
			// place the k-th diagonal in s[k].
			// Compute 2-norm of k-th column without under/overflow.
			s[k] = 0.0;
			for (i = k; i < m; i++)
			{
				s[k] = Hypot2(s[k], a[i][k]);
			}
			if (s[k] != 0.0)
			{
				if (a[k][k] < 0.0)
				{
					s[k] = -s[k];
				}
				for (i = k; i < m; i++)
				{
					a[i][k] /= s[k];
				}
				a[k][k] += 1.0;
			}
			s[k] = -s[k];
		}
		for (j = k + 1; j < n; j++)
		{
			if ((k < nct) && (s[k] != 0.0))
			{

				// Apply the transformation.

				double t = 0;
				for (i = k; i < m; i++)
				{
					t += a[i][k] * a[i][j];
				}
				t = -t / a[k][k];
				for (i = k; i < m; i++)
				{
					a[i][j] += t * a[i][k];
				}
			}

			// Place the k-th row of a into e for the
			// subsequent calculation of the row transformation.

			e[j] = a[k][j];
		}
		if (wantu && (k < nct))
		{

			// Place the transformation in u for subsequent back
			// multiplication.

			for (i = k; i < m; i++)
			{
				u[i][k] = a[i][k];
			}
		}
		if (k < nrt)
		{

			// Compute the k-th row transformation and place the
			// k-th super-diagonal in e[k].
			// Compute 2-norm without under/overflow.
			e[k] = 0;
			for (i = k + 1; i < n; i++)
			{
				e[k] = Hypot2(e[k], e[i]);
			}
			if (e[k] != 0.0)
			{
				if (e[k + 1] < 0.0)
				{
					e[k] = -e[k];
				}
				for (i = k + 1; i < n; i++)
				{
					e[i] /= e[k];
				}
				e[k + 1] += 1.0;
			}
			e[k] = -e[k];
			if ((k + 1 < m) && (e[k] != 0.0))
			{

				// Apply the transformation.

				for (i = k + 1; i < m; i++)
				{
					work[i] = 0.0;
				}
				for (j = k + 1; j < n; j++)
				{
					for (i = k + 1; i < m; i++)
					{
						work[i] += e[j] * a[i][j];
					}
				}
				for (j = k + 1; j < n; j++)
				{
					double t = -e[j] / e[k + 1];
					for (i = k + 1; i < m; i++)
					{
						a[i][j] += t * work[i];
					}
				}
			}
			if (wantv)
			{

				// Place the transformation in v for subsequent
				// back multiplication.

				for (i = k + 1; i < n; i++)
				{
					v[i][k] = e[i];
				}
			}
		}
	}

	// Set up the final bidiagonal matrix or order p.

	int p = Min(n, m + 1);
	if (nct < n)
	{
		s[nct] = a[nct][nct];
	}
	if (m < p)
	{
		s[p - 1] = 0.0;
	}
	if (nrt + 1 < p)
	{
		e[nrt] = a[nrt][p - 1];
	}
	e[p - 1] = 0.0;

	// If required, generate u.

	if (wantu)
	{
		for (j = nct; j < nu; j++)
		{
			for (i = 0; i < m; i++)
			{
				u[i][j] = 0.0;
			}
			u[j][j] = 1.0;
		}
		for (k = nct - 1; k >= 0; k--)
		{
			if (s[k] != 0.0)
			{
				for (j = k + 1; j < nu; j++)
				{
					double t = 0;
					for (i = k; i < m; i++)
					{
						t += u[i][k] * u[i][j];
					}
					t = -t / u[k][k];
					for (i = k; i < m; i++)
					{
						u[i][j] += t*u[i][k];
					}
				}
				for (i = k; i < m; i++)
				{
					u[i][k] = -u[i][k];
				}
				u[k][k] = 1.0 + u[k][k];
				for (i = 0; i < k - 1; i++)
				{
					u[i][k] = 0.0;
				}
			}
			else
			{
				for (i = 0; i < m; i++)
				{
					u[i][k] = 0.0;
				}
				u[k][k] = 1.0;
			}
		}
	}

	// If required, generate v.

	if (wantv)
	{
		for (k = n - 1; k >= 0; k--)
		{
			if ((k < nrt) && (e[k] != 0.0))
			{
				for (j = k + 1; j < nu; j++)
				{
					double t = 0;
					for (i = k + 1; i < n; i++)
					{
						t += v[i][k] * v[i][j];
					}
					t = -t / v[k + 1][k];
					for (i = k + 1; i < n; i++)
					{
						v[i][j] += t * v[i][k];
					}
				}
			}
			for (i = 0; i < n; i++)
			{
				v[i][k] = 0.0;
			}
			v[k][k] = 1.0;
		}
	}

	// Main iteration loop for the singular values.

	int pp = p - 1;
	int iter = 0;
	double eps = pow(2.0, -52.0);
	while (p > 0)
	{
		int k = 0;
		int kase = 0;

		// Here is where a test for too many iterations would go.

		// This section of the program inspects for
		// negligible elements in the s and e arrays.  On
		// completion the variables kase and k are set as follows.

		// kase = 1     if s(p) and e[k-1] are negligible and k<p
		// kase = 2     if s(k) is negligible and k<p
		// kase = 3     if e[k-1] is negligible, k<p, and
		//              s(k), ..., s(p) are not negligible (qr step).
		// kase = 4     if e(p-1) is negligible (convergence).

		for (k = p - 2; k >= -1; k--)
		{
			if (k == -1)
			{
				break;
			}
			if (Abs(e[k]) <= eps * (Abs(s[k]) + Abs(s[k + 1])))
			{
				e[k] = 0.0;
				break;
			}
		}
		if (k == p - 2)
		{
			kase = 4;
		}
		else
		{
			int ks;
			for (ks = p - 1; ks >= k; ks--)
			{
				if (ks == k)
				{
					break;
				}
				double t = (ks != p ? Abs(e[ks]) : 0.) +
					(ks != k + 1 ? Abs(e[ks - 1]) : 0.);
				if (Abs(s[ks]) <= eps * t)
				{
					s[ks] = 0.0;
					break;
				}
			}
			if (ks == k)
			{
				kase = 3;
			}
			else if (ks == p - 1)
			{
				kase = 1;
			}
			else
			{
				kase = 2;
				k = ks;
			}
		}
		k++;
		// Perform the task indicated by kase.

		switch (kase)
		{

			// Deflate negligible s(p).

		case 1:
		{
			double f = e[p - 2];
			e[p - 2] = 0.0;
			for (j = p - 2; j >= k; j--)
			{
				double t = Hypot2(s[j], f);
				double cs = s[j] / t;
				double sn = f / t;
				s[j] = t;
				if (j != k)
				{
					f = -sn * e[j - 1];
					e[j - 1] = cs * e[j - 1];
				}
				if (wantv)
				{
					for (i = 0; i < n; i++)
					{
						t = cs * v[i][j] + sn * v[i][p - 1];
						v[i][p - 1] = -sn * v[i][j] + cs * v[i][p - 1];
						v[i][j] = t;
					}
				}
			}
		}
		break;

		// Split at negligible s(k).

		case 2:
		{
			double f = e[k - 1];
			e[k - 1] = 0.0;
			for (j = k; j < p; j++)
			{
				double t = Hypot2(s[j], f);
				double cs = s[j] / t;
				double sn = f / t;
				s[j] = t;
				f = -sn * e[j];
				e[j] = cs * e[j];
				if (wantu)
				{
					for (i = 0; i < m; i++)
					{
						t = cs * u[i][j] + sn * u[i][k - 1];
						u[i][k - 1] = -sn * u[i][j] + cs * u[i][k - 1];
						u[i][j] = t;
					}
				}
			}
		}
		break;

		// Perform one qr step.

		case 3:
		{
			// Calculate the shift.

			double scale = Max(Max(Max(Max(Abs(s[p - 1]), Abs(s[p - 2])), Abs(e[p - 2])), Abs(s[k])), Abs(e[k]));
			double sp = s[p - 1] / scale;
			double spm1 = s[p - 2] / scale;
			double epm1 = e[p - 2] / scale;
			double sk = s[k] / scale;
			double ek = e[k] / scale;
			double b = ((spm1 + sp) * (spm1 - sp) + epm1 * epm1) / 2.0;
			double c = (sp * epm1) * (sp * epm1);
			double shift = 0.0;
			if ((b != 0.0) || (c != 0.0))
			{
				shift = sqrt(b * b + c);
				if (b < 0.0)
				{
					shift = -shift;
				}
				shift = c / (b + shift);
			}
			double f = (sk + sp) * (sk - sp) + shift;
			double g = sk * ek;
			// Chase zeros.

			for (j = k; j < p - 1; j++)
			{
				double t = Hypot2(f, g);
				double cs = f / t;
				double sn = g / t;
				if (j != k)
				{
					e[j - 1] = t;
				}
				f = cs * s[j] + sn * e[j];
				e[j] = cs * e[j] - sn * s[j];
				g = sn * s[j + 1];
				s[j + 1] = cs * s[j + 1];
				if (wantv)
				{
					for (i = 0; i < n; i++)
					{
						t = cs * v[i][j] + sn * v[i][j + 1];
						v[i][j + 1] = -sn * v[i][j] + cs * v[i][j + 1];
						v[i][j] = t;
					}
				}
				t = Hypot2(f, g);
				cs = f / t;
				sn = g / t;
				s[j] = t;
				f = cs * e[j] + sn * s[j + 1];
				s[j + 1] = -sn * e[j] + cs * s[j + 1];
				g = sn * e[j + 1];
				e[j + 1] = cs * e[j + 1];
				if (wantu && (j < m - 1))
				{
					for (i = 0; i < m; i++)
					{
						t = cs * u[i][j] + sn * u[i][j + 1];
						u[i][j + 1] = -sn * u[i][j] + cs * u[i][j + 1];
						u[i][j] = t;
					}
				}
			}
			e[p - 2] = f;
			iter = iter + 1;
		}
		break;

		// Convergence.

		case 4:
		{
			// Make the singular values positive.

			if (s[k] <= 0.0)
			{
				s[k] = (s[k] < 0.0 ? -s[k] : 0.0);
				if (wantv)
				{
					for (i = 0; i <= pp; i++)
					{
						v[i][k] = -v[i][k];
					}
				}
			}

			// Order the singular values.

			while (k < pp)
			{
				if (s[k] >= s[k + 1])
				{
					break;
				}
				double t = s[k];
				s[k] = s[k + 1];
				s[k + 1] = t;
				if (wantv && (k < n - 1))
				{
					for (i = 0; i < n; i++)
					{
						t = v[i][k + 1]; v[i][k + 1] = v[i][k]; v[i][k] = t;
					}
				}
				if (wantu && (k < m - 1))
				{
					for (i = 0; i < m; i++)
					{
						t = u[i][k + 1]; u[i][k + 1] = u[i][k]; u[i][k] = t;
					}
				}
				k++;
			}
			iter = 0;
			p--;
		}
		break;
		}
	};
}

//---Class implementation of QR--------------------------------------
//Classical QR Decomposition:
//For an m-by-n matrix a with m >= n, the QR decomposition is an m-by-n
//orthogonal matrix q and an n-by-n upper triangular matrix r so that a = Q*R.
//
//The QR decomposition always exists, even if the matrix does not have
//full rank, so the constructor will never fail.  The primary use of the
//QR decomposition is in the least squares solution of nonsquare systems
//of simultaneous linear equations.  
//This will fail if IsFullRank() returns false.
//
//The Q and R factors can be retrived via the GetQ() and GetR() methods. 
//Furthermore, a solve() method is provided to find the
//least squares solution of Ax=b using the QR factors.  
//
//(Adapted from JAMA, a Java Matrix Library, developed by jointly 
//by the Mathworks and NIST; see  http://math.nist.gov/javanumerics/jama).
class QR
{
private:
	Matrix m_qr;
	int m;
	int n;
	Diagonal rdiag;

public:
	//construct a QR decomposition of a
	QR(const Matrix& a)
	{
		m = 0;
		n = 0;
		if (a.Dim1() < 1 || a.Dim2() < 1) Matrix::XError(3, "QR::construct()");
		if (a.Dim1() < a.Dim2()) Matrix::XError(4, "QR::construct()");
		m = a.Dim1();
		n = a.Dim2();
		m_qr = a;
		rdiag = Diagonal(n);
		int i = 0;
		int j = 0;
		int k = 0;
		for (k = 0; k < n; k++)
		{
			// Compute 2-norm of k-th column without under/overflow.
			double nrm = 0;
			for (i = k; i < m; i++) nrm = Hypot2(nrm, m_qr[i][k]);
			if (nrm != 0.0)
			{
				// Form k-th Householder vector.
				if (m_qr[k][k] < 0) nrm = -nrm;
				for (i = k; i < m; i++) m_qr[i][k] /= nrm;
				m_qr[k][k] += 1.0;

				// Apply transformation to remaining columns.
				for (j = k + 1; j < n; j++)
				{
					double s = 0.0;
					for (i = k; i < m; i++) s += m_qr[i][k] * m_qr[i][j];
					s = -s / m_qr[k][k];
					for (i = k; i < m; i++)  m_qr[i][j] += s * m_qr[i][k];
				}
			}
			rdiag[k] = -nrm;
		}
	}

	//Return true if matrix is full rank, false otherwise.
	bool IsFullRank() const
	{
		//for (int j = 0; j < n; j++) if (rdiag[j] == 0) return false;
		double eps = m_qr.Epsilon();	//REJ 2011
		for (int j = 0; j < n; j++) if (Abs(rdiag[j]) <= eps) return false;	//REJ 2011
		return true;
	}

	//Return the upper triangular factor, r, of the QR factorization
	Matrix GetR() const
	{
		if (m < 1 || n < 1) Matrix::XError(3, "QR::GetR()");
		Matrix r(n, n);
		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
			{
				if (i < j)
				{
					r[i][j] = m_qr[i][j];
				}
				else if (i == j)
				{
					r[i][j] = rdiag[i];
				}
				else
				{
					r[i][j] = 0.0;
				}
			}
		return r;
	}

	//Generate and return the orthogonal factor q
	Matrix GetQ() const
	{
		if (m < 1 || n < 1) Matrix::XError(3, "QR::GetQ()");
		int i = 0, j = 0, k = 0;
		Matrix q(m, n);
		for (k = n - 1; k >= 0; k--)
		{
			for (i = 0; i < m; i++) q[i][k] = 0.0;
			q[k][k] = 1.0;
			for (j = k; j < n; j++)
			{
				if (m_qr[k][k] != 0)
				{
					double s = 0.0;
					for (i = k; i < m; i++) s += m_qr[i][k] * q[i][j];
					s = -s / m_qr[k][k];
					for (i = k; i < m; i++) q[i][j] += s * m_qr[i][k];
				}
			}
		}
		return q;
	}

	//Least squares solution of a*x = b
	//b m-length array (vector).
	//return the Vector x that minimizes the two norm of Q*R*x-b
	Vector Solve(Vector& b) const
	{
		if (m < 1 || n < 1)    Matrix::XError(3, "QR::Solve");
		if (b.Dim1() != m) Matrix::XError(4, "QR::Solve");
		if (!IsFullRank()) Matrix::XError(6, "QR::Solve");
		Vector x = b;

		// Compute Y = Transpose(q)*b
		for (int k = 0; k < n; k++)
		{
			double s = 0.0;
			for (int i = k; i < m; i++) s += m_qr[i][k] * x[i];
			s = -s / m_qr[k][k];
			for (int i = k; i < m; i++) x[i] += s * m_qr[i][k];
		}

		// Solve R*x = Y;
		for (int k = n - 1; k >= 0; k--)
		{
			x[k] /= rdiag[k];
			for (int i = 0; i < k; i++) x[i] -= x[k] * m_qr[i][k];
		}

		//temp work space has to be deleted
		Vector xx(n);
		for (int i = 0; i < n; i++) xx[i] = x[i];
		return xx;
	}
};	//end class QR

//------------------------------------------------------------------
//The following eigenvalue routines are not needed by the Matrix solvers.
//They are provided as a convenience for the users of the Matrix package.
//------------------------------------------------------------------

//Computes eigenvalues and eigenvectors of a real symmetric matrix
//If A is symmetric, then A = V*D*V' where the eigenvalue matrix D is
//diagonal and the eigenvector matrix V is orthogonal. That is,
//the diagonal values of D are the eigenvalues, and
//V*V' = I, where I is the identity matrix.  The columns of V 
//represent the eigenvectors in the sense that A*V = V*D.
//(Adapted from JAMA, a Java Matrix Library, developed by jointly 
//by the Mathworks and NIST; see  http://math.nist.gov/javanumerics/jama).

void Tred2(Matrix& v, Vector& d, Vector& e)
{
	// Symmetric Householder reduction to tridiagonal form.
	//  This is derived from the Algol procedures Tred2 by
	//  Bowdler, Martin, Reinsch, and Wilkinson, Handbook for
	//  Auto. Comp., Vol.ii-Linear Algebra, and the corresponding
	//  Fortran subroutine in EISPACK.
	int n = v.Dim1();
	for (int j = 0; j < n; j++)
	{
		d[j] = v[n - 1][j];
	}

	// Householder reduction to tridiagonal form.

	for (int i = n - 1; i > 0; i--)
	{

		// Scale to avoid under/overflow.

		double scale = 0.0;
		double h = 0.0;
		for (int k = 0; k < i; k++)
		{
			scale = scale + Abs(d[k]);
		}
		if (scale == 0.0)
		{
			e[i] = d[i - 1];
			for (int j = 0; j < i; j++)
			{
				d[j] = v[i - 1][j];
				v[i][j] = 0.0;
				v[j][i] = 0.0;
			}
		}
		else
		{

			// Generate Householder vector.

			for (int k = 0; k < i; k++)
			{
				d[k] /= scale;
				h += d[k] * d[k];
			}
			double f = d[i - 1];
			double g = sqrt(h);
			if (f > 0)
			{
				g = -g;
			}
			e[i] = scale * g;
			h = h - f * g;
			d[i - 1] = f - g;
			for (int j = 0; j < i; j++)
			{
				e[j] = 0.0;
			}

			// Apply similarity transformation to remaining columns.

			for (int j = 0; j < i; j++)
			{
				f = d[j];
				v[j][i] = f;
				g = e[j] + v[j][j] * f;
				for (int k = j + 1; k <= i - 1; k++)
				{
					g += v[k][j] * d[k];
					e[k] += v[k][j] * f;
				}
				e[j] = g;
			}
			f = 0.0;
			for (int j = 0; j < i; j++)
			{
				e[j] /= h;
				f += e[j] * d[j];
			}
			double hh = f / (h + h);
			for (int j = 0; j < i; j++)
			{
				e[j] -= hh * d[j];
			}
			for (int j = 0; j < i; j++)
			{
				f = d[j];
				g = e[j];
				for (int k = j; k <= i - 1; k++)
				{
					v[k][j] -= (f * e[k] + g * d[k]);
				}
				d[j] = v[i - 1][j];
				v[i][j] = 0.0;
			}
		}
		d[i] = h;
	}

	// Accumulate transformations.

	for (int i = 0; i < n - 1; i++)
	{
		v[n - 1][i] = v[i][i];
		v[i][i] = 1.0;
		double h = d[i + 1];
		if (h != 0.0)
		{
			for (int k = 0; k <= i; k++)
			{
				d[k] = v[k][i + 1] / h;
			}
			for (int j = 0; j <= i; j++)
			{
				double g = 0.0;
				for (int k = 0; k <= i; k++)
				{
					g += v[k][i + 1] * v[k][j];
				}
				for (int k = 0; k <= i; k++)
				{
					v[k][j] -= g * d[k];
				}
			}
		}
		for (int k = 0; k <= i; k++)
		{
			v[k][i + 1] = 0.0;
		}
	}
	for (int j = 0; j < n; j++)
	{
		d[j] = v[n - 1][j];
		v[n - 1][j] = 0.0;
	}
	v[n - 1][n - 1] = 1.0;
	e[0] = 0.0;
}

// Symmetric tridiagonal QL algorithm.
   
void TQL2(Matrix& v, Vector& d, Vector& e)
{
	//  This is derived from the Algol procedures TQL2, by
	//  Bowdler, Martin, Reinsch, and Wilkinson, Handbook for
	//  Auto. Comp., Vol.ii-Linear Algebra, and the corresponding
	//  Fortran subroutine in EISPACK.
	int n = v.Dim1();
	for (int i = 1; i < n; i++)
	{
		e[i - 1] = e[i];
	}
	e[n - 1] = 0.0;

	double f = 0.0;
	double tst1 = 0.0;
	double eps = pow(2.0, -52.0);
	for (int l = 0; l < n; l++)
	{

		// Find small subdiagonal element

		tst1 = Max(tst1, Abs(d[l]) + Abs(e[l]));
		int m = l;

		// Original while-loop from Java code
		while (m < n)
		{
			if (Abs(e[m]) <= eps * tst1)
			{
				break;
			}
			m++;
		}


		// If m == l, d[l] is an eigenvalue,
		// otherwise, iterate.

		if (m > l)
		{
			int iter = 0;
			do
			{
				iter = iter + 1;  // (Could check iteration count here.)

				// Compute implicit shift

				double g = d[l];
				double p = (d[l + 1] - g) / (2.0 * e[l]);
				double r = Hypot2(p, 1.0);
				if (p < 0)
				{
					r = -r;
				}
				d[l] = e[l] / (p + r);
				d[l + 1] = e[l] * (p + r);
				double dl1 = d[l + 1];
				double h = g - d[l];
				for (int i = l + 2; i < n; i++)
				{
					d[i] -= h;
				}
				f = f + h;

				// Implicit QL transformation.

				p = d[m];
				double c = 1.0;
				double c2 = c;
				double c3 = c;
				double el1 = e[l + 1];
				double s = 0.0;
				double s2 = 0.0;
				for (int i = m - 1; i >= l; i--)
				{
					c3 = c2;
					c2 = c;
					s2 = s;
					g = c * e[i];
					h = c * p;
					r = Hypot2(p, e[i]);
					e[i + 1] = s * r;
					s = e[i] / r;
					c = p / r;
					p = c * d[i] - s * g;
					d[i + 1] = h + s * (c * g + s * d[i]);

					// Accumulate transformation.

					for (int k = 0; k < n; k++)
					{
						h = v[k][i + 1];
						v[k][i + 1] = s * v[k][i] + c * h;
						v[k][i] = c * v[k][i] - s * h;
					}
				}
				p = -s * s2 * c3 * el1 * e[l] / dl1;
				e[l] = s * p;
				d[l] = c * p;

				// Check for convergence.

			} while (Abs(e[l]) > eps * tst1);
		}
		d[l] = d[l] + f;
		e[l] = 0.0;
	}

	// Sort eigenvalues and corresponding vectors.

	for (int i = 0; i < n - 1; i++)
	{
		int k = i;
		double p = d[i];
		for (int j = i + 1; j < n; j++)
		{
			if (d[j] < p)
			{
				k = j;
				p = d[j];
			}
		}
		if (k != i)
		{
			d[k] = d[i];
			d[i] = p;
			for (int j = 0; j < n; j++)
			{
				p = v[j][i];
				v[j][i] = v[j][k];
				v[j][k] = p;
			}
		}
	}
}

void PrintSVDQuality(const Matrix& a, const Matrix& u, const Diagonal& s, const Matrix& v, std::ostream& mout)
{
	using namespace std;
	int mn;
	Matrix b = u * s * Transpose(v);
	Matrix utu;
	utu = Transpose(u) * u;
	Matrix vtv;
	vtv = Transpose(v) * v;
	mn = utu.Dim1();
	Matrix iu = Matrix::Identity(mn, mn);
	mn = vtv.Dim1();
	Matrix iv = Matrix::Identity(mn, mn);

	mout << "SVD quality: max absolute value of a - u*s*trans(v) is: " << (a - b).MaxAbs();
	mout << endl;
	mout << "SVD quality: max orthonomality error in u is: " << (utu - iu).MaxAbs();
	mout << endl;
	mout << "SVD quality: max orthonomality error in v is: " << (vtv - iv).MaxAbs();
	mout << endl;
	mout << endl;
}
//gmatrix.h-----------------------------------------------------------
//******************************************************************
//General Matrix solvers.
//******************************************************************

//Computes the eigenvectors and eigenvalues of a real, square, symmetric matrix.
//To assure the matrix is symmetric only the upper right triange of A is used.
//The lower left triangle is copied from the upper right triangle.

//Note the eigenvalues are returned in a Vector.
//The user may want a Diagonal instead.  
//In that case, just construct Diagonal D(d);

void SymEig(const Matrix& a, Matrix& v, Vector &d)
{
	if (a.Dim1() != a.Dim2()) Matrix::XError(2, "Eigenvalue");
	v = a;
	int n = a.Dim2();
	d = Vector(n);
	Vector e(n);

	//copy upper right triangle to lower left
	for (int j = 0; j < n; j++)
	{
		for (int i = 0; i < j; i++)
			v[j][i] = v[i][j];
	}

	Tred2(v, d, e);
	TQL2(v, d, e);
}

//returns a "compact" SVD of a
void MySVD(const Matrix& a, Matrix& u, Diagonal& s, Matrix& v)
{
	int m = a.Dim1();
	int n = a.Dim2();
	if (m * n < 1) { u.Clear(); s.Clear(); v.Clear(); return; }

	if (m < n)
	{
		Matrix at = Transpose(a);
		SVD(at, v, s, u);
	}
	else
	{
		SVD(a, u, s, v);
	}
}

//returns a "compact" SVD of a with reduced rank
void MySVDRank(const Matrix& a, int rank, Matrix& u, Diagonal& s, Matrix& v)
{
	int m = a.Dim1();
	int n = a.Dim2();
	if (m * n < 1) { u.Clear(); s.Clear(); v.Clear(); return; }

	if (m < n)
	{
		Matrix at = Transpose(a);
		SVD(at, v, s, u);
		if (rank < m)
		{
			v.Resize(v.Dim1(), rank); s.Resize(rank, rank); u.Resize(u.Dim1(), rank);
		}
	}
	else
	{
		SVD(a, u, s, v);
		if (rank < n)
		{
			u.Resize(u.Dim1(), rank); s.Resize(rank, rank); v.Resize(v.Dim1(), rank);
		}
	}
}

//Matrix norm == s[0] ... use carefully due to expense
double MatrixNorm(const Matrix& a)
{
	Matrix u;
	Matrix v;
	Diagonal s;
	MySVD(a, u, s, v);
	return s[0];
}

//return condition number = largest singular value / smallest non-zero singular value
double ConditionNumber(const Matrix& a)
{
	Matrix u;
	Matrix v;
	Diagonal s;
	MySVD(a, u, s, v);
	return ConditionNumber(s);
}
   
//returns v * s+ * Transpose(u)
Matrix PseudoInverse(const Matrix& a)
{
	Matrix u;
	Matrix v;
	Diagonal s;
	MySVD(a, u, s, v);
	Matrix x = v * PseudoInverse(s) * Transpose(u);
	return x;
}

//general pseudoinverse solver... solves a*x = b.
//Note that b can have multiple columns.
Matrix Solve(const Matrix &a, const Matrix &b)
{
	if (a.Dim1() != b.Dim1()) Matrix::XError(2, "Solve(a,b)");
	return PseudoInverse(a) * b;
}

//Allows user to say either x=Solve(a,b) or x=b/a
//Note that b can have multiple columns.
Matrix operator/(const Matrix &b, const Matrix &a) { return Solve(a, b); }

//power of a Matrix
Matrix Power(const Matrix& a, int p)
{
	int m = a.Dim1();
	int n = a.Dim2();
	if (m != n) Matrix::XError(4, "Power");
	if (p == 0) { Matrix c(m, m); c.Identity(); return c; }

	//if exponent is negative compute the inverse
	Matrix b(a);
	int q = p;
	if (q < 0) { b = PseudoInverse(a); q = -q; }

	//use binary decomposition: for example x^13 = x^1 * x^4 * x^8
	Matrix c;
	int started = 0;
	while (q > 0)
	{
		int r = q - 2 * int(q / 2);
		if (r > 0)
		{
			if (started == 0) { c = b; started = 1; }
			else c = c * b;
		}
		q = q / 2;
		if (q > 0) b = b * b;  //b**2, b**4, b**8, etc
	}
	return c;
}

//This routine returns an m by k matrix, where k is the number of
//separate linear dependencies among the rows of a.
//For example, if there is one set of dependencies, 
//the returned matrix will be an m x 1 matrix.
//If that one dependency is between rows 3, 9, and 12,
//then the returned matrix will contain the values
// 3.0, 9.0, 12.0, -1.,-1., ... , -1.
//(Meaningless elements are set to -1.)
//If there are two separate depencies, then the matrix would
//be of size m x 2, and the second column might contain
// 4.0, 5.0, 15.0, 16.0, -1., -1., ... , -1.
Matrix GetLinearDependencies(const Matrix& a)
{
	int m = a.Dim1();
	int n = a.Dim2();
	if (m < 2 || n < 2) { Matrix b; return b; } //return 0 x 0 matrix

	Matrix u;
	Matrix v;
	Diagonal s;
	MySVD(a, u, s, v);

	double frac = Min(0.1 / double(m), 0.0001);
	double neglect = s[0] * sqrt(Matrix::RoundOff());

	Matrix dep(m, 0);
	int ns = Min(m, n);

	for (int j = ns - 1; j >= 0; j--)  //walk from smallest singular value to largest
	{
		if (s[j] > neglect) break;
		Vector u = u.GetColumn(j);
		Vector d(m);  //working vector for new dependency
		d = -1.0;
		int r;
		for (r = 0; r < m; r++)
		{
			int i = u.IMaxAbs();
			d[r] = i; //another element of the dependency
			u[i] = 0.0;
			if (u.Norm() < frac) break;
		}
		if (r > 0) dep.AppendColumns(d);
	}
	return dep;
}


#endif
//end of MATRIX_H