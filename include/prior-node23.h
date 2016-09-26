#ifndef x_h_
#define x_h_

#include <utility>
#include <memory>
#include <algorithm>
/*
 Make Node23 work for Node23::move_keys_values();
 */
template<class Key, class Value>
class Node23 {

   union Pair {
      std::pair<Key, Value>        p1; // write to this one...
      std::pair<const Key, Value>  p2;  // but return this one.
   };

   std::array<Pair, 2> keys_values;

  public:
     Node23() {}
     Node23(Key key, const Value& value);
     Node23(Key key1, const Value& value1, Key key2, const Value& value2);
     Node23(const Node23& lhs);

     Node23(Node23&& lhs);

     Node23& operator=(const Node23& lhs);

     Node23& operator=(Node23&& lhs);
};

template<class Key, class Value>
inline Node23<Key, Value>::Node23(Key key, const Value& value) 
{
   keys_values[0].p1.first = key;
   keys_values[0].p1.second = value;
}

template<class Key, class Value>
inline Node23<Key, Value>::Node23(Key key1, const Value& value1, Key key2, const Value& value2) 
{
   keys_values[0].p1.first = key1;
   keys_values[0].p1.second = value1;
   keys_values[0].p1.first = key2;
   keys_values[0].p1.second = value2;
}

template<class Key, class Value>
inline Node23<Key, Value>::Node23(const Node23& lhs) : keys_values{lhs.keys_values}
{

}

template<class Key, class Value>
inline Node23<Key, Value>::Node23(Node23&& lhs) : keys_values{std::move(lhs.keys_values)}
{
  // destroy lhs.p1.value
}

template<class Key, class Value>
inline Node23<Key, Value>& Node23<Key, Value>::operator=(const Node23& lhs)
{
  if (&lhs == this) {

      return *this;
  } 

  keys_values = lhs.keys_values;

  return *this;
}


template<class Key, class Value>
inline Node23<Key, Value>& Node23<Key, Value>::operator=(Node23&& lhs)
{
  if (&lhs == this) {

      return *this;
  } 

  keys_values = std::move(lhs.keys_values);

  //TODO: explicitly destroy each Value: invoke dtor of Values explicitly.

  return *this;
}
#endif
