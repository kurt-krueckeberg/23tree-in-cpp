# Things to work on

## Test new code

Test tree23 copy constructor and assignment operator have been implemented using `CloneTree()` and `DestroyTree()`. They have not been tested.

## Implement an External In-Order, STL-like Iterator

### Implementation Links and Discusssion

<<<<<<< HEAD
1. [odu] - Traversing Trees with Iterator, an STL-compatible iterator Q&A teaching discussion
2. [geeksforgeeks] - Conceptual Discussion with C code implementation using a stack.
3. [stackoverflow] - Stackoverflow Discussion with a link to SGI STL implmentation 
4. [FSU] - FSU lecture that discusses external bidirectional tree iterator in C++
5. [cmu] - See Non-Recursive Traversals section that discuss forward iteration in Java using a stack
6. [csohio] - Discusses trees and has an implementation in Java
=======
1. [odu] - Traversing Trees with Iterators, tutorial in Q & A format of C++ STL-compatible external tree iterator
2. [geeksforgeeks] - Conceptual Discussion and C code implementation, uses inOder() method to advance iterator
3. [stackoverflow] - Stackoverflow Discussion with a link to SGI STL red-black tree bidirectional iterator implmentation 
4. [FSU] - FSU lecture that discusses external STL-compatible iterators, both a stack-based and in order successor-based implementations
5. [cmu] - See Non-Recursive Traversals section, a Java stack-based implementation
>>>>>>> 207bc0f3f7171d7f4f6c1b6be327e53c465061c4


[odu]: <https://secweb.cs.odu.edu/~zeil/cs361/web/website/Lectures/treetraversal/page/treetraversal.html> 
[geeksforgeeks]: <http://www.geeksforgeeks.org/inorder-tree-traversal-without-recursion/>
[stackoverflow]: <http://stackoverflow.com/questions/12684191/implementing-an-iterator-over-binary-or-arbitrary-tree-using-c-11>
[FSU]: <http://www.cs.fsu.edu/~lacher/courses/COP4530/lectures/binary_search_trees3/index.html?$$$slide05i.html$$$>
[cmu]: <https://www.cs.cmu.edu/~adamchik/15-121/lectures/Trees/trees.html>
<<<<<<< HEAD
[csohio]: <http://grail.cba.csuohio.edu/~matos/notes/cis-265/lecture-notes/11-26slide.pdf>
=======
>>>>>>> 207bc0f3f7171d7f4f6c1b6be327e53c465061c4

New prospective external iterator code has been started in tree23.h. Prior tree23 code that successfully compiles and runs is in
tree23.h.orig. The getChildIndex() method is not the only technique that could be used. We could simply compare current->\keys\_values[key\_index] with the 
keys in current->parent->keys\_values[]. But this would require also passing the child_index of current in its parent, I believe.

Does getLeftChildXXXX()-whatever really get the left child pointer in the same way as comparing pointers would?

See include/rbtree.h for stdlibc++ source code for rb tree increment().
