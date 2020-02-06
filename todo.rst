TODO
----

Design Considerations
~~~~~~~~~~~~~~~~~~~~~


Using shared_ptr improves efficiency. For example, when we copy a tree, we only have to copy a shared_ptr, but there is a drawback. For the details see:

*  `C++11 Smart Pointer – Part 5: shared_ptr, Binary trees and the problem of Cyclic References <https://thispointer.com/shared_ptr-binary-trees-and-the-problem-of-cyclic-references/>`_
*  `Implementation of Binary Search Trees Via Smart Pointer <https://thispointer.com/shared_ptr-binary-trees-and-the-problem-of-cyclic-references/>`_
*  `Implementation of Binary Search Trees Via Smart Pointer <https://thispointer.com/shared_ptr-binary-trees-and-the-problem-of-cyclic-references/>`_

Problem with this thinking: See comments in ~/bst/todo.rst 

shared_ptr Considerations
^^^^^^^^^^^^^^^^^^^^^^^^^

Using shared_ptr improves efficiency. For example, when we copy a tree, we only have to copy a shared_ptr, but there is a drawback. For the details see:

*  `C++11 Smart Pointer Tutorial Series: Part 1 of **shared_ptr Tutorial and Examples** <https://thispointer.com/learning-shared_ptr-part-1-usage-details/>`_
*  `C++11 Smart Pointer – Part 5: shared_ptr, Binary trees and the problem of Cyclic References <https://thispointer.com/shared_ptr-binary-trees-and-the-problem-of-cyclic-references/>`_
*  `Implementation of Binary Search Trees Via Smart Pointer <https://thispointer.com/shared_ptr-binary-trees-and-the-problem-of-cyclic-references/>`_

Thoughts on emplace()
---------------------

The 4-node is used to combine a 3-node and a new key, initially the key to be inserted. The question is, what do we do with the value associated
with the key, or in the case of emplace(), what do we do with the constructor arguments associated with a value that will be created in
place from existing memory? See http://algs4.cs.princeton.edu/33balanced.

Technical References on Trees and Their Iterators
-------------------------------------------------

Implementation Links and Discusssion
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. `odu <https://secweb.cs.odu.edu/~zeil/cs361/web/website/Lectures/treetraversal/page/treetraversal.html>`__
   - Traversing Trees with Iterator, an STL-compatible iterator Q&A teaching discussion
2. `geeksforgeeks <http://www.geeksforgeeks.org/inorder-tree-traversal-without-recursion/>`__
   - Conceptual Discussion with C code implementation using a stack.
3. `stackoverflow <http://stackoverflow.com/questions/12684191/implementing-an-iterator-over-binary-or-arbitrary-tree-using-c-11>`__
   - Stackoverflow Discussion with a link to SGI STL implmentation
4. `FSU <http://www.cs.fsu.edu/~lacher/courses/COP4530/lectures/binary_search_trees3/index.html?$$$slide05i.html$$$>`__
   - FSU lecture that discusses external bidirectional tree iterator in C++
5. `cmu <https://www.cs.cmu.edu/~adamchik/15-121/lectures/Trees/trees.html>`__
   - See Non-Recursive Traversals section that discuss forward iteration in Java using a stack
6. `csohio <http://grail.cba.csuohio.edu/~matos/notes/cis-265/lecture-notes/11-26slide.pdf>`__
   - Discusses trees and has an implementation in Java
