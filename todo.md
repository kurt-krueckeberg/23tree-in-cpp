# TODO

Implement emplace after understanding the Qs below

    template<class Key, class Value> class tree23 {
    
       //...
       public:
       //...
        template <class... Args> void emplace (Args&&... args);
    };

    template<class Key, class Value>  template<class Args>
    void tree23<Key, Value>::emplace(Args&&... args)
    {
     /* 
       Q1:  How do you get the key, which is the first argument, so you can set the value of pair::first?
       Q2:  How do you get the remaining arguments, minus the first argument, so you can std::forward() them to pair::second?
      */
    }

Answer: This cppreference.com article on map's [emplace method](http://en.cppreference.com/w/cpp/container/map/emplace) mentions that pair's template move constructor is
call for `tempate<class Args...> iterator map<key, value>::emplace(Args&&...args);

    std::map<std::string, std::string> m;
 
    // uses pair's template constructor
    m.emplace("d", "ddd"); //<-- The key is "d", the 

The std::pair code called is:

    template<class _U1, class _U2, class = typename
	       enable_if<__and_<is_convertible<_U1, _T1>,
				is_convertible<_U2, _T2>>::value>::type>
	constexpr pair(_U1&& __x, _U2&& __y)
	: first(std::forward<_U1>(__x)), second(std::forward<_U2>(__y)) { }

So `template< class U1, class U2 > constexpr pair( U1&& x, U2&& y );` is used. So if I don't use pair, then maybe this will work:

    template<class Key, class Value> void emplace(Key&& key, Value&& v); // method of class KeyValue

For Help See These Links

1. [vt] - Variadic Templates C++11 
2. [cpp] - std::map emplace()

[vt]: <https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.1.0/com.ibm.zos.v2r1.cbclx01/variadic_templates.htm>
[cpp]: <http://en.cppreference.com/w/cpp/container/map/emplace>

## Overview 

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

From http://www.sgi.com/tech/stl/stl\_tree.h

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
