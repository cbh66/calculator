/*---------------------------------------------------------------------------*\
 *                                fraction.cpp                               *
 *                     Implementation of the Fraction class                  *
 *                                                                           *
 *     Created by: Colin Hamilton, Tufts University                          *
 *  Last Modified: May 8, 2014                                               *
\*---------------------------------------------------------------------------*/
#include<iostream>
#include<math.h>
#include "fraction.h"
using namespace std;

/* Default constructor sets fraction to zero.                                *
 */
Fraction::Fraction()
{
  negative = false;
  numerator = 0;
  denominator = 1;
}


/*  Sets the fraction to be the given number.                                *
 */
Fraction::Fraction(long long number)
{
  negative = (number < 0);
  numerator = negative ? (-number) : number;
  denominator = 1;
}


/*  Sets the fraction to be (numerator/denominator), properly reduced        * 
 */
Fraction::Fraction(long long numerator, long long denominator)
{
  /* Equivalent to logical XOR; negative iff exactly one is negative  */
  negative = ((numerator < 0) != (denominator < 0));
  this->numerator = (numerator < 0) ? (-numerator) : numerator;
  this->denominator = (denominator < 0) ? (-denominator) : denominator;
  reduce(&(this->numerator), &(this->denominator));
}


/*-------------NEEDS TESTING----------------*/
/*  Sets the fraction based on the given mixed number.                       *
 *  If the whole number is negative, it signifies that the entire fraction is*
 *    negative.  If this is not the desired behavior, make use of arithmetic *
 *    instead.                                                               *
 */
Fraction::Fraction(long long number, unsigned long long numerator,
		   unsigned long long denominator)
{
  reduce(&numerator, &denominator);
  negative = (number < 0);
  if (negative) number = -number;
  long long total_numerator = (number * denominator) + numerator;
  this->numerator = total_numerator;
  this->denominator = denominator;
}


unsigned long long Fraction::getNumerator()
{
  return numerator;
}


unsigned long long Fraction::getDenominator()
{
  return denominator;
}


bool Fraction::isNegative()
{
  /* Accounts for nan; nan is not negative. */
  return (negative && numerator != 0);
}


Fraction Fraction::operator=(long long rhs)
{
  if (rhs < 0) {
    negative = true;
    rhs = -rhs;
  }
  numerator = rhs;
  denominator = 1;
  return *this;
}


Fraction Fraction::operator-()
{
  negative = !negative;
  return *this;
}


/*  All arithmetic operations reduce the given numbers as quickly as possible,*
 *    to avoid overflow where possible.                                       *
 */
Fraction Fraction::operator*=(long long rhs)
{
  if (rhs < 0) {
    negative = !negative;
    rhs = -rhs;
  }
  reduce(&(this->denominator), (long long unsigned *) &rhs);
  numerator *= rhs;
  return *this;
}


Fraction Fraction::operator*=(Fraction rhs)
{
  unsigned long long num = rhs.getNumerator();
  unsigned long long den = rhs.getDenominator();
  reduce(&numerator, &den);
  reduce(&num, &denominator);
  numerator *= num;
  denominator *= den;
  if (rhs.negative) negative = !negative;
  return *this;
}


Fraction Fraction::operator/=(long long rhs)
{
  if (rhs < 0) {
    negative = !negative;
    rhs = -rhs;
  }
  reduce(&(this->numerator), (long long unsigned *) &rhs);
  denominator *= rhs;
  return *this;
}


Fraction Fraction::operator/=(Fraction rhs)
{
  unsigned long long num = rhs.getNumerator();
  unsigned long long den = rhs.getDenominator();
  reduce(&numerator, &num);
  reduce(&den, &denominator);
  numerator *= den;
  denominator *= num;
  if (rhs.isNegative()) negative = !negative;
  return *this;
}


/*  Must account for three cases:                                            *
 *  -A positive fraction becoming negative, by adding a negative number of   *
 *     greater magnitude                                                     *
 *  -A negative fraction becoming positive, by adding a positive number of   *
 *     greater magnitude                                                     *
 *  -A fraction maintaining its sign                                         *
 */
Fraction Fraction::operator+=(long long rhs)
{
  if ((!negative && rhs < 0) &&
      ((unsigned long long)(-rhs * denominator) > numerator)) {
    negative = true;
    numerator = (-rhs * denominator) - numerator;
  } else if ((negative && rhs > 0) &&
	     ((unsigned long long)(rhs * denominator) > numerator)) {
    negative = false;
    numerator = (rhs * denominator) - numerator;
  } else {
    numerator += (rhs * denominator);
  }
  reduce(&numerator, &denominator);
  return *this;
}

/*  Must account for three cases:                                            *
 *  -Adding a fraction of the same sign                                      *
 *  -Adding a fraction of a different sign, greater magnitude                *
 *  -Adding a fraction of a different sign, lesser magnitude                 *
 */
Fraction Fraction::operator+=(Fraction rhs)
{
  unsigned long long lcm = LCM(denominator, rhs.denominator);
  numerator *= (lcm / denominator);
  rhs.numerator *= (lcm / rhs.denominator);
  if (rhs.negative == negative) {
    numerator += rhs.numerator;
  } else if (rhs.numerator > numerator) {
    numerator = rhs.numerator - numerator;
    negative = !negative;
  } else {
    numerator -= rhs.numerator;
  }
  denominator = lcm;
  reduce(&numerator, &denominator);
  return *this;
}

Fraction Fraction::operator-=(long long rhs)
{
  return operator+=(-rhs);
}

Fraction Fraction::operator-=(Fraction rhs)
{
  return operator+=(-rhs);
}

bool Fraction::operator==(Fraction rhs)
{
  return ((negative == rhs.negative) &&
	  (denominator == rhs.denominator) &&
	  (numerator == rhs.numerator));
}

/* Must account for four cases:                                              *
 * -Zero (denominator may be anything but zero, which signals nan)           *
 * -Non-integer (cannot equal an integer)                                    *
 * -Positive and negative numbers                                            *
 */
bool Fraction::operator==(long long rhs)
{
  if (rhs == 0) return numerator == 0 && denominator != 0;
  if (denominator > 1) return false;
  if (rhs < 0) return negative && numerator == (unsigned long long)-rhs;
  return numerator == (unsigned long long)rhs;
}


bool Fraction::operator<(long long rhs)
{
  return (*this - rhs).isNegative();
}


bool Fraction::operator<(Fraction rhs)
{
  return (*this - rhs).isNegative();
}


/*  Returns the result of raising the current fraction to the given integer  *
 *    exponent.                                                              *
 *  Does not modify the existing fraction.                                   *
 */
Fraction Fraction::power(int exp)
{
  Fraction result(1);
  Fraction base = *this;
  for (int i = 0; i < exp; i++) {
    result *= base;
  }
  return result;
}


/*  Returns the reciprocal without modifying the existing fraction.          *
 */
Fraction Fraction::reciprocal()
{
  return Fraction((negative ? -denominator : denominator), numerator);
}


/*  Returns the sqare root (or nan) without modifying the existing fraction. *
 */
Fraction Fraction::sqroot()
{
  if(!negative)
    return Fraction(sqrt((double)numerator), sqrt((double)denominator));
  return Fraction(1, 0);
}

/*  Determines the length, in characters, of the existing fraction.          *
 */
unsigned Fraction::length()
{
  if (numerator == 0) return 1;         /* Fraction is 0 */
  if (denominator == 0) return 3;       /* Fraction is nan */
  unsigned len = 0;
  if (negative) len++;                  /* Account for the '-' */
  len += length(numerator);
  if (denominator == 1) return len;     /* Fraction is an integer */
  return len + 1 + length(denominator); /* Account for the '/' */
}

/*  Returns the length of an integer.                                        *
 */
unsigned Fraction::length(unsigned long long x)
{
  if (x == 0) return 1;
  unsigned len = 0;
  while (x > 0) {
    len++;
    x /= 10;
  }
  return len;
}


/*  Prints the fraction, accounting for nan, negative numbers, and integers.*
 */
void Fraction::print(ostream &stream)
{
  if (numerator == 0) {
    stream << 0;
  } else if (denominator == 1) {
    stream << (negative? "-" : "") << numerator;
  } else if (denominator == 0) {
    stream << "nan";
  } else {
    stream << (negative ? "-" : "")  << numerator << "/" << denominator;
  }
}


/*  Reduces the given numbers by dividing out all multiples.                 *
 *  Takes the numbers by reference, modifying both (if necessary).           *
 */
void Fraction::reduce(unsigned long long *num1, unsigned long long *num2)
{
  int n1 = *num1;    /* Use local variables to avoid frequent dereferencing */
  int n2 = *num2;
  for (int i = 2; i <= n1 && i <= n2; i++) {
    if (n1 % i == 0 && n2 % i == 0) {
      n1 /= i;
      n2 /= i;
    }
  }
  *num1 = n1;
  *num2 = n2;
}


/*  Returns the Least Common Multiple of the given two numbers.              *
 *  Takes advantage of the fact LCM(a,b) == |a*b|/GCD(a,b)                   *
 *  Takes unsigned numbers, so no need to worry about absolute value.        *
 *  Order of caluclations is to make overflow less likely.                   *
 */
unsigned long long Fraction::LCM(unsigned long long num1,
				 unsigned long long num2)
{
  return (num1 / GCD(num1, num2)) * num2;
}


/*  Returns the Greatest Common Divisor of the given two numbers.            *
 *  Utilizes the Euclidean algorithm.                                        *
 *  The first base case is merely for speed considerations, and to avoid     *
 *    stack overflow when one of the numbers has become very small.          *
 *  NOTE:  Change to loop.
 */
unsigned long long Fraction::GCD(unsigned long long num1,
				 unsigned long long num2)
{
  if (num1 == num2) return num1;
  if (num1 > num2) return GCD(num2, num1 - num2);
  return GCD(num1, num2 - num1);
}

