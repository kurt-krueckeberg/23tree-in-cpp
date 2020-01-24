## TODO

## FixTree() has an special case that seems "odd" and must be checked in many places

 TODO: Clarify the comments first, so the remove algorithm is clear.
 fixTree() starts at a leaf node and can recurse if the parent becomes empty. If the root is reached--what happen
 How can a Node operated on by fixTree() sometimes have only one non-nullptr child? This happens in one case: ....

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

