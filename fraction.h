/*---------------------------------------------------------------------------*\
 *                             fraction.h                                    *
 *                    Interface for the Fraction class                       *
 *                                                                           *
 *     Created by: Colin Hamilton, Tufts University                          *
 *  Last Modified: May 8, 2014                                               *
 *                                                                           *
 *  Purpose:                                                                 *
 *    Represents a real number to a high degree of accuraccy.                *
 *    Numbers are represented as (sign) (numerator / denominator)            *
 *    This class is excellent for preserving accuraccy in intermediate       *
 *      calculations, when floating-point precision are likely to round.     *
 *    For example, (1 / 3) in a calculation will be represented exactly as   *
 *      such in this class, instead of being rounded, as in a double.        *
 *                                                                           *
 *  Notes:                                                                   *
 *   -Overflow is possible on a fraction.  The maximumum value for a         *
 *      numerator or denominator is ULLONG_MAX.                              *
 *   -The result of a successful computation is guaranteed to be in lowest   *
 *      terms.                                                               *
 *   -When calculations fail, the resulting Fraction is represented as       *
 *      "Not-A-Number", signaled by the value of the denominator being 0.    *
 *      Such a value is printed as "nan".                                    *
\*-------------------------------------------------------------------------- */

#ifndef FRACTION_CLASS_INCLUDED
#define FRACTION_CLASS_INCLUDED
#include<iostream>
using namespace std;

class Fraction
{
 public:
  /*  Constructors                                                           *
   *  Default constructor initializes the fraction to zero.                  *
   *  Second constructor initializes to an integer.                          *
   *  Third constructor initializes based on a numerator and denominator.    *
   *  Fourth constructor initializes based on a mixed number.                *
   */
  Fraction();
  Fraction(long long number);
  Fraction(long long numerator, long long denominator);
  Fraction(long long number, unsigned long long numerator,
           unsigned long long denominator);

  /*  Return the numerator, denominator, and sign, respectively.             *
   */
  unsigned long long getNumerator();
  unsigned long long getDenominator();
  bool isNegative();

  /*  Assignment operators do assignment as expected.  Arithmetic is based   *
   *    on rules for fraction arithmetic, as one would expect.               *
   *  All operators work both with fractions and with integers.              *
   *  Non-assigning arithmetic operators are defined below the class.        *
   *  Modulus is not defined for fractions.                                  *
   */
  Fraction operator=(long long rhs);
  Fraction operator-();
  Fraction operator*=(long long rhs);
  Fraction operator*=(Fraction rhs);
  Fraction operator/=(long long rhs);
  Fraction operator/=(Fraction rhs);
  Fraction operator+=(long long rhs);
  Fraction operator+=(Fraction rhs);
  Fraction operator-=(long long rhs);
  Fraction operator-=(Fraction rhs);

  /*  Comparison operators can compare with either another fraction or with  *
   *    an integer.                                                          *
   *  Other comparison operators are defined below the class.                *
   */
  bool operator==(long long rhs);
  bool operator==(Fraction rhs);
  bool operator<(long long rhs);
  bool operator<(Fraction rhs);

  /*  Returns the square root of the fraction.                               *
   *  Note that the numerator and denominator may be rounded, so this        *
   *    calculation may not be exact.                                        *
   *  If called on a negative fraction, returns nan.                         *
   */
  Fraction sqroot();

  /*  Returns the reciprocal of the fraction.                                *
   */
  Fraction reciprocal();

  /*  Returns the fraction raised to the given power.  Beware overflow.      *
   */
  Fraction power(int exp);

  /*  Returns the number of characters that the fraction will take up when   *
   *    printed.  Useful for alignment purposes.                             *
   */
  unsigned length();

  /*  Prints the fraction to the given ostream in the form [-]###/###        *
   */
  void print(ostream &stream);

 private:
  bool negative;
  unsigned long long numerator;
  unsigned long long denominator;

  void reduce(unsigned long long *num1, unsigned long long *num2);
  unsigned long long GCD(unsigned long long num1, unsigned long long num2);
  unsigned long long LCM(unsigned long long num1, unsigned long long num2);
  unsigned length(unsigned long long x);
};



/*  The following define further arithmetic operators in terms of the        *
 *    assignment operators already declared within the class.                *
 *  They are defined so that arithmetic may proceed between integers and     *
 *    fractions in any order.                                                *
 */
inline Fraction operator*(Fraction lhs, long long rhs)
{
  lhs *= rhs;
  return lhs;
}

inline Fraction operator*(long long lhs, Fraction rhs)
{
  return rhs * lhs;
}

inline Fraction operator*(Fraction lhs, Fraction rhs)
{
  lhs *= rhs;
  return lhs;
}

inline Fraction operator/(Fraction lhs, long long rhs)
{
  lhs /= rhs;
  return lhs;
}

inline Fraction operator/(Fraction lhs, Fraction rhs)
{
  lhs /= rhs;
  return lhs;
}

inline Fraction operator/(long long lhs, Fraction rhs)
{
  Fraction temp(lhs);
  return temp / rhs;
}

inline Fraction operator+(Fraction lhs, long long rhs)
{
  lhs += rhs;
  return lhs;
}

inline Fraction operator+(long long lhs, Fraction rhs)
{
  rhs += lhs;
  return rhs;
}

inline Fraction operator+(Fraction lhs, Fraction rhs)
{
  lhs += rhs;
  return lhs;
}

inline Fraction operator-(Fraction lhs, long long rhs)
{
  lhs += -rhs;
  return lhs;
}

inline Fraction operator-(Fraction lhs, Fraction rhs)
{
  lhs += -rhs;
  return lhs;
}

inline Fraction operator-(long long lhs, Fraction rhs)
{
  rhs = lhs + -rhs;
  return rhs;
}


/*  The following define further comparison operators in terms of the        *
 *    operators already implemented within the class.                        *
 *  Comparisons may be made between fractions and integers in any order.     *
 */
inline bool operator==(long long lhs, Fraction rhs)
{
  return rhs == lhs;
}

inline bool operator!=(Fraction lhs, long long rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(long long lhs, Fraction rhs)
{
  return !(rhs == lhs);
}

inline bool operator!=(Fraction lhs, Fraction rhs)
{
  return !(lhs == rhs);
}

inline bool operator>(long long lhs, Fraction rhs)
{
  return rhs < lhs;
}

inline bool operator>(Fraction lhs, Fraction rhs)
{
  return rhs < lhs;
}

inline bool operator<=(Fraction lhs, Fraction rhs)
{
  return !(lhs > rhs);
}

inline bool operator<=(long long lhs, Fraction rhs)
{
  return !(lhs > rhs);
}

inline bool operator>=(Fraction lhs, long long rhs)
{
  return !(lhs < rhs);
}

inline bool operator>=(Fraction lhs, Fraction rhs)
{
  return !(lhs < rhs);
}

inline bool operator<=(Fraction lhs, long long rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>(Fraction lhs, long long rhs)
{
  return !(lhs <= rhs);
}

inline bool operator>=(long long lhs, Fraction rhs)
{
  return rhs <= lhs;
}

inline bool operator<(long long lhs, Fraction rhs)
{
  return !(lhs >= rhs);
}

#endif
