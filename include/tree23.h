#ifndef tree23_h_18932492374
#define tree23_h_18932492374

#include <array>
#include <memory>
#include <queue>
#include <utility>
#include <stack>
#include <sstream>
#include <iostream>  
#include <exception>
#include <iterator>
#include <utility>
#include "debug.h"
#include "level-order-invariant-report.h"

template<class Key, class Value> class tree23; // Forward declaration of template class tree23...

//...that is required by these friend functions
template<class Key, class Value> std::ostream& operator<<(std::ostream& ostr, const typename tree23<Key, Value>::Node23& node23); 
template<class Key, class Value> std::ostream& operator<<(std::ostream& ostr, const typename tree23<Key, Value>::Node4& node4); 

template<class Key, class Value> class tree23 {
    
  private:
  class Node4;    
  
  public:
  
  class KeyValue { // Used by nest Node23 class.
    public:
     Key   key;
     Value value;
     KeyValue() = default;
     KeyValue(Key k, Value&& v) : key{k}, value{std::move(v)} {} 
     KeyValue(Key k, const Value& v) : key{k}, value{v} {} 

     KeyValue(KeyValue&& lhs) : key{lhs.key}, value{std::move(lhs.value)} {} 
     KeyValue(const KeyValue&) = default;

     KeyValue& operator=(KeyValue&& lhs)
     {
        key = lhs.key;
        value = std::move(lhs.value);
     }
 
     KeyValue& operator=(KeyValue& lhs) = default; 
     friend std::ostream& operator<<(std::ostream& ostr, const KeyValue& key_value)
     {
         ostr << "{" << key_value.key << ',' <<  key_value.value <<  "}, ";
	 return ostr;
     }
	     
   };
 
   class Node23 {

        friend class tree23<Key, Value>;             

     public:   
        Node23(Key key, const Value& value, Node23 *ptr2parent=nullptr);
        Node23(Node4&);

        Node23(const Node23&) = delete; 
        Node23& operator=(const Node23&) = delete; 

        // Just copy the keys and values. 
	Node23(const std::array<KeyValue, 2>& lhs_keys_values, Node23 * const lhs_parent, int lhs_totalItems) noexcept; 

        Node23(Node23&&); // ...but we allow move assignment and move construction.
        Node23& operator=(Node23&&) noexcept;

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
               
           std::array<KeyValue, 2> keys_values;

           std::array<std::unique_ptr<Node23>, 3> children;

           void move_keys_values(std::array<std::unique_ptr<KeyValue>, 2>&& lhs);

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

  using node23_type = tree23<Key, Value>::Node23;
      
  private: 
    class Node4 { // Class Node4 is used during insert().

       // Always hold three keys and four children. 
      friend class tree23<Key, Value>; 
     
      private:
         std::array<KeyValue, 3> keys_values;

         // Takes ownership of four 23-nodes 
         std::array<std::unique_ptr<Node23>, 4> children; 

         Node23 *parent; // Set to the parent of the 3-node passed to its constructor 

         static const int FourNodeItems = 3;
         static const int FourNodeChildren = 4;

         void connectChild(int childIndex, std::unique_ptr<Node23> child)  noexcept;
                      
    public: 
        Node4() noexcept {}

        /* Constructor that takes an internal 3-node */
        Node4(Node23 *threeNode, Key new_key, const Value& value, int child_index, std::unique_ptr<Node23> heap_2node) noexcept;

        /* Constructor for a leaf 3-node, all child pointers will be zero. */
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
    int height;

    // subroutines called by insert()
    int findInsertNode(Key new_key, std::stack<int>& descent_indecies, Node23 *&pinsert_start) const noexcept;

    void CreateNewRoot(Key new_key, const Value& new_value, std::unique_ptr<Node23> leftChild, std::unique_ptr<Node23> rightChild) noexcept;  
   
    void CreateRoot(Key key, const Value& value) noexcept;

    void split(Node23 *current, Key new_key, const Value& new_value, std::stack<int>& child_indecies, \
            std::unique_ptr<Node23> heap_2node) noexcept;

    // subroutines called by remove()
    Node23* findRemovalStartNode(Key key, std::stack<int>& child_indecies, int& found_index) const noexcept;

    Node23 *remove_getSuccessor(Node23 *pnode, int found_index, std::stack<int>& child_indecies) const noexcept;

    void fixTree(Node23 *pnode, std::stack<int>& child_indecies) noexcept;

    void reassignRoot() noexcept;

    void barrowSiblingKey(Node23 *pnode, int child_index, int sibling_index) noexcept;
 
    // subroutines called by barrowSiblingKey()
    void shiftChildrenRight(Node23 *node, Node23 *sibling) noexcept;
    void shiftChildrenRight(Node23 *node, Node23 *middleChild, Node23 *sibling) noexcept;

    void shiftChildrenLeft(Node23 *node, Node23 *sibling) noexcept;
    void shiftChildrenLeft(Node23 *node, Node23 *middleChild, Node23 *sibling) noexcept;

    std::unique_ptr<Node23> mergeNodes(Node23 *pnode, int child_index) noexcept;
    
    std::unique_ptr<Node23> merge2Nodes(Node23 *pnode, int child_index) noexcept;
    std::unique_ptr<Node23> merge3NodeWith2Node(Node23 *pnode, int child_index) noexcept;

    void shiftChildren(Node23 *node, Node23 *sibling, int node_index, int sibling_index) noexcept;
     
    // traversal subroutines
    template<typename Functor> void DoInOrderTraverse(Functor f, const std::unique_ptr<Node23>& root) const noexcept;

    template<typename Functor> void DoPostOrderTraverse(Functor f,  const std::unique_ptr<Node23>& root) const noexcept;

    template<typename Functor> void DoPreOrderTraverse(Functor f, const std::unique_ptr<Node23>& root) const noexcept;

    /* 
     * Can a functor be used instead along with pre-order traversal to clone the tree: CloneTreeFunctor(...)?
    */

   // Called by copy constructor and copy assignment operators.
   void CloneTree(const std::unique_ptr<Node23>& Node2Copy, std::unique_ptr<Node23>& NodeCopy) noexcept;
   void DestroyTree(std::unique_ptr<Node23> &root) noexcept; 

   // Useful utilities. For example, class iterator uses these methods.
   const Node23 *getSmallestNode(const Node23 *subtree_root) const noexcept;	    

  public:


    //--class iterator_base : public std::iterator<std::forward_iterator_tag, std::pair<const Key,Value>> { // Maybe I need to reimplement the tree to hold a pair?      
                              // rather than a KeyValue? 
    class iterator_base : public std::iterator<std::forward_iterator_tag, typename tree23<Key, Value>::KeyValue > { 
                                                 
       friend class tree23<Key, Value>;   
      public:

         tree23<Key, Value>& tree; 

         const typename tree23<Key, Value>::Node23 *current;

         int key_index;  // The index is such that current == current->parent->children[child_index]

         int   getChildIndex(const typename tree23<Key, Value>::Node23 *p) const noexcept;
         void  getSuccessor() noexcept;

         const Node23 *getInternalNodeSuccessor(const typename tree23<Key, Value>::Node23 *pnode) const noexcept;

         std::pair<const typename tree23<Key, Value>::Node23 *, int>  getLeafNodeSuccessor(const typename tree23<Key, Value>::Node23 *) const noexcept;

         std::pair<const typename tree23<Key, Value>::Node23 *, int> findLeftChildAncestor() noexcept;

         void getPredecessor() noexcept;
         void seekToSmallest() noexcept;    
         void seekToLargest() noexcept;    

      public:
         iterator_base(tree23<Key, Value>& lhs); // for use by iterator derived class
         iterator_base(tree23<Key, Value>& lhs, int); // for use by end()

         iterator_base(const iterator_base& lhs);
         iterator_base(iterator_base&& lhs);

         bool operator==(const iterator_base& lhs) const;
         bool operator!=(const iterator_base& lhs) const { return !operator==(lhs); }
         
         typename tree23<Key, Value>::KeyValue&        operator*() noexcept; // KeyValue& is wrong. We don't want to change the key. How about std::pair<Key, Value&>?
         const typename tree23<Key, Value>::KeyValue&  operator*() const noexcept; // KeyValue& is wrong. We don't want to change the key. How about std::pair<Key, Value&>?
         typename tree23<Key, Value>::KeyValue*        operator->() { return &operator*(); } // KeyValue& or pair<Key, Value&>????
    };

    class iterator : protected iterator_base { // in order iterator
        public:

         iterator(tree23<Key, Value>& lhs) : iterator_base{lhs} {}
         iterator(const iterator& lhs) : iterator_base{lhs} {}
         iterator(iterator&& lhs) : iterator_base{std::move(lhs)} {}
         
         bool operator==(const iterator& lhs) const { return iterator_base::operator==(static_cast< const iterator_base& >(lhs)); }
         bool operator!=(const iterator& lhs) const { return iterator_base::operator!=(static_cast< const iterator_base& >(lhs)); }
         
         iterator& operator++() noexcept; 
         iterator operator++(int) noexcept;
         
         // new methods
         typename tree23<Key, Value>::KeyValue& operator*() noexcept; // KeyValue& is wrong. We don't want to change the key. How about std::pair<Key, Value&>?
         typename tree23<Key, Value>::KeyValue *operator->() const noexcept;
    };

    class const_iterator: protected iterator_base { // in order iterator

      public:
         const_iterator(tree23<Key, Value>& lhs) : iterator_base{lhs} {}
         const_iterator(const const_iterator& lhs) : iterator_base{lhs} {}
         const_iterator(const_iterator&& lhs) : iterator_base{std::move(lhs)} {}
         const_iterator(); // end() const;

         bool operator==(const const_iterator& lhs) const { return iterator_base::operator==(static_cast< const iterator_base& >(lhs)); }
         bool operator!=(const const_iterator& lhs) const { return iterator_base::operator!=(static_cast< const iterator_base& >(lhs)); }
         
         const_iterator& operator++() noexcept;
         const_iterator operator++(int) noexcept;
         const typename tree23<Key, Value>::KeyValue&  operator*() const noexcept; // KeyValue& is wrong. We don't want to change the key. How about std::pair<Key, Value&>?
         const typename tree23<Key, Value>::KeyValue *operator->() const noexcept { return &operator*(); } // KeyValue& or pair<Key, Value&>????
    };

    iterator begin() noexcept;  
    iterator end() noexcept;  

    const_iterator begin() const noexcept;  
    const_iterator end() const noexcept;  

    tree23() noexcept;

    void test_invariant() const noexcept;

    std::ostream& test_height(std::ostream& ostr) const noexcept;

    std::string test_invariant(const Node23& p) const noexcept; // new

    tree23(const tree23&) noexcept; // TODO: Not implemented
    tree23& operator=(const tree23&) noexcept; // TODO: Implemented?

    tree23(tree23&&) noexcept;

    tree23& operator=(tree23&&) noexcept;

    int getHeight() const noexcept;
    void insert(Key key, const Value& value);
    bool isEmpty() const noexcept;

    /* 
    TODO: implement later
    void insert(Key key, Value&& value);
    */
    bool find(Key key) const noexcept;

    const Value& operator[](Key key) const;

    Value& operator[](Key key);

    void remove(Key key);

    // Breadth-first traversal
    template<class Functor> void levelOrderTraverse(Functor f) const noexcept;

    // Depth-first traversals
    template<typename Functor> void inOrderTraverse(Functor f) const noexcept;

    // traverse() is synonymous with inOrderTraverse()
    template<typename Functor> void traverse(Functor f) const noexcept { DoInOrderTraverse(f, root); }

    template<typename Functor> void postOrderTraverse(Functor f) const noexcept;
    template<typename Functor> void preOrderTraverse(Functor f) const noexcept;
};

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
  keys_values[0].key = key;
  keys_values[0].value = value;
 
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

template<class Key, class Value> inline void tree23<Key, Value>::Node23::move_keys_values(std::array<std::unique_ptr<KeyValue>, 2>&& lhs)
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
template<class Key, class Value> inline tree23<Key, Value>::Node23::Node23(const std::array<KeyValue, 2>& lhs_keys_values,\
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

  // If it is not a leaf, then it is an internal node. This is the recursive remove case when the parent of the inital empty 2-node leaf is itself
  // also a 2-node that will be merged with its sole non-empty child. Thus, we test for one and only one non-nullptr child.
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

template<class Key, class Value> inline bool tree23<Key, Value>::isEmpty() const noexcept
{
  return root == nullptr ? true : false;
}

template<class Key, class Value> inline void tree23<Key, Value>::test_invariant() const noexcept
{
  levelOrderInvariantReport<tree23<Key, Value>> reporter(const_cast<const tree23<Key,Value>&>(*this), std::cout);

  levelOrderTraverse(reporter); 
}

template<class Key, class Value> std::ostream& tree23<Key, Value>::test_height(std::ostream& ostr) const noexcept
{
  // Test that height is correct
  int depth = 0;

  for (auto current = root.get(); current != nullptr; current = current->children[0].get()) {

       ++depth;
  }

  if (height != depth) {
     ostr << "error: The height is " << height << " but actual calculated height is " << depth << ".\n";
  }
}

template<class Key, class Value> std::string tree23<Key, Value>::test_invariant(const Node23& const_node) const noexcept
{
  std::ostringstream oss;

  Node23& node = const_cast<Node23&>(const_node);

  switch(node.totalItems) {

      case 0:
         node.test_remove_invariant(oss);
         break;

      case Node23::TwoNodeItems:

         node.test_2node_invariant(oss, root.get());
         break;   
      
      case Node23::ThreeNodeItems:

         node.test_3node_invariant(oss, root.get());
         break;   
    
      default:
         // If we come here, then node.totalItems is wrong.
         oss << " error: node.totalItems is " << node.totalItems << ".\n"; 
         break;
  }

  std::string msg;

  if (oss.str().length() > 0) { 

     msg = " --> " + oss.str();
  } 

  return msg; 
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

         //--ostr << "{" << keys_values[i].key << "}, ";

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

template<class Key, class Value> inline typename tree23<Key, Value>::iterator tree23<Key, Value>::begin() noexcept
{
    return iterator{const_cast<tree23<Key, Value>&>(*this)};
}

/*
 end() calls the iterator constructor that sets current to nullptr and key_index to 0. 
 */

template<class Key, class Value> inline typename tree23<Key, Value>::iterator tree23<Key, Value>::end() noexcept
{
    return iterator{const_cast<tree23<Key, Value>&>(*this), 1};
}

template<class Key, class Value> inline typename tree23<Key, Value>::const_iterator tree23<Key, Value>::begin() const noexcept
{
    const_iterator const_iter{ const_cast<tree23<Key, Value>&>( *this ) }; // cast away const

    const_iter.seekToSmallest();
    
    return const_iter;
}

template<class Key, class Value> inline typename tree23<Key, Value>::const_iterator tree23<Key, Value>::end() const noexcept
{
   return const_iterator{const_cast<tree23<Key, Value>&>(*this)};
}

// non const tree23<Key, Value>& passed to ctor. Used by begin()
template<class Key, class Value> inline tree23<Key, Value>::iterator_base::iterator_base(tree23<Key, Value>& lhs_tree) : tree{lhs_tree}, current{lhs_tree.root.get()}, \
                                                                 key_index{0} 
{
}

template<class Key, class Value> inline void tree23<Key, Value>::iterator_base::seekToSmallest() noexcept
{
  for (const Node23 *cursor = tree.root.get(); cursor != nullptr; cursor = cursor->children[0].get()) {
           current = cursor;
  }

  key_index = 0;
}

template<class Key, class Value> inline void tree23<Key, Value>::iterator_base::seekToLargest() noexcept
{
  for (const Node23 *cursor = tree.root.get(); cursor != nullptr; cursor = cursor->children[cursor->totalItems].get()) {
           current = cursor;
  }

  key_index = (current->isThreeNode()) ? 1 : 0;
}
/*
 constructor called by end()
 */
template<class Key, class Value> inline tree23<Key, Value>::iterator_base::iterator_base(tree23<Key, Value>& lhs_tree, int x) : tree{lhs_tree}, current{nullptr},\
                                     key_index{0} 
{
}

template<class Key, class Value>  tree23<Key, Value>::iterator_base::iterator_base(iterator_base&& lhs) : \
             tree{lhs.tree}, current{lhs.current}, key_index{lhs.key_index} 
{
   lhs.current = nullptr; // set to end
}

template<class Key, class Value> inline bool tree23<Key, Value>::iterator_base::operator==(const iterator_base& lhs) const
{
 if (this == &lhs) {

     return true;

 } else if (&lhs.tree == &tree) {

    if (lhs.current == nullptr && current == nullptr) { // == end() test

        return true;

    } else {
     
      return  current == lhs.current && key_index == lhs.key_index;
   }

 } 

  return false;
 
}

// ctor called by end()
/*
template<class Key, class Value> inline tree23<Key, Value>::iterator_base::iterator_base(const iterator_base<Tree>& lhs_tree, Node23 *ptr) : tree{lhs_tree}, current{nullptr}
{
}
*/
/*
 getChildIndex(Node23 *pnode)

 Requires:
 pnode a node in the tree for which we want child_index such that

 current->parent->children[child_index] == current

 Returns:
 child_index as shown above. 

 */

template<class Key, class Value> inline int tree23<Key, Value>::iterator_base::getChildIndex(const typename tree23<Key, Value>::Node23 *p) const noexcept
{
  // Determine child_index such that current == current->parent->children[child_index]
  int child_index = 0;

  for (; child_index <= current->parent->totalItems; ++child_index) {

       if (current == current->parent->children[child_index].get())
              break;
  }

  return child_index;
}
/*

Finding the successor of a given node 
-------------------------------------

From: http://ee.usc.edu/~redekopp/cs104/slides/L19_BalancedBST_23.pdf

On how to find the successor in a binary tree.

If left child exists, predecessor is the right most node of the left subtree. Internal node's of a 2 3 tree always have a right branch because 2 3 trees are
balanced.

Else walk up the ancestor chain until you traverse the first right child pointer (find 
the first node who is a right child of his parent...that parent is the predecessor)

If you get to the root w/o finding a node who is a right child, there is no predecessor
 */
template<class Key, class Value> void tree23<Key, Value>::iterator_base::getSuccessor() noexcept
{
  if (current == nullptr) { // If we are at the end, return. 

      return; 
  }
 
  if (current->isLeaf()) { // If leaf node

     std::pair<const Node23 *, int> results = getLeafNodeSuccessor(current);
     current = results.first;
     key_index = results.second;

  } else { // else internal node

      current = getInternalNodeSuccessor(current);
      key_index = 0; // it will always be the first key
  }
}
/* 
   getInternalNodeSuccessor()

   Requires:
   1. pnode is an internal node not a leaf node.
   2. If pnode is a 3-node, key_index is 1 not 0.

   Returns:
   1. pointer to successor node.

Bug: When a 2 3 tree node is a 3-node, it two "right" chidren from its first key and two "left" children from its second key.
 */
template<class Key, class Value> inline const typename tree23<Key, Value>::Node23 *tree23<Key, Value>::iterator_base::getInternalNodeSuccessor(const typename tree23<Key, Value>::Node23 *pnode) const noexcept	    
{
 // Get next right of pnode based on key_index.
 const Node23 *rightChild;

 if (pnode->isThreeNode()) {

      if (key_index == 0) {

        rightChild = pnode->children[1].get();

      } else { // it is 1     

        rightChild = pnode->children[2].get(); 
      }

 } else { // It is a 2-node

        rightChild = pnode->children[1].get();
 }
 // TODO: I'm not sure this is right. Does it take into account that fact that a node may have already been visited in order?
 // Get th smallest node in the subtree rooted at the rightChild, i.e., its left most node...
 for (const Node23 *cursor = rightChild; cursor != nullptr; cursor = cursor->children[0].get()) {

    pnode = cursor;
 }
 
 return pnode;
}
/*
 Requires:
 1. pnode is a leaf node, either a 2 or 3-node
 2. If pnode is 3-node, then key_index, the key index into pnode->keys_values[].key must be 1, the second key. It can never be 0, the first key.

 */
template<class Key, class Value> std::pair<const typename tree23<Key, Value>::Node23 *, int> tree23<Key, Value>::iterator_base::getLeafNodeSuccessor(const typename tree23<Key, Value>::Node23 *pnode) const noexcept
{
  // If the leaf node is a 3-node and key_index points to the first key, this is trivial: we simply set key_index to 1. 
  if (pnode->isThreeNode() && key_index == 0) {

      return std::make_pair(current, 1); 
  }

  // Determine child_index such that pnode == pnode->parent->children[child_index]
  int child_index = getChildIndex(pnode);

  int suc_key_index;

  /*
   Handle easy cases first:
   1. If child_index is 0, then the successor -- when pnode is either a 2-node of 3-node -- is pnode->parent and the suc_key_index is 0.
   2. If child_index is 1 and parent is a 3-node, the successor is pnode->parent and suc_key_index is 1.
   */
  switch (child_index) {

      case 0: /*
             pnode is either the left most child of either a 2-node or 3-node parent. If pnode is a 3-node, its key_index equals 1 (because it is was 0,
             this was already handled at the beginning of this method. 
             The possibilities are:

            (a)   [20]       (b) [20]    (c)   [20, 40]       (d) [20,  40]    
                  / \            /  \          /   |  \            /   |  \ 
               [15]  [x]    [15, 18] [x]    [15]  [ ]  [ ]   [15, 18] [ ] [ ]   Note: if leaf is a 3-node, key_index is 1.

          In all four scenarios above, we advance to the first key of the parent. 
               */
         pnode = pnode->parent;
         suc_key_index = 0;
         break;
 
      case 1: /* 
            pnode is either the left child of a 2-node or the middle child of a 3-node. If pnode is a 3-node, key_index equals 1 (because if it was a 0, it was
            already handled above).  The possibilities look like this

             If parent is a 2-node, there are two possible cases: 

              (a)   [20]    (b)   [20]
                    / \           /  \  
                  [x]  [30]    [x]  [30, 32]

             In (a), key_index is 0. In (b), key_index is 1.   

             If parent is a 3-node, there are two possible cases: 

              (a)   [20,  40]    (b)   [20,   40]
                    /   |   \         /    |    \ 
                  [x]  [30] [ ]     [x]  [30,32] [ ] 
             
              In (a) above, key_index is 0. In (b), key_index is 1. 
               
              */ 
         if (pnode->parent->isThreeNode()) { // This is the trivial case, we advance to the 2nd key of the parent 3-node. 

            pnode = pnode->parent;
            suc_key_index = 1;
            break;
         } 

         /* If the parent is a 2-node, we fall through to 'case 2' */
    case 2: 
   /* 
    The possibilites for this case are: 

       (a)   [20]    (b)   [20]       (c)   [20,   40]     (d)   [20,   40]        
             / \           /  \            /    |    \          /    |    \        
           [x]  [30]    [x]  [30, 32]    [ ]   [ ]   [50]     [ ]   [ ]   [50, 60] 

    In (a), pnode is [30]. In (b), pnode is [30, 32] and key_index is 1. In (c), pnode is [50]. In (d), pnode is [50, 60] and key_index of 1.

    In all four cases, the logic is identical. We walk up the ancestor chain until we traverse the first left child pointer, that is, we find the first node that is
    a left child of its parent. That parent is the successor. If we get to the root without finding a node that is a left child, there is no successor.

    Note: In a 2 3 tree, a "left child pointer" isn't always the first child. A "left child pointer" simply means a pointer to a subtree with smaller values than
    the parent. In a 2 3 tree, the middle child pointer of a 3-node parent is a "left child pointer" of the 2nd key because all the values of the subtree rooted at
    the middle child are less than the 2nd key of the middle child's parent 3-node. 

    So when we walk up the ancestor chain, we stop when we find a child pointer that is not the right most child pointer of its parent. If we get to the root without
    finding a non-right most child pointer, there is no successor. For example, in the tree portion shown below

                  [17,       60]   <-- 3-node
                  /       |     \
                 /        |      \
              [10]       [35]     [70, 100]
             /   \       /  \      /  |  \
           [5]  [15]   [20] [50]  <-- pnode points to leaf node [50]. 
           / \   / \   / \  / \   
          0   0 0   0 0   0 0  0  ... 

      In the tree above, if [50] is the pnode leaf node, the successor of [50] is the second key of the 3-node [17, 60]. When we walk up the parent chain from [50],
      the first left child pointer we encounter is the middle child of the 3-node [17, 60], which is the "left" child of 60. So [60] is the next largest key.

      The same logic applies to all four possilbe cases (a) through (d). For example, for case (b), illustrate in the tree below
 
                  [17,            60]   <-- 3-node
                  /       |         \
                 /        |          \
              [10]       [35]        [70, 100]
             /   \       /  \        /  |  \
           [5]  [15]   [20] [50, 55]                   <-- pnode points to key 55 in leaf node [50, 55]. 
           / \   / \   / \  / \   
          0   0 0   0 0   0 0  0  ... 
    
      again, 60 is the successor by applying the same reasoning.

      */
        {
           const Node23 *prior_node = pnode;
           const Node23 *__parent = pnode->parent;
           
           // Ascend the parent pointers as long as pnode is the right most child of its parent.
           while(pnode == __parent->children[__parent->totalItems].get() )  {
           
               // pnode is still the right most child but now its parent is the root, therefore there is no successor. 
               if (__parent == tree.root.get()) {
           
                   return std::make_pair(nullptr, 0);  // Because pnode is still the right most child of its parent it has no successor.
                                                       // To indicate this we set current to nullptr and key_index to 0.
               }
           
               prior_node = pnode;
               pnode = __parent;
               __parent = __parent->parent;
           }
           
           prior_node = pnode; 
           pnode = __parent;
           
           // If pnode is a 3-node, determine if we ascended from the first child, children[0], or the middle child, children[1], and set suc_key_index accordingly. 
           if (pnode->isThreeNode()) {

              suc_key_index = (prior_node == pnode->children[0].get()) ? 0 : 1; 

           } else { // pnode is a 2-node

              suc_key_index = 0;
           }
         }
         break;

    default:
       break;

  } // end switch

  return std::make_pair(pnode, suc_key_index); 
}

/*
 non-recursive version
 */
template<class Key, class Value> std::pair<const typename tree23<Key, Value>::Node23 *, int> tree23<Key, Value>::iterator_base::findLeftChildAncestor() noexcept
{
  // Ascend ancester's until a key greater than current->keys_values[key_index].key is found or root is encountered.
  for (Node23 *parent = current->parent; parent != tree.root.get(); parent = parent->parent)

    for (int i = 0; i <= parent->totalItems; ++i) {

        if (current->keys_values[key_index].key < parent->keys_values[i]) {

            return std::make_pair(parent, i);

            return;
        } 
   }

   return std::make_pair(nullptr, 0); 
}

/* 
 Recursive version

 Requires:
 current is a leaf node that is the right most child of its parent. 
 Therefore the successor is the first left child in the anscester trail of parents (before the root is encountered).

 Promises:
 1. Returns successor node.
 2. sets key_index to the index of the successor within that node's keys_values[].
 */

template<class Key, class Value> void tree23<Key, Value>::iterator_base::getPredecessor() noexcept	    
{
  // TODO: implement
}

template<class Key, class Value> typename tree23<Key, Value>::KeyValue& tree23<Key, Value>::iterator_base::operator*() noexcept
{
   if (current == nullptr) {
         return ???; 
   }
   return const_cast<typename tree23<Key, Value>::KeyValue&>( current->keys_values[key_index] ); // cast away const
}
template<class Key, class Value> const  typename tree23<Key, Value>::KeyValue& tree23<Key, Value>::iterator_base::operator*() const noexcept
{
   return const_cast<const tree23<Key, Value>::KeyValue&>(current->keys_values[key_index]);
}

template<class Key, class Value> typename tree23<Key, Value>::KeyValue& tree23<Key, Value>::iterator::operator*() noexcept	    
{
  return iterator_base::operator*();
}

template<class Key, class Value> typename tree23<Key, Value>::iterator& tree23<Key, Value>::iterator::operator++() noexcept	    
{
   iterator_base::getSuccessor(); // call base class method that sets current and key_index 
}

template<class Key, class Value> typename tree23<Key, Value>::iterator tree23<Key, Value>::iterator::operator++(int) noexcept	    
{
 iterator tmp{*this};

 iterator_base::getSuccessor(); 

 return *this;
}

template<class Key, class Value> typename tree23<Key, Value>::const_iterator& tree23<Key, Value>::const_iterator::operator++() noexcept	    
{
   iterator_base::getSuccessor(); // call base class method that sets current and key_index 
}

template<class Key, class Value> typename tree23<Key, Value>::const_iterator tree23<Key, Value>::const_iterator::operator++(int) noexcept	    
{
 const_iterator tmp{*this};

 iterator_base::getSuccessor(); 

 return *this;
}

template<class Key, class Value> const typename tree23<Key, Value>::KeyValue& tree23<Key, Value>::const_iterator::operator*() const noexcept	    
{
  return iterator_base::operator*(); // invokes const version of iterator_base<Tree>::operator* 

  //--return const_cast<const tree23::KeyValue&>( iterator_base::operator*() );
}


////////////// end insert

/*
 Checks if any sibling (not just adjacent siblings, but also those that are two hops away) are 3-nodes: has two keys.

 Parameters
 ==========

 1. child_index is such that: parent->children[child_index] == this
 2. If any sibling is found that is a 3-node, parent->children[silbing_index] = the 3-node sibling

 */
template<class Key, class Value> bool tree23<Key, Value>::Node23::siblingHasTwoItems(int child_index, int& sibling_index) const noexcept
{

 if (parent->isTwoNode()) { // Bug: In a recursive case, the parent has 0 totalItems. It also has only one non-nullptr child.

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

template<class Key, class Value> inline tree23<Key, Value>::tree23() noexcept : root{nullptr}, height{0} 
{

}   

template<class Key, class Value> inline tree23<Key, Value>::tree23(const tree23<Key, Value>& lhs) noexcept
{
  // traverse the tree copying each of its nodes
 if (root == lhs.root) { // are they the same?

       return;
  }

  DestroyTree(root); // free all the nodes of the current tree 

  height = lhs.height;
  
  // Traverse in pre-order using the clone functor. See todo.txt
  CloneTree(lhs.root, root);
}   

/*
 Does pre-order traversal, copying source node reference in left parameter to node reference in right parameter.
 */
template<class Key, class Value>  void tree23<Key, Value>::CloneTree(const std::unique_ptr<typename tree23<Key, Value>::Node23>& Node2Copy, \
        std::unique_ptr<typename tree23<Key, Value>::Node23>& NodeCopy) noexcept
{
  if (Node2Copy != nullptr) { 
                              
   switch (Node2Copy->totalItems) {

      case 1: // two node
      {    
            NodeCopy = std::make_unique<Node23>(Node2Copy->keys_values, Node2Copy->parent, Node2Copy->totalItems);
             
            NodeCopy->parent = Node2Copy->parent;
            
            CloneTree(Node2Copy->children[0], NodeCopy->children[0]); 
            
            CloneTree(Node2Copy->children[1], NodeCopy->children[1]); 

            break;

      }   // end case
      case 2: // three node
      {
            NodeCopy = std::make_unique<Node23>(Node2Copy->keys_values, Node2Copy->parent, Node2Copy->totalItems); 

            NodeCopy->parent = Node2Copy->parent;
            
            CloneTree(Node2Copy->children[0], NodeCopy->children[0]);
            
            CloneTree(Node2Copy->children[1], NodeCopy->children[1]);
            
            CloneTree(Node2Copy->children[2], NodeCopy->children[2]);

            break;
      } // end case
   }  // end switch
 } else {

    NodeCopy = nullptr;
 } 
}

/*
 * Post order traversal, deleting nodes
 */
template<class Key, class Value> void tree23<Key, Value>::DestroyTree(std::unique_ptr<Node23> &current) noexcept 
{
  if (current == nullptr) {

      return;
  }
  
  for(auto i = 0; i < current->totalItems; ++i) {

        DestroyTree(current->children[i]);
   }

   current.reset(); // deletes the underlying pointer. 
}

// Move constructor
template<class Key, class Value> inline tree23<Key, Value>::tree23(tree23<Key, Value>&& lhs) noexcept : root{std::move(lhs.root)}, height{lhs.height}
{
  lhs.height = 0;
}   

// Move assignment
template<class Key, class Value> inline tree23<Key, Value>& tree23<Key, Value>::operator=(tree23<Key, Value>&& lhs) noexcept
{
  if (this == &lhs)  {
      
      return *this;
  }
  
  root = std::move(lhs.root);  
  
  height = lhs.height;
  
  lhs.height = 0;
  return *this;
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
               keys_values[dest].key = new_key; 
               keys_values[dest].value = new_value; 
               ++dest;

         }  else {

               keys_values[dest] = std::move(p3node->keys_values[src]);  // This was done to improper efficiency.
               ++dest;
               ++src;
         } 
   }
   
   if (!copied) {
        keys_values[dest].key = new_key; 
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
        keys_values[0].key = key; // key is the smallest value, so we put in the first position... 
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
 
        keys_values[1].key = key;
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
    
         keys_values[2].key = key; // key is the largest value in 4-node
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

  since 'this' is of type Node4 *. 
  */

  Node23 *parent_of_node23 = (child == nullptr) ? nullptr : child->parent; 
  
  children[childIndex] = std::move(child); // invokes move assignment of std::unique_ptr<Node23>.

  parent = parent_of_node23;
}            

template<class Key, class Value> inline std::ostream& tree23<Key, Value>::Node4::debug_print(std::ostream& ostr) const noexcept
{
  return this->print(ostr);
}


template<class Key, class Value> std::ostream& tree23<Key, Value>::Node4::print(std::ostream& ostr) const noexcept
{
   ostr << this << " [";

   for (auto i = 0; i < Node4::FourNodeItems; ++i) {

       ostr << "{ " << keys_values[i].key << ", " << keys_values[i].values << "}, "; 
   }
   
   ostr << "] children:  [ ";  
   
   for (auto& pChild : children) {
       
       if (pChild == nullptr) {
           
           ostr << "nullptr, ";   
           
       } else {

         for (auto i = 0; i < pChild->totalItems; ++i) {  
            
            ostr << "{ " << pChild->keys_values[i].key << ", " << pChild->keys_values[i].value << "}, ";
         } 

       }
   }
   ostr << "]" << std::endl;
   return ostr;
}

template<class Key, class Value> inline int tree23<Key, Value>::getHeight() const noexcept
{
  return height;

  /* This is a double check, a sort of assertion  
  int depth = 0;

  for (auto current = root.get(); current != nullptr; current = current->children[0].get()) {

       ++depth;
  }

  return depth;
  */
}

template<class Key, class Value> template<typename Functor> inline void tree23<Key, Value>::inOrderTraverse(Functor f) const noexcept
{
   DoInOrderTraverse(f, root);
}

template<class Key, class Value> template<typename Functor> inline void tree23<Key, Value>::postOrderTraverse(Functor f) const noexcept
{
   DoPostOrderTraverse(f, root);
}

template<class Key, class Value> template<typename Functor> inline void tree23<Key, Value>::preOrderTraverse(Functor f) const noexcept
{
   DoPreOrderTraverse(f, root);
}

template<class Key, class Value> template<typename Functor> void tree23<Key, Value>::DoInOrderTraverse(Functor f, const std::unique_ptr<Node23>& current) const noexcept
{
   if (current == nullptr) {

      return;
   }

   switch (current->getTotalItems()) {

      case 1: // two node
            DoInOrderTraverse(f, current->children[0]);

            f(const_cast<const KeyValue&>(current->keys_values[0]));

            DoInOrderTraverse(f, current->children[1]);
            break;

      case 2: // three node
            DoInOrderTraverse(f, current->children[0]);

            f(const_cast<const KeyValue&>(current->keys_values[0]));

            DoInOrderTraverse(f, current->children[1]);
 
            f(const_cast<const KeyValue&>(current->keys_values[1]));

            DoInOrderTraverse(f, current->children[2]);
            break;
   }
}

template<class Key, class Value> template<typename Functor> inline void tree23<Key, Value>::DoPostOrderTraverse(Functor f, const std::unique_ptr<Node23>& current) const noexcept
{
   if (current == nullptr) {

        return;
   }

   switch (current->totalItems) {

      case 1: // two node
            DoPostOrderTraverse(f, current->children[0]);

            DoPostOrderTraverse(f, current->children[1]);

            f(const_cast<const KeyValue&>(current->keys_values[0]));
            break;

      case 2: // three node
            DoPostOrderTraverse(f, current->children[0]);

            DoPostOrderTraverse(f, current->children[1]);

            f(const_cast<const KeyValue&>(current->keys_values[0]));

            DoPostOrderTraverse(f, current->children[2]);

            f(const_cast<const KeyValue&>(current->keys_values[1]));
            break;
   }
}

template<class Key, class Value> template<typename Functor> inline void tree23<Key, Value>::DoPreOrderTraverse(Functor f, const std::unique_ptr<Node23>& current) const noexcept
{
  if (current == nullptr) {

        return;
   }

   switch (current->totalItems) {

      case 1: // two node
            f(const_cast<const KeyValue&>(current->keys_values[0]));

            DoPreOrderTraverse(f, current->children[0]);

            DoPreOrderTraverse(f, current->children[1]);

            break;

      case 2: // three node
            f(const_cast<const KeyValue&>(current->keys_values[0]));

            DoPreOrderTraverse(f, current->children[0]);

            DoPreOrderTraverse(f, current->children[1]);

            f(const_cast<const KeyValue&>(current->keys_values[1]));

            DoPreOrderTraverse(f, current->children[2]);

            break;
   }
}

template<class Key, class Value> template<typename Functor> void tree23<Key, Value>::levelOrderTraverse(Functor f) const noexcept
{
   std::queue< std::pair<const Node23*, int> > queue; 

   Node23 *proot = root.get();

   if (proot == nullptr) return;
      
   auto initial_level = 1; // initial, top level is 1, the root.
   
   // 1. pair.first  is: const tree<Key, Value>::Node23*
   // 2. pair.second is: current level of tree.
   queue.push(std::make_pair(proot, initial_level));

   while (!queue.empty()) {

        std::pair<const Node23 *, int> pair_ = queue.front();

        const tree23<Key, Value>::Node23 *current = pair_.first;

        int current_tree_level = pair_.second;

        f(*current, current_tree_level);  
        
        if (current != nullptr && !current->isLeaf()) {

            if (current->totalItems == 0) { // This can happen only during remove() when an internal 2-node becomes empty temporarily...

                   //...when only and only one of the empty 2-node's children will be nullptr. 
                   queue.push( std::make_pair( (current->children[0] == nullptr) ? nullptr : current->children[0].get(), current_tree_level + 1) ); 
                   queue.push( std::make_pair( (current->children[1] == nullptr) ? nullptr : current->children[1].get(), current_tree_level + 1) ); 

	    } else {
            
                for(auto i = 0; i < current->getChildCount(); ++i) {
    
                   queue.push(std::make_pair(current->children[i].get(), current_tree_level + 1));  
                }
	    }
        }

        queue.pop(); 
   }
}

template<class Key, class Value> bool tree23<Key, Value>::find(Key key) const noexcept
{
  if (isEmpty()) {
      return false;
  }

  Node23 *current = root.get();

  while (current != nullptr) {
      
       auto totalItems = current->totalItems;   
       auto i = 0;

       for(; i < totalItems; ++i) {

           if (key < current->keys_values[i].key) {

                current = current->children[i].get();
                break;

           } else if (key == current->keys_values[i].key) {

                return true;
           } 
       }

       if (i == totalItems) {

           current = current->children[totalItems].get(); // key > largest key
       } 
  }

  return false;
}

template<class Key, class Value> inline void tree23<Key, Value>::CreateRoot(Key key, const Value& value) noexcept
{
   root = std::make_unique<Node23>(key, value);
   height = 1; // first node added to tree, the root.
}
/* 
   If new_key is already in the tree, we overwrite its associate value with new_value. If it is not in the tree, we descend to the leaf where the
   insertion should begin. If the leaf is a 2-node, we insert the new key. If it is a 3-node we call split(), passing a throw away unique_ptr<Node23> that
   holds a nullptr.
 
 */
template<class Key, class Value> void tree23<Key, Value>::insert(Key new_key, const Value& new_value)
{
  if (root == nullptr) {
      
      // Create the initial unique_ptr<Node23> in the tree.
      CreateRoot(new_key, new_value);
      return;
  }

/*
  new_key will be inserted between the next largest value in the tree and the next smallest:

        in order predecessor key < new_key < in order successor key

   "stack<int> child_indecies" tracks each branch taken descending to the leaf where new_key should be inserted. This aids in creating 4-nodes
   from internal 3-nodes. child_indecies tells us the branches take from the root to pinsert_start. 

   Thus, for example, the code such as that below, which converts the descent branches contained in the stack<int> named child_indecies into a deque<int> named
   branches.  "branches" can then be used to duplicate the exact descent braches take root leaf where new_key insertion should begin:

       // convert stack to deque
       deque<int> branches;

       while (!child_indecies.empty()) {
              branches.push_back(stk.top());
              child_indecies.pop();
       }
 
       Node23 *current = root.get();
       
       for (auto branch : branches) { // descend to pinsert_start from root
            current = current->children[branch]; 
       } 
*/

  std::stack<int> child_indecies; 

  Node23 *pinsert_start;

  int found_index = findInsertNode(new_key, child_indecies, pinsert_start);

  if (found_index != Node23::NotFoundIndex) { // new_key already exists. Overwrite its associated value with the new value.

       pinsert_start->keys_values[found_index].value = new_value;
       return;  
  }

  // new_key was not found in the tree; therefore we know pinsert_start is a leaf.
  if (pinsert_start->isThreeNode()) { 
    
      // Converts pinsert_start from a 3-node to a 2-node.
      split(pinsert_start, new_key, new_value, child_indecies, std::unique_ptr<Node23>{nullptr}); 

  } else { // else we have room to insert new_new_key/new_value into leaf node.
      
     pinsert_start->insertKeyInLeaf(new_key, new_value);
  }
}
/*
 Requires:
 ========= 
 1. new_key is the new key to be inserted.  
 2. pinsert_start will be the leaf node where insertion should start, if new_key is not found in the tree; otherwise, it will be the node where new_key was found.
 3. descent_indecies is a stack<int> that will hold the child branches taken descending to pinsert_start

 Promises:
 =========
 1. Returns the index into pinsert_start->keys_values[] such that, if new_key already exists in the tree
    new_key == pinsert_start->keys[found_index]. However, if new_key is not in the tree, then the return value is Node23::NotFoundIndex.
 2. pinsert_start will be the node where new_key was found, if it already exists in the tree; otherwise, it will be the leaf node where insertion should
    begin.
 3. descent_indecies will hold the child branches take to descend to pinsert_start.

 */

template<class Key, class Value> int tree23<Key, Value>::findInsertNode(Key new_key, std::stack<int>& child_indecies, \
                                                           typename tree23<Key, Value>::Node23 *&pinsert_start) const noexcept
{
   int found_index = Node23::NotFoundIndex;

   pinsert_start = root.get();

   while(1) { // Search for new_key until found or if we search a leaf node and didn't find the key.
   
      int child_index; 
      
      bool found = pinsert_start->NodeDescentSearch(new_key, found_index, child_index); 
   
      if (found || pinsert_start->isLeaf()) {
          break;  
   
      } else { 
          
         child_indecies.push(child_index); // remember which child node branch we took. Note: If the node is a leaf, nothing will be pushed onto the stack. 
         
         pinsert_start = pinsert_start->children[child_index].get();
      }
   }

   return found_index;
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
Parameters:

1. pnode is the 3-node that needs to be split into two 2-nodes.

2. child_index is only used during recursive calls to split(). child_index is the index in pnode->children[] such that 

    pnode->children[child_index] = the prior 3-node that was split in the prior invocation of split.

(It now is a 2-node holding the smallest value of its paralell 4-node, node4.keys_values[0].key).
 
2. heap_2node is either a Node23 rvalue that holds nullptr or a newly allocated 2-node created in the prior call to split(). On the initial call it
   heap_2node is only used on recursive calls to split()

Algorithm overview:

split() is a recursive method. It has two base cases that terminate the recursions: 1.) when a 2-node parent is found, or when pnode is the root, which
causes the tree to grow upward one level.  

<TODO: rewrite these comments below which are sort of rambling>
This method splits a 3-node by creating a 4-node on the stack. It holds both the 3-node's keys and values and the new key and value inserted. If
pnode is a leaf, node4's children are all nullptrs. If pnode is an internal node, we pop its child index from the child_indecies stack<in>. "child
index" is the index such that 
  
    pnode->children[child_index].get() == "the previous level's 3-node that was just processed by split into which a new key and value were also
                                            inserted."
   
After creating the 4-node, we "split" it into two 2-nodes:  

  1. We convert the 3-node pnode into a 2-node holding only node4.keys_values[0].key and node4.keys_values[0].value, and whose children were the
     two left most children of node4.

  2. We create a new 2-node on the heap holding node4.keys_values[2].key and node4.keys_values[2].value, and whose children were the two right most
     children of node4.

Next, we attempt to insert keys_values[1].key, the middle value, in the parent. If the parent is a 2-node, this works: we convert it to a 3-node and connect the heap_2node child,
and we are done. If the parent is a 3-node, we recurse, which may ulimately result in splitting the root, creating a new node above the current root.

 split(Node23 *pnode, Key new_key, const Value& new_value)
 {
     if (pnode is the root) 
        Create a new node p and insert the item
        return
     else 
        Let p be the parent of pnode
     
       Create a 4-node node4 on the stack representing the inserted key and value and transferred ownership of pnode's three children. 
 
       From the 4-node create two nodes n1 and n2, so that p is the parent.
 
       Downsize pnode from a 3-node to a 2-node. It will hold the smallest key in node4.
       Give n2 the item  wth the large value in node4. n2 is allocated on the heap.
 
       Connect the two left most children of node4 to n1
       Connect the two right most children of node4 to n2
     
       Move the item with the middle value in p4node up to p
 
       if (p now has three items) {
 
          split(p);
       }
 }
*/
template<class Key, class Value> void tree23<Key, Value>::split(Node23 *pnode, Key new_key, const Value& new_value, std::stack<int>& child_indecies, std::unique_ptr<Node23> heap_2node)  noexcept
{
  // get the actual parent              
  Node23 *parent = pnode->parent;
  
  // Debug only next line:
  Node23 *pheap_2node = heap_2node.get(); // debug-only line?
      
  // Create 4-node on stack that will aid in splitting the 3-node that receives new_key (and new_value).
  Node4 node4;

  int child_index;
 
  if (pnode->isLeaf()) { // pnode->isLeaf() if and only if heap_2node == nullptr

      node4 = Node4{pnode, new_key, new_value}; // We construct a 4-node from the 3-node leaf. The = invokes move assignment--right? 

  } else { // It is an internal leaf, so we need to get its child_index such that:
           // pnode == pnode->parent->children[child_index]. TODO: Is this statement correct--double check this fact!!!!

      child_index = child_indecies.top();
      child_indecies.pop();

      node4 = Node4{pnode, new_key, new_value, child_index, std::move(heap_2node)}; 
  }
   
  /* 
     Next we check if the 3-node, pnode, is the root. If so, we create a new top level Node23 and make it the new root.
     If not, we use node4 to 
    
      1.) We downsize pnode to a 2-node, holding the smallest value in the 4-node, node4.keys_values[0], and we connect the two left most
          children of node4 as the two children of pnode. The code to do this is
    
             pnode->convertTo2Node(node4); 
    
      2.) We allocate a new Node23 2-node on the heap that will hold the largest value in node4, nod4.keys_values[2]. Its two children will be the
          two right most children of node4. The code to do this this is the Node23 constructor that takes a Node4 reference as input.

          std::unique_ptr<Node23> larger_2node{std::make_unique<Node23>(node4)}; 
   */
  pnode->convertTo2Node(std::move(node4)); 

  // 2. Create an entirely new 2-node that contains the largest value in node4, node4.keys_values[2].key, and whose children are the two right most children of node4
  //    the children of pnode. This is what the Node23 constructor that takes a Node4 does.
  std::unique_ptr<Node23> larger_2node{std::make_unique<Node23>(node4)}; 
  
  if (pnode == root.get()) {

       // We pass node4.keys_values[1].key and node4.keys_values[1].value as the key and value for the new root.
       // pnode == root.get(), and pnode is now a 2-node. larger_2node is the 2-node holding node4.keys_values[2].key.
        
       CreateNewRoot(node4.keys_values[1].key, node4.keys_values[1].value, std::move(root), std::move(larger_2node)); 

  } else if (parent->isTwoNode()) { // Since pnode is not the root, its parent is an internal node. If it, too, is a 2-node,

      // we convert it to a 3-node by inserting the middle value into the parent, and passing it the larger 2-node, which it will adopt.
      parent->convertTo3Node(node4.keys_values[1].key, node4.keys_values[1].value, std::move(larger_2node));

  } else { // parent is a 3-node, so we recurse.

     // parent now has three items, so we can't insert the middle item. We recurse to split it.
     split(parent, node4.keys_values[1].key, node4.keys_values[1].value, child_indecies, std::move(larger_2node)); 
  } 

  return;
}

/*
  Requires: currentRoot is the root. tree::root was moved to the parameter currentRoot by the caller. currentRoot has been down sized to a 2-node.
            rightChild is a heap allocated 2-node unique_ptr<Node23> holding the largest key (and its associated value) in the formerly 3-node root.   
            new_key is such that pCurrentRoot->keys_values[0].key < new_key < leftChild->keys_values[0].key, and will be added above the current root, growing the tree upward
            one level. 
  Promises: A new root is added growing the tree upward one level.
 */
template<class Key, class Value> void tree23<Key, Value>::CreateNewRoot(Key new_key, const Value& new_value, std::unique_ptr<Node23> currentRoot, \
                  std::unique_ptr<Node23> rightChild) noexcept
{
   // 1. create new root node.
   std::unique_ptr<Node23> new_root = std::make_unique<Node23>(new_key, new_value);

   // 2. Release the current root, so that it does not inadvertanely get deleted during a move(). It will be the leftChild or new_root.
   std::unique_ptr<Node23> leftChild { currentRoot.release() };
 
   // 3. connect left and right children.
   new_root->connectChild(0, std::move(leftChild));
   new_root->connectChild(1, std::move(rightChild));

   // 4. Make new_root the actual root.
   root = std::move(new_root);

   // 5. increase tree's height
   ++height;
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

      keys_values[0].key = new_key;
      keys_values[0].value = new_value; 

  } else {

      keys_values[1].key = new_key;
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

       keys_values[0].key = key;
       keys_values[0].value = new_value;

   } else { // key > keys_values[0].key

       keys_values[1].key = key;
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

       keys_values[0].key = key;
       keys_values[0].value = std::move(new_value);

   } else { // key > keys_values[0].key

       keys_values[1].key = key;
       keys_values[1].value = std::move(new_value);  
   }

   totalItems = Node23::ThreeNodeItems; 
   return;
}
/*
 remove pseudo code:

 If key does not exist
    return
 else
    save its node pointer and the index within keys[].

 If it is an internal node
    swap the key with the key's in order successor's, which will always be in a leaf. Thus deletion always starts at a leaf node.
    
 delete swapped key from leaf node 

 if leaf is now empty
     fixTree(empty_leaf_node)  

 Comment: A a stack of indecies records the route taken descending.
 */
template<class Key, class Value> void tree23<Key, Value>::remove(Key key)
{
  if (isEmpty()) {

     return;
  }

  std::stack<int> descent_indecies; 

  int found_index = Node23::NotFoundIndex;
  
  Node23 *premove_start = findRemovalStartNode(key, descent_indecies, found_index);

  if (premove_start == nullptr) return;

  Node23 *pLeaf;
 
  if (!premove_start->isLeaf()) { // If it is an internal node...

      // ...get its in order successor, which will be keys_values[0].key of a leaf node.
      //--pLeaf = getSuccessor(premove_start, found_index, descent_indecies); 
      pLeaf = remove_getSuccessor(premove_start, found_index, descent_indecies); 
          
      /*  
       * Swap the internal key( and its associated value) with its in order successor key and value. The in order successor is always in
       * keys_values[0].key.
       */
      std::swap(premove_start->keys_values[found_index], pLeaf->keys_values[0]); 
        
  } else { // ...premove_start is a leaf, and the key is in premove_start->keys[found_index]
      
      pLeaf = premove_start;
  } 
  
  pLeaf->removeLeafKey(key); // remove key from leaf         
  
  // We now have reduced the problem to removing the key (and its value) from a leaf node, pLeaf. 
  if (pLeaf->isEmpty()) { 
      
      fixTree(pLeaf, descent_indecies);  
  }

  return;
}
/*
 * Assume tree is not empty. root is not nullptr.
 */ 
template<class Key, class Value> inline typename tree23<Key, Value>::Node23 *tree23<Key, Value>::findRemovalStartNode(Key key, std::stack<int>& child_indecies,\
                                                                                                                 int& found_index) const noexcept
{
    
  found_index = Node23::NotFoundIndex;
  Node23 *premove_start = root.get();
  
  while(1) { // Search for key until found, or we reach a leaf and it is not found when we simply return.

    int child_index; 
    
    bool found = premove_start->NodeDescentSearch(key, found_index, child_index); 

    if (found) {

        break;  

    } else if (premove_start->isLeaf()) {

        return nullptr;

    } else {  // continue looking...
        
       child_indecies.push(child_index); // ...remembering which child node branch we took.

       premove_start = premove_start->children[child_index].get();
    }
  }  

  return premove_start;
}
/*
 Finds the in order successor of pnode, which must be an internal node.
 
 Input:
 1. pnode must be an internal node.
 2. found_index is the index of the key being removed within pnode->keys[]
 3. child_indecies is the stack of indecies into keys[] tracing the descent to the internal node pnode. 

 Output:
 1. pointer to leaf node of in order successor
 2. child_indecies traces the descent route to the in order successor.
*/

template<class Key, class Value> inline typename tree23<Key, Value>::Node23* tree23<Key, Value>::remove_getSuccessor(Node23 *pnode, int found_index, \
                                                                                                  std::stack<int>& child_indecies) const noexcept
{
  int child_index = found_index + 1;

  child_indecies.push(child_index);

  pnode = pnode->children[child_index].get();

  child_index = 0;

  while(!pnode->isLeaf()) {
       
      child_indecies.push(child_index);
      pnode = pnode->children[child_index].get();
  }
  
  return pnode;
}
/*
 Requires: Called only by a leaf not. 
 */
template<class Key, class Value> inline void tree23<Key, Value>::Node23::removeLeafKey(Key key) noexcept
{
  
  if (isThreeNode() && key == keys_values[0].key) {

      keys_values[0] = std::move(keys_values[1]);   // removes keys_values[0].key
      
  }  // ...otherwise, we don't need to overwrite keys_values[0].key; we just decrease totalItems. 

  --totalItems;
}
/*
 Overview
 ======== 
 fixTree is called when a node has become empty, and the tree needs to be rebalanced. It is initially called when a leaf node becomes empty. It first attempts
 to barrow a key from a 3-node sibling. silbingHasTwoItems() is called to determine if any 3-node sibling exists. If one does, it calls barrowSiblingKey(), which will supply a
 remove a key/value from sibling, and then shift it left or right so that the tree is re-balanced, and the empty node is filled with a key/value.  

 If no adjacent sibling is a 3-node, a key/value from the parent is brought down and merged with a sibling of pnode. Any non-empty children of pnode are moved to the 
 sibling. Upon return, pnode is deleted from the tree by a calling to unique_ptr<Node23>::reset().  

 If the parent of pnode has now become empty (because merge2Nodes was called), a recursive call to fixTree is made.

 Parameters
 ==========
 1. pnode: an empty node, initially a leaf. During recursive calls, pnode is an empty internal 2-node with only one non-nullptr child.  
 2. child_index: The child index in the parent such that 

       pnode->parent->children[child_index] == pnode 
*/

template<class Key, class Value> void tree23<Key, Value>::fixTree(typename tree23<Key, Value>::Node23 *pnode, std::stack<int>& descent_indecies) noexcept
{
  if (pnode == root.get()) {

     // make the merged sibling the root.
     reassignRoot(); // This decrease the tree height--I believe.

     // delete the root and make its only non-empty child the new root.
     return;
  }

  int child_index = descent_indecies.top();
  descent_indecies.pop();

  // case 1. If the empty node has a sibling with two keys, then we can shift keys and barrow a key for pnode from its parent. 
  int sibling_index;

  if (pnode->siblingHasTwoItems(child_index, sibling_index)) { 

      barrowSiblingKey(pnode, child_index, sibling_index);
     
  } else  { // No sibling has two items, so we merge a key/value from pnode's parent with the appropriate sibling. 

      Node23 *parent = pnode->parent;

      // child_index is such that: parent->children[child_index] == pnode

      std::unique_ptr<Node23> node2Delete = mergeNodes(pnode, child_index); 

      node2Delete.reset(); 

       if (parent->isEmpty()) { 

          // recurse. parent is an internal empty 2-node with only one non-nullptr child.
          fixTree(parent, descent_indecies);
      }
  }   
}

template<class Key, class Value> inline void tree23<Key, Value>::reassignRoot() noexcept
{
   // case 1: The root is a leaf
   if (root->isLeaf()){

      root = nullptr; // deletes the memory held by the unique_ptr<Node>.

   } else {
   // case 2: recursive remove() case
   // If the root has a sole non-empty child, make it the new root. unique_ptr's assignment operator will first delete the current empty root
   // node pointer before doing the assignment.
      root = std::move(root->getNonNullChild());  
      root->parent = nullptr;   
   }
   
   if (height > 0 ) {
      --height;
   }
}

/* Parameters 
 * ==========
 * 
 *  1. node is the empty 2-node that needs a key/value.
 *  2. child_index is such that node->parent->children[child_index] == node.
 *  3. silbing_index is the sibling with two keys/values from which we barrow key/value.
 *

    Comments:
    ---------
    If the node parameter is not a leaf node, then barrowSiblingKey() was called during a recursive call to fixTree(), and node is an internal node.
    A recursive call only occurs after a 2-node is merged with one of its 2-node children. Its other child was deleted from the tree in the previous
    call to fixTree.  This means node has only one non-nullptr child, namely, the merged node created by merge2Nodes() in the prior call to fixTree(). 

    Recursion can be checked by testing if node is a leaf node (as remove always starts with a leaf). If it is an internal node, this is a recursive call,
    and we determine which child--0 or 1--is the non-nullptr child of node. This is done in the shiftChildrenXXX() routines. 
 *
 */                                                              
template<class Key, class Value> void tree23<Key, Value>::barrowSiblingKey(Node23 *node, int child_index, int sibling_index) noexcept
{
  Node23 *parent = node->parent; 
  Node23 *sibling = parent->children[sibling_index].get();

 // If node is an internal node, this implies fixTree() has recursed, and node will have only subtree, one non-nullptr child.
 if (parent->isTwoNode()) {

     // barrowSiblingKey keys and their associated values. 
     node->keys_values[0] = std::move(parent->keys_values[0]);  

     if (sibling_index < child_index) {  // sibling is to left of node

        parent->keys_values[0] = std::move(sibling->keys_values[1]);

     } else { // sibling is to the right

        parent->keys_values[0] = std::move(sibling->keys_values[0]);

        sibling->keys_values[0] = std::move(sibling->keys_values[1]);
     } 

     node->totalItems = Node23::TwoNodeItems;
     sibling->totalItems = Node23::TwoNodeItems;

     // Check if this is a recursive case or leaf node case. 
     if (node->isLeaf()) return;

     if (sibling_index < child_index) {  // If sibling is to left of node...

         // ...detemine if the left child is the non-nullptr child, make it the right child; otherwise, 
	 // leave it alone.
         if (node->children[0] != nullptr) {
 
            node->connectChild(1, std::move(node->children[0])); // Shift the non-nullptr child, the sole child of node, to be the right child of
	                                                         // node.
         }

         node->connectChild(0, std::move(sibling->children[2])); // Make the left sibling's right child the left child of node. 

     } else { // it is to the right of node

	 // ...determine if the right child of node is the sole child. If so, make it the left child.    
         if (node->children[1] != nullptr) {
		 
            // Shift the non-nullptr child, the sole child of node, to position 0.
            node->connectChild(0, std::move(node->children[1]));
         }   

         node->connectChild(1, std::move(sibling->children[0])); // Make the right sibling's left child the right child of node.

         // Shift its two right most children left one slot.
         sibling->connectChild(0, std::move(sibling->children[1]));
         sibling->connectChild(1, std::move(sibling->children[2]));
     }

  } else { // parent is a 3-node
     /*
        First, determine if there are two hops to the sibling from which we will barrow a key. Two hops can only occurs when child_index is 0, and
       	the sibling_index is 2, or vice versa. 
      */

        Node23 *middleChild = node->parent->children[1].get();

        if (child_index == 0 && sibling_index == 2) {

            // handles two hops left
            node->keys_values[0] = std::move(parent->keys_values[0]);

            node->totalItems = Node23::TwoNodeItems;

            parent->keys_values[0]= std::move(middleChild->keys_values[0]);

            middleChild->keys_values[0] = std::move(parent->keys_values[1]); 

            parent->keys_values[1] = std::move(sibling->keys_values[0]);

            sibling->keys_values[0] = std::move(sibling->keys_values[1]);              

            sibling->totalItems = Node23::TwoNodeItems;

            // Shift the children appropriately below.
            shiftChildrenLeft(node, middleChild, sibling);
            return;

        } else if (child_index == 2 && sibling_index == 0) {

            // handle two hops
            node->keys_values[0] = std::move(parent->keys_values[1]);
            node->totalItems = Node23::TwoNodeItems;

            parent->keys_values[1]= std::move(middleChild->keys_values[0]);
            middleChild->keys_values[0] = std::move(parent->keys_values[0]); 

            parent->keys_values[0] = std::move(sibling->keys_values[1]);

            sibling->totalItems = Node23::TwoNodeItems;

            // Shift the children appropriately below.
            shiftChildrenRight(node, middleChild, sibling);
            return;
        }  
     
     // All one hop causes are handled below, like this one hop example:     
            /* 
              (20,      40)            (30,     40)
             /      |     \    ==>     /     |    \
          Hole   (30,35)   50         20    35     50
            |    /  | \    /\        / \   /  \    /\
            C   29 32  37 60 70     C  29 32   37 60 70

          where C is the sole child */

     switch (sibling_index) { 

        case 0:
           
           node->keys_values[0] = std::move(parent->keys_values[0]); 
           parent->keys_values[0] = std::move(sibling->keys_values[1]); 
           sibling->totalItems = Node23::TwoNodeItems;

           shiftChildrenRight(node, sibling);
           break;      

         case 1: // If sibling_index == 1, then child_index == 0 or child_index == 2/ 

           if (child_index == 0) {

               node->keys_values[0] = std::move(parent->keys_values[0]); 
               parent->keys_values[0] = std::move(sibling->keys_values[0]); 
               
               shiftChildrenLeft(node, sibling); 

            } else { // child_index == 2

               node->keys_values[0]= std::move(parent->keys_values[1]);  
               parent->keys_values[1]= std::move(sibling->keys_values[1]); 
               
               shiftChildrenRight(node, sibling); 
            }
           
            sibling->totalItems = Node23::TwoNodeItems;
            break;

         case 2: 

           node->keys_values[0] = std::move(parent->keys_values[1]); 
           parent->keys_values[1] = std::move(sibling->keys_values[0]); 
           sibling->keys_values[0] = std::move(sibling->keys_values[1]);
           sibling->totalItems = Node23::TwoNodeItems;
           
           shiftChildrenLeft(node, sibling); 
           break;
    }
  }

  node->totalItems = Node23::TwoNodeItems; 
  sibling->totalItems = Node23::TwoNodeItems;
  return;
}
/*
 * Does shifting of children left for the one hop case
 * Assumes node and sibling are not leaf nodes. Therefore this is a recurisve call to fixTree()
 */
template<class Key, class Value> void tree23<Key, Value>::shiftChildrenLeft(Node23 *node, Node23 *sibling) noexcept
{
  // If node is a leaf, then return.
  if (node->isLeaf()) return;

  // Determine which child of node is non-nullptr.
  int sole_child = node->children[0] != nullptr ? 0 : 1;

  // If sole_child is 1, then shift it left.
  if (sole_child == 1) {

      node->connectChild(0, std::move(node->children[1]));  
   }

   node->connectChild(1, std::move(sibling->children[0]));  

   sibling->connectChild(0, std::move(sibling->children[1]));  
   sibling->connectChild(1, std::move(sibling->children[2]));  
}
/*
 * Does shifting of children right for the one hop case
 * Assumes node and sibling are not leaf nodes. Therefore this is a recurisve call to fixTree()
 */
template<class Key, class Value> void tree23<Key, Value>::shiftChildrenRight(Node23 *node, Node23 *sibling) noexcept
{
  if (node->isLeaf()) return;

  // Determine which child of node is non-nullptr.
  int sole_child = node->children[0] != nullptr ? 0 : 1;

  if (sole_child == 0) {

     node->connectChild(1, std::move(node->children[0]));  
  }

  node->connectChild(0, std::move(sibling->children[2]));  
}
/*
 * Does shifting of children left for the two hop case
 * Assumes node and sibling are not leaf nodes. Therefore this is a recurisve call to fixTree()
 */
template<class Key, class Value> void tree23<Key, Value>::shiftChildrenLeft(Node23 *node, Node23 *middleChild, Node23 *sibling) noexcept
{
  if (node->isLeaf()) return;

  // Determine which child of node is non-nullptr.
  int sole_child = node->children[0] != nullptr ? 0 : 1;

  // If sole_child is 1, then shift it left.
  if (sole_child == 1) {

      node->connectChild(0, std::move(node->children[1]));  
   }

   node->connectChild(1, std::move(middleChild->children[0]));

   middleChild->connectChild(0, std::move(middleChild->children[1]));
   middleChild->connectChild(1, std::move(sibling->children[0]));

   sibling->connectChild(0, std::move(sibling->children[1]));
   sibling->connectChild(1, std::move(sibling->children[2]));
}
/*
 * Does shifting of children right for the two hop case
 * Assumes node and sibling are not leaf nodes. Therefore this is a recurisve call to fixTree()
 */
template<class Key, class Value> void tree23<Key, Value>::shiftChildrenRight(Node23 *node, Node23 *middleChild, Node23 *sibling) noexcept
{
  if (node->isLeaf()) return;

  // Determine which child of node is non-nullptr.
  int sole_child = node->children[0] != nullptr ? 0 : 1;

  if (sole_child == 0) {

     node->connectChild(1, std::move(node->children[0]));  
  }

  node->connectChild(0, std::move(middleChild->children[1]));

  middleChild->connectChild(1 , std::move(middleChild->children[0]));
  middleChild->connectChild(0 , std::move(sibling->children[2]));
}
/*
 Requires: 
 1. pnode is empty. 
 2. All siblings of pnode are 2-nodes. 
 3. child_index is such that: pnode == pnode->parent->children[child_index] 

 Upon return: Either the parent is empty or the tree is rebalanced tree.
 returns:  the std::unique_ptr<Node23> to be deleted.
  
 */
template<class Key, class Value> std::unique_ptr<typename tree23<Key, Value>::Node23> tree23<Key, Value>::mergeNodes(Node23 *pnode, int child_index) noexcept
{
  if (pnode->parent->isTwoNode()) { 
      /* 
         When the parent is a 2-node, then both pnode's sibling and the parent have one key. We merge the parent's sole key/value with
         pnode's sibling, which is pnode->parent->children[!child_index]. This leaves the parent empty, which we handle recursively by calling
	 fixTree() again. 
       */
       return merge2Nodes(pnode, !child_index); 


  } else { // 

      /* 
       * parent is a 3-node, but has only 2-node children. In this case, we can successfully rebalance the tree. We merge one of the parent keys (and
       * its associated value) with a sibling. This now makes the parent a 2-node. We move the effected children involved appropriately.  We can then
       * safely delete pnode from the tree.
       */

    std::unique_ptr<Node23> node2Delete = merge3NodeWith2Node(pnode, child_index);
    return node2Delete;
  }
}
/*
 Overview
 ========
 
 Parameters:
 1. pnode is empty 2-node. If it is not a leaf node, it has one child, one subtree; the other child, when pnode is an internal node, is nullptr.
 2. child_index is such that pnode->parent->children[child_index] == pnode

 Requires: The pnode->parent is a 3-node, and all pnode's siblings are 2-nodes.

 Promises: Merges one of the keys/values of pnode->parent with one of pnode's 2-node siblings to rebalance the tree. It shifts the children of the
 effected siblings appropriately, transfering ownership of the sole non-nullptr child of pnode, when pnode is an internal node (which only occurs during a recursive call to
 fixTree()). 
 
 */
template<class Key, class Value> std::unique_ptr<typename tree23<Key, Value>::Node23> \
tree23<Key, Value>::merge3NodeWith2Node(Node23 *pnode, int child_index) noexcept
{
    Node23 *parent = pnode->parent;

    // If pnode is a leaf, then all children are nullptrs. soleChild is only needed when pnode is an internal node.
    std::unique_ptr<Node23> soleChild = (!pnode->isLeaf()) ? std::move(pnode->getNonNullChild()) : nullptr; 

    std::unique_ptr<Node23> node2Delete;

    // TODO: In all three cases below, we are only moving the parent's grandchildren. We also need to move the immediate children of the
    // parent.  The parent had three children. It needs to now only have two children. So we must move the 2nd and 3rd child left one 
    // position.
    switch (child_index) {// is the index of node in parent's children. It is not the sibling's index.
         
         case 0: 
	 // pnode->parent->children[0].get() == pnode 
         {
          // First, move children[1]->keys_values[0] to children[1]->keys_values[1], then move parent's key_values[0] into children[1]->keys_values[0].
          parent->children[1]->keys_values[1] = std::move(parent->children[1]->keys_values[0]); 
          parent->children[1]->keys_values[0] = std::move(parent->keys_values[0]);

          parent->keys_values[0] = std::move(parent->keys_values[1]);
          
          parent->children[1]->totalItems = Node23::ThreeNodeItems;
          parent->totalItems = Node23::TwoNodeItems;

          node2Delete = std::move(parent->children[0]); 

          if (!parent->children[1]->isLeaf()) { // We need to shift the 2 right-most children (of the former 3-node) left since their
		                               // parent is now a 2-node.
	       // move children appropriately. This is the recursive case when pnode is an internal node.
               parent->children[1]->connectChild(2, std::move(parent->children[1]->children[1])); 
               parent->children[1]->connectChild(1, std::move(parent->children[1]->children[0])); 
               parent->children[1]->connectChild(0, std::move(soleChild)); 
	  }
          
          // Move the parent's children left one position so the parent has only two children.
          parent->connectChild(0, std::move(parent->children[1]));
          parent->connectChild(1, std::move(parent->children[2]));
         }
         break;  
         
         case 1:
	 // pnode->parent->children[1].get() == pnode 
         {
          // First, move parent's key_values[0] into children[0]->keys_values[1], then move parent's keys_values[1] to keys_values[0].
          parent->children[0]->keys_values[1] = std::move(parent->keys_values[0]);
          parent->keys_values[0] = std::move(parent->keys_values[1]);

          parent->children[0]->totalItems = Node23::ThreeNodeItems;
          parent->totalItems = Node23::TwoNodeItems;

          node2Delete = std::move(parent->children[1]); 

          if (!parent->children[0]->isLeaf()) {// We still need to shift the children[2] to be children[1] because its parent is now a 2-node.		  

	      // This is the recursive case when pnode is an internal node. We move the sole child of the empty node to the parent's first child,
	      // making it the 3rd child. 
              parent->children[0]->connectChild(2, std::move(soleChild)); 
	  }
          // Move the parent's right children one position left, so the parent has only two children.
          parent->connectChild(1, std::move(parent->children[2]));
         } 
         break;  
         
         case 2:
	 // pnode->parent->children[2].get() == pnode 
         {
          // Move parent's key_values[1] into children[1]->keys_values[1].
          parent->children[1]->keys_values[1] = std::move(parent->keys_values[1]);

          parent->children[1]->totalItems = Node23::ThreeNodeItems;
          parent->totalItems = Node23::TwoNodeItems;
    
          node2Delete = std::move(parent->children[2]); 

          if (!parent->children[1]->isLeaf()) {// If it is a leaf, we don't need to shift the existing children of children[1] because there is no "gap"
                                               // to fill.

             // Adopt sole child of pnode.  This is the recursive case when pnode is an internal node.
             parent->children[1]->connectChild(2,  std::move(soleChild)); 
          }
         // We do not need to move any children of the parent. The third child is node2Delete, which the caller will delete, setting
         // the third child to nullptr.
         }
         break;  
    }

    return node2Delete;
}
/*

requires
1. pnode is an empty 2-node, and its parent is a 2-node. 
2. sibling_index is the index of pnode's sibling in parent->children[].

overview:

Its sibling will accept its parent's key and associated value.

promises:
1. the sibling of pnode will accept the parent's key and associated value, and it will adopt, if pnode is not a leaf, the sole non-empty
   child of pnode.
2. parent will become empty, too.
 */
template<class Key, class Value> inline std::unique_ptr<typename tree23<Key, Value>::Node23> tree23<Key, Value>::merge2Nodes(Node23 *pnode, int sibling_index) noexcept
{
  Node23 *parent = pnode->parent;
   
  Node23 *sibling = parent->children[sibling_index].get();

  std::unique_ptr<Node23> node2Delete = std::move(parent->children[!sibling_index]); 

  if (sibling_index == 1) { // sibling is right child.

      sibling->keys_values[1] = std::move(sibling->keys_values[0]);
      sibling->keys_values[0] = std::move(parent->keys_values[0]);

  } else { // sibling is the left child.

      sibling->keys_values[1] = std::move(parent->keys_values[0]);
  } 

  parent->totalItems = 0;
  
  sibling->totalItems = Node23::ThreeNodeItems;

  if (sibling->isLeaf()) {

      return node2Delete; 
  } 

  // Recursive case: we adopt the sole child of pnode. The other child was deleted from the tree and so sibling->children[!child_index] == nullptr.
  std::unique_ptr<Node23>& nonemptyChild = pnode->getNonNullChild();

  // Is sibling to the left? 
  if (sibling_index == 0) {
        
      sibling->connectChild(2, std::move(nonemptyChild));    

  } else { // no, it is to the right.

      sibling->connectChild(2, std::move(sibling->children[1]));
      sibling->connectChild(1, std::move(sibling->children[0]));
      sibling->connectChild(0, std::move(nonemptyChild));    
  }

  return node2Delete;
}
/* rvalue version  TODO: finish later
template<class Key, class Value> void tree23<Key, Value>::insert(Key key, Value&& value)
{
  int index;
  if (root == nullptr) {
      return;
  }
  Node23 *pleaf = find(key, index, root.get()); // How do we determine if it wasn't in tree? Should I return pair<bool, Node23 *>.
  if (!pleaf->isLeaf()) {
       return;  // already exists.
  }
  // Further test that it is not in the leaf
}
*/
#endif
