## TODO

## Double check ~tree23() functions as expected

1. Change to use the default ~tree23() {}. It should work fine. 
2. And change use of unique\_ptr to shared\_ptr.


findNode() return 'Node *' and not 'const Node *' which leads to compiler errors. Is findNode() itself 'findNode() const'?
I need have to review these basic points of logic, and I need to complete the code with the ~/t4 code, and I will need to review the basic 2-3 tree insert and remove algorithms, ...etc.

## Thoughts on emplace()

The 4-node is used to combine a 3-node and a new key, initially the key to be inserted. The question is, what do we do with the value associated with the key, or in the
case of emplace(), what do we do with the constructor arguments associated with a value that will be created in place from existing memory? See http://algs4.cs.princeton.edu/33balanced. 

## Technical References on Trees and Their Iterators 

### Implementation Links and Discusssion

1. [odu] - Traversing Trees with Iterator, an STL-compatible iterator Q&A teaching discussion
2. [geeksforgeeks] - Conceptual Discussion with C code implementation using a stack.
3. [stackoverflow] - Stackoverflow Discussion with a link to SGI STL implmentation 
4. [FSU] - FSU lecture that discusses external bidirectional tree iterator in C++
5. [cmu] - See Non-Recursive Traversals section that discuss forward iteration in Java using a stack
6. [csohio] - Discusses trees and has an implementation in Java

[odu]: <https://secweb.cs.odu.edu/~zeil/cs361/web/website/Lectures/treetraversal/page/treetraversal.html> 
[geeksforgeeks]: <http://www.geeksforgeeks.org/inorder-tree-traversal-without-recursion/>
[stackoverflow]: <http://stackoverflow.com/questions/12684191/implementing-an-iterator-over-binary-or-arbitrary-tree-using-c-11>
[FSU]: <http://www.cs.fsu.edu/~lacher/courses/COP4530/lectures/binary_search_trees3/index.html?$$$slide05i.html$$$>
[cmu]: <https://www.cs.cmu.edu/~adamchik/15-121/lectures/Trees/trees.html>
[csohio]: <http://grail.cba.csuohio.edu/~matos/notes/cis-265/lecture-notes/11-26slide.pdf>

