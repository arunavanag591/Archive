# EquationCalculator


This a C++ console application created using Visual Studio Community 2015, which takes in equations from a .txt file and solves it in a sorted manner. 

Problem Description
-------------------
A command-line program is written to evaluate a set of equations, each specified on separate lines. An equation is defined by:
LHS=RHS
LHS is the left-hand side of the equation and is always a variable name. A variable name is only be composed of letters from the
alphabet (e.g. for which isalpha(c) is 1). RHS is the right hand side of the equation and can be composed of variables, unsigned
integers, and the + operator. Here is one example set of equations:

- offset = 4 + random + 1
- location = 1 + origin + offset
- origin = 3 + 5
- random = 2

The program takes in a filename as input. The file contains a set of equations, like the above. It should evaluate the set of equations
and print the unsigned integer value of each variable.
variable name = unsigned integer value
The output is sorted in ascending order by variable name. The output for the example above would be:

- location = 16
- offset = 7
- origin = 8
- random = 2

Assumptions made: 
- input is well formed
- one or more white spaces betweeen inputs taken
- variables are defined once and has a valid integer solution


Created: Arunava Nag

Date: 27th September 2016
