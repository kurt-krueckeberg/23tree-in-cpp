# TODO

1. The new versions of the nested tree23 classes Node23 and Node4 found in include/new-nested-tree23.h have been merged into include/new-tree23.h 
Most of the old `Node23::XXXX() methods are still there. Remove them. Make sure `std::pair<const Key,Value>` behaves like KeyValue.

Make sure `pair<const Key, Value>` behaves like KeyValue did:

    class KeyValue {  // Class KeyValue is used by Node23.

    const Key   key;
    Value     value;
    KeyValue() : key{}  {} 
    KeyValue(Key k, Value&& v) : key{k}, value{std::move(v)} {} // Does pair have this? If not, shouldn't I provide a partial template specialization?
    KeyValue(Key k, const Value& v) : key{k}, value{v} {} 

    KeyValue(KeyValue&& lhs) : key{lhs.key}, value{std::move(lhs.value)} {} // Does pair have this? If not, shouldn't I provide a partial template specialization?
    KeyValue(const KeyValue& lhs) : key{lhs.key}, value{lhs.value} {} // Does pair have this? If not, shouldn't I provide a partial template specialization?

    KeyValue& operator=(KeyValue&& lhs) // Does pair have this? If not, shouldn't I provide a partial template specialization?
    {
       const_cast<Key&>(key) = lhs.key;
       value = std::move(lhs.value);
    }
 
    KeyValue& operator=(KeyValue& lhs)  { const_cast<Key&>(key) = lhs.key; value = lhs.value;  } 

    friend std::ostream& operator<<(std::ostream& ostr, const KeyValue& key_value)
    {
        ostr << "{" << key_value.key << ',' <<  key_value.value <<  "}, ";
        return ostr;
    }


2. Implement emplace after understanding perfect forwarding

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
