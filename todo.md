# Things to work on

## Test new code

Test tree23 copy constructor and assignment operator have been implemented using CloneTree() and DestroyTree(). 

## Implement and External In-Order, STL-like Iterator

### Implementation Links and Discusssion

1. [odu] - Traversing Trees with Iterators
2. [geeksforgeeks] - Conceptual Discussion and C code implementation
3. [stackoverflow] - Stackoverflow Discussion with a link to SGI implmentation 
4. [FSU] - FSU lecture that discusses external iterator

[odu]: <https://secweb.cs.odu.edu/~zeil/cs361/web/website/Lectures/treetraversal/page/treetraversal.html> 
[geeksforgeeks]: <http://www.geeksforgeeks.org/inorder-tree-traversal-without-recursion/>
[stackoverflow]: <http://stackoverflow.com/questions/12684191/implementing-an-iterator-over-binary-or-arbitrary-tree-using-c-11>
[FSU]: <http://www.cs.fsu.edu/~lacher/courses/COP4530/lectures/binary_search_trees3/index.html?$$$slide05i.html$$$>

Once an external iterator has been implemented, we can add `begin()` and `end()` methods to class tree23\<Key, Value\>.
