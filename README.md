calculator
==========

What is it?
-----------
A calculator, of course.  But more specifically, a calculator that can handle integers, rational numbers, and matrices.  The program is designed to maintain maximum precision throughout calculations, and to make it easy to keep track of several values at once.

How to Use
----------
When running the program, simply enter the values you want to operate on, then the operation you want to perform.  The way this calculator operates, it lends itself to Reverse Polish Notation.  Instead of writing
    2 + 3
We instead write
    2 3 +
This makes it easy to specify many operations and operands without getting bogged down in parentheses.  YOU specify the order in which operations are performed, so there is no ambiguity.  To compute
    (2 + 3 * 4) / (7 + 11 / 2)
We instead write
    2 3 4 * + 7 11 2 / + /
You can also write them one operation at a time: each time you press enter, the program will show you which values it has stored.  This calculator also supports more complex operation, including the use of matrices and stack operations.  To learn the available tools in more detail, read the sections below.

Contents
--------
- [What is it?](#what-is-it)
- [How to Use](#how-to-use)
- [Getting Started](#getting-started)
- [Literals](#literals)
- [Main Screen Commands](#main-screen-commands)
    - [Arithmetic Commands](#arithmetic-commands)
    - [Stack Commands](#stack-commands)
    - [Screen Commands](#screen-commands)
- [Matrices](#matrices)

Getting Started
---------------
Assuming you have the executable in the directory, simply run with ./calc
When the program begins, you are placed in the *main screen*.
From here, the program will begin accepting input.  There are two kinds of input:
* Literals, which come in the form of numbers
* Commands, which are always a single non-numeric character.

Literals
--------
The literals accepted in the calculator program are:
* *Integers*, simply a sequence of numeric characters
* *Floating-point numbers*, a sequence of numbers with a single decimal somewhere
* *Fractions*, two integers, separated by a slash ('/') with *no space between the slash
   and the integers*
If you type a literal while in the main screen, it will place the associated value onto
the *stack*, which contains the list of values that the program is keeping track of.
The contents of the stack is printed out any time you press enter while on the main screen.

Main Screen Commands
--------------------
The commands accepted while on the main screen fall into three main categories:
* *Arithmetic commands*, which perform arithmetic operations on values in the stack
* *Stack commands*, which manipulate the contents of the stack
* *Screen commands*, which change the current screen of the program
Note that commands and literals can be written one after the other, leading to a compact
representation.  For example, the sequence
    2 3 +
represents three commands: "Add 2 to the stack", "Add 3 to the stack", "Add the two values
on the stack".  After this command, the stack will contain the single value 5.

### Arithmetic Commands

The arithmetic commands perform an operation on one or more values in the stack, and
replace them with the result of the computation.  The operations are performed always
on the *top* (most recently entered) values on the stack; the result becomes the
*new* top of the stack.
The available arithmetic commands are:
* +: Adds the top two values on the stack
        2 3 +
    results in 5
* -: Subtracts the top value on the stack from the value beneath it
        2 3 -
    results in -1
* *: Multiplies the top two values on the stack
        2 3 *
    results in 6
* /: Divides the second value on the stack by the top value
        2 3 /
    results in 2/3 or 0.6666
* ^: Raises the second value on the stack to an exponent specified by the top value
        2 3 ^
    results in 8
* !: Takes the factorial of the top value on the stack
        4!
    results in 24
* c: Changes the sign of the top value on the stack
        4c
    results in -4
* r: Takes the square root of the top value on the stack
        4r
    results in 2

### Stack Commands

The stack commands change the values on the stack.  The available stack commands are:
* d: Creates a duplicate of the top value on the stack
* p: "Pops", or removes, the top value on the stack
* s: Swaps the top two values on the stack
* z: "Zeroes", or empties, the stack

### Screen Commands

The screen commands switch to one of the other screens, where different and more
specialized commands are available.  The available screen commands are:
* h: Opens the help screen, from which you can access information about the program
* o: Opens the options screen, from which you can set certain aspects of the program
* m: Opens the matrix screen, from which you can create or manipulate matrices
* q: Quit the program

Matrices
--------
The stack can contain matrices, which respond to the same stack commands, and to the
arithmetic operations that are defined on matrices (Addition and subtraction with
same-sized matrices, multiplication with numbers or correctly-sized matrices, sign
change, and power with numbers).
From the matrix screen, you can create a matrix, or you can manipulate the matrix
on top of the stack (if there is one) with matrix-specific operations.
Available creation commands are:
* i: Creates an identity matrix of a particular size.  After running this command,
     you will be asked for the number of rows and columns.
* n: Creates a new matrix.  After running this command, you will be asked for the
     number of rows and columns in the matrix, and then for the values of the
     matrix, one-by-one.
Available matrix-specific operations are:
* a: Add a multiple of one row to another.  You will be asked which row to add to,
     which row to add, and by what factor.
* e: Reduce the matrix to row-echelon form.
* m: Multiply a row by a certain factor.
* s: Swap two rows.  You will be asked for the numbers of the rows to swap.
