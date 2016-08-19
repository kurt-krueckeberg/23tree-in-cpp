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

The constructor for iterator\_base goes to seekToSmallest() if pos==beg or seekToLargest() if iterator\_base==end.

The current design of iterator\_base is a design error. We don't have a proper test for the end iterator. Currently, its constructor, operator==(), increment() and
decrement() are not setting position in such a way so that we can dereference the last key when iterating forward, or the first key when iterating backward. Instead
the loop test for equlaity terminate early.

Currently operator==(const iterator\_base& lhs) compares the position member variable. But if we have not yet dereferenced the last key in the tree within our
loop, operator==() will return true, and the loop will be terminated.

Can increment() somehow properly set the position variable to avoid this premature loop termination? Does it somehow have to position to in\_between until we have
dereference the largest key of the last node? That is not possible with the current implementation.

The same problem happens when we start at position end and iterate backward to position beg: if we are at position end, we still need to still be able to dereference
the iterator when we reach "beg", but the operator==() will return true when compared to the "start iterator", so we here again, we won't be able to dereference it
with a for-loop.


Thoughts:

getSuccessor() determines if we have reached the last node. Therefore it makes most sense to have getSuccessor() and and getPredecessor() set `position` when the last
node (or for getPredecessor() the first node) is encountered.  Should be use finite states, or can't we more simply rely on getSuccessor() and getPrecessor() detecting
when the last of first node has been reached. The constructor called by end() wil do seekToLastNode(), so it can properly set a last node pointer, if needed.

Actually having a two pointers allows us to determine the "state".

The code needs to reflect a clearly thought out design.  Currently, we have five total finite states:

    enum class iterator_position {beg, first_key, in_between, last_key, end}; // possible finite states of iterator

The state transitions are reflected in a hand drawn finte state machine diagram. 

How do these states work with an empty tree and with a tree that has only one node? And as first stated, isn't there a way to rely on getSuccessor() and getPredecessor()
and maybe some simly calculations?

New code for tree23 is in include/new.h. It uses `first_node` and `last_node`. Therefore code to check `key_index` needs to be added


### How the constructor sets position

If the tree is empty, the ctor sets position to `beg`. If the tree is not empty, position is set to `first_node`, and, then, the ctor calls seekToSmallest() and current
is set to the first node and `key_index` is set to zero. 

### How end() calls the two parameter constructor

For end(), we invoke the two parameter version of the constructor with `iterator_position::end` as the 2nd parameter.

### TODO

Draw out the state transitions and how `current` and `key_index` are set or not altered when the state changes. Scan the drawing and added it to the repository. 

### TDOO

Change **typdefs** to **usings**/

The getSuccessor() code needs to be changes, and maybe other subroutines to set `position`. It might be simpliest to change `getSuccessor()` and `getPredecessor()`
or `operator--()` and `operator++()` to set position. 


## External In-Order, STL-like Iterator has been implemented for forward iteration. We need to add the capability to get the predecessor to enable bidirectional iteration

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


