/*---------------------------------------------------------------------------*\
 *                                 matrix.h                                  *
 *                        Interface for the Matrix class                     *
 *                                                                           *
 *    Written by: Colin Hamilton, Tufts University                           *
 * Last Modified: May 8, 2014                                                *
 *                                                                           *
 *  Purpose:                                                                 *
 *    Represents a mathematical matrix of rational numbers.                  *
 *    Allows for the creation of a matrix of any size and the filling in of  *
 *    values at locations in the matrix.                                     *
 *    Defines arithmetic operations on matrices                              *
 *    Defines other matrix operations, including row operations, transposing,*
 *    and finding the determinant.                                           *
 *                                                                           *
 *  Notes:                                                                   *
 *    Invalid operations leave the matrix unchanged, and return the empty    *
 *    matrix.                                                                *
\*---------------------------------------------------------------------------*/
#ifndef MATRIX_CLASS_INCLUDED
#define MATRIX_CLASS_INCLUDED
#include "fraction.h"

class Matrix
{
 public:
  /*  Constructors create a zero matrix of the specified size.               *
   *  Default constructor creates an empty matrix.                           *
   *  Second constructor creates a vector (a 1-column matrix) of the given   *
   *    size.                                                                *
   *  Third constructor creates a matrix with the given rows and columns.    *
   */
  Matrix();
  Matrix(int size);
  Matrix(int rows, int cols);

  /*  Allow for assignment of matrices.                                      *
   */
  Matrix(const Matrix &rval);
  Matrix operator=(Matrix rval);

  /*  Get and set the value at the given coordinates in the matrix.          *
   *  Providing invalid coordinates results in set doing nothing,            *
   *  get returning nan.                                                     *
   */
  Fraction get(int row, int col);
  void set(int row, int col, Fraction val);

  /*  Give the dimensions of the matrix.                                     *
   */
  int getRows();
  int getCols();

  /*  Row operations produce a matrix that is row-equivalent.                *
   *  switchRows takes the coordinates of the two rows to exchange.          *
   *  multiplyRow takes a row's coordinate, and the factor to multiply by.   *
   *  addRow adds the first row, multiplied by a given factor, to the second.*
   */
  void switchRows(int r1, int r2);
  void multiplyRow(int row, Fraction factor);
  void addRow(int first, Fraction factor, int second);

  /*  Row reduces the matrix, turning it into reduced echelon form.          *
   */
  void reduce();

  /*  Returns the determinant of the matrix.
   */
  Fraction determinant();

  /*  Assignment operators implement matrix arithmetic, including:           *
   *  -Matrix addition/subtraction (matrices must have same size)            *
   *  -Scalar multiplication                                                 *
   *  -Matrix multiplication; note that order matters, and if this is an     *
   *   mxn matrix, the right matrix must be nxp; an mxp matrix is produced.  *
   */
  Matrix operator-();
  Matrix operator+=(Matrix rval);
  Matrix operator-=(Matrix rval);
  Matrix operator*=(Fraction rval);
  Matrix operator*=(Matrix rval);
  Matrix operator*(Matrix rval);

  /*  Prints the matrix to a given stream.  Entries will be lined up, padded *
   *    with spaces between them, with '|'s on either side of the matrix.    *
   *  The latter takes a string to start each line, useful for indenting.    *
   */
  void print(ostream &stream);
  void print(ostream &stream, string lineStart);
  ~Matrix();

 private:
  Fraction **matrix;
  int rows;
  int cols;

  int nextNonzero(int prev, int lastRow);
  int getPivot(int row, int lastRow);
  void getColLengths(int lens[]);
  int colLen(int col);
  bool validCoord(int row, int col);
};


/*  Creates a sizexsize identity matrix (a matrix with 1s on the diagonal).  *
 */
Matrix identityMatrix(int size);

/*  Returns the transpase of a given matrix (turning rows into columns and   *
 *    vice-versa).                                                           *
 */
Matrix transpose(Matrix m);

#endif
