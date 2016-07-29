/*
 * From: http://www.sgi.com/tech/stl/stl_tree.h
 */
struct tree23_base_iterator
{
  typedef tree23_node_base::_Base_ptr _Base_ptr;
  typedef bidirectional_iterator_tag iterator_category;
  typedef ptrdiff_t difference_type;

  _Base_ptr current;

  void increment()
  {
    if (current->right != 0) {
      current = current->right;
      while (current->left != 0)
        current = current->left;
    }
    else {
      _Base_ptr y = current->parent;
      while (current == y->right) {
        current = y;
        y = y->parent;
      }
      if (current->right != y)
        current = y;
    }
  }

  void decrement()
  {
    if (current->_M_color == _S_rb_tree_red &&
        current->parent->parent == current)
      current = current->right;
    else if (current->left != 0) {
      _Base_ptr y = current->left;
      while (y->right != 0)
        y = y->right;
      current = y;
    }
    else {
      _Base_ptr y = current->parent;
      while (current == y->left) {
        current = y;
        y = y->parent;
      }
      current = y;
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
  tree23_iterator(_Link_type __x) { current = __x; }
  tree23_iterator(const iterator& __it) { current = __it.current; }

  reference operator*() const { return _Link_type(current)->_M_value_field; }
#ifndef __SGI_STL_NO_ARROW_OPERATOR
  pointer operator->() const { return &(operator*()); }
#endif /* __SGI_STL_NO_ARROW_OPERATOR */

  _Self& operator++() { increment(); return *this; }
  _Self operator++(int) {
    _Self __tmp = *this;
    increment();
    return __tmp;
  }
    
  _Self& operator--() { decrement(); return *this; }
  _Self operator--(int) {
    _Self __tmp = *this;
    decrement();
    return __tmp;
  }
};

inline bool operator==(const tree23_base_iterator& __x,
                       const tree23_base_iterator& __y) {
  return __x.current == __y.current;
}

inline bool operator!=(const tree23_base_iterator& __x,
                       const tree23_base_iterator& __y) {
  return __x.current != __y.current;
}
