23tree-in-cpp
=============

2 3 tree implemented in C++. See [Implementing a 2 3 Tree in C++](http://cplusplus.kurttest.com/notes/tree23.html ). The code was compiled using g++
compiler with the option **-std=c++2a** . 

Everything seems to work, including the iterator classes, with the exeception that rbegin() seems to return an iterator that does not work properly

Design Considerations
---------------------

Using ``std::shared_ptr`` improves efficiency. For example, when we copy a tree, we only have to copy a shared_ptr, but there is a drawback. For the details see:

*  `C++11 Smart Pointer Tutorial Series: Part 1 of **shared_ptr Tutorial and Examples** <https://thispointer.com/learning-shared_ptr-part-1-usage-details/>`_
*  `C++11 Smart Pointer – Part 5: shared_ptr, Binary trees and the problem of Cyclic References <https://thispointer.com/shared_ptr-binary-trees-and-the-problem-of-cyclic-references/>`_
*  `Implementation of Binary Search Trees Via Smart Pointer <https://thispointer.com/shared_ptr-binary-trees-and-the-problem-of-cyclic-references/>`_

Problem with this thinking: See comments in ~/bst/todo.rst 

Thoughts on emplace()
---------------------

The 4-node is used to combine a 3-node and a new key, initially the key to be inserted. The question is, what do we do with the value associated
with the key, or in the case of emplace(), what do we do with the constructor arguments associated with a value that will be created in
place from existing memory? See http://algs4.cs.princeton.edu/33balanced.

Technical References on Trees and Their Iterators
-------------------------------------------------

`Tree Iteration Implementation Discussions <https://data-structs.kurttest.com/notes/tree-iteration.html>`_
