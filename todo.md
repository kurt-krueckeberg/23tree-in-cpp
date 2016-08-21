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

### Compile Bugs

Check whether the `iterator_case::decrement()` operator seems is working.

There was a problem with the position member vairable. The idea of a logical position beyond the end and before the front wasn't coded correctly. The client can
call either increment() or decrement() at any time. If there are two logical positions for the first node( and for the last node), the first node's first key will
be retrieved twice (and the last node's --last?--key will be retrieved twice if decrement is called consecutively in a loop.
So there is an error in logic.

TODO: Determine how `position` is set all of the subroutines called by increment() and decrement(). Compare this with the finite state machine. Create a new finite
state machine and an accompanying algorithm that properly sets `current` and `key_index` in all possible use-case scenarios involving increment() and decrement(). 

TODO: Try to set position in just one routine or one subroutine, so the code is not so hard to follow. Should this be in increment() and decrement()?

### Latest Code TODOES

Decide if this code in `decrement()`

    case iterator_position::first_node:

       /* 
         current should already point to first node, and key_index should already be set to first key in tree 
        */
        if (current->isThreeNode() && key_index == 1) {

            key_index = 0;

        } else {

           position = iterator::beg;
        } 
        break;

should be moved to `getLeafNodePredecessor()`, and similarly, if the analogous code in `increment()` should be moved to `getLeafNodeSuccessor()`.

### How the constructor sets position

If the tree is empty, the ctor sets position to `beg`. If the tree is not empty, position is set to `first_node`, and, then, the ctor calls `seekToSmallest()` and
`current` is set to the first node and `key_index` is set to zero. 

### How end() calls the two parameter constructor

For end(), we invoke the two parameter version of the constructor with `iterator_position::end` as the 2nd parameter.

### TODO

Draw out the state transitions and how `current` and `key_index` are set or not altered when the state changes. Scan the drawing and added it to the repository. 

### TDOO

Change **typdefs** to **usings**/

The getSuccessor() code needs to be changes, and maybe other subroutines to set `position`. It might be simpliest to change `getSuccessor()` and `getPredecessor()`
or `operator--()` and `operator++()` to set position. 

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


