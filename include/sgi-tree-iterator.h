/*
 * From: http://www.sgi.com/tech/stl/stl_tree.h
 */
struct tree23_base_iterator
{
  typedef tree23_node_base::_Base_ptr _Base_ptr;
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

template <class _Value, class _Ref, class _Ptr>
struct tree23_iterator : public tree23_base_iterator
{
  typedef _Value value_type;
  typedef _Ref reference;
  typedef _Ptr pointer;
  typedef tree23_iterator<_Value, _Value&, _Value*>             
    iterator;
  typedef tree23_iterator<_Value, const _Value&, const _Value*> 
    const_iterator;
  typedef tree23_iterator<_Value, _Ref, _Ptr>                   
    _Self;
  typedef tree23_node<_Value>* _Link_type;

  tree23_iterator() {}
  tree23_iterator(_Link_type __x) { _M_node = __x; }
  tree23_iterator(const iterator& __it) { _M_node = __it._M_node; }

  reference operator*() const { return _Link_type(_M_node)->_M_value_field; }
#ifndef __SGI_STL_NO_ARROW_OPERATOR
  pointer operator->() const { return &(operator*()); }
#endif /* __SGI_STL_NO_ARROW_OPERATOR */

  _Self& operator++() { _M_increment(); return *this; }
  _Self operator++(int) {
    _Self __tmp = *this;
    _M_increment();
    return __tmp;
  }
    
  _Self& operator--() { _M_decrement(); return *this; }
  _Self operator--(int) {
    _Self __tmp = *this;
    _M_decrement();
    return __tmp;
  }
};

inline bool operator==(const tree23_base_iterator& __x,
                       const tree23_base_iterator& __y) {
  return __x._M_node == __y._M_node;
}

inline bool operator!=(const tree23_base_iterator& __x,
                       const tree23_base_iterator& __y) {
  return __x._M_node != __y._M_node;
}
