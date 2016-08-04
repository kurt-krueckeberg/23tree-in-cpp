# Things to work on

## Test new code

Test tree23 copy constructor and assignment operator have been implemented using `CloneTree()` and `DestroyTree()`. They have not been tested.

## Implement an External In-Order, STL-like Iterator

### Implementation Links and Discusssion

1. [odu] - Traversing Trees with Iterators, tutorial in Q & A format of C++ STL-compatible external tree iterator
2. [geeksforgeeks] - Conceptual Discussion and C code implementation, uses inOder() method to advance iterator
3. [stackoverflow] - Stackoverflow Discussion with a link to SGI STL red-black tree bidirectional iterator implmentation 
4. [FSU] - FSU lecture that discusses external STL-compatible iterators, both a stack-based and in order successor-based implementations
5. [cmu] - See Non-Recursive Traversals section, a Java stack-based implementation


[odu]: <https://secweb.cs.odu.edu/~zeil/cs361/web/website/Lectures/treetraversal/page/treetraversal.html> 
[geeksforgeeks]: <http://www.geeksforgeeks.org/inorder-tree-traversal-without-recursion/>
[stackoverflow]: <http://stackoverflow.com/questions/12684191/implementing-an-iterator-over-binary-or-arbitrary-tree-using-c-11>
[FSU]: <http://www.cs.fsu.edu/~lacher/courses/COP4530/lectures/binary_search_trees3/index.html?$$$slide05i.html$$$>
[cmu]: <https://www.cs.cmu.edu/~adamchik/15-121/lectures/Trees/trees.html>

New prospective external iterator code has been started in tree23.h. Prior tree23 code that successfully compiles and runs is in tree23.h.orig.
