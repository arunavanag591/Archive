#include "BST.h"

/*
NOTE:Comparison chart :
Time taken by a BST for for:
	    BST		|	array(unsorted)	        |	Linked Lists	        |	array(sorted) |
---------------------------------------------------------------------------------------------------------------
search(x): O(logn)	|	O(n)			|	O(n)			|	O(logn)	      |
insert(x): O(logn)	|	O(1)			|	O(1)			|	O(n)          |
remove(x): O(logn)	|	O(n)			|	O(n)			|	O(n)          |
---------------------------------------------------------------------------------------------------------------
which makes it much faster than 
1. array (unsorted)
2. Linked Lists
3. array (sorted)
*/

BST::BST()
{
}


BST::~BST()
{
}
