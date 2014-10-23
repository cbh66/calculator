/*---------------------------------------------------------------------------*\
 *                               matrixCalc.cpp                              *
 *                   A calculator with support for matrices.                 *
 *                                                                           *
 *    Written by: Colin Hamilton, Tufts University                           *
 * Last Modified: May 8, 2014                                                *
 *                                                                           *
 *  Calculator uses RPN notation.  Objects (numbers or matrices) are stored  *
 *    on a stack, and operators may be used on them.  Numbers may be entered *
 *    as literals.  Matrices can be created with the "m" command.            *
 *    Numbers are stored as fraction, for minimal loss of accuracy.          *
 *  Use the -h option (or read the description() function) for more          *
 *    detailed explanations of how to use the calculator.                    *
\*---------------------------------------------------------------------------*/
#include<iostream>
#include<climits>
#include "fraction.h"
#include "matrix.h"
using namespace std;

void info()
{
  cout << "-----------------------------------------------" << endl;
  cout << "Matrix Calculator v1.0" << endl
       << "  Written by Colin Hamilton, Tufts University" << endl
       << "March 12, 2014" << endl
       << "Last Update: March 14, 2014" << endl;
  cout << "-----------------------------------------------" << endl;
}

void description()
{
  cout << "-----------------------------------------------" << endl;
  cout << "This calculator handles integers, fractions, and matrices." << endl;
  cout << "It utilizes Reverse Polish Notation, which means that" << endl
       << "operations are written after their operands.  For example," << endl
       << "\"2+3\" would be written \"2 3 +\"." << endl;
  cout << "When numbers or matrices are entered, they are put on" << endl
       << "\"the stack,\" where they are stored until they are used," << endl
       << "with the most recent entries on the top of the stack." << endl;
  cout << "Operations really operate on the stack - unary operands" << endl
       << "like 'c' (change sign) operate on the top entry, while" << endl
       << "binary operands like '+' work on the top two entires." << endl;
  cout << "-----------------------------------------------" << endl;
}

/* Global variables determine modes in which to run the calculator. */
bool PROMPT = true;
bool DECIMAL = false;

/* Different types of objects allowed on stack */
enum nodetype {NUMBER, MATRIX};

/* List structure is used for the stack. */
typedef struct Node {
  nodetype type;
  Matrix mdata;
  Fraction fdata;
  Node *rest;
} *List;

/* For arithmetic operations, whose operands come from the stack */
typedef bool (*StackOp)(List);

/*  Functions that deal with input/output.                                   *
 */
void runCalc(List *stack);
void processCommand(List *stack, char command);
void matrixOperate(List *stack);
void help(string state);
void instructions();
void options();
Fraction readFraction();
void readDecimal(List *stack, bool add);

/*  Different types of operations.  Deal with the stack and error-checking;  *
 *  the functions passed should leave their answer on the stack.             *
 */
void binary(StackOp operation, List *stack);
void unary(StackOp operation, List *stack);
void matrixOp(Matrix (*operation)(Matrix), List stack);

/*  Binary arithmetic functions.                                             *
 */
bool add(List stack);
bool subtract(List stack);
bool multiply(List stack);
bool divide(List stack);
bool power(List stack);

/*  Unary arithmetic functions.                                              *
 */
bool changeSign(List stack);
bool factorial(List stack);
bool root(List stack);
bool inverse(List stack);
bool transpose(List stack);
void determinant(List *stack);

/*  Marix operations.                                                        *
 */
Matrix swap(Matrix m);
Matrix multRow(Matrix m);
Matrix addRow(Matrix m);
Matrix reduce(Matrix m);

/*  Create a matrix.                                                         *
 */
void newMatrix(List *stack);
void identity(List *stack);

/*  Stack operations.                                                        *
 */
void printStack(List stack);
bool makeDecimal(List stack);
void empty(List *stack);
void pop(List *stack);
void swap(List *stack);
void duplicate(List *stack);

/*  Errors and prompts.                                                      *
 */
void tooFew();
void error(string message);
void prompt(string message);



int main(int argc, char *argv[])
{
  List stack = NULL;
  cout.precision(9);
  for (int i = 1; i < argc; i++) {
    switch (argv[i][0]) {
    case 'h': help("begin");             break;
    case 'd': DECIMAL = true;            break;
    case 'f': DECIMAL = false;           break;
    case 'p': PROMPT = true;             break;
    case 'e': PROMPT = false;            break;
    default:
      cout << "Unknown option:  " << argv[i] << endl;
    }
  }
  runCalc(&stack);
  empty(&stack);
  return 0;
}


/*  Argument specifies what will happen when the function returns; eg.       *
 *    "quit" or "return to".  (Should be a verb with "the program" as D.O.). *
 */
void help(string state)
{
  char command = '\0';
  do {
    cout << "Enter 'h' for help on using the program." << endl;
    cout << "Enter 'c' for a list of commands that can be used." << endl;
    cout << "Enter 'i' for information on the program." << endl;
    cout << "Enter 'r' to " << state << " the program." << endl;
    cout << "Enter 'q' to quit the program." << endl;
    cin >> command;
    switch (command) {
    case 'c': instructions();     break;
    case 'h': description();      break;
    case 'i': info();             break;
    case 'q': case 'r':           break;
    }
  } while (command != 'r' && command != 'q');
  if (command == 'q') {
    cin.putback(command);
  }
}


void runCalc(List *stack)
{
  char command ='\0'; 
  cout << "For help using this calculator, enter 'h' " << endl;
  do {
    processCommand(stack, command);
    cin.get(command);
  } while (command != 'q');
}


/*  Two possibilities for a command:                                         *
 *  1) It is a digit, in which case the number should be read and added to   *
 *     the stack.                                                            *
 *  2) It is alphabetic, in which case the appropriate function should be    *
 *     called.                                                               *
 *  For explanations of which commands do what, see instructions()           *
 */
void processCommand(List *stack, char command)
{
  if (isdigit(command)) {
    cin.putback(command);
    List temp = new Node;
    long long input;
    cin >> input;
    temp->type = NUMBER;
    temp->fdata = input;
    temp->rest = *stack;
    *stack = temp;
    if (cin.peek() == '.') {  /* cin left off at first non-digit */
      cin.get(command);
      readDecimal(stack, true);
    }
    return;
  }

  switch (command) {
  case '\0':                                               break;
  case '\n': printStack(*stack);                           break;
  case '.': readDecimal(stack, false);                     break;
  case '+': binary(add, stack);                            break;
  case '-': binary(subtract, stack);                       break;
  case '*': binary(multiply, stack);                       break;
  case '/': binary(divide, stack);                         break;
  case '^': binary(power, stack);                          break;
  case '!': unary(factorial, stack);                       break;
  case '|': determinant(stack);                            break;
  case 'c': unary(changeSign, stack);                      break;
  case 'd': duplicate(stack);                              break;
  case 'h': help("return to");                             break;
  case 'i': unary(inverse, stack);                         break;
  case 'm': matrixOperate(stack);                          break;
  case 'o': options();                                     break;
  case 'p': pop(stack);                                    break;
  case 'r': unary(root, stack);                            break;
  case 's': swap(stack);                                   break;
  case 't': unary(transpose, stack);                       break;
  case 'z': empty(stack);                                  break;
  default:
    if (!isspace(command)) {
      cout << "Unknown command: " << command << endl;
    }
  }
}


/* Describes the above commands. */
void instructions()
{
  cout << "-----------------------------------------------" << endl;
  cout << "'+': Add the top two entires on the stack." << endl;
  cout << "'-': Subtract the top entry from the entry below it." << endl;
  cout << "'*': Multiply the top two entries on the stack." << endl
       << "     For matrices, A * B is calculated if A is below B." << endl;
  cout << "'/': Divide the second entry on the stack by the top entry."<< endl;
  cout << "'^': Raises the second entry to the power of the top entry."<< endl;
  cout << "'!': Takes the factorial of the top number on the stack." << endl;
  cout << "'c': Changes the sign of the top entry on the stack." << endl;
  cout << "'d': Duplicates the top entry on the stack." << endl;
  cout << "'h': Opens the help screen." << endl;
  cout << "'o': Opens the options screen." << endl;
  cout << "'p': Pops the top entry off of the stack." << endl;
  cout << "'r': Take the sqare root of the top entry." << endl;
  cout << "'s': Swaps the top two entries on the stack." << endl;
  cout << "'z': \"Zeroes,\" or empties, the stack." << endl;
  cout << "'m': Opens the matrix operation screen, which allows the" << endl
       << "     creation of matrices, or the modification of the top" << endl
       << "     entry on the stack, if it is a matrix." << endl;
  cout << "From the matrix operation screen, the following commands are "
          "allowed" << endl;
  cout << "'a': Add a multiple of one row to another." << endl;
  cout << "'i': Create an identity matrix of a particular size." << endl;
  cout << "'m': Multiply a row by a certain factor." << endl;
  cout << "'n': Create a new matrix, to push onto the stack." << endl;
  cout << "'s': Swap two rows of a matrix." << endl;
  cout << "'r': Return to the calculator." << endl;
  cout << "From any screen, you may type 'q' to quit the calculator." << endl;
  cout << "-----------------------------------------------" << endl;
}


/*  The "matrix operation screen/mode."  The part of the program that allows *
 *  for the creation of matrices and row operations on them.                 *
 */
void matrixOperate(List *stack)
{
  char command;
  do {
    cin.get(command);
      switch (command) {
      case '\n':
	if (*stack != NULL && (*stack)->type == MATRIX) {
	  prompt("Operating on matrix:  ");
	  (*stack)->mdata.print(cout, "   ");
	} else {
	  prompt("No matrix on top of stack.  Create a new one with 'm' or"
		 " 'i'\n");
	}                                                      break;
      case 'a': matrixOp(addRow, *stack);                      break;
      case 'e': matrixOp(reduce, *stack);                      break;
      case 'i': identity(stack);                               break;
      case 'm': matrixOp(multRow, *stack);                     break;
      case 'n': newMatrix(stack);                              break;
      case 's': matrixOp(swap, *stack);                        break;
      case 'r': case 'q':  break;
      default:
	cout << "Unknown command: " << command << endl;
      }
  } while (command != 'r' && command != 'q');
  if (command == 'q') {
    cin.putback(command);
  }
}


/*  Creates a new matrix based on the user's input.  User supplies size of   *
 *  the matrix, and values of each entry.                                    *
 *  cin.fail() cases are in case the user types, eg, "q" to quit.            *
 *  (but also just as a general safety check)                                *
 */
void newMatrix(List *stack)
{
  List temp = new Node;
  temp->type = MATRIX;
  int row, col;
  row = col = 0;
  prompt("Rows?  ");
  cin >> row;
  prompt("Cols?  ");
  cin >> col;
  if (cin.fail()) {
    delete temp;
    cin.clear();
    cout << endl;
    return;
  }
  temp->mdata = Matrix(row, col);
  prompt("Please enter the entries in the matrix.\n");
  for (int i = 0; i < row; i++) {
    cout << "Row " << i+1 << "  ";
    for (int j = 0; j < col; j++) {
      temp->mdata.set(i, j, readFraction());
      if (cin.fail()) {
	delete temp;
	cin.clear();
	return;
      }
    }
  }
  temp->rest = (*stack);
  (*stack) = temp;
}


void identity(List *stack)
{
  int size;
  List temp = new Node;
  temp->type = MATRIX;
  prompt("What size identity matrix?  ");
  cin >> size;
  temp->mdata = identityMatrix(size);
  temp->rest = *stack;
  *stack = temp;
}


/*  All binary operations remove their operand from the stack and push their *
 *  answer.  The workaround used here is indistinguishable from that.        *
 *  Binary operations leave their answer as the *second* thing on the stack. *
 *  This allows binaryOp() to remove the top object.                         *
 */
void binary(StackOp operation, List *stack)
{
  if (*stack == NULL || (*stack)->rest == NULL) {
    tooFew();
    return;
  }
  if(!operation(*stack)) return;
  List temp = (*stack)->rest;
  delete *stack;
  *stack = temp;
}


bool add(List stack)
{
  List temp = stack->rest;
  if (stack->type == MATRIX && temp->type == MATRIX) {
    if (stack->mdata.getRows() == temp->mdata.getRows() &&
	stack->mdata.getCols() == temp->mdata.getCols()) {
      temp->mdata += stack->mdata;
    } else {
      error("Matrices have incompatible sizes.");
      return false;
    }
  } else if (stack->type == NUMBER && temp->type == NUMBER) {
    temp->fdata += stack->fdata;
  } else {
    error("Mismatched types.");
    return false;
  }
  return true;
}


bool subtract(List stack)
{
  List temp = stack->rest;
  if (stack->type == MATRIX && temp->type == MATRIX) {
    if (stack->mdata.getRows() == temp->mdata.getRows() &&
	stack->mdata.getCols() == temp->mdata.getCols()) {
      temp->mdata -= stack->mdata;
    } else {
      error("Matrices have incompatible sizes.");
      return false;
    }
  } else if (stack->type == NUMBER && temp->type == NUMBER) {
    temp->fdata -= stack->fdata;
  } else {
    error("Mismatched types.");
    return false;
  }
  return true;
}


bool multiply(List stack)
{
  List temp = stack->rest;
  if (stack->type == MATRIX) {
    if (temp->type == NUMBER) {
      stack->mdata *= temp->fdata;
      stack->rest = temp->rest;
      *temp = *stack;
      stack->rest = temp;
    } else {
      if (temp->mdata.getCols() == stack->mdata.getRows()) {
	  temp->mdata *= stack->mdata;
	} else {
	  error("Incompatible matrix sizes.");
	  return false;
	}
    }
  } else if (stack->type == NUMBER) {
    if (temp->type == MATRIX) {
      temp->mdata *= stack->fdata;
    } else if (temp->type == NUMBER) {
      temp->fdata *= stack->fdata;
    }
  }
  return true;
}


bool power(List stack)
{
  if (stack->type == MATRIX || stack->fdata.getDenominator() != 1) {
    error("Exponents must be integers.");
    return false;
  }
  Fraction exp = stack->fdata;
  if (stack->rest->type == NUMBER) {
    List temp = stack->rest;
    Fraction base = temp->fdata;
    Fraction result = 1;
    while (exp > 0) {
      result *= base;
      exp -= 1;
    }
    temp->fdata = result;
  } else {
    return false;
  }
  return true;
}


bool divide(List stack)
{
  List temp = stack->rest;
  if (stack->type == MATRIX) {
    error("Division by a matrix is undefined.");
    return false;
  }
  if (temp->type == MATRIX) {
    temp->mdata *= stack->fdata.reciprocal();
  } else if (temp->type == NUMBER) {
    if (stack->fdata == 0) {
      error("Division by zero is undefined.");
      return false;
    }
    temp->fdata /= stack->fdata;
  }
  return true;
}


/*  Unary Operations leave their answer as the top thing on the stack.       *
 *  In so doing, they destroy the former object on top of the stack.         *
 */
void unary(StackOp operation, List *stack)
{
  if (*stack == NULL) {
    tooFew();
    return;
  }
  operation(*stack);
}


bool changeSign(List stack)
{
  if (stack->type == MATRIX) {
    stack->mdata = -(stack->mdata);
  } else if (stack->type == NUMBER) {
    stack->fdata = -(stack->fdata);
  }
  return true;
}


bool factorial(List stack)
{
  if (stack->type != NUMBER || stack->fdata.isNegative() ||
      stack->fdata.getDenominator() != 1) {
    error("Factorial is only defined for nonnegative integers.");
    return false;
  }
  unsigned long long result = 1;
  unsigned  base = stack->fdata.getNumerator();
  for (unsigned i = 2; i <= base; i++) {
    result *= i;
  }
  stack->fdata = result;
  return true;
}


bool root(List stack)
{
  if (stack->type == MATRIX) {
    stack->mdata.reduce();
    return true;
  }
  if (stack->fdata.isNegative()) {
    error("Square roots of negative numbers are imaginary.");
    return false;
  }
  stack->fdata = stack->fdata.sqroot();
  return true;
}


bool inverse(List stack)
{
  if (stack->type == MATRIX) {
    error("No inverse for matrices has yet been implemented.");
    return false;
  } else {
    stack->fdata = stack->fdata.reciprocal();
  }
  return true;
}


bool transpose(List stack)
{
  if (stack->type != MATRIX) {
    error("Transpose is only defined for matrices.");
    return false;
  } else {
    stack->mdata = transpose(stack->mdata);
    return true;
  }
}


/*  Operation for "|" operator.  This is "absolute value" for a number,      *
 *  and "determinant" for a matrix.                                          *
 *  The former changes the top value on the stack (if it is negative).       *
 *  The latter does not change or delete the matrix.  It simply pushes the   *
 *  number found on top of the matrix.                                       *
 *  These divergent operations make this function unique.                    *
 */
void determinant(List *stack)
{
  if (*stack == NULL) {
    tooFew();
    return;
  }
  if ((*stack)->type == NUMBER && (*stack)->fdata.isNegative()) {
    (*stack)->fdata = -((*stack)->fdata);
  } else {
    if ((*stack)->mdata.getRows() != (*stack)->mdata.getCols()) {
      error("Determinants can only be found for square matrices.");
    } else {
      Fraction result = (*stack)->mdata.determinant();
      List temp = new Node;
      temp->type = NUMBER;
      temp->fdata = result;
      temp->rest = *stack;
      *stack = temp;
    }
  }
}


/*  Matrix operations return a modified matrix.                              *
 */
void matrixOp(Matrix (*operation)(Matrix), List stack)
{
  if (stack == NULL || stack->type != MATRIX) {
    error("Need a matrix on the stack for that operation.");
    return;
  }
  stack->mdata = operation(stack->mdata);
}


Matrix swap(Matrix m)
{
  int r1, r2;
  prompt("Which rows do you want to swap?  ");
  cin >> r1 >> r2;
  m.switchRows(r1 - 1, r2 - 1);
  return m;
}


Matrix multRow(Matrix m)
{
  int row;
  prompt("Multiply which row?  ");
  cin >> row;
  prompt("By what factor?  ");
  m.multiplyRow(row - 1, readFraction());
  return m;
}


Matrix addRow(Matrix m)
{
  int r1, r2;
  prompt("Add a multiple of which row?  ");
  cin >> r1;
  prompt("To what other row?  ");
  cin >> r2;
  prompt("By what factor?  ");
  m.addRow(r1 - 1, readFraction(), r2 - 1);
  return m;
}


Matrix reduce(Matrix m)
{
  m.reduce();
  return m;
}

/*  Reads in a number, accounting for the fact that it may have been input   *
 *  as a fraction, with "/" seperating numerator and denominator.            *
 */
Fraction readFraction()
{
  long long num, den = 1;
  char next;
  cin >> num;
  next = cin.peek();
  while (isspace(next) && next != '\n') {
    cin.get(next);
    next = cin.peek();
  }
  if (next == '/') {
    cin.get(next);
    cin >> den;
  }
  return Fraction(num, den);
}

/*  Reads a decimal from standard input and converts it to a fraction.       *
 *  The function expects a decimal point to have been the last thing read in.*
 *  If the "add" bool is set, the number will be added to the top number on  *
 *  the stack.  Otherwise, the number is pushed  onto the stack.             *
 */
void readDecimal(List *stack, bool add)
{
  long long num = 0, den = 1;
  long long max = LLONG_MAX / 10;
  char next;
  cin.get(next);
  while (isdigit(next)) {
    if (den < max) {
      num *= 10;
      den *= 10;
      num += next - '0';
    }
    cin.get(next);
  }
  cin.putback(next);
  Fraction toAdd(num, den);
  if (!add || *stack == NULL || (*stack)->type == MATRIX) {
    List temp = new Node;
    temp->type = NUMBER;
    temp->fdata = toAdd;
    temp->rest = *stack;
    *stack = temp;
  } else {
    (*stack)->fdata += toAdd;
  }
}


bool makeDecimal(List stack)
{
  if (stack == NULL || stack->type != NUMBER) {
    error("Topmost entry must be a number.");
    return false;
  }
  cout << (double) stack->fdata.getNumerator() / stack->fdata.getDenominator()
       << endl;
  return true;
}


void printStack(List stack)
{
  if (stack == NULL) {
    prompt("Stack empty.\n");
  }
  while (stack != NULL) {
    cout << ">>>  ";
    if (stack->type == MATRIX) {
      cout << stack->mdata.getRows() << "x" << stack->mdata.getCols() << endl;
      stack->mdata.print(cout, "     ");
    } else if (stack->type == NUMBER) {
      if (DECIMAL) {
	makeDecimal(stack);
      } else {
	stack->fdata.print(cout);
	cout << endl;
      }
    }
    stack = stack->rest;
  }
}


void pop(List *stack)
{
  List temp = (*stack)->rest;
  delete *stack;
  *stack = temp;
}


void duplicate(List *stack)
{
  if (*stack == NULL) {
    return;
  }
  List temp = new Node;
  *temp = **stack;
  temp->rest = *stack;
  *stack = temp;
}


void swap(List *stack)
{
  if (*stack == NULL || (*stack)->rest == NULL) {
    return;
  }
  List temp = (*stack)->rest;
  (*stack)->rest = temp->rest;
  temp->rest = (*stack);
  *stack = temp;
}


void empty(List *stack)
{
  List cur = *stack;
  while (cur != NULL) {
    List temp = cur->rest;
    delete cur;
    cur = temp;
  }
  *stack = NULL;
}


void tooFew()
{
  error("Too few entries on the stack for that operation.");
}


void error(string message)
{
  cout << ">>>  " << message << endl;
}


void prompt(string message)
{
  if (PROMPT) {
    cout << message;
  }
}


/*  Allows user to toggle display settings.                                  *
 */
void options()
{
  char command = '\0';
  do {
    cin.get(command);
    switch(command) {
    case '\n':
      cout << "Enter 'd' to toggle fraction/decimal display." << endl;
      cout << "Enter 'p' to toggle prompts." << endl;
      cout << "Enter 'r' to return to the calculator." << endl;
      break;
    case 'd': DECIMAL = !DECIMAL;
      cout << "Numbers will now be displayed as "
	   << (DECIMAL ? "decimal" : "fraction") << "s." << endl;
      break;
    case 'p': PROMPT = !PROMPT;
      cout << "Prompts are now " << (PROMPT ? "en" : "dis") << "abled." <<endl;
      break;
    case 'r': case 'q': break;
    default:
      cout << "Unknown option: " << command << endl;
    }
  } while (command != 'r' && command != 'q');
  if (command == 'q') {
    cin.putback(command);
  }
}

