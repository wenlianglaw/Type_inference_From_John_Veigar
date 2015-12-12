Programming)Assignment) 3)
CS 6353 Section 1, Fall 2015 
 
In this programming assignment, you will be build a type inference engine, using the unification algorithm 
discussed in class.   You have the choice of C, C++, Go or Rust.  The requirements for your submission are 
listed in sections 1 and 2 below.  Follow%the%requirements%carefully%and%precisely%–%they%will%be%thoroughly%
tested.  Section 3 provides a few example outputs. 
 
As usual, I will grade your program by running it against a large test suite.  You will be graded by the 
percentage of test cases you get correct. 
 
 
1.)Program)Specification)
Your program will construct a type environment, and then perform unifications in that type environment, 
until either the program ends, or a unification fails.  For each unification, you will be outputting the most 
generic type that unifies the two inputs (not the set of substitutions that would unify the type). 
 
That means, as you learn specific information about a generic type, you must keep track of how that 
information is bound. 
 
For instance, you may be first asked to unify \`a and [\`b].  After that unification, our type environment must 
remember that \`a should be substituted with [\`b].   So, if the next query is to unify \`a and \`c, the most 
specific generic type will again be [\`b]. 
 
1. The grammar for the type language is as follows: 
 
TYPE ::= TYPEVAR | PRIMITIVE_TYPE | FUNCTYPE | LISTTYPE;
PRIMITIVE_TYPE ::= ‘int’ | ‘real’ | ‘str’;
TYPEVAR ::= ‘`’ VARNAME; // Note, the character is a backwards apostrophe!
VARNAME ::= [a-zA-Z][a-zA-Z0-9]*; // Initial letter, then can have numbers
FUNCTYPE ::= ‘(‘ ARGLIST ‘)’ -> TYPE | ‘(‘ ‘)’ -> TYPE;
ARGLIST ::= TYPE ‘,’ ARGLIST | TYPE;
LISTTYPE ::= ‘[‘ TYPE ‘]’;
 
2. A single input consists of a pair of types on a single line, separated by an ampersand: 
UNIFICATION_QUERY ::= TYPE ‘&’ TYPE NEWLINE;
NEWLINE ::= ‘\n’;
 
3. White space (tabs and spaces) cannot occur in the middle of a TYPEVAR or PRIMITIVE_TYPE, but can be 
added anywhere else.  This is true for both input and output. 
 
4. When a UNIFICAITON_QUERY does not match the above grammar, your program shall output only 
“ERR” followed by a newline, and then exit. 
 
5. Valid input to your program follows this grammar: 
VALID_INPUT ::= QUERY_LIST ‘QUIT’ ‘\n’;
QUERY_LIST ::= UNIFICATION_QUERY QUERY_LIST | ;
 
6. After you read in each unification query, you must output the correct result, which should either be: 
a) The most general unifier, if it exists.  It should follow the type grammar above. 
b) If no most general unifier exists, output: “BOTTOM” (all caps) on a line by itself, and then exit. 
 
7. When the program input consists of a “QUIT” command instead of a UNIFICATION_QUERY, your 
program should exit without reading further input or producing further output. 
 
8. When your program outputs either “ERR” or “BOTTOM”, you must not read any additional input, or 
produce any additional output. 
 
9. All input is case sensitive.  For instance, \`a and \`A represent different type variables. 
 
10. If unification would create a recursive type, you must output “BOTTOM”. 
 
11. Different primitive types cannot unify with each other. 
 
12. When you unify two type variables, it does not matter which name you use as the proper name.  For 
instance, if you’re asked to unify \`a and \`b, either \`a or \`b are acceptable as an output. 
 
3.)Test)Cases)
Below are four sample test cases for you, which I will use in my testing.  Typically, I use anywhere from 
20g50 test cases, and will definitely use these three.  I strongly recommend you create your own test 
harness and come up with a large number of test cases to help you get the best possible grade. 
 
For test cases, what one would type on the command line is BLACK, input is in GREEN, and 
output is in BLUE. 
Case 1)

./unify

(int, int) -> \`a & (\`a, int) -> int

(int, int)->int

QUIT 

Case 2)

./unify

[\`a] ^ int

BOTTOM

Case 3)

./unify

(int, int) & (\`a, int)

ERR

Case 4)

./unify

(\`a, \`b) -> int & ([\`b], \`c) -> \`d

([\`c], \`c) -> int

`z & int

int

`a & int

BOTTOM

Note that, for the first output, it’s equally acceptable to output: 

([\`b], \`b) -> int

For the third unification in this case, it should fail because \`a is already bound to a list type (specifically, a list that contains items of type \`c).
