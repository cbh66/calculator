/*---------------------------------------------------------------------------*\
 *                                matrix.cpp                                 *
 *                     Implementation of the Matrix class                    *
 *                                                                           *
 *    Written by: Colin Hamilton, Tufts University                           *
 * Last Modified: May 8, 2014                                                *
 *                                                                           *
 *  Note on representation:                                                  *
 *    The matrix is a dynamic 2D array.  The first dereference accesses a    *
 *    particular row; the second dereference gives the cell within that row. *
 *    Accessing an element should thus be matrix[row][col].                  *
 *    Thus, for allocating a new matrix, rows are allocated first, then      *
 *    each given an array of [columns] cells.                                *
\*---------------------------------------------------------------------------*/

#include<iostream>
#include "fraction.h"
#include "matrix.h"
using namespace std;


/*  Constructors allocate space for the matrix based on the size needed,     *
 *    and initialize all values to zero.                                     *
 */
Matrix::Matrix()
{
  rows = 0;
  cols = 0;
  matrix = NULL;
}


Matrix::Matrix(int size)
{
  Matrix(size, 1);
  /*  rows = size;
  cols = 1;
  matrix = new Fraction *[size];
  for (int i = 0; i < size; i++) {
    matrix[i] = new Fraction[1];
    matrix[i][0] = 0;
    }*/
}


Matrix::Matrix(int rows, int cols)
{
  this->rows = rows;
  this->cols = cols;
  matrix = new Fraction *[rows];
  for (int i = 0; i < rows; i++) {
    matrix[i] = new Fraction[cols];
    for (int j = 0; j < cols; j++) {
      matrix[i][j] = 0;
    }
  }
}


Matrix::~Matrix()
{
  for (int i = 0; i < rows; i++) {
    delete [] matrix[i];
  }
  delete [] matrix;
}


Matrix::Matrix(const Matrix &rval)
{
  rows = rval.rows;
  cols = rval.cols;
  matrix = new Fraction *[rows];
  for (int i = 0; i < rows; i++) {
    matrix[i] = new Fraction[cols];
    for (int j = 0; j < cols; j++) {
      matrix[i][j] = rval.matrix[i][j];
    }
  }
}


Matrix Matrix::operator=(Matrix rval)
{
  for (int i = 0; i < rows; i++) {
    delete [] matrix[i];
  }
  delete [] matrix;

  rows = rval.rows;
  cols = rval.cols;
  matrix = new Fraction *[rows];
  for (int i = 0; i < rows; i++) {
    matrix[i] = new Fraction[cols];
    for (int j = 0; j < cols; j++) {
      matrix[i][j] = rval.matrix[i][j];
    }
  }
  return *this;
}


Fraction Matrix::get(int row, int col)
{
  if (validCoord(row, col)) return matrix[row][col];
  return Fraction(1, 0);
}


void Matrix::set(int row, int col, Fraction val)
{
  if (validCoord(row, col)) matrix[row][col] = val;
}


int Matrix::getRows()
{
  return rows;
}


int Matrix::getCols()
{
  return cols;
}


/*  Arithmetic operators operate on every value in the matrix.               *
 */
Matrix Matrix::operator-()
{
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      matrix[i][j] = -matrix[i][j];
    }
  }
  return *this;
}


Matrix Matrix::operator+=(Matrix rval)
{
  if (rows != rval.rows || cols != rval.cols) return Matrix();
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      matrix[i][j] += rval.matrix[i][j];
    }
  }
  return *this;
}


Matrix Matrix::operator-=(Matrix rval)
{
  if (rows != rval.rows || cols != rval.cols) return Matrix();
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      matrix[i][j] -= rval.matrix[i][j];
    }
  }
  return *this;
}


Matrix Matrix::operator*=(Fraction rval)
{
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      matrix[i][j] *= rval;
    }
  }
  return *this;
}

/*  Matrix multiplication results in a matrix with as many rows as the left, *
 *    as many columns as the right.  Each entry is the result of multiplying *
 *    the corresponding row on the left with the corresponding column on the *
 *    right.  To do that, one adds the product of the first entries to the   *
 *    product of the second entries, etc.                                    *
 */
Matrix Matrix::operator*(Matrix rval)
{
  if (cols == rval.rows) {
    Matrix retVal(rows, rval.cols);
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < rval.cols; j++) {
	Fraction val = 0;
	for (int k = 0; k < cols; k++) {
	  val += matrix[i][k] * rval.matrix[k][j];
	}
	retVal.set(i, j, val);
      }
    }
    return retVal;
  } else {
    return Matrix();
  }
}


Matrix Matrix::operator*=(Matrix rval)
{
  *this = *this * rval;
  return *this;
}


/*  Row operations are simple arithmetic / switching, checked for validity  *
 */
void Matrix::switchRows(int r1, int r2)
{
  if (validCoord(r1, 0) && validCoord(r2, 0)) {
    Fraction *temp = matrix[r1];
    matrix[r1] = matrix[r2];
    matrix[r2] = temp;
  }
}


/* A row cannot be multiplied by zero. */
void Matrix::multiplyRow(int row, Fraction factor)
{
  if (validCoord(row, 0) && factor != 0) {
    for (int j = 0; j < cols; j++) {
      matrix[row][j] *= factor;
    }
  }
}


void Matrix::addRow(int first, Fraction factor, int second)
{
  if (validCoord(first, 0) && validCoord(second, 0)) {
    for (int j = 0; j < cols; j++) {
      matrix[second][j] += matrix[first][j] * factor;
    }
  }
}


/*  Returns the index of the next column without all zeros below a given     *
 *    index.                                                                 *
 *  The first argument represents the previous column; that is, where to     *
 *    begin looking.                                                         *
 *  The second represents the first row to consider in each column.          *
 *  Note:  If there is no such column, returns cols, which is the highest    *
 *         index plus one.                                                   *
 */
int Matrix::nextNonzero(int prev, int firstRow)
{
  bool found = false;
  int i, j;
  for (j = prev + 1; j < cols && !found; j++) {
    found = false;
    for (i = firstRow; i < rows; i++) {
      if (matrix[i][j] != 0) {
	found = true;
	break;
      }
    }
  }
  if (found) return j - 1;
  else return cols;
}


/*  Finds the entry in a given column with the highest absolute value.       *
 *  Returns the index of the entry within the column; that is, its row.      *
 *  The first argument representes the column to search within.              *
 *  The second represents the row to begin searching at.  The function will  *
 *    only consider indices greater than or equal to this argument.          *
 *  Note: If all entries are zero, returns -1, as there is no pivot in such  *
 *        a column.
 */
int Matrix::getPivot(int col, int lastRow)
{
  Fraction max = matrix[lastRow][col];
  if (max < 0) max = -max;
  int maxIndex = lastRow;
  for (int i = lastRow + 1; i < rows; i++) {
    if (matrix[i][col] > max) {
      max = matrix[i][col];
      maxIndex = i;
    } else if (-1 * matrix[i][col] > max) {
      max = -1 * matrix[i][col];
      maxIndex = i;
    }
  }
  if (max != 0) {
    return maxIndex;
  } else {
    return -1;
  }
}


/*  Reduces the matrix, using row operations to turn it to reduced echelon   *
 *    form.  The algorithm is:                                               *
 *  1) Find the first nonzero row.                                           *
 *  2) Choose an entry to use as a pivot; switch its row with the top row.   *
 *  3) Turn the pivot to 1 by multiplying the row by the pivot's reciprocal. *
 *  4) Use the pivot to eliminate entries above and below it through         *
 *     row addition.                                                         *
 *  5) Repeat from step one, ignoring the row that was just given a pivot.   *
 *     Stop when all rows are filled and/or there are no more nonzero rows.  *
 */
void Matrix::reduce()
{
  int iMax = 0;
  int current_row = 0;
  for (int j = nextNonzero(-1, 0); j < cols; j = nextNonzero(j, current_row)) {
    iMax = getPivot(j, current_row);
    switchRows(current_row, iMax);
    multiplyRow(current_row, matrix[current_row][j].reciprocal());

    for (int i = current_row + 1; i < rows; i++) {
      addRow(current_row, -1 * matrix[i][j], i);
    }
    for (int i = current_row - 1; i >= 0; i--) {
      addRow(current_row, -1 * matrix[i][j], i);
    }
    current_row++;
  }

}


/*  Calculates the determinant of a matrix, which is only possible for a     *
 *    square matrix.  The algorithm uses row reduction as above,             *
 *    with each step factoring into the calculation of the determinant.      *
 *  1) Switching rows changes the sign of the determinant each time.         *
 *  2) Multiplying a row by a constant multiplies the determinant by the     *
 *     same constant.                                                        *
 *  3) Adding a multiple of one row to another does not change the           *
 *     determinant.                                                          *
 *  The result of this algorithm is a triangular matrix, the determinant of  *
 *    which can be calculated by multiplying the diagonal entries.           *
 *    In this algorithm, the diagonal entries will all be either 0 or 1.     *
 */
Fraction Matrix::determinant()
{
  if (rows != cols) return Fraction(1, 0);
  Matrix temp = *this;
  int iMax = 0;
  int iterations = 0;
  Fraction result = 1;
  for (int j = nextNonzero(-1, 0); j < cols;
       j = nextNonzero(j, iterations)) {
    iMax = getPivot(j, iterations);
    if (iterations != iMax) {
      result = -result;
      switchRows(iterations, iMax);
    }
    result *= matrix[iterations][j];
    multiplyRow(iterations, matrix[iterations][j].reciprocal());

    for (int i = iterations + 1; i < rows; i++) {
      addRow(iterations, -1 * matrix[i][j], i);
    }
    iterations++;
  }

  /* In reduced echelon form, if the lower-right entry is not zero, no diagonal
     entries are zero.  If it is zero, the determinant is zero. */
  result *= matrix[rows - 1][cols - 1];

  *this = temp;
  return result;
}


bool Matrix::validCoord(int row, int col)
{
  return (row < rows && col < cols && row >= 0 && col >= 0);
}


void Matrix::print(ostream &stream)
{
  print(stream, "");
}


void Matrix::print(ostream &stream, string lineStart)
{
  //  stream << rows << " x " << cols << endl;
  if (rows == 0 || cols == 0) return;
  int lengths[cols];
  getColLengths(lengths);
  for (int i = 0; i < rows; i++) {
    stream << lineStart << "|";
    for (int j = 0; j < cols; j++) {
      if (!matrix[i][j].isNegative()) {
	stream << " ";
      }
      matrix[i][j].print(stream);
      int spaces = lengths[j] - matrix[i][j].length();
      for (int k = 0; k <= spaces; k++) {
	stream << " ";
      }
      if (matrix[i][j].isNegative()) {
	stream << " ";
      }
    }
    stream << "|" << endl;
  }
}


/*  Takes an array by reference, storing the size of the largest entry       *
 *  (in terms of characters) in the respective array locations.              *
 *  Used to line up the entries in a column, for printing.                   *
 */
void Matrix::getColLengths(int lens[])
{
  for (int j = 0; j < cols; j++) {
    lens[j] = colLen(j);
  }
}


/*  Finds the largest entry (in chars) in a column, which establishes the    *
 *  width of the column, for printing.                                       *
 */
int Matrix::colLen(int col)
{
  int max = matrix[0][col].length();
  if (matrix[0][col].isNegative()) max--;
  for (int i = 1; i < rows; i++) {
    int t = matrix[i][col].length();
    if (matrix[i][col].isNegative()) t--;
    if (t > max) max = t;
  }
  return max;
}


Matrix identityMatrix(int size)
{
  Matrix r_val(size, size);
  for (int i = 0; i < size; i++) {
    r_val.set(i, i, 1);
  }
  return r_val;
}


Matrix transpose(Matrix m)
{
  int cols = m.getRows();
  int rows = m.getCols();
  Matrix r_val(rows, cols);
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      r_val.set(i, j, m.get(j, i));
    }
  }
  return r_val;
}


