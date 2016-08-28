# Overview 

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

### Top level pseudo code.

`begin()` calls a constructor that sets position to `beg`, and it calls `seekToSmallest()` to set `current` and `key_index` to the first key.  `end()` likewise calls
a constructor that sets position to `end`, and it calls `seekToLargest()` to set `current` and `key_index` to the last key.

If the `position` is 'beg', `decrement()` does a no-op, and none of the member varibles changes. If the `position` is 'end' and `increment()` is called, and it, too,
does a no-op, and none of the member varibles changes. 

## Issues

### TODO

Decide if initialize(pos) needs to have the position passed to it since it is set, I believe, in all cases before initialize(pos) is called--right?

### Bugs

`std::reverse_iterator` holds an instance of its `_Iterator` template parameter:

    template<class _Iterator>
    class iterator {

          _Iterator current;
      //... snip
     };

If  the `_Iterator` instance is const, then current, too, will be const. `reverse_iterator::base() const` needs to return a copy (non-const) of current.
So a copy constructor is needed, and copy constructors are always of the form

    xyz(const xyz&)

where the input to the copy constructor is const. But we have defined such a copy constructor. So that is not the issue. Also the `reverse_iterator::operator*() const` method is const. Its definition is

    reference
    operator*() const
    {
       _Iterator __tmp = current;
       return *--__tmp;
    }

`__tmp` also requires a copy constructor.

Question: what does `reverse_iterator<const tree23<int, int>>` means and what should it do versus what `reverse_iterator<tree23<int, int>>` means and does.  The same
comments apply to `const_reverse_iterator` and `const_iterator`.
 
What does const mean exactly when it comes to `some_iterator::operator*()`. When should `operator*()` be const and when should it not be const? 
What are the reasons for choosing one versus the other?

Note: `reverse_iterator<tree23<int, int>::iterator>::operator==(...) and its `!=` operator both call `reverse_iterator::base() const` method, which doesn't compile.
Currently the code in test.cpp's test_nonconst_iterator that causes the bug is commented out. I have reproduced the bug using a test class called reverse_iterator_sim. Its `base() const` method has the same bug.

   reverse_iterator_sim::base() const 

I think what might be needed is an implicit conversion constructor in iterator and/or const_iterator that converts an 2 3 tree's iterator to a const_iterator and
maybe vice versa? Crazy that this one-line method is the culprit because it is a const method.

It would be best to read up on `iterators`. `iterator_traits`, `reverse_iterator` and `const_reverse_iterator`, as well as on how to implement a custom iterator
properly. See C++Prog. Lang 4th edition and Modern C++ Programming by Scott Meyers. He has an good discussion about compiler type checking involving template and 
how, and how it all works from the comipler's point of view. 


### Red Black code

This is the red black tree increment method fo stdlibc++ below that was used to guide the findLeafNodeSuccessor() code:

    // stdlibc++ source code for red black tree increment
    static _Rb_tree_node_base*
      local_Rb_tree_increment(_Rb_tree_node_base* __x) throw ()
      {
        // If there is a right subtree, get its left most node.
        if (__x->_M_right != 0) 
          {
            __x = __x->_M_right;
            while (__x->_M_left != 0)
              __x = __x->_M_left;
          }
        else
          {
           // An illustration what the loop below is doing (in a balanced red black tree)
           // It ascend x's parent nodes as long as they are right children. It stops when the parent is not 
           /*
                   57 
                   / \
                 55
                   \
                    40  
                   /  \
                 35   45
                / \   / \
               30 38 43  50 

              At this point we have a leaf node most likely. It could be a node that is a left child of its parent already, or it could be
              a right child, and its parent could likewise be a right child. In this case, we ascend until the node is no longer a right child.

            _Rb_tree_node_base* __y = __x->_M_parent; // else retrieve its parent 
           
            while (__x == __y->_M_right) // Ascend as long as the parent node is always a right child.
              {
                __x = __y;
                __y = __y->_M_parent;
              }
            if (__x->_M_right != __y) // I am not sure what this does?
                __x = __y;
          }
        return __x;
      }
    
      _Rb_tree_node_base*
      _Rb_tree_increment(_Rb_tree_node_base* __x) throw ()
      {
        return local_Rb_tree_increment(__x);
      }
    
      const _Rb_tree_node_base*
      _Rb_tree_increment(const _Rb_tree_node_base* __x) throw ()
      {
        return local_Rb_tree_increment(const_cast<_Rb_tree_node_base*>(__x));
      }

From http://www.sgi.com/tech/stl/stl_tree.h

    struct _Rb_tree_base_iterator
    {
      typedef _Rb_tree_node_base::_Base_ptr _Base_ptr;
      typedef bidirectional_iterator_tag iterator_category;
      typedef ptrdiff_t difference_type;
      _Base_ptr _M_node;
    
      void _M_increment()
      {
        if (_M_node->_M_right != 0) {
          _M_node = _M_node->_M_right;
          while (_M_node->_M_left != 0)
            _M_node = _M_node->_M_left;
        }
        else {
          _Base_ptr __y = _M_node->_M_parent;
          while (_M_node == __y->_M_right) {
            _M_node = __y;
            __y = __y->_M_parent;
          }
          // Again, what is this for, comparing it to the illustration at
          // http://stackoverflow.com/questions/12684191/implementing-an-iterator-over-binary-or-arbitrary-tree-using-c-11 
          if (_M_node->_M_right != __y) 

            _M_node = __y;
        }
      }
    
      void _M_decrement()
      {
        if (_M_node->_M_color == _S_rb_tree_red &&
            _M_node->_M_parent->_M_parent == _M_node)
          _M_node = _M_node->_M_right;
        else if (_M_node->_M_left != 0) {
          _Base_ptr __y = _M_node->_M_left;
          while (__y->_M_right != 0)
            __y = __y->_M_right;
          _M_node = __y;
        }
        else {
          _Base_ptr __y = _M_node->_M_parent;
          while (_M_node == __y->_M_left) {
            _M_node = __y;
            __y = __y->_M_parent;
          }
          _M_node = __y;
        }
      }
    };

### Articles to read

Something interesting to read and better learn:
Read [The Standard Librarian: Bitsets and Bit Vectors](http://www.drdobbs.com/the-standard-librarian-bitsets-and-bit-v/184401382) to understand bitsets and
`vector<bool>`. See also [bitsets explained](http://www.cppstdlib.com/cppstdlib_supplementary.pdf).  


