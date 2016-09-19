#ifndef tree23_h_
#define tree23_h_

#include <array>
#include <memory>
#include <utility>
#include <sstream>
#include <iostream>  
#include <exception>
#include <utility>

template<class Key, class Value> class tree23;
//...required by these friend functions
template<class Key, class Value> std::ostream& operator<<(std::ostream& ostr, const typename tree23<Key, Value>::Node23& node23); 
template<class Key, class Value> std::ostream& operator<<(std::ostream& ostr, const typename tree23<Key, Value>::Node4& node4); 

template<class Key, class Value> class tree23 {

 public:
 class Node4;

 class Node23 {


     public:   
        using value_type = std::pair<const Key, Value>;

        Node23(Key key, const Value& value, Node23 *ptr2parent=nullptr);

        Node23(Node4&);
        // We disallow copy construction and assignment...
        Node23(const Node23&) = delete; 
        Node23& operator=(const Node23&) = delete; 

        Node23(Node23&&); // ...but we allow move assignment and move construction.
        Node23& operator=(Node23&&) noexcept;

        // Constructor for just coping the keys and values. 
	Node23(const std::array<value_type, 2>& lhs_keys_values, Node23 * const lhs_parent, int lhs_totalItems) noexcept; 

        constexpr bool isLeaf() const noexcept { return (children[0] == nullptr && children[1] == nullptr) ? true : false; } 
        constexpr bool isEmpty() const noexcept { return (totalItems == 0) ? true : false; } 

        constexpr bool isThreeNode() const noexcept { return (totalItems == Node23::ThreeNodeItems) ? true : false; }
        constexpr bool isTwoNode() const noexcept { return (totalItems == Node23::TwoNodeItems) ? true : false; }
        
        constexpr int getTotalItems() const noexcept { return totalItems; }
        constexpr int getChildCount() const noexcept { return totalItems + 1; }

        constexpr std::unique_ptr<Node23>& getNonNullChild() noexcept;

	std::ostream& test_parent_ptr(std::ostream& ostr, const Node23 *root) const noexcept;

        bool siblingHasTwoItems(int child_index, int& sibling_index) const noexcept;

        std::ostream& test_keys_ordering(std::ostream& ostr) const noexcept;

        std::ostream& test_remove_invariant(std::ostream& ostr) const noexcept; 

        std::ostream& test_2node_invariant(std::ostream& ostr, const Node23 *root) const noexcept;

        std::ostream& test_3node_invariant(std::ostream& ostr, const Node23 *root) const noexcept;

        std::ostream& debug_print(std::ostream& ostr, bool show_addresses=false) const;

        std::ostream& print(std::ostream& ostr) const noexcept;
   
        friend std::ostream& operator<<(std::ostream& ostr, const Node23& node23)
        { 
	   return node23.print(ostr);
        }

        private:

           Node23 *parent;

           static const int TwoNodeItems = 1;
           static const int TwoNodeChildren = 2;
           static const int ThreeNodeItems = 2;
           static const int ThreeNodeChildren = 3;
           static const int NotFoundIndex = -1;
               
           std::array<Node23::value_type, 2> keys_values;

           std::array<std::unique_ptr<Node23>, 3> children;

           //void move_keys_values(std::array<  std::unique_ptr<std::pair<const Key, Value>>, 2>&& lhs);
           void move_keys_values(std::array<  std::unique_ptr<value_type>, 2>&& lhs);

           void move_children(std::array<std::unique_ptr<Node23>, 3>&& lhs);

           void removeLeafKey(Key key) noexcept;
       
           int totalItems; // set to either Node23::TwoNodeItems or Node23::ThreeNodeItems

           void connectChild(int childIndex, std::unique_ptr<Node23> child)  noexcept;
           void connectChild(std::unique_ptr<Node23>& dest, std::unique_ptr<Node23> src)  noexcept;
          
           void convertTo2Node(Node4&& node4) noexcept; 

           void convertTo3Node(Key key, const Value& value, std::unique_ptr<Node23> pnode23) noexcept; 

           bool NodeDescentSearch(Key value, int& index, Node23 *next) noexcept;          // called during find()  
           bool NodeDescentSearch(Key value, int& index, int& next_child_index) noexcept; // called during insert()

           void insertKeyInLeaf(Key key, const Value& value);
           void insertKeyInLeaf(Key key, Value&& new_value);
  }; 

  private:

  class Node4 { // Class Node4 is only used to aid insert()
     public:
         using value_type = std::pair<const Key, Value>;
      private:
         std::array<value_type, 3> keys_values;

         // Takes ownership of four 23-nodes 
         std::array<std::unique_ptr<Node23>, 4> children; 

         Node23 *parent; // Set to the parent of the 3-node passed to its constructor 

         static const int FourNodeItems = 3;
         static const int FourNodeChildren = 4;

         void connectChild(int childIndex, std::unique_ptr<Node23> child)  noexcept;
                      
    public: 
        Node4() noexcept {}

        // Constructor that takes an internal 3-node 
        Node4(Node23 *threeNode, Key new_key, const Value& value, int child_index, std::unique_ptr<Node23> heap_2node) noexcept;

        // Constructor for a leaf 3-node, all child pointers will be zero. 
        Node4(Node23 *p3node, Key new_key, const Value& new_value) noexcept;

        Node4& operator=(Node4&& lhs) noexcept;
        Node4& operator=(const Node4& lhs) = delete;

        const Key& operator[](int i) const noexcept { return keys_values[i].key; }  

        std::ostream& print(std::ostream& ostr) const noexcept;
        std::ostream& debug_print(std::ostream& ostr) const noexcept;

        constexpr bool isLeaf() const noexcept { return (children[0] == nullptr) ? true : false; } 

        friend std::ostream& operator<<(std::ostream& ostr, const Node4& node4) 
        { 
            return node4.print(ostr); 
        }
    };

    std::unique_ptr<Node23> root; 

}; // end tree23<Key, Value>

/*
  Constructs a new 2-node from a Node4: its key will be the node4.keys_values[2].key, largest key in node4, and its associate value. 
  Its children become the former the two tight most children of node4. Their ownership is transferred to the 2-node.
 */
template<class Key, class Value> tree23<Key, Value>::Node23::Node23(Node4& node4) : totalItems{Node23::TwoNodeItems}, parent{node4.parent}
{
  keys_values[0] = std::move(node4.keys_values[2]); // Prefer move() to default copy assignment.

  connectChild(0, std::move(node4.children[2]));
  connectChild(1, std::move(node4.children[3]));
}

/*
  Constructs a new 2-node that is a leaf node; i.e., its children are nullptr.
 */
template<class Key, class Value> tree23<Key, Value>::Node23::Node23(Key key, const Value& value, Node23 *ptr2parent) : \
          parent{ptr2parent}, totalItems{Node23::TwoNodeItems}
{
  const_cast<Key &>(keys_values[0].first) = key;
  keys_values[0].second = value;
 
  for(auto& child : children) {

       child = nullptr; 
  } 
}
/*
 "this" must be 2-node with only one non-nullptr child
 */
template<class Key, class Value> inline constexpr std::unique_ptr<typename tree23<Key, Value>::Node23>& tree23<Key, Value>::Node23::getNonNullChild() noexcept
{
  return (children[0] == nullptr) ?  children[1] : children[0];
}

template<class Key, class Value> inline void tree23<Key, Value>::Node23::move_keys_values(std::array<std::unique_ptr<Node23::value_type>, 2>&& lhs)
{
  for (auto i = 0; i < totalItems; ++i) {

     keys_values[i] = std::move(lhs.keys_values[i]); 
  }
}

template<class Key, class Value> inline void tree23<Key, Value>::Node23::move_children(std::array<std::unique_ptr<Node23>, 3>&& lhs)
{
  for (auto i = 0; i < getChildCount(); ++i) {

     connectChild(i, std::move(lhs[i]));
  }
}
/*
 * This ctor is used by Clone Tree. Does the default ctor for
 *
     std::array<Node23, 3> children
  */     
template<class Key, class Value> inline tree23<Key, Value>::Node23::Node23(const std::array<Node23::value_type, 2>& lhs_keys_values,\
	       	Node23 *const lhs_parent, int lhs_totalItems) noexcept : keys_values{lhs_keys_values}, parent{lhs_parent}, totalItems{lhs_totalItems}
{
  // we don't copy the children.   
}

// move constructor
template<class Key, class Value> tree23<Key, Value>::Node23::Node23(Node23&& node23) : parent{node23.parent}, totalItems{node23.totalItems}
{
  move_keys_values(node23);
  
  move_children(node23); 
}
// move assignment operator
template<class Key, class Value> typename tree23<Key, Value>::Node23& tree23<Key, Value>::Node23::operator=(Node23&& node23) noexcept
{
  if (this == &node23) {

       return *this;
  }

  parent = node23.parent;
  totalItems = node23.totalItems;

  move_keys_values(node23);

  move_children(node23); 

}

template<class Key, class Value> inline std::ostream& tree23<Key, Value>::Node23::test_keys_ordering(std::ostream& ostr) const noexcept
{
 if (totalItems == Node23::ThreeNodeItems) {

     if (keys_values[0].key >= keys_values[1].key) {

        ostr << "error: " << keys_values[0].key << " is not less than " << keys_values[1].key << "\n";
        return ostr;
     }
  }
  return ostr;
}

template<class Key, class Value> inline std::ostream& tree23<Key, Value>::Node23::test_parent_ptr(std::ostream& ostr, const Node23 *root) const noexcept
{
   if (this == root) { // If this is the root...
       
        if (parent != nullptr) {

 	  ostr << " node is root and parent is not nullptr ";
        }

   } else if (this == parent || parent == nullptr) { // ...otherwise, just check that it is not nullptr or this.

	ostr << " parent pointer wrong ";
   }	   
   return ostr;
}	

template<class Key, class Value> std::ostream& tree23<Key, Value>::Node23::test_remove_invariant(std::ostream& ostr) const noexcept
{
  if (isLeaf()) {

     ostr << " empty leaf node implies key was removed.";
     return ostr;
  }

  // If it is not a leaf, then it is an internal node. This is the recursive remove case when the parent of the inital empty
  // 2-node leaf is itself also a 2-node that will be merged with its sole non-empty child. Thus, we test for one and only one
  //  non-nullptr child.
  int count = 0;

  for(auto i = 0; i < TwoNodeChildren; ++i) {

      if (children[i] == nullptr) ++count;
  } 

  ostr <<  " is an internal node recursive remove case. The node has " << count << " children. ";  

  if (count != 1) {

     ostr << "It should only have one."; 
  }	  

  for(auto i = 0; i < TwoNodeChildren; ++i) {

      if (children[i] != nullptr) {

	  if (children[i]->parent != this) {
 
              ostr << "children[" << i << "]->parent does not point to 'this', which is " << this << ").";
	  }
      }
  } 

  return ostr;
} 

template<class Key, class Value> std::ostream& tree23<Key, Value>::Node23::test_2node_invariant(std::ostream& ostr, const Node23 *root) const noexcept
{
 //  test parent pointer	
  test_parent_ptr(ostr, root);
	 
  if (isLeaf()) return ostr;

  // check ordering of children's keys with respect to parent. 
  for (int child_index = 0; child_index < Node23::TwoNodeChildren; ++child_index) {

       if (children[child_index] == nullptr) {
     
            ostr << "error: children[" << child_index << "] is nullptr\n";
            continue;
       } 

       for (auto i = 0; i < children[child_index]->totalItems; ++i) {
          
           switch (child_index) {

             case 0:

              if (children[0]->keys_values[i].key >= keys_values[0].key) { // If any are greater than or equal to keys_values.keys[0], then it is an error.
              
                 ostr << "error: children[0]->keys_values[" << i << "].key = " << children[0]->keys_values[i].key << " is not less than " << keys_values[0].key << ".\n";
              }  

              break;

              case 1:

                if (children[1]->keys_values[i].key <= keys_values[0].key) { // are any less than or equal to keys_values.keys[0], then it is an error.
          
                   ostr << "error: children[1]->keys_values[" << i << "].key = " << children[1]->keys_values[i].key << " is not greater than " << keys_values[0].key << ".\n";
                }

                break;
  
              default:
                ostr << "error: totalItems = " << totalItems << ".\n";
                break;

          } // end switch 
       }  // end inner for    
  } // end outer for
          
  const Node23 *child; 

  // test children's parent point. 
  for (auto i = 0; i < TwoNodeChildren; ++i) {

       if (children[i] == nullptr) continue; // skip if nullptr 
      
       child = children[i].get();   
       
       if (child->parent != this)	 {

            ostr << "children[" << i << "]->parent does not point to 'this', which is " << this << ").";
       } 
  }
}

template<class Key, class Value> std::ostream& tree23<Key, Value>::Node23::test_3node_invariant(std::ostream& ostr, const Node23 *root) const noexcept
{
  // If node is a 3-node, so we test keys[] ordering.
  test_keys_ordering(ostr);
  
  //  test parent pointer	
  test_parent_ptr(ostr, root);

  // Test keys ordering
  if (keys_values[0].key  >= keys_values[1].key) {

      ostr <<  keys_values[0].key << " is greater than " <<keys_values[1].key;
  }

  if (isLeaf()) return ostr; 

  for (int child_index = 0; child_index < Node23::ThreeNodeChildren; ++child_index) {

     if (children[child_index] == nullptr) {
   
          ostr << "error: children[" << child_index << "] is nullptr\n";
          continue;
     }

    for (auto i = 0; i < children[child_index]->totalItems; ++i) {

      switch (child_index) {

       case 0:  
       // Test that all left child's keys are less than node's keys_values.keys[0]
     
           if (children[0]->keys_values[i].key >= keys_values[0].key) { // If any are greater than or equal to keys_values.keys[0], it is an error
     
              // problem
              ostr << "error: children[0]->keys_values[" << i << "].key = " << children[0]->keys_values[i].key << " is not less than " << keys_values[0].key << ".\n";
           }  
       break; 

       case 1:
 
       // Test middle child's keys, key, are such that: keys_values.keys[0] < key < keys_values.keys[1]
           if (!(children[1]->keys_values[i].key > keys_values[0].key && children[1]->keys_values[i].key < keys_values[1].key)) {
     
              // problem
              ostr << "error: children[1]->keys_values[" << i << "].key = " << children[1]->keys_values[i].key << " is not between " << keys_values[0].key << " and " << keys_values[1].key << ".\n";
           }

       break;

      case 2:     
       // Test right child's keys are all greater than nodes sole key
     
           if (children[2]->keys_values[i].key <= keys_values[1].key) { // If any are less than or equal to keys_values.keys[1], it is an error.
     
              // problem
              ostr << "error: children[2]->keys_values[" << i << "].key = " << children[2]->keys_values[i].key << " is not greater than " << keys_values[1].key << ".\n";
           }

       break;

      default:
         ostr << "error: totalItems = " << totalItems << ".\n";
         break;
     } // end switch
   } // end inner for
 } // end outer for
     
 // test children's parent point. 
 for (auto i = 0; i < ThreeNodeChildren; ++i) {

    if (children[i] == nullptr) continue; // skip if nullptr 

    if (children[i]->parent != this)	 {

        ostr << "children[" << i << "]->parent does not point to 'this', which is " << this << ").";
    } 
 }

  return ostr; 
}

template<class Key, class Value> std::ostream& tree23<Key, Value>::Node23::debug_print(std::ostream& ostr, bool show_addresses) const 
{
   ostr << " { ["; 
   
   if (totalItems == 0) { // remove() situation when merge2Nodes() is called

       ostr << "empty"; 

   } else {

        for (auto i = 0; i < totalItems; ++i) {

            ostr << keys_values[i].key; // or to print both keys and values do: ostr << keys_values[i]

            if (i + 1 == totalItems)  {
                continue;

            } else { 
                ostr << ", ";
            }
        }
   }

   ostr << "] : parent(" << parent << "), " << this;

   if (parent == this) { 
      
      ostr << " BUG: parent == this " << std::flush;
      
      std::ostringstream oss;
      
      oss << "parent == this for node [";
      
      for (auto i = 0; i < totalItems; ++i) {

         ostr << keys_values[i] << "}, ";
       }
      
      oss << "]";
   } 

   if (show_addresses) {

      ostr << ' ';

      for (auto i = 0; i < getChildCount(); ++i) {
          
   
               if (children[i] == nullptr) {
   
                    ostr <<  "nullptr" << ", ";
   
               } else {
     
                   ostr <<  children[i].get() << ", ";
               }
      }
   
   }
   ostr << "] }";

   return ostr;
}


template<class Key, class Value> std::ostream& tree23<Key, Value>::Node23::print(std::ostream& ostr) const noexcept
{
   ostr << "[";

   if (totalItems == 0) { // remove() situation when merge2Nodes() is called

       ostr << "empty"; 

   } else {

        for (auto i = 0; i < totalItems; ++i) {

            ostr << keys_values[i].key; // or to print both keys and values do: ostr << keys_values[i];

            if (i + 1 == totalItems)  {
                continue;

            } else { 
                ostr << ", ";
            }
        }
   }

   ostr << "]";
   return ostr;
/*
   for (auto& pChild : children) {
       
       if (pChild == nullptr) {
           
           ostr << "nullptr, ";   
           
       } else {
           
         for (auto i = 0; i < pChild->totalItems; ++i) {  
            
            ostr << "{ " << pChild->keys[i] << ", " << pChild->values[i] << "}, ";
         } 
       }
   }
   ostr << std::endl; 
   return ostr;
*/
}

/*
 Checks if any sibling--not just adjacent siblings, but also those that are two hops away--are 3-nodes, from which we can "steal" a key.

 Parameters
 ==========

 1. child_index is such that: parent->children[child_index] == this
 2. If any sibling is found that is a 3-node, parent->children[silbing_index] = the 3-node sibling

 */
template<class Key, class Value> bool tree23<Key, Value>::Node23::siblingHasTwoItems(int child_index, int& sibling_index) const noexcept
{

 if (parent->isTwoNode()) { // In a recursive case, the parent has 0 totalItems, and it has only one non-nullptr child.

     sibling_index = !child_index;
     
     if (parent->children[sibling_index]->isThreeNode()) { // toggle between 0 or 1

        return true;

     } else {

        return false;
     } 
 } 

 /* 
   3-node parent cases below. Determine if any immediate sibling is a 3-node. There will only be two children to inspect when the parent is a 3-node and child_index is 1.
   */
  switch (child_index) {
      case 0:

        if (parent->children[1]->isThreeNode()) {
  
            sibling_index = 1;  
  
        } else if (parent->children[2]->isThreeNode()) {
  
            sibling_index = 2;  
  
        } else {

	    return false;
  
        }
        break;

      case 1:
        if (parent->children[0]->isThreeNode()) {
  
            sibling_index = 0;  
  
        } else if (parent->children[2]->isThreeNode()) {
  
            sibling_index = 2;  
  
        } else {
	    return false;
        }
        break;
  
      case 2:
        if (parent->children[1]->isThreeNode()) {
  
            sibling_index = 1;  
  
        } else if (parent->children[0]->isThreeNode()) {
  
            sibling_index = 0;  
  
        } else {
  
	    return false;
        }
        break;

      default:
       break; 
  }
  return true;   
}

/*
  Parameters:

1. p3node is a leaf 3-node. 
2. new_key and new_value are the new key and value passed to insert().

  Overview:

 */
template<class Key, class Value> tree23<Key, Value>::Node4::Node4(Node23 *p3node, Key new_key, const Value& new_value) noexcept : parent{p3node->parent} 
{
   bool copied = false;
   int dest = 0;
   int src = 0;

   while (src < Node23::ThreeNodeItems) {
  
         if (!copied && new_key < p3node->keys_values[src].key) {

               copied = true;
               const_cast<Key&>(keys_values[dest].key) = new_key; 
               keys_values[dest].value = new_value; 
               ++dest;

         }  else {

               keys_values[dest] = std::move(p3node->keys_values[src]);  // This was done to improper efficiency.
               ++dest;
               ++src;
         } 
   }
   
   if (!copied) {
        const_cast<Key&>(keys_values[dest].key) = new_key; 
        keys_values[dest].value = new_value; 
   }
     
   for(auto& child : children) {
      child = nullptr;
   }
}

/*
 * This constructor is called when split() encounters an internal 3-node. 
 *
Parameter requirements:

1. p3node is an internal 3-node.

2. child_index is such that

   p3node->children[child_index].get() == the prior lower level 3-node that split just handled, in which it downsized to this prior level 3-node to a
   2-node. 

3. key (and its associated value) are values split "pushed up" one level when it recursed.

4. heap_2node is the 2-node allocated on the heap in the prior call to split. 

Overview if how it works:

child_index is such that

   p3node->children[child_index].get() == the prior lower level 3-node that was downsized to a 2-node in the immediately-prior call to split

child_index is used to:

1.) determine the index to use in inserting key into Node4::keys[], and 
2.) to maintain the same general child relationships in the 4-node that existed within the 3-node. We know, for example, that heap_2node will always be
to the right the previous p3node. We also use child_index to determine where heap_2node should be placed in Node4::children[]. 

heap_2node is the 2-node allocated on the heap in the prior call to split when the 4-node created on the stack was split into two 2-nodes. heap_2node is the
larger of those two 2-nodes. 
*/

template<class Key, class Value> tree23<Key, Value>::Node4::Node4(Node23 *p3node, Key key, const Value& value, int child_index, \
  std::unique_ptr<Node23> heap_2node) noexcept : parent{p3node->parent} 
{
  switch(child_index) {
 
      case 0: // key was pushed up from the 4-node constructed from the child at p3node->children[0] (and another key). It will therefore become the
	     // smallest value in the 4-node.

      {
        const_cast<Key &>(keys_values[0].key) = key; // key is the smallest value, so we put in the first position... 
        keys_values[0].value = value;

        //...followed by the current p3node's keys and values
        for(auto i = 0; i < Node23::ThreeNodeItems; ++i) {
 
              keys_values[i + 1] = std::move(p3node->keys_values[i]); // Added move() 07/11/2016
        } 
  
        connectChild(0, std::move(p3node->children[0])); // This is still the smallest child. It is the converted 3-node downsize to a 2-node
                                                            
        connectChild(1, std::move(heap_2node));   // This is the next smallest child.

        connectChild(2, std::move(p3node->children[1])); // The others just sort of shift into the final two slots.  
        connectChild(3, std::move(p3node->children[2]));   

      }
      break;
      
      case 1: // If child_index = 1, then key was pushed up from the 4-node constructed from p3node->children[1] (plus an extra key). i.e., from
              // middle child of the prior, lower-level 3-node that split just handled, and so it will become the middle child of the 4-node since: 
              // p3node.keys_values[0].key < key && key < p3node.keys_values[1].key

      {  
        keys_values[0] = p3node->keys_values[0];
 
        const_cast<Key &>(keys_values[1].key) = key;
        keys_values[1].value = value; 
        
        keys_values[2] = p3node->keys_values[1];

        // children get moved in this manner to maintain the same relationships as those that existed in p3node
        connectChild(0, std::move(p3node->children[0]));
        connectChild(1, std::move(p3node->children[1]));
        connectChild(3, std::move(p3node->children[2]));
    
        connectChild(2, std::move(heap_2node)); // heap_2node's key is larger than the downsized 3-node in p3node->children[0], but less than its next child.
      }
      break;

      case 2: // If child_index == 2, then key was pushed up from the 4-node constructed from p3node->children[2] (plus an extra key), and
              // therefore key will be larger than all the keys in p3node:
	      //  key > p3node->keys_values[1].key

      { 
         for(auto i = 0; i < Node23::ThreeNodeItems; ++i) {   
                               
               keys_values[i] = p3node->keys_values[i]; 
         } 
    
         const_cast<Key &>(keys_values[2].key) = key; // key is the largest value in 4-node
         keys_values[2].value = value;
    
         for(auto i = 0; i < Node23::ThreeNodeChildren; ++i) { // connect p3node's current children in the same order 
    
            connectChild(i, std::move(p3node->children[i]));
         }

         // invoke Node23's move assignment operator.
         children[3] = std::move(heap_2node); // heap_2node's key is larger the p3node's largest key: p3node->keys_values[1].key 
      }
      break; 

      default:
        break; 
  }
}

template<class Key, class Value> typename tree23<Key, Value>::Node4& tree23<Key, Value>::Node4::operator=(Node4&& lhs) noexcept
{
  if (this == &lhs) return *this;

  keys_values = std::move(lhs.keys_values);

  children = std::move(lhs.children); /* This invokes std::array<Node23>'s move assignment operater. For Node23 copy or move construction one must not do this,
                           but rather call Node23::connectChild() for each child, which properly sets the parent pointer in the node; but for Node4 construction
                         the parent pointer does not need to be properly set--I believe--because Node4 is only owns the children temporarily, until it is split
                         into two Node23s, at which time Node23::connectChild() is call to properly set the node's parent pointer. */
                                     
  parent = lhs.parent;

  lhs.parent = nullptr; 
  
  return *this; 
}

template<class Key, class Value> inline void tree23<Key, Value>::Node4::connectChild(int childIndex, std::unique_ptr<typename tree23<Key, Value>::Node23> child) noexcept 
{
 /*
  Because Node4::parent is of type Node23 *, we cannot do

        parent = this;

  since 'this' is of type, Node4 *. 
  */

  Node23 *parent_of_node23 = (child == nullptr) ? nullptr : child->parent; 
  
  children[childIndex] = std::move(child); // invokes move assignment of std::unique_ptr<Node23>.

  parent = parent_of_node23;
}            
/*
 Advances cursor next if key not found in current node. If found sets found_index.
 */
template<class Key, class Value> inline bool tree23<Key, Value>::Node23::NodeDescentSearch(Key new_key, int& found_index, Node23 *next) noexcept
{
  for(auto i = 0; i < totalItems; ++i) {

     if (new_key < keys_values[i].key) {
            
         next = children[i].get(); 
         return false;

     } else if (keys_values[i].key == new_key) {

         found_index = i;
         return true;
     }
  }

  // It must be greater than the last key (because it is not less than or equal to it).
  next = children[totalItems].get(); 

  return false;
}
/*
 Advances cursor next if key not found in current node. If found sets found_index.
 */
template<class Key, class Value> inline bool tree23<Key, Value>::Node23::NodeDescentSearch(Key new_key, int& found_index, int& next_child_index) noexcept
{
  for(auto i = 0; i < totalItems; ++i) {

     if (new_key < keys_values[i].key) {
            
         next_child_index = i; 
         return false;

     } else if (keys_values[i].key == new_key) {

         found_index = i;
         return true;
     }
  }

  // It must be greater than the last key (because it is not less than or equal to it). 
  next_child_index = totalItems;  // 

  return false;
}

/*
  This method converts a 3-node into a 2-node
  Note: parent node is already correct and does not need to be set.
*/
template<class Key, class Value> void tree23<Key, Value>::Node23::convertTo2Node(Node4&& node4) noexcept
{ 
  keys_values[0] = std::move(node4.keys_values[0]);

  totalItems = Node23::TwoNodeItems; 

  // Take ownership of the two left most children of node4 
  connectChild(0, std::move(node4.children[0]));
  connectChild(1, std::move(node4.children[1]));
} 

/*
 Requires: this must be a 2-node
 Promises: creates a 3-node.
 */
template<class Key, class Value> void tree23<Key, Value>::Node23::convertTo3Node(Key new_key, const Value& new_value, std::unique_ptr<Node23> newChild) noexcept
{ 
  if (keys_values[0].key > new_key) {

      keys_values[1] = std::move(keys_values[0]);  

      const_cast<Key &>(keys_values[0].key) = new_key;
      keys_values[0].value = new_value; 

  } else {

      const_cast<Key &>(keys_values[1].key) = new_key;
      keys_values[1].value = new_value;

      // Note: This tells us that newChild will be the right most child, and the existing children do not need to move.
  }

  // Determine where newChild should be inserted.
  int child_index = 0;

  for (; child_index < Node23::TwoNodeChildren; ++child_index) {
       
       if (newChild->keys_values[0].key < children[child_index]->keys_values[0].key) { // Do I need to worry about children[child_index]->keys_values[1].key ever?
           break;
       }
  }

  if (child_index == Node23::TwoNodeChildren) {
       child_index = 2; 
  }

  // shift children to right as needed 
  for (auto i = Node23::TwoNodeChildren - 1; i >= child_index; --i) {

        connectChild(i + 1, std::move(children[i]));
  }
  // insert newChild
  connectChild(child_index, std::move(newChild));

  totalItems = Node23::ThreeNodeItems; 
} 

template<class Key, class Value> void  tree23<Key, Value>::Node23::connectChild(int childIndex, std::unique_ptr<typename tree23<Key, Value>::Node23> child) noexcept 
{
  children[childIndex] = std::move(child); 
  
  if (children[childIndex] != nullptr) { 

       children[childIndex]->parent = this; 
  }
}

template<class Key, class Value> inline void tree23<Key, Value>::Node23::connectChild(std::unique_ptr<typename tree23<Key, Value>::Node23>& dest,\
                                                                                     std::unique_ptr<typename tree23<Key, Value>::Node23> src) noexcept 
{  
  dest = std::move(src); 
  
  if (dest != nullptr) { 

      dest->parent = this; 
  }
}            

/*
 Requires: this is a 2-node leaf
 Promises: To insert key in the right position
 */
template<class Key, class Value> inline void tree23<Key, Value>::Node23::insertKeyInLeaf(Key key, const Value& new_value)
{
   if (key < keys_values[0].key) {

       keys_values[1]= std::move(keys_values[0]);

       const_cast<Key &>(keys_values[0].key) = key;
       keys_values[0].value = new_value;

   } else { // key > keys_values[0].key

       const_cast<Key &>(keys_values[1].key) = key;
       keys_values[1].value = new_value;  
   }

   ++totalItems;
   return;
}
/*
  Requires: this is a 2-node, ie, this->totalItems == Node23::TwoNodeItems
  rvalue or universal reference version.
 */
template<class Key, class Value> inline void tree23<Key, Value>::Node23::insertKeyInLeaf(Key key, Value&& new_value)
{
   if (key < keys_values[0].key) {

       keys_values[1] = std::move(keys_values[0]); 

       const_cast<Key &>(keys_values[0].key) = key;
       keys_values[0].value = std::move(new_value);

   } else { // key > keys_values[0].key

       const_cast<Key &>(keys_values[1].key) = key;
       keys_values[1].value = std::move(new_value);  
   }

   totalItems = Node23::ThreeNodeItems; 
   return;
}
#endif
