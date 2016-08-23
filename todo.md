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

## Logic Error in increment() and decrement()

Check whether the `iterator_case::decrement()` operator seems is working.

There is a logic error involing the states 'beg' and 'end' for the `position` member.  We have two states in which `current` and `key_index` point to the first key:
`beg` and `first_node`; and we have two possible states, `last_node` and `end`, when `last_key` and `current` point to the last key. `beg' and 'end' are only logically
positions. 'beg' indicates one position before the first key, and 'end' one position past the last key. The code does not handle the fact that `increment()` and
`decrement()` can be called in any order for all states of position. 

Fort example, of `increment()` is called when position is `beg` and the first node has only one key, then position should change to `in_between` after `increment()`.
Similarly, if `decrement()` is called when position is `end`, and the last node has only one key, then position must change to `in_between`, and the values of `current`
and `key_index` be advanced to the second in-order key (if the first node is a 2-node). Likewise, the values for `current` and `key_index` should, when `increment()`
is called when position is `end`, point to the next to last key, if the last node is a 2-node. Obviously, is the first node is a 3-node, then `current` won't change,
but `key_index` will. Likewise, if the last node is a 3-node when 'decrement()` is called, position should become 'last_node`, current should not change, but `key_index`
should change from 1 to 0. 

The logical error results from not properly handling the fact that `increment()` and `decrement()` can be called in any order for any given state of position.

TODO: Draw a new finite state machine that handles these two problemic use-cases (and any others). Change `increment()` and `decrement()` to relect the corrected
finite state machind. Then compare the corrected finite state machine, with the current code, and alter the current code so that `current`, `key_index` and `position`
are properly set to handle all use-case scenarios involving increment() and decrement(). 

Write new pseudo code that fixes how the code should properly set `current` and `key_index` in such a way that is easy to follow. We therefore must simply the methods
`begin()`, `end()` (and the constructor methods they invoke), in addition to the methods `indrement()` and `decrement()` and all their subroutines.

Since the new finite state machine has been drawn and new comments added for begin(), end(), their associated constructors, increment() and decrement(), check that
these new comments reflect the new finite state machine and pseudo code.

### Top level pseudo code.

`begin()` calls a constructor that sets position to `beg`, and it calls `seekToSmallest()` to set `current` and `key_index` to the first key.  `end()` likewise calls a
constructor that sets position to `end`, and it calls `seekToLargest()` to set `current` and `key_index` to the last key.

If the `position` is 'beg', `decrement()` does a no-op, and none of the member varibles changes. If the `position` is 'end' and `increment()` is called, it, too, does
a no-op, and none of the member varibles changes. 

TODO: complete include/new.h. See TODOES at top of file.

## General Comments

According to C++PL, 4th Ed., begin() should return am iterator that "points" to the container's first element, and end() should return a "pointer" to one-past the
last element in the container. My code does not reflect this.

I believe my iterator classes need to provide the typenames that iterator_traits<MyIterator> exposes.

### How the constructor sets position

If the tree is empty, the ctor sets position to `beg`. If the tree is not empty, position is set to `first_node`, and, then, the ctor calls `seekToSmallest()` and
`current` is set to the first node and `key_index` is set to zero. 

### How end() calls the two parameter constructor

For end(), we invoke the two parameter version of the constructor with `iterator_position::end` as the 2nd parameter.

## Changes related to typederfs

Change **typdefs** to **usings**.
 
## External In-Order, STL-like Iterator Notes

In the stl, once the end of a map is reached using the map's bidirectoinal iterator, you can still call the `operator--()` of the iterator to go to the last node in
the tree. I am not sure how this is implemented, but it does one to go back from the one-past the last key/value.

Question: According to "The C++ Programming Language, 4th Edition" begin() returns the first element in the container. So what should or does 
operator--() return when it is called immediately after begin()?  

Answer: It should not do anything. 

We simply define a `postion_state`s enough `enum { beg, middle, end}` of **three mutually exclusive states**--beginning, niddle, end--is clearer
and better.
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


