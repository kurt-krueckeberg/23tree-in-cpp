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

If the `position` is 'beg', `decrement()` does a no-op, and none of the member varibles changes. If the `position` is 'end' and `increment()` is called, it, too,
does a no-op, and none of the member varibles changes. 

## TODO

Comments:

Rename KeyValue to something else, I don't know what, `KeyValue_internal`. Then provide it with a conversion operator to the class which we want iterator to return, 
say, `std::pair<const Key, Value&>` or `KeyValue_outer`


    class KeyValue_internal { 

       //...

      public: 
       KeyValue_internal::operator KeyValue_outer(); 
       //...
    };

Where `KeyValue_outer` is defined as 

    struct KeyValue_outer {
          const Key key;
          Value& value;
        public:
           KeyValue(Key k, Value& v) : key{k}, value{v} {}         
    };
 
    KeyValue_internal::operator KeyValue_outer()  
    {
       return KeyValue_outer{key, value};
    }  

    KeyValue_internal::operator std::pair<const Key, Value&>()  
    {
       return std::pair<const Key, Value&>{key, value};
    } 

Have class iterator return `KeyValue_outer&` and class `const_iterator` return `const KeyValue_outer`/

class ConstKeyValue

1. Started to change `iterator::operator*()` to return `std::pair<const Key, Value&>` and change `const_iterator::operator*()` to return
`std::pair<const Key, Value&>` and `std::pair<const Key, const Value&>` respectively. I also started to change all the test code in test.cpp.

Currently, I am getting a compile error about initialization of non-const reference 'std::pair<const int, int&>&}’ from an rvalue of type `std::pair<const int, int&>'

`reverse_iterator::operator*() const` returns a value of type `reference`:

    reference reverse_iterator::operator*() const
    {
      _Iterator tmp = current;
       return *--tmp; // <-- compile error here
    } 

and 
This is triggered by line 359 of test.cpp, which uses a `const_reverse_iterator`. Somehow reverse iterator's `operator*()` is not returning the right type.
I think this is because `const_iterator` composes `iterator`. 
specialize these methods for the `tree23<Key, Value>::reverse_iterator`?


2. Change notes/tree.rst to contain these new code changes.

3. Change `CloneTree()` to either use a functor and call DoInOrderTraversal() rather than having CloneTree() do its own in order traversal, or better yet, use the
external iterators and an insert iterator of some sort to clone a tree.

Do the same thing with DestroyTree(): change it, too, to use external iterators.

It would be best to read up on `iterators`. `iterator_traits`, `reverse_iterator` and `const_reverse_iterator`, as well as on how to implement a custom iterator
properly. See C++Prog. Lang 4th edition and Modern C++ Programming by Scott Meyers. He has an good discussion about compiler type checking involving template and 
how, and how it all works from the comipler's point of view. I also need to understand again what an explicit constructor does. 

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


