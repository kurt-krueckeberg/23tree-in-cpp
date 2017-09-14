#ifndef tree23_h_18932492374
#define tree23_h_18932492374

#include <initializer_list>
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
#include "level-order-display.h"

template<class Key, class Value> class tree23; // This forward declaration... 

//...is required by these friend functions
template<class Key, class Value> std::ostream& operator<<(std::ostream& ostr, const typename tree23<Key, Value>::Node& node23); 
template<class Key, class Value> std::ostream& operator<<(std::ostream& ostr, const typename tree23<Key, Value>::Node4& node4); 

template<class Key, class Value> class tree23 {
    
  private:

  union KeyValue { // A union is used to hold to two types of pairs, one of which (nc_pair) has a non-const Key; the other has a const Key.
      friend class tree23<Key, Value>;
 
      std::pair<Key, Value>        nc_pair;  // ...this eliminates constantly casting of const_cast<Key>(p.first) = some_noconst_key;
      std::pair<const Key, Value>  const_pair;  // but always return this member of the union.
      
      KeyValue() {} 
      KeyValue(Key key, const Value& value) : nc_pair{key, value} {}
      
      KeyValue(const KeyValue& lhs) : nc_pair{lhs.nc_pair} {}
      
      KeyValue(Key k, Value&& v) : nc_pair{k, std::move(v)} {} 

      KeyValue(KeyValue&& lhs) :  nc_pair{move(lhs.nc_pair)} {}//nc_pair{lhs.key(), std::move(lhs.value())} {}

      KeyValue& operator=(const KeyValue& lhs);  
      KeyValue& operator=(KeyValue&& lhs); 
 
     constexpr Key&  key()  { return nc_pair.first; }
     constexpr const Key& key() const { return const_pair.first; }

     constexpr Value&  value()  { return nc_pair.second; }
     constexpr const Value& value() const { return const_pair.second; }
     
     friend std::ostream& operator<<(std::ostream& ostr, const KeyValue& key_value)
     {
         ostr << "{" << key_value.key() << ',' <<  key_value.value() <<  "}, ";
	 return ostr;
     }
  };

  class Node4;   // Fwd declaration 

  public:
  
   // The tree's heap-allocated Node nodes are managed by std::unique_ptr<Node>s.
    class Node {

        friend class tree23<Key, Value>;             

     public:   
        Node(Key key, const Value& value, Node *ptr2parent=nullptr);

        // Used when tree23::emplace(arg...) is called.
        // TODO: Is this actually used--check to confirm.
        template<class... Args> Node(Key key, Args... arg, Node *ptr2parent=nullptr);
        
        Node(Node4&);

        // We disallow copy construction and assignment...
        Node(const Node&) = delete; 
        Node& operator=(const Node&) = delete; 

        Node(Node&&); // ...but we allow move assignment and move construction.
        Node& operator=(Node&&) noexcept;

        // Constructor for just coping the keys and values. 
	Node(const std::array<KeyValue, 2>& lhs_keys_values, Node * const lhs_parent, int lhs_totalItems) noexcept; 

        constexpr bool isLeaf() const noexcept { return (children[0] == nullptr && children[1] == nullptr) ? true : false; } 
        constexpr bool isEmpty() const noexcept { return (totalItems == 0) ? true : false; } 

        constexpr bool isThreeNode() const noexcept { return (totalItems == Node::ThreeNode) ? true : false; }
        constexpr bool isTwoNode() const noexcept { return (totalItems == Node::TwoNode) ? true : false; }
        
        constexpr int getTotalItems() const noexcept { return totalItems; }

        constexpr int getChildCount() const noexcept { return totalItems + 1; }

        constexpr const Node *getRightMostChild() const noexcept { return children[getTotalItems()].get(); }

        constexpr std::unique_ptr<Node>& getNonNullChild() noexcept;

        constexpr int getSoleChildIndex() const noexcept; // called from subroutine's of tree23<Key,Value>::remove(Key)

	std::ostream& test_parent_ptr(std::ostream& ostr, const Node *root) const noexcept;

        bool siblingHasTwoItems(int child_index, int& sibling_index) const noexcept;

        std::ostream& test_remove_invariant(std::ostream& ostr) const noexcept; 

        std::ostream& test_2node_invariant(std::ostream& ostr, const Node *root) const noexcept;

        std::ostream& test_3node_invariant(std::ostream& ostr, const Node *root) const noexcept;

        std::ostream& debug_print(std::ostream& ostr, bool show_addresses=false) const;

        std::ostream& print(std::ostream& ostr) const noexcept;
   
        friend std::ostream& operator<<(std::ostream& ostr, const Node& node23)
        { 
	   return node23.print(ostr);
        }

        private:

           Node *parent;

           static const int TwoNode = 1;
           static const int TwoNodeChildren = 2;
           static const int ThreeNode = 2;
           static const int ThreeNodeChildren = 3;
           static const int NotFoundIndex = -1;
               
           std::array<KeyValue, 2> keys_values;

           std::array<std::unique_ptr<Node>, 3> children;

           void move_keys_values(std::array<std::unique_ptr<KeyValue>, 2>&& lhs);

           void move_children(std::array<std::unique_ptr<Node>, 3>&& lhs);

           void removeLeafKey(Key key) noexcept;
       
           int totalItems; // set to either Node::TwoNode or Node::ThreeNode

           void connectChild(int childIndex, std::unique_ptr<Node> child)  noexcept;
           void connectChild(std::unique_ptr<Node>& dest, std::unique_ptr<Node> src)  noexcept;
          
           void convertTo2Node(Node4&& node4) noexcept; 

           void convertTo3Node(Key key, const Value& value, std::unique_ptr<Node> pnode23) noexcept; 

           bool NodeDescentSearch(Key value, int& index, Node *next) noexcept;          // called during find()  
           bool NodeDescentSearch(Key value, int& index, int& next_child_index) noexcept; // called during insert()

           void insertKeyInLeaf(Key key, const Value& value);
           void insertKeyInLeaf(Key key, Value&& new_value);
      }; 
      
  private: 
    class Node4 { // Class Node4 is only used to aid insert()

       // Always hold three keys and four children. 
      friend class tree23<Key, Value>; 
     
      private:
         std::array<KeyValue, 3> keys_values;

         // Takes ownership of four 23-nodes 
         std::array<std::unique_ptr<Node>, 4> children; 

         Node *parent; // Set to the parent of the 3-node passed to its constructor 

         static const int FourNodeItems = 3;
         static const int FourNodeChildren = 4;

         void connectChild(int childIndex, std::unique_ptr<Node> child)  noexcept;
                      
    public: 
        Node4() noexcept {}

        // Constructor that takes an internal 3-node 
        Node4(Node *threeNode, Key new_key, const Value& value, int child_index, std::unique_ptr<Node> heap_2node) noexcept;

        // Constructor for a leaf 3-node, all child pointers will be zero. 
        Node4(Node *p3node, Key new_key, const Value& new_value) noexcept;

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

    std::unique_ptr<Node> root; 

    int height;   // adjusted only by insert() and remove()

    // Subroutines called by insert()
    int findInsertNode(Key new_key, std::stack<int>& descent_indecies, Node *&pinsert_start) const noexcept;

    void CreateNewRoot(Key new_key, const Value& new_value, std::unique_ptr<Node> leftChild, std::unique_ptr<Node> rightChild) noexcept;  
   
    void CreateRoot(Key key, const Value& value) noexcept;

    template<class... Args> void EmplaceRoot(Key key, Args&&... arg) noexcept;

    void split(Node *current, std::stack<int>& child_indecies, std::unique_ptr<Node> heap_2node, \
               Key new_key, const Value& new_value) noexcept;
    /*
     Prospective method:

    template<class... Args> void split(Node *current,, std::stack<int>& child_indecies, \
       std::unique_ptr<Node> heap_2node, Key new_key, Args&&...args) noexcept;
     */

    // Subroutines called by remove()
    Node* findRemovalStartNode(Key key, std::stack<int>& child_indecies, int& found_index) const noexcept;

    Node *getSuccessor(Node *pnode, int found_index, std::stack<int>& child_indecies) const noexcept;

    void fixTree(Node *pnode, std::stack<int>& child_indecies) noexcept;

    void reassignRoot() noexcept;

    void barrowSiblingKey(Node *pnode, int child_index, int sibling_index) noexcept;
 
    // Subroutines called by barrowSiblingKey()
    void shiftChildrenRight(Node *node, Node *sibling) noexcept;
    void shiftChildrenRight(Node *node, Node *middleChild, Node *sibling) noexcept;

    void shiftChildrenLeft(Node *node, Node *sibling) noexcept;
    void shiftChildrenLeft(Node *node, Node *middleChild, Node *sibling) noexcept;

    std::unique_ptr<Node> merge2Nodes(Node *pnode, int child_index) noexcept;
    std::unique_ptr<Node> merge3NodeWith2Node(Node *pnode, int child_index) noexcept;

    void shiftChildren(Node *node, Node *sibling, int node_index, int sibling_index) noexcept;
     
    template<typename Functor> void DoInOrderTraverse(Functor f, const std::unique_ptr<Node>& root) const noexcept;

    /* These methods work but have been commented out
    template<typename Functor> void DoPostOrderTraverse(Functor f,  const std::unique_ptr<Node>& root) const noexcept;
    template<typename Functor> void DoPreOrderTraverse(Functor f, const std::unique_ptr<Node>& root) const noexcept;
    */  

   // Called by copy constructor and copy assignment operators, respectively.
   void CloneTree(const std::unique_ptr<Node>& Node2Copy, std::unique_ptr<Node>& NodeCopy, const Node * parent) noexcept;
   void DestroyTree(std::unique_ptr<Node> &root) noexcept; 

  public:
    // Container typedef's used by STL.

    using value_type      = std::pair<const Key, Value>; 
    using difference_type = long int;
    using pointer         = value_type*; 
    using reference       = value_type&; 
    using node_type       = Node; 

   /*  enum iterator_position represents one of the three possible finite states: 

     1. end --  the logical state of one-past the last, largest key/value in the tree. When the iterator is at the 'end' state, the value
        of current and key_index will always be the same: the last, largest key/value.  

     2. beg -- the logical state representing the first element.

     3. in_between -- the state of being in-between beg and end: !beg && !end
    */
                                
    enum class iterator_position {beg, in_between, end}; 

    class iterator : public std::iterator<std::bidirectional_iterator_tag, typename tree23<Key, Value>::value_type> { 
                                                 
       friend class tree23<Key, Value>;   

      private:
         tree23<Key, Value>& tree; 

         const typename tree23<Key, Value>::Node *current;

         /*
         Relationship of iterator_position to key_index and vice versa:
    
          key_index will be zero if and only if state is beg. 
          key_index will be (current->totalItems - 1) if and only if state is end
          */
         int key_index;  

         iterator_position position;

         void initialize(iterator_position pos); // reuseable constructor code. 

         int getChildIndex(const typename tree23<Key, Value>::Node *p) const noexcept;

         std::pair<const Node *, int> getSuccessor(const Node *current, int key_index) const noexcept;

         std::pair<const Node *, int> getInternalNodeSuccessor(const typename tree23<Key, Value>::Node *pnode, int index_of_key) const noexcept;

         std::pair<const typename tree23<Key, Value>::Node *, int>  getLeafNodeSuccessor(const typename tree23<Key, Value>::Node *, int) const noexcept;

         std::pair<const Node *, int> getPredecessor(const Node *current, int key_index) const noexcept;

         std::pair<const Node *, int> getInternalNodePredecessor(const typename tree23<Key, Value>::Node *pnode, int index) const noexcept;

         std::pair<const Node *, int>  getLeafNodePredecessor(const typename tree23<Key, Value>::Node *p, int index) const noexcept;

         std::pair<const typename tree23<Key, Value>::Node *, int> findLeftChildAncestor() noexcept;

         void seekToSmallest();    
         void seekToLargest();    

         iterator& increment() noexcept; 

         iterator& decrement() noexcept;

      public:

         explicit iterator(tree23<Key, Value>&); 

         iterator(tree23<Key, Value>& lhs, tree23<Key, Value>::iterator_position);  

         iterator(const iterator& lhs); // What does explicit do?

         iterator(iterator&& lhs); 
 
         bool operator==(const iterator& lhs) const;
         constexpr bool operator!=(const iterator& lhs) const { return !operator==(lhs); }

         constexpr reference dereference() noexcept { return const_cast<std::pair<const Key, Value>&>(current->keys_values[key_index].const_pair); } 

         constexpr const std::pair<const Key, Value>& dereference() const noexcept { \
                         return const_cast<const std::pair<const Key, Value>&>( current->keys_values[key_index].const_pair);} 

         iterator& operator++() noexcept; 
         iterator operator++(int) noexcept;

         iterator& operator--() noexcept;
         iterator operator--(int) noexcept;
         
         std::pair<const Key, Value>& operator*() noexcept { return dereference(); } 

         const std::pair<const Key, Value>& operator*() const noexcept { return dereference(); }
         
         typename tree23<Key, Value>::KeyValue *operator->() noexcept;
    };

    class const_iterator : public std::iterator<std::bidirectional_iterator_tag, const value_type> {

      private:
        iterator iter; 
      public:
         
         explicit const_iterator(const tree23<Key, Value>& lhs);

         const_iterator(const tree23<Key, Value>& lhs, iterator_position pos); 

         const_iterator(const const_iterator& lhs);
         const_iterator(const_iterator&& lhs); 
         const_iterator(const iterator& lhs);

         bool operator==(const const_iterator& lhs) const;
         bool operator!=(const const_iterator& lhs) const;
         
         const_iterator& operator++() noexcept;
         const_iterator operator++(int) noexcept;
         const_iterator& operator--() noexcept;
         const_iterator operator--(int) noexcept;

         const std::pair<const Key,Value>&  operator*() const noexcept 
         {
           return iter.dereference(); 
         } 

         const std::pair<const Key, Value> *operator->() const noexcept { return &this->operator*(); } 
    };

    iterator begin() noexcept;  
    iterator end() noexcept;  
  
    const_iterator begin() const noexcept;  
    const_iterator end() const noexcept;  
  
    using  reverse_iterator       = std::reverse_iterator<typename tree23<Key, Value>::iterator>; 
    using  const_reverse_iterator = std::reverse_iterator<typename tree23<Key, Value>::const_iterator>;

    reverse_iterator rbegin() noexcept;  
    reverse_iterator rend() noexcept;  
 
    const_reverse_iterator rbegin() const noexcept;  
    const_reverse_iterator rend() const noexcept;  
     
    tree23() noexcept;

    tree23(std::initializer_list<value_type> list); 

    void test_invariant() const noexcept;

    std::ostream& test_height(std::ostream& ostr) const noexcept;

    std::string test_invariant(const Node& p) const noexcept; 

    tree23(const tree23&) noexcept; 

    tree23& operator=(const tree23&) noexcept; 

    tree23(tree23&&) noexcept; // move constructor

    tree23& operator=(tree23&&) noexcept; // move assignemtn

    int getHeight() const noexcept;
    void insert(Key key, const Value& value);
    bool isEmpty() const noexcept;

    void printlevelOrder(std::ostream&) const noexcept;
    void printInOrder(std::ostream& ostr) const noexcept;   

    // TODO: Should emplace() return an iterator?
    template<class... Args> void emplace(Key key, Args&&... args); // <-- Implementation not complete.
       
    bool find(Key key) const noexcept;

    const Value& operator[](Key key) const;

    Value& operator[](Key key);

    void remove(Key key);

    // Breadth-first traversal
    template<class Functor> void levelOrderTraverse(Functor f) const noexcept;

    // Depth-first traversals
    template<typename Functor> void inOrderTraverse(Functor f) const noexcept;
};


/*
  Constructs a new 2-node from a Node4: its key will be the node4.keys_values[2].key, largest key in node4, and its associate value. 
  Its children become the former the two tight most children of node4. Their ownership is transferred to the 2-node.
 */
template<class Key, class Value> tree23<Key, Value>::Node::Node(Node4& node4) : totalItems{Node::TwoNode}, parent{node4.parent}
{
  keys_values[0] = std::move(node4.keys_values[2]); // Prefer move() to default copy assignment.

  connectChild(0, std::move(node4.children[2]));
  connectChild(1, std::move(node4.children[3]));
}

/*
  Constructs a new 2-node that is a leaf node; i.e., its children are nullptr.
 */
template<class Key, class Value> tree23<Key, Value>::Node::Node(Key key, const Value& value, Node *ptr2parent) : \
          parent{ptr2parent}, totalItems{Node::TwoNode}
{
  keys_values[0].key() = key;
  keys_values[0].value() = value;
 
  for(auto& child : children) {

       child = nullptr; 
  } 
}

template<class Key, class Value> template<class... Args>  tree23<Key, Value>::Node::Node(Key key, Args... arg, Node *ptr2parent) : \
          parent{ptr2parent}, totalItems{Node::TwoNode}
{
  keys_values[0].key() = key;

  void *location = &keys_values[0].value();

  new(location)  Value(std::forward<Args>(arg)...);
 
  for(auto& child : children) {

       child = nullptr; 
  } 
}
/*
 "this" must be 2-node with only one non-nullptr child
 */
template<class Key, class Value> inline constexpr std::unique_ptr<typename tree23<Key, Value>::Node>& tree23<Key, Value>::Node::getNonNullChild() noexcept
{
  return (children[0] == nullptr) ?  children[1] : children[0];
}

/*
 Only called from tree23<Key,Value>::remove(Key kye)'s subroutines when a 2-node being deleted briefly has only one non-nullptr child.
 */
template<class Key, class Value> inline constexpr int tree23<Key, Value>::Node::getSoleChildIndex() const noexcept
{
  return (children[0] != nullptr) ? 0 : 1; 
}

template<class Key, class Value> inline void tree23<Key, Value>::Node::move_keys_values(std::array<std::unique_ptr<KeyValue>, 2>&& lhs)
{
  for (auto i = 0; i < totalItems; ++i) {

     keys_values[i] = std::move(lhs.keys_values[i]); 
  }
}

template<class Key, class Value> inline void tree23<Key, Value>::Node::move_children(std::array<std::unique_ptr<Node>, 3>&& lhs)
{
  for (auto i = 0; i < getChildCount(); ++i) {

     connectChild(i, std::move(lhs[i]));
  }
}
/*
 * This ctor is used by Clone Tree. Does the default ctor for
 *
     std::array<Node, 3> children
  */     
template<class Key, class Value> inline tree23<Key, Value>::Node::Node(const std::array<KeyValue, 2>& lhs_keys_values,\
	       	Node *const lhs_parent, int lhs_totalItems) noexcept : keys_values{lhs_keys_values}, parent{lhs_parent}, totalItems{lhs_totalItems}
{
  // we don't copy the children.   
}

// move constructor
template<class Key, class Value> tree23<Key, Value>::Node::Node(Node&& node23) : parent{node23.parent}, totalItems{node23.totalItems}
{
  move_keys_values(node23);
  
  move_children(node23); 
}
// move assignment operator
template<class Key, class Value> typename tree23<Key, Value>::Node& tree23<Key, Value>::Node::operator=(Node&& node23) noexcept
{
  if (this == &node23) {

       return *this;
  }

  parent = node23.parent;
  totalItems = node23.totalItems;

  move_keys_values(node23);

  move_children(node23); 

}

template<class Key, class Value> inline std::ostream& tree23<Key, Value>::Node::test_parent_ptr(std::ostream& ostr, const Node *root) const noexcept
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

template<class Key, class Value> std::ostream& tree23<Key, Value>::Node::test_remove_invariant(std::ostream& ostr) const noexcept
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

template<class Key, class Value> std::ostream& tree23<Key, Value>::Node::test_2node_invariant(std::ostream& ostr, const Node *root) const noexcept
{
 //  test parent pointer	
  test_parent_ptr(ostr, root);
	 
  if (isLeaf()) return ostr;

  // check ordering of children's keys with respect to parent. 
  for (int child_index = 0; child_index < Node::TwoNodeChildren; ++child_index) {

       if (children[child_index] == nullptr) {
     
            ostr << "error: children[" << child_index << "] is nullptr\n";
            continue;
       } 

       for (auto i = 0; i < children[child_index]->totalItems; ++i) {
          
           switch (child_index) {

             case 0:

              if (children[0]->keys_values[i].key() >= keys_values[0].key()) { // If any are greater than or equal to keys_values.keys[0], then it is an error.
              
                 ostr << "error: children[0]->keys_values[" << i << "].key() = " << children[0]->keys_values[i].key() << " is not less than " << keys_values[0].key() << ".\n";
              }  

              break;

              case 1:

                if (children[1]->keys_values[i].key() <= keys_values[0].key()) { // are any less than or equal to keys_values.keys[0], then it is an error.
          
                   ostr << "error: children[1]->keys_values[" << i << "].key()= " << children[1]->keys_values[i].key() << " is not greater than " << keys_values[0].key() << ".\n";
                }

                break;
  
              default:
                ostr << "error: totalItems = " << totalItems << ".\n";
                break;

          } // end switch 
       }  // end inner for    
  } // end outer for
          
  const Node *child; 

  // test children's parent point. 
  for (auto i = 0; i < TwoNodeChildren; ++i) {

       if (children[i] == nullptr) continue; // skip if nullptr 
      
       child = children[i].get();   
       
       if (child->parent != this)	 {

            ostr << "children[" << i << "]->parent does not point to 'this', which is " << this << ").";
       } 
  }
}

template<class Key, class Value> std::ostream& tree23<Key, Value>::Node::test_3node_invariant(std::ostream& ostr, const Node *root) const noexcept
{
  //  test parent pointer	
  test_parent_ptr(ostr, root);

  // Test keys ordering
  if (keys_values[0].key() >= keys_values[1].key() ) {

      ostr <<  keys_values[0].key() << " is greater than " <<keys_values[1].key();
  }

  if (isLeaf()) return ostr; 

  for (int child_index = 0; child_index < Node::ThreeNodeChildren; ++child_index) {

     if (children[child_index] == nullptr) {
   
          ostr << "error: children[" << child_index << "] is nullptr\n";
          continue;
     }

    for (auto i = 0; i < children[child_index]->totalItems; ++i) {

      switch (child_index) {

       case 0:  
       // Test that all left child's keys are less than node's keys_values.key()[0]
     
           if (children[0]->keys_values[i].key() >= keys_values[0].key() ) { // If any are greater than or equal to keys_values.key()[0], it is an error
     
              // problem
              ostr << "error: children[0]->keys_values[" << i << "].key() = " << children[0]->keys_values[i].key() << " is not less than " << keys_values[0].key() << ".\n";
           }  
       break; 

       case 1:
 
       // Test middle child's keys, key, are such that: keys_values.key() [0] < key < keys_values.key()[1]
           if (!(children[1]->keys_values[i].key() > keys_values[0].key() && children[1]->keys_values[i].key() < keys_values[1].key())) {
     
              // problem
              ostr << "error: children[1]->keys_values[" << i << "].key() = " << children[1]->keys_values[i].key() << " is not between " << keys_values[0].key() << " and " << keys_values[1].key() << ".\n";
           }

       break;

      case 2:     
       // Test right child's keys are all greater than nodes sole key
     
           if (children[2]->keys_values[i].key() <= keys_values[1].key()) { // If any are less than or equal to keys_values.key()[1], it is an error.
     
              // problem
              ostr << "error: children[2]->keys_values[" << i << "].key() = " << children[2]->keys_values[i].key() << " is not greater than " << keys_values[1].key() << ".\n";
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

template<class Key, class Value> std::string tree23<Key, Value>::test_invariant(const Node& const_node) const noexcept
{
  std::ostringstream oss;

  Node& node = const_cast<Node&>(const_node);

  switch(node.totalItems) {

      case 0:
         node.test_remove_invariant(oss);
         break;

      case Node::TwoNode:

         node.test_2node_invariant(oss, root.get());
         break;   
      
      case Node::ThreeNode:

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

template<class Key, class Value> std::ostream& tree23<Key, Value>::Node::debug_print(std::ostream& ostr, bool show_addresses) const 
{
   ostr << " { ["; 
   
   if (totalItems == 0) { // remove() situation when merge2Nodes() is called

       ostr << "empty"; 

   } else {

        for (auto i = 0; i < totalItems; ++i) {

            ostr << keys_values[i].key(); // or to print both keys and values do: ostr << keys_values[i]

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


template<class Key, class Value> std::ostream& tree23<Key, Value>::Node::print(std::ostream& ostr) const noexcept
{
   ostr << "[";

   if (totalItems == 0) { // remove() situation when merge2Nodes() is called

       ostr << "empty"; 

   } else {

        for (auto i = 0; i < totalItems; ++i) {

            ostr << keys_values[i].key(); // or to print both keys and values do: ostr << keys_values[i];

            if (i + 1 == totalItems)  {
                continue;

            } else { 
                ostr << ", ";
            }
        }
   }

   ostr << "]";
   return ostr;
}

// Called by begin()
template<class Key, class Value> inline tree23<Key, Value>::iterator::iterator(tree23<Key, Value>& lhs_tree) : tree{lhs_tree},\
                                                            current{lhs_tree.root.get()}, key_index{0}
{
  initialize(iterator_position::beg);
}

template<class Key, class Value> void tree23<Key, Value>::iterator::initialize(tree23<Key, Value>::iterator_position pos) 
{
  position = pos;

  // If the tree is empty, there is nothing over which to iterate...
   if (tree.root.get() == nullptr) {
         
      current = nullptr;
      key_index = 0;
      position = iterator_position::end;

  } else if (position == iterator_position::end) {

      seekToLargest();  // Go to the largest node, and thus allow decrement() to be called on a non-empty tree.

   } else if (position == iterator_position::beg) {

      seekToSmallest(); // Go to the smallest node, and thus allow increment() to be called

   } else { // any other position value is invalid

      throw std::logic_error("iterator constructor called with wrong position paramater");
   }
}

template<class Key, class Value> inline tree23<Key, Value>::iterator::iterator(const iterator& lhs) : tree{lhs.tree}, current{lhs.current}, \
         key_index{lhs.key_index}, position{lhs.position} 
{
  initialize(position);
}

// non const tree23<Key, Value>& passed to ctor. Called only by end()
template<class Key, class Value> inline tree23<Key, Value>::iterator::iterator(tree23<Key, Value>& lhs_tree, \
                                 typename tree23<Key, Value>::iterator_position pos) : tree{lhs_tree}, position{pos} 
{
  initialize(position);
}

template<class Key, class Value> inline typename tree23<Key, Value>::iterator tree23<Key, Value>::begin() noexcept
{
  return iterator{*this, iterator_position::beg};
}

template<class Key, class Value> inline typename tree23<Key, Value>::const_iterator tree23<Key, Value>::begin() const noexcept
{
  return const_iterator{*this, iterator_position::beg};
}

/*
 end() calls the iterator constructor that sets current to nullptr and key_index to 0. 
 */

template<class Key, class Value> inline typename tree23<Key, Value>::iterator tree23<Key, Value>::end() noexcept
{
   return iterator(const_cast<tree23<Key, Value>&>(*this), iterator_position::end);
}

template<class Key, class Value> inline typename tree23<Key, Value>::const_iterator tree23<Key, Value>::end() const noexcept
{
   return const_iterator(const_cast<tree23<Key, Value>&>(*this), tree23<Key, Value>::iterator_position::end);
}

template<class Key, class Value> inline typename tree23<Key, Value>::reverse_iterator tree23<Key, Value>::rbegin() noexcept
{
   return reverse_iterator{ end() }; 
}

template<class Key, class Value> inline typename tree23<Key, Value>::const_reverse_iterator tree23<Key, Value>::rbegin() const noexcept
{
    return const_reverse_iterator{ end() }; 
}

template<class Key, class Value> inline typename tree23<Key, Value>::reverse_iterator tree23<Key, Value>::rend() noexcept
{
    return reverse_iterator{ begin() }; 
}

template<class Key, class Value> inline typename tree23<Key, Value>::const_reverse_iterator tree23<Key, Value>::rend() const noexcept
{
    return const_reverse_iterator{ begin() }; 
}
/*
 Moves to first, smallest node in tree.
 Sets:
 1. current to smallest node
 2. key_index to 0
 3. position is set to value passed 
 */
template<class Key, class Value> void tree23<Key, Value>::iterator::seekToSmallest() 
{
  if (position != iterator_position::beg) {

      throw std::logic_error("iterator constructor called with wrong position paramater");
  }

  for (const Node *cursor = tree.root.get(); cursor != nullptr; cursor = cursor->children[0].get()) {
           current = cursor;
  }

  key_index = 0;
}

template<class Key, class Value> inline void tree23<Key, Value>::iterator::seekToLargest() 
{
  if (position != iterator_position::end) {

      throw std::logic_error("iterator constructor called with wrong position paramater");
  }

  for (const Node *cursor = tree.root.get(); cursor != nullptr; cursor = cursor->children[cursor->totalItems].get()) {
           current = cursor;
  }

  key_index = (current->isThreeNode()) ? 1 : 0;
}

template<class Key, class Value> inline tree23<Key, Value>::iterator::iterator(iterator&& lhs) : \
             tree{lhs.tree}, current{std::move(lhs.current)}, key_index{std::move(lhs.key_index)}, position{std::move(lhs.position)} 
{
   lhs.current = nullptr; // set to end
   lhs.key_index = 0;
   lhs.position = iterator_position::end;
}
/*
 */
template<class Key, class Value> bool tree23<Key, Value>::iterator::operator==(const iterator& lhs) const
{
 if (&lhs.tree == &tree) {

    // If we are not in_between...
    if (lhs.position == iterator_position::end && position == iterator_position::end) { // check whethert both iterators are at the end...

        return true;

    } else if (lhs.position == iterator_position::beg && position == iterator_position::beg) { // ...or at beg. 

        return true;

    } else if (lhs.position == position && lhs.current == current && lhs.key_index == key_index) { // else check whether position, current and key_index
                                                                                                   // are all equal.
        return true;
   }
 }
 
 return false;
}

/*
 int getChildIndex(Node *pnode)

 Requires: pnode is not root, and  pnode is a node in the tree for which we want child_index such that
      current->parent->children[child_index] == current
 Returns: child_index as shown above. 
 */

template<class Key, class Value> int tree23<Key, Value>::iterator::getChildIndex(const typename tree23<Key, Value>::Node *p) const noexcept
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
 TODO: The comments here sometimes be confuse predecessor with successor; likewise, the comments for iterator::getSuccessor confuse the successor with the predecessor!

Two cases are possible: 1.) when current is an internal node and 2.) when current is a leaf node.

case 2:
If current is a leaf node, and if it is a a 3-node and key-index is 1, then the predecessor is trivial: the first key is the predecessor. If, however, the key_index is 0, we ascend the parent
chain until we enounter the first parent that is not a left most-child (of its parent). If the root is enounter before such a parent is found, then there is no predecessor.



 nodes as long as the parent is the right-most (or left-most???) child (of its parent). If we reach the root, there is no predecessor.
Else upon reaching a parent (before the root) that is a middle or left-most child (of its parent), we find the smallest key in the parent's "right" subtree.
 */
template<class Key, class Value> std::pair<const typename tree23<Key, Value>::Node *, int> tree23<Key, Value>::iterator::getPredecessor(const typename  tree23<Key, Value>::Node *current, int key_index) const noexcept
{
  if (current->isLeaf()) { // If leaf node

     if (current == tree.root.get()) { // root is leaf      

         if (tree.root->isThreeNode() && key_index == 1) {

             return std::make_pair(current, 0);
         }
                  
         return std::make_pair(nullptr, 0);
            
     } else {

        return getLeafNodePredecessor(current, key_index);
     }

  } else { // else internal node

      return getInternalNodePredecessor(current, key_index);
  }
}

template<class Key, class Value> std::pair<const typename tree23<Key, Value>::Node *, int> tree23<Key, Value>::iterator::getInternalNodePredecessor(\
     const typename tree23<Key, Value>::Node *pnode, int key_index) const noexcept	    
{
 // Get next left child node of pnode based on key_index.
 const Node *leftChild;

 if (pnode->isThreeNode()) {

      if (key_index == 0) {

        leftChild = pnode->children[0].get();

      } else { // it is 1     

        leftChild = pnode->children[1].get(); // middle child 
      }

 } else { // It is a 2-node

        leftChild = pnode->children[0].get();
 }

 for (const Node *cursor = leftChild; cursor != nullptr; cursor = cursor->children[cursor->totalItems].get()) {

    pnode = cursor;
 }

 return std::make_pair(pnode, pnode->totalItems - 1); 
}
/* 
Finding the predecessor of a given node 
---------------------------------------
  Pseudo code and illustration is at From http://ee.usc.edu/~redekopp/cs104/slides/L19_BalancedBST_23.pdf slides #7 and #8
  If left child exists, predecessor is the right most node of the left subtree
  Else we walk up the ancestor chain until you traverse the first right child pointer (find the first node that is a right child of its 
  parent...that parent is the predecessor)
  If you get to the root w/o finding a node that is a right child, there is no predecessor
*/

template<class Key, class Value> std::pair<const typename tree23<Key, Value>::Node *, int> tree23<Key, Value>::iterator::getLeafNodePredecessor(const typename tree23<Key, Value>::Node *pnode, int index) const noexcept
{
  // Handle trivial case: if the leaf node is a 3-node and key_index points to the second key, simply set key_index to 0. 
  if (pnode->isThreeNode() && index == 1) {

      return std::make_pair(current, 0); 
  }

  // Determine child_index such that pnode == pnode->parent->children[child_index]
  int child_index = getChildIndex(pnode);

  int pred_key_index;

   /*
   Handle easy cases first:
   1. If child_index is 2, then the predecessor -- when pnode is either a 2-node of 3-node -- is pnode->parent and the pred_key_index is the parent's right most key.
   2. If child_index is 1 and parent is a 3-node, the predecessor is pnode->parent and pred_key_index is 0.
   */
  switch (child_index) {

      case 2:
              /*
                 We know that is pnode is a 3-node, then key_index is 0 because if it was 1, this was already handled at the start of this method.
                 If pnode is a 2-node, then obviously, key_index is also 0. So in either case, pnode is the right most child of its parent, so the 
                 predecessor will be the right most key of the parent.
               */
         pnode = pnode->parent;

         pred_key_index = pnode->isThreeNode() ? 1 : 0;               
         break;

   case 1:
          /* 
            pnode is either the right child of a 2-node or the middle child of a 3-node. If pnode is a 3-node, key_index equals 0 because if it was a 1, it was
            already handled at the start of this method.  The possibilities look like this:

            If parent is a 2-node, there are two possible cases: 

              (a)   [20]    (b)   [20]
                    /  \          /  \  
                  [x]  [30]    [x]  [30, 32]

            Note: in case (a), key_index is 0, and in case (b), key_index is also 0.   

            If parent is a 3-node, there are two possible cases: 

              (c)   [20,  40]    (d)   [20,   40]
                    /   |   \         /    |     \ 
                  [x]  [30] [y]     [x] [30, 32]  [y] 
             
            Note:  In case (c) above, key_index is 0, and in case (d), key_index is also 0. 

            Thus, in all four cases, the predecessor is the first key of the parent, which is 20.
           */ 

            pnode = pnode->parent;
            pred_key_index = 0;
            break;

    case 0: 
   /* 
    The possibilites for this case are: 

       (a)   [20]    (b)   [20]       (c)   [20,   40]     (d)   [20,   40]        
             / \           /  \            /    |    \          /    |    \        
          [15]  [x]   [15, 19] [x]    [15, 19] [x]   [y]     [15]   [x]   [y] 

    In (a), pnode is [15]. In (b), pnode is [15, 19] and key_index is 0. In (c), pnode is [15] and key_index is 0. In (d), pnode is also [15] and key_index of 0.

    In all four cases, to find the next largest node the logic is identical: We walk up the parent chain until we traverse the first parent that is not a left-most child 
    of its parent. That parent is the predecessor. If we get to the root without finding a node that is a right child, there is no predecessor.

    Note: In a 2 3 tree, a "right" child pointer will be either the second child of a 2-node or the second, the middle, or the third child of a 3-node. "right" child
    pointer means a pointer to a subtree with larger keys. In a 2 3 tree, the middle child pointer of a 3-node parent is a "right child pointer" of the 1st key
    because all the keys of the subtree whose root is the second (or middle) child pointer are greater than 1st key of the subtree's parent. 

    So when we walk up the ancestor chain as long as the parent is the first child. For example, in the tree portion shown below

              [5,   10]  
              /   |   \                              
          ...    ...  [27,       70]  
                       /       |     \
                      /        |      \   
                   [20]       [45]    [80, 170]
                   /   \      /  \     /  |  \
                [15]  [25]  [30] [60]  <-- pnode points to leaf node [20]. 
                / \   / \   / \  / \   
               0   0 0   0 0   0 0  0  ... 
     
    if [15] is the pnode leaf node, the predecessor of [15] is the second key of the 3-node [5, 10] because when we walk up the parent chain from [15], the first
    right child pointer we encounter is the parent of [27, 70], which is [5, 10]. So [10] is the next smallest key. In this example

              [5,   10]  
              /   |   \                              
          ...    ...  [27,       70]  
                       /       |     \
                      /        |      \   
                   [20]       [45]     [80, 170]
                  /   \       /  \      /  |  \
                [15]  [25]  [30] [60]  <-- pnode points to leaf node [20]. 
                / \   / \   / \  / \   
               0   0 0   0 0   0 0  0  ... 
     
      if [30] is the pnode leaf node, the predecessor of [30] is the first key of the 3-node [27, 70] because when we walk up the parent chain from [30], the first
      non-first child pointer we encounter is the parent of [45], which is [27, 70]. So the key at index 0, which is [27], is the next smallest key. Therefore, if our
      loop above terminates without encountering the root, we must determine the child index of prior_node in pnode. If pnode is a 2-node, it is trivial: the child
      index is one. If pnode is a three node, the child index is either one or two:

      int child_index = 1; // assume pnode is a 2-node.

      if (pnode->isThreeNode()) { // if it is a 3-nodee, compare prior_node to children[1]
          child_index = prior_node == pnode->children[1].get() ? 1 : 2;
      }
  
      Now that we know the child_index such that
            pnode->children[child_index] == prior_node;
      
      Determine which key is the predecessor. If child_index is one, the middle child, then the predecessor is pnode->keys_values[0]. If child_index is two, then
      the predecessor is pnode->keys_values[1].key(). Thus, the predecessor is the key at child_index - 1.
      */
        {
          const Node *ancestor = pnode->parent;
          const Node *prior_node = pnode;
          
          // Ascend the parent pointer chain as long as pnode is the left most child of its parent.
          while(pnode == ancestor->children[0].get() )  {
          
              // pnode is still the left most child, but if its is the root, we cannot ascend further and there is no predecessor.  
              if (ancestor == tree.root.get()) {
                    
                  return std::make_pair(nullptr, 0);  // To indicate this we set current, the member of the pair, to nullptr and key_index, the second member, to 0.
              }
          
              prior_node = pnode;
              pnode = ancestor;
              ancestor = ancestor->parent;
          }
          
          prior_node = pnode; 
          pnode = ancestor;

          int child_index = prior_node == pnode->children[1].get() ? 1 : 2; // If pnode is a 2-node, then prior_node will always be pnode->children[1].get().
                                                                        // If pnode is a 3-node, then we must compare prior_node to  pnode->children[1].get().
                                                                        // If prior_node is not the second child, it must be the third child.

          // Using child_index, we know the key of the next smallest key will be child_index -1.
          return std::make_pair(pnode, child_index - 1);
      }
      break;
 
    default:
       break;

  } // end switch

  return std::make_pair(pnode, pred_key_index); 
}

/*
Finding the successor of a given node 
-------------------------------------
Requires:
1. If position is beg, current and key_index MUST point to first key in tree. 
2. If position is end,  current and key_index MUST point to last key in tree.
  
3. If position is in_between, current and key_index do not point to either the first key in the tree or last key in tree. If the tree has only one node,
the state can only be in_between if the first node is a 3-node.
Returns:
pair<const Node *, int>, where the Node pointer is the node with the next key and value in in-order sequence. key_index is the index into
Node::keys_values[].  If the last key has already been visited, the pointer returned will be nullptr.
Questions: Will position ever be end or beg, or do the callers increment() and decrement() ensure that it is never end or beg?
pseudo code for getting successor from: http://ee.usc.edu/~redekopp/cs104/slides/L19_BalancedBST_23.pdf

 TODO: The comments here sometimes be confuse predecessor with successor; likewise, the comments for iterator::getSuccessor confuse the successor with the predecessor!

If left child exists, predecessor is the right most node of the left subtree. Internal node's of a 2 3 tree always have a right branch because 2 3 trees are
balanced.
Else walk up the ancestor chain until you traverse the first right child pointer (find 
the first node who is a right child of his parent...that parent is the predecessor)
If you get to the root w/o finding a node who is a right child, there is no predecessor
May set:
1. current
2. key_index
3. position
 */
template<class Key, class Value> std::pair<const typename tree23<Key, Value>::Node *, int> tree23<Key, Value>::iterator::getSuccessor(const Node *current,\
                                                                                                           int index_of_key) const noexcept
{
  if (current->isLeaf()) { // If leaf node
     
     if (current == tree.root.get()) { // root is leaf      

         if (tree.root->isThreeNode() && key_index == 0) { // TODO: ??? This needs to be double checked

             return std::make_pair(current, 1);
         }
                  
         return std::make_pair(nullptr, 0);
 
     } else {

        return getLeafNodeSuccessor(current, index_of_key);
     }

  } else { // else internal node

      return getInternalNodeSuccessor(current, index_of_key);
  }
}

/* 
   Requires:
   1. pnode is an internal node not a leaf node.
   2. If pnode is a 3-node, key_index is 1 not 0.
   Returns:
   pointer to successor node.
Note: When a 2 3 tree node is a 3-node, it has two "right" chidren from the point of view of its first key and two "left" children from the point of view of its
second key.
 */
template<class Key, class Value> std::pair<const typename tree23<Key, Value>::Node *, int> tree23<Key, Value>::iterator::getInternalNodeSuccessor(const typename tree23<Key, Value>::Node *pnode, \
 int index_of_key) const noexcept	    
{
 // Get next right child node of pnode based on key_index.
 const Node *rightChild;

 if (pnode->isThreeNode()) {

      if (index_of_key == 0) {

        rightChild = pnode->children[1].get();

      } else { // it is 1     

        rightChild = pnode->children[2].get(); 
      }

 } else { // It is a 2-node

        rightChild = pnode->children[1].get();
 }

 // Question: Does it take into account that fact that a node may have already been visited in order?
 // Get the smallest node in the subtree rooted at the rightChild, i.e., its left most node...
 for (const Node *cursor = rightChild; cursor != nullptr; cursor = cursor->children[0].get()) {

    pnode = cursor;
 }

 return std::make_pair(const_cast<Node *>(pnode), 0);
}
/*
 Requires:
 1. pnode is a leaf node, either a 2 or 3-node
 2. If pnode is 3-node, then key_index, the key index into pnode->keys_values[].key() must be 1, the second key. It can never be 0, the first key.
 */
template<class Key, class Value> std::pair<const typename tree23<Key, Value>::Node *, int> tree23<Key, Value>::iterator::getLeafNodeSuccessor(const \
 typename tree23<Key, Value>::Node *pnode, int index_of_key) const noexcept
{
  // If the leaf node is a 3-node and key_index points to the first key, this is trivial: we simply set key_index to 1. 
  if (pnode->isThreeNode() && index_of_key == 0) {

      return std::make_pair(current, 1); 
  }

  // Determine child_index such that pnode == pnode->parent->children[child_index]
  int child_index = getChildIndex(pnode); // BUG: This blows up if pnode == root.get())

  int suc_key_index;

  /*
   Handle easy cases first:
   1. If child_index is 0, then the successor -- when pnode is either a 2-node of 3-node -- is pnode->parent and the suc_key_index is 0.
   2. If child_index is 1 and parent is a 3-node, the successor is pnode->parent and suc_key_index is 1.
   */
  switch (child_index) {

      case 0: /*
             pnode is either the left most child of either a 2-node or 3-node parent. If pnode is a 3-node, its key_index equals 1 (because if it is was 0,
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
            pnode is either the right child of a 2-node or the middle child of a 3-node. If pnode is a 3-node, key_index equals 1 because if it was a 0, it was
            already handled at the start of this method.  The possibilities look like this
             If parent is a 2-node, there are two possible cases: 
              (a)   [20]    (b)   [20]
                    / \           /  \  
                  [x]  [30]    [x]  [30, 32]
             In (a), key_index is 0. In (b), key_index is 1.   
             If parent is a 3-node, there are two possible cases: 
              (c)   [20,  40]    (d)   [20,   40]
                    /   |   \         /    |    \ 
                  [x]  [30] [ ]     [x]  [30,32] [ ] 
             
              In (c) above, key_index is 0. In (d), key_index is 1. 
               
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
           [5]  [15]   [20] [50, 55]             <-- pnode points to key 55 in leaf node [50, 55]. 
           / \   / \   / \  / \   
          0   0 0   0 0   0 0  0  ... 
    
      again, 60 is the successor by applying the same reasoning.
      */
        {
           const Node *prior_child = pnode;
           const Node *__parent = pnode->parent;
           
           // Ascend the tree, the parent pointers, as long as pnode is the right most child of its parent. 
           while(pnode == __parent->getRightMostChild())  { 
           
               // pnode is still the right most child. If it is also the root, there is no successor (because pnode was the largest node in the tree). 
               if (__parent == tree.root.get()) {
                  
                   return std::make_pair(nullptr, 0);  // To indicate no-successor we set current to nullptr and key_index to 0.
               }
           
               prior_child = pnode;
               pnode = __parent;
               __parent = __parent->parent;
           }
           
           prior_child = pnode; 
           pnode = __parent;
 
           // If pnode is a 3-node, determine if we ascended from the first child, children[0], or the middle child, children[1], and set suc_key_index accordingly. 
           if (pnode->isThreeNode()) {

              suc_key_index = (prior_child == pnode->children[0].get()) ? 0 : 1; 

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

template<class Key, class Value> inline typename tree23<Key, Value>::iterator& tree23<Key, Value>::iterator::increment() noexcept	    
{
  if (tree.isEmpty()) {

     return *this;  // If tree is empty, do nothing.
  }

  switch (position) {

     case iterator_position::end:

           // no-op for increment. current and key_index still point to largest key/value in tree
           break;
      
     case iterator_position::beg:
     case iterator_position::in_between:
     {
           std::pair<const Node *, int> pair = getSuccessor(current, key_index);

           if (pair.first == nullptr) { // nullptr implies there is no successor. Therefore current and key_index already pointed to last key/value in tree.

                // Therefore current doesn't change, nor key_index, but the state becomes 'end', one-past last key. 
                position = iterator_position::end;

           } else if (current == pair.first) {

                key_index = pair.second; // current has no change, but key_index has.
  
           } else {  // curent has changed, so we adjust current and key_index. To ensure position is no longer 'beg', we unconditionally set position to 'in_between'.

               current = pair.first;
               key_index = pair.second;
               position = iterator_position::in_between; 
           }
     }
           break;
     default:
           break;
   }

   return *this;
}

template<class Key, class Value> typename tree23<Key, Value>::iterator& tree23<Key, Value>::iterator::decrement() noexcept	    
{
  if (tree.isEmpty()) {

     return *this; 
  }

  switch (position) {

   case iterator_position::beg:
     // no op. Since current and key_index still point to smallest key and its value., we don't change them. 
     break;

   case iterator_position::in_between: // 'in_between' means current and key_index range from the second key/value in tree and its last key/value.
                                        // 'in_between' corresponds to the inclusive half interval [second key, last key), while 'beg' refers only to
                                        //  first key/value.  
    {      
       std::pair<const Node *,int> pair = getPredecessor(current, key_index); // returns current and key_index of predecessor

       if (pair.first == nullptr) { // If nullptr, there is no successor: current and key_index already point to the first key/value in the tree. 

            // Therefore current doesn't change, nor key_index, but the state becomes 'beg'. 
            position = iterator_position::beg;

       } else if (current == pair.first) { 

            key_index = pair.second;  // current hasn't change, key_index may have, so we set it.

       } else { // Current changed, so we update both current and key_index

           current = pair.first;
           key_index = pair.second;
       }
    }
    break;

   case iterator_position::end:

        // current and key_index already point to last key/value, so we merely change the position state to indicate they are 'in_between'.
        position = iterator_position::in_between;
        break;

   default:
        break;
 }

 return *this;
}
   
template<class Key, class Value> inline typename tree23<Key, Value>::KeyValue& tree23<Key, Value>::KeyValue::operator=(typename tree23<Key, Value>::KeyValue&& lhs)
{
  if (this == &lhs) {
      return *this;
  }

  key() = lhs.key();
  value() = std::move(lhs.value());

  return *this; 
}
 
template<class Key, class Value> inline  typename tree23<Key, Value>::KeyValue& tree23<Key, Value>::KeyValue::operator=(const typename tree23<Key, Value>::KeyValue& lhs)
{
  if (this == &lhs) {
      return *this;
  }

  key() = lhs.key();
  value() = lhs.value();

  return *this; 
}

template<class Key, class Value> inline typename tree23<Key, Value>::iterator& tree23<Key, Value>::iterator::operator++() noexcept	    
{
  increment();
  return *this;
}

template<class Key, class Value> typename tree23<Key, Value>::iterator tree23<Key, Value>::iterator::operator++(int) noexcept	    
{
 iterator tmp{*this};

 increment(); 

 return *this;
}

template<class Key, class Value> inline typename tree23<Key, Value>::iterator& tree23<Key, Value>::iterator::operator--() noexcept	    
{
  iterator::decrement();
  return *this;
}

template<class Key, class Value> typename tree23<Key, Value>::iterator tree23<Key, Value>::iterator::operator--(int) noexcept	    
{
 iterator tmp{*this};

 decrement(); 

 return *this;
}

/*
 tree23<Key, Value>::const_iterator constructors
 */
template<class Key, class Value> inline tree23<Key, Value>::const_iterator::const_iterator(const tree23<Key, Value>& lhs) : \
                                                                                  iter{const_cast<tree23<Key, Value>&>(lhs)} 
{
}

template<class Key, class Value> inline tree23<Key, Value>::const_iterator::const_iterator(const tree23<Key, Value>& lhs, iterator_position pos) : \
 iter{const_cast<tree23<Key, Value>&>(lhs), pos} 
{
}

template<class Key, class Value> inline tree23<Key, Value>::const_iterator::const_iterator::const_iterator(const typename tree23<Key, Value>::const_iterator& lhs) : \
 iter{lhs.iter}
{
}

template<class Key, class Value> inline tree23<Key, Value>::const_iterator::const_iterator::const_iterator(typename tree23<Key, Value>::const_iterator&& lhs) : \
  iter{std::move(lhs.iter)}
{
}
/*
 * This constructor also provides implicit type conversion from a iterator to a const_iterator
 */
template<class Key, class Value> inline tree23<Key, Value>::const_iterator::const_iterator::const_iterator(const typename tree23<Key, Value>::iterator& lhs) : \
  iter{lhs}
{
}


template<class Key, class Value> inline bool tree23<Key, Value>::const_iterator::operator==(const const_iterator& lhs) const 
{ 
  return iter.operator==(static_cast< const iterator& >(lhs.iter)); 
}

template<class Key, class Value> inline  bool tree23<Key, Value>::const_iterator::operator!=(const const_iterator& lhs) const
{ 
  return iter.operator!=(static_cast< const iterator& >(lhs.iter)); 
}
     
template<class Key, class Value> inline typename tree23<Key, Value>::const_iterator& tree23<Key, Value>::const_iterator::operator++() noexcept	    
{
  iter.increment();
  return *this;
}

template<class Key, class Value> inline typename tree23<Key, Value>::const_iterator tree23<Key, Value>::const_iterator::operator++(int) noexcept	    
{
 const_iterator tmp{*this};

 iter.increment(); 

 return *this;
}

template<class Key, class Value> inline typename tree23<Key, Value>::const_iterator& tree23<Key, Value>::const_iterator::operator--() noexcept	    
{
   iter.decrement();
   return *this;
}

template<class Key, class Value> inline typename tree23<Key, Value>::const_iterator tree23<Key, Value>::const_iterator::operator--(int) noexcept	    
{
 const_iterator tmp{*this};

 iter.decrement(); 

 return *this;
}

/*
 Checks if any sibling--not just adjacent siblings, but also those that are two hops away--are 3-nodes, from which we can "steal" a key.
 Parameters
 ==========
 1. child_index is such that: parent->children[child_index] == this
 2. If any sibling is found that is a 3-node, parent->children[silbing_index] = the 3-node sibling
 */
template<class Key, class Value> bool tree23<Key, Value>::Node::siblingHasTwoItems(int child_index, int& sibling_index) const noexcept
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
   3-node parent cases below. Determine if any immediate sibling is a 3-node. There will only be two children to inspect when the parent is a 3-node and child_index
   is 1.
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

template<class Key, class Value> inline tree23<Key, Value>::tree23(std::initializer_list<typename tree23<Key, Value>::value_type> list) 
{
   for (auto& v : list) {
       insert(v.first, v.second);  
   }
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
  CloneTree(lhs.root, root, nullptr);
}   

/*
 Does a pre-order traversal, using recursion and copying the source node, the first parameter, into the destination node, the second parameter.
 Note: We don't want to copy the parent of srcNode (only its key and value) into destNode. Instead we pass in the previously clone parent node of destNode.
 */
template<class Key, class Value>  void tree23<Key, Value>::CloneTree(const std::unique_ptr<typename tree23<Key, Value>::Node>& srcNode, \
        std::unique_ptr<typename tree23<Key, Value>::Node>& destNode, const typename tree23<Key, Value>::Node *parent) noexcept
{
  if (srcNode != nullptr) { 
                              
   switch (srcNode->totalItems) {

      case 1: // two node
      {    
            destNode = std::make_unique<Node>(srcNode->keys_values, const_cast<Node *>(parent), srcNode->totalItems);
             
            //destNode->parent = srcNode->parent; // TODO: Is this redundant? And is it correct? Aren't we setting the destNode::parent incorrectly? 
            
            CloneTree(srcNode->children[0], destNode->children[0], destNode.get()); 
            
            CloneTree(srcNode->children[1], destNode->children[1], destNode.get()); 

            break;

      }   
      case 2: // three node
      {
            destNode = std::make_unique<Node>(srcNode->keys_values, const_cast<Node *>(parent), srcNode->totalItems); 

            //destNode->parent = srcNode->parent;// TODO: Isn't this redundant? 
            
            CloneTree(srcNode->children[0], destNode->children[0], destNode.get());
            
            CloneTree(srcNode->children[1], destNode->children[1], destNode.get());
            
            CloneTree(srcNode->children[2], destNode->children[2], destNode.get());

            break;
      } // end case
   }  // end switch
 } else {

    destNode = nullptr;
 } 
}

/*
 * Does a post order tree traversal, using recursion and deleting nodes as they are visited.
 */
template<class Key, class Value> void tree23<Key, Value>::DestroyTree(std::unique_ptr<Node> &current) noexcept 
{
  if (current == nullptr) {

      return;
  }
  
  for(auto i = 0; i < current->totalItems; ++i) {

        DestroyTree(current->children[i]);
   }

   current.reset(); // deletes the underlying pointer. 
}

// Move constructorusing external iterator
template<class Key, class Value> inline tree23<Key, Value>::tree23(tree23<Key, Value>&& lhs) noexcept : root{std::move(lhs.root)}, height{lhs.height}
{
  lhs.height = 0;
}   

// Copy assignment
template<class Key, class Value> tree23<Key, Value>& tree23<Key, Value>::operator=(const tree23<Key, Value>& lhs) noexcept
{
  if (this == &lhs)  {
      
      return *this;
  }
  
  DestroyTree(root); // free all the nodes of the current tree 

  height = lhs.height;
  
  // Traverse in pre-order using the clone functor. See todo.txt
  CloneTree(lhs.root, root);

   return *this;
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
 */
template<class Key, class Value> tree23<Key, Value>::Node4::Node4(Node *p3node, Key new_key, const Value& new_value) noexcept : parent{p3node->parent} 
{
   bool copied = false;
   int dest = 0;
   int src = 0;

   while (src < Node::ThreeNode) {
  
         if (!copied && new_key < p3node->keys_values[src].key()) {

               copied = true;
               keys_values[dest].key() = new_key; 
               keys_values[dest].value() = new_value; 
               ++dest;

         }  else {

               keys_values[dest] = std::move(p3node->keys_values[src]);  // This was done to improper efficiency.
               ++dest;
               ++src;
         } 
   }
   
   if (!copied) {
        keys_values[dest].key() = new_key; 
        keys_values[dest].value() = new_value; 
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

template<class Key, class Value> tree23<Key, Value>::Node4::Node4(Node *p3node, Key key, const Value& value, int child_index, \
  std::unique_ptr<Node> heap_2node) noexcept : parent{p3node->parent} 
{
  switch(child_index) {
 
      case 0: // key was pushed up from the 4-node constructed from the child at p3node->children[0] (and another key). It will therefore become the
	     // smallest value in the 4-node.

      {
        keys_values[0].key() = key; // key is the smallest value, so we put in the first position... 
        keys_values[0].value() = value;

        //...followed by the current p3node's keys and values
        for(auto i = 0; i < Node::ThreeNode; ++i) {
 
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
              // p3node.nc_pair.keys_values[0].key() < key && key < p3node.nc_pair.keys_values[1].key()

      {  
        keys_values[0] = p3node->keys_values[0];
 
        keys_values[1].key() = key;
        keys_values[1].value() = value; 
        
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
	      //  key > p3node->keys_values[1].key()

      { 
         for(auto i = 0; i < Node::ThreeNode; ++i) {   
                               
               keys_values[i] = p3node->keys_values[i]; 
         } 
    
         keys_values[2].key() = key; // key is the largest value in 4-node
         keys_values[2].value() = value;
    
         for(auto i = 0; i < Node::ThreeNodeChildren; ++i) { // connect p3node's current children in the same order 
    
            connectChild(i, std::move(p3node->children[i]));
         }

         // invoke Node's move assignment operator.
         children[3] = std::move(heap_2node); // heap_2node's key is larger the p3node's largest key: p3node->keys_values[1].key() 
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

  children = std::move(lhs.children); /* This invokes std::array<Node>'s move assignment operater. For Node copy or move construction one must not do this,
                           but rather call Node::connectChild() for each child, which properly sets the parent pointer in the node; but for Node4 construction
                         the parent pointer does not need to be properly set--I believe--because Node4 is only owns the children temporarily, until it is split
                         into two Nodes, at which time Node::connectChild() is call to properly set the node's parent pointer. */
                                     
  parent = lhs.parent;

  lhs.parent = nullptr; 
  
  return *this; 
}

template<class Key, class Value> inline void tree23<Key, Value>::Node4::connectChild(int childIndex, std::unique_ptr<typename tree23<Key, Value>::Node> child) noexcept 
{
 /*
  Because Node4::parent is of type Node *, we cannot do
        parent = this;
  since 'this' is of type, Node4 *. 
  */

  Node *parent_of_node23 = (child == nullptr) ? nullptr : child->parent; 
  
  children[childIndex] = std::move(child); // invokes move assignment of std::unique_ptr<Node>.

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

       ostr << "{ " << keys_values[i].key() << ", " << keys_values[i].second << "}, "; 
   }
   
   ostr << "] children:  [ ";  
   
   for (auto& pChild : children) {
       
       if (pChild == nullptr) {
           
           ostr << "nullptr, ";   
           
       } else {

         for (auto i = 0; i < pChild->totalItems; ++i) {  
            
            ostr << "{ " << pChild->keys_values[i].key() << ", " << pChild->keys_values[i].value() << "}, ";
         } 

       }
   }
   ostr << "]" << std::endl;
   return ostr;
}

template<class Key, class Value> inline int tree23<Key, Value>::getHeight() const noexcept
{
  return height;
}

template<class Key, class Value> template<typename Functor> inline void tree23<Key, Value>::inOrderTraverse(Functor f) const noexcept
{
   DoInOrderTraverse(f, root);
}

template<class Key, class Value> template<typename Functor> void tree23<Key, Value>::DoInOrderTraverse(Functor f, const std::unique_ptr<Node>& current) const noexcept
{
   if (current == nullptr) {

      return;
   }

   switch (current->getTotalItems()) {

      case 1: // two node
            DoInOrderTraverse(f, current->children[0]);
 
            f(current->keys_values[0].const_pair);   // current->key(1)

            DoInOrderTraverse(f, current->children[1]);
            break;

      case 2: // three node
            DoInOrderTraverse(f, current->children[0]);

            f(current->keys_values[0].const_pair);

            DoInOrderTraverse(f, current->children[1]);
 
            f(current->keys_values[1].const_pair);

            DoInOrderTraverse(f, current->children[2]);
            break;
   }
}

template<class Key, class Value> template<typename Functor> void tree23<Key, Value>::levelOrderTraverse(Functor f) const noexcept
{
   std::queue< std::pair<const Node*, int> > queue; 

   Node *proot = root.get();

   if (proot == nullptr) return;
      
   auto initial_level = 1; // initial, top root level is 1.
   
   // 1. pair.first  is: const tree<Key, Value>::Node*, the current node to visit.
   // 2. pair.second is: current level of tree.
   queue.push(std::make_pair(proot, initial_level));

   while (!queue.empty()) {

        std::pair<const Node *, int> pair_ = queue.front();

        const tree23<Key, Value>::Node *current = pair_.first;

        int current_tree_level = pair_.second;

        f(*current, current_tree_level);  
        
        if (current != nullptr && !current->isLeaf()) {

            if (current->totalItems == 0) { // This can happen only during remove() when an internal 2-node becomes empty temporarily  ...

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

  Node *current = root.get();

  while (current != nullptr) {
      
       auto totalItems = current->totalItems;   
       auto i = 0;

       for(; i < totalItems; ++i) {

           if (key < current->keys_values[i].key()) {

                current = current->children[i].get();
                break;

           } else if (key == current->keys_values[i].key()) {

                return true;
           } 
       }

       if (i == totalItems) {

           current = current->children[totalItems].get(); // key > largest key
       } 
  }

  return false;
}

template<typename Key, typename Value> inline void tree23<Key, Value>::printlevelOrder(std::ostream& ostr) const noexcept
{
  levelOrderDisplay<tree23<Key, Value>> tree_printer(*this, ostr);  
  
  levelOrderTraverse(tree_printer);
}

template<typename Key, typename Value> inline void tree23<Key, Value>::printInOrder(std::ostream& ostr) const noexcept
{
  auto lambda = [&](const std::pair<Key, Value>& pr) { ostr << pr.first << ' '; };
  inOrderTraverse(lambda); 
}

template<class Key, class Value> inline void tree23<Key, Value>::CreateRoot(Key key, const Value& value) noexcept
{
   root = std::make_unique<Node>(key, value);
   height = 1; // first node added to tree, the root.
}
/* 
   If new_key is already in the tree, we overwrite its associate value with new_value. If it is not in the tree, we descend to the leaf where the
   insertion should begin. If the leaf is a 2-node, we insert the new key. If it is a 3-node we call split(), passing a throw away unique_ptr<Node> that
   holds a nullptr.
 */
template<class Key, class Value> void tree23<Key, Value>::insert(Key new_key, const Value& new_value)
{
  if (root == nullptr) {
      
      // Create the initial unique_ptr<Node> in the tree.
      CreateRoot(new_key, new_value);
      return;
  }

/*
  new_key will be inserted between the next largest value in the tree and the next smallest: in order predecessor key < new_key < in order successor key

   "stack<int> child_indecies" tracks each branch taken descending to the leaf in which new_key should be inserted. This aids in creating 4-nodes
   from internal 3-nodes. child_indecies tells us the branches taken from the root to pinsert_start. 

   Thus, for example, in the code like that below, which converts the descent branches contained in the stack<int> named child_indecies into a deque<int> named
   branches, "branches" can then be used to duplicate the exact descent braches taken from the root to the leaf where new_key insertion should begin:

       // convert stack to deque
       deque<int> branches;
       while (!child_indecies.empty()) {
              branches.push_back(stk.top());
              child_indecies.pop();
       }
 
       Node *current = root.get();
       
       for (auto branch : branches) { // descend to pinsert_start from root
            current = current->children[branch]; 
       } 
*/

  std::stack<int> child_indecies; 

  Node *pinsert_start;

  int found_index = findInsertNode(new_key, child_indecies, pinsert_start);

  if (found_index != Node::NotFoundIndex) { // new_key already exists, so we overwrite its associated value with the new value.

       pinsert_start->keys_values[found_index].value() = new_value;
       return;  
  }

  // new_key was not found in the tree; therefore we know pinsert_start is a leaf.
  if (pinsert_start->isThreeNode()) { 
    
      // split() converts pinsert_start from a 3-node to a 2-node.
      split(pinsert_start, child_indecies, std::unique_ptr<Node>{nullptr}, new_key, new_value); 

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
    new_key == pinsert_start->keys[found_index]. However, if new_key is not in the tree, then the return value is Node::NotFoundIndex.
 2. pinsert_start will be the node where new_key was found, if it already exists in the tree; otherwise, it will be the leaf node where insertion should
    begin.
 3. descent_indecies will hold the child branches take to descend to pinsert_start.
 */

template<class Key, class Value> int tree23<Key, Value>::findInsertNode(Key new_key, std::stack<int>& child_indecies, \
                                                           typename tree23<Key, Value>::Node *&pinsert_start) const noexcept
{
  int found_index = Node::NotFoundIndex;

  int child_index; 

  pinsert_start = root.get();

  while(pinsert_start->NodeDescentSearch(new_key, found_index, child_index) == false && !pinsert_start->isLeaf()) { // Search for new_key until found or if we search a leaf node and didn't find the key.
   
      child_indecies.push(child_index); // remember which child node branch we took. Note: If the node is a leaf, nothing will be pushed onto the stack. 
         
      pinsert_start = pinsert_start->children[child_index].get();
   }

   return found_index;
}
/*
 Advances cursor next if key not found in current node. If found sets found_index.
 */
template<class Key, class Value> inline bool tree23<Key, Value>::Node::NodeDescentSearch(Key new_key, int& found_index, Node *next) noexcept
{
  for(auto i = 0; i < totalItems; ++i) {

     if (new_key < keys_values[i].key()) {
            
         next = children[i].get(); 
         return false;

     } else if (keys_values[i].key() == new_key) {

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
template<class Key, class Value> inline bool tree23<Key, Value>::Node::NodeDescentSearch(Key new_key, int& found_index, int& next_child_index) noexcept
{
  for(auto i = 0; i < totalItems; ++i) {

     if (new_key < keys_values[i].key()) {
            
         next_child_index = i; 
         return false;

     } else if (keys_values[i].key() == new_key) {

         found_index = i;
         return true;
     }
  }

  // It must be greater than the last key (because it is not less than or equal to it). 
  next_child_index = totalItems;  // 

  return false;
}
// can I rename this CreateRoot()?

template<class Key, class Value> template<class... Args> inline void tree23<Key, Value>::EmplaceRoot(Key key, Args&&... args) noexcept
{
   root = std::make_unique<Node>(key, std::forward<Args>(args)...); // Uses Node's variadic template constructor.
   height = 1; // first node added to tree, the root.
}

// See split() variadic template method in ~/test/main.cpp

template<class Key, class Value> template <class... Args>
void tree23<Key, Value>::emplace(Key key, Args&&... args)
{
  if (root == nullptr) {
      
      // Create the initial unique_ptr<Node> in the tree.
      EmplaceRoot(key, std::forward<Args>(args)...); // TODO: Can I overload CreateRoot()?
      return;
  }
  // The code is the tree23<K, V>::insert() code. It has not been modified to work with emplace(), which needs to do
  //    ::new((void*)place)  Value(std::forward<Args>(__args)...); 
  // but not use the first element, which is the key.

  std::stack<int> child_indecies; 

  Node *pinsert_start;

  int found_index = findInsertNode(key, child_indecies, pinsert_start);

  if (found_index != Node::NotFoundIndex) { // if key already exists, overwrite its associated value.

       // Destruct the current value by explicitly invoking Value's destructor. 
       pinsert_start->~keys_values[found_index].value(); 

       // Now use its location to instantate the new Value with the forwarded arguments.
       void *location = &pinsert_start->keys_values[found_index].const_pair.second;

       new(location) Value(std::forward<Args>(args)... );
  
       return;  
  }

  // key is not in the tree; therefore we know pinsert_start must be a leaf.
  if (pinsert_start->isThreeNode()) { 
    
      // Converts pinsert_start from a 3-node to a 2-node.

      //
      // TODO: Create a template<class... Args> version of split()?
      // This version of split() not yet implemented.
      split(pinsert_start, key, child_indecies, std::unique_ptr<Node>{nullptr}, std::forward<Args>(args)...);  

  } else { // else we have room to insert new_new_key/new_value into leaf node.

     // TODO: Line below won't compile 
     // pinsert_start->insertKeyInLeaf(key, std::forward<Args>(args)...); // <-- BUG  was "->insertKeyInLeaf(key, new_value);
  }
}
  
/*
Input Parameters:

1. pnode is the 3-node that needs to be split into two 2-nodes.
2. child_index is only used within a recursive call to split(). child_index is the index within pnode->children[] such that 
    pnode->children[child_index] = the prior 3-node that was split in the prior invocation of split.
(It now is a 2-node holding the smallest value of its paralell 4-node, node4.keys_values[0].key()).
 
2. heap_2node is either a Node rvalue that holds nullptr or a newly allocated 2-node created in the prior call to split(). heap_2node is only used on recursive calls to split()

Algorithm overview:

split() is a recursive method. It has two base cases that terminate the recursions: 1.) when a 2-node parent is found, or when pnode is the root, which
causes the tree to grow upward one level.  

This method splits a 3-node by creating a 4-node on the stack, which holds both the 3-node's keys and values and the new key and value being inserted. If
pnode is a leaf, node4's children are all nullptrs. If pnode is an internal node, we pop its child index from the child_indecies stack<int>, where "child
index" is the index such that 
  
    pnode->children[child_index].get() == "the previous level's 3-node that was just processed by split into which a new key and value were also
                                            inserted."
   
After creating the 4-node, we "split" it into two 2-nodes:  

  1. We convert the 3-node pnode into a 2-node holding only node4.keys_values[0].key() and node4.keys_values[0].second, and whose children were the
     two left most children of node4.

  2. We create a new 2-node on the heap holding node4.keys_values[2].key() and node4.keys_values[2].second, and whose children were the two right most
     children of node4.

Next, we attempt to insert keys_values[1].key(), the middle value, in the parent. If the parent is a 2-node, this works: we convert it to a 3-node and connect the heap_2node child,
and we are done. If the parent is a 3-node, we recurse, which may ulimately result in splitting the root, creating a new node above the current root.
 split(Node *pnode, Key new_key, const Value& new_value)
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

template<class Key, class Value> void tree23<Key, Value>::split(Node *pnode, std::stack<int>& child_indecies, std::unique_ptr<Node> heap_2node, \
                                                                Key new_key, const Value& new_value) noexcept
{
  // get the actual parent              
  Node *parent = pnode->parent;
  
  // Create 4-node on stack that will aid in splitting the 3-node that receives new_key (and new_value).
  Node4 node4;

  int child_index;
 
  if (pnode->isLeaf()) { // pnode->isLeaf() if and only if heap_2node == nullptr

      node4 = Node4{pnode, new_key, new_value}; // We construct a 4-node from the 3-node leaf. The = invokes move assignment--right? 

  } else { // It is an internal leaf, so we need to get its child_index such that:
           // pnode == pnode->parent->children[child_index]. 

      child_index = child_indecies.top();
      child_indecies.pop();

      node4 = Node4{pnode, new_key, new_value, child_index, std::move(heap_2node)}; 
  }
   
  /* 
     Next we check if the 3-node, pnode, is the root. If so, we create a new top level Node and make it the new root.
     If not, we use node4 to 
    
      1.) We downsize pnode to a 2-node, holding the smallest value in the 4-node, node4.keys_values[0], and we connect the two left most
          children of node4 as the two children of pnode. The code to do this is
    
             pnode->convertTo2Node(node4); 
    
      2.) We allocate a new Node 2-node on the heap that will hold the largest value in node4, nod4.nc_pair.keys_values[2]. Its two children will be the
          two right most children of node4. The code to do this this is the Node constructor that takes a Node4 reference as input.
          std::unique_ptr<Node> larger_2node{std::make_unique<Node>(node4)}; 
   */
  pnode->convertTo2Node(std::move(node4)); 

  // 2. Create an entirely new 2-node that contains the largest value in node4, node4.keys_values[2].key(), and whose children are the two right most children of node4
  //    the children of pnode. This is what the Node constructor that takes a Node4 does.
  std::unique_ptr<Node> larger_2node{std::make_unique<Node>(node4)}; 
  
  if (pnode == root.get()) {

       // We pass node4.keys_values[1].key() and node4.keys_values[1].value() as the Key and Value for the new root.
       // pnode == root.get(), and pnode is now a 2-node. larger_2node is the 2-node holding node4.keys_values[2].key().
        
       CreateNewRoot(node4.keys_values[1].key(), node4.keys_values[1].value(), std::move(root), std::move(larger_2node)); 

  } else if (parent->isTwoNode()) { // Since pnode is not the root, its parent is an internal node. If it, too, is a 2-node, ...

      // ...we convert the parent to a 3-node by inserting the middle value into the parent, and passing it the larger 2-node, which it will adopt.
      parent->convertTo3Node(node4.keys_values[1].key(), node4.keys_values[1].value(), std::move(larger_2node));

  } else { // parent is a 3-node, so we recurse.

     // parent now has three items, so we can't insert the middle item. We recurse to split the parent.
     split(parent, child_indecies, std::move(larger_2node), node4.keys_values[1].key(), node4.keys_values[1].value()); 
  } 

  return;
}
/* TODO: Not yet implemented. See TODOes
template<class Key, class Value> template<class... Args> void tree23<Key, Value>::split(Node *pnode, std::stack<int>& child_indecies, \
                                                                          Key new_key, std::unique_ptr<Node> heap_2node, Args&&...args) noexcept
{
  // get the actual parent              
  Node *parent = pnode->parent;
  
  // Debug only next line:
  Node *pheap_2node = heap_2node.get(); // debug-only line?
      
  // Create 4-node on stack that will aid in splitting the 3-node that receives new_key (and new_value).
  Node4 node4;

  int child_index;
 
  if (pnode->isLeaf()) { // pnode->isLeaf() if and only if heap_2node == nullptr

      //--node4 = Node4{pnode, new_key, new_value}; // We construct a 4-node from the 3-node leaf. The = invokes move assignment--right? 
      node4 = Node4{pnode, new_key, std::forward<Args>(args)...}; // TODO: We need a variadic version of Node4 apparently.

  } else { // It is an internal leaf, so we need to get its child_index such that:
           // pnode == pnode->parent->children[child_index]. 

      child_index = child_indecies.top();
      child_indecies.pop();

      //--node4 = Node4{pnode, new_key, new_value, child_index, std::move(heap_2node)}; 
      node4 = Node4{pnode, std::move(heap_2node), new_key, std::forward<Args>(args)...}; // TODO: Variadic version needed
  }
   
  // 
  //   Next we check if the 3-node, pnode, is the root. If so, we create a new top level Node and make it the new root.
  //   If not, we use node4 to 
  //  
  //    1.) We downsize pnode to a 2-node, holding the smallest value in the 4-node, node4.keys_values[0], and we connect the two left most
  //        children of node4 as the two children of pnode. The code to do this is
  //  
  //           pnode->convertTo2Node(node4); 
  //  
  //    2.) We allocate a new Node 2-node on the heap that will hold the largest value in node4, nod4.nc_pair.keys_values[2]. Its two children will be the
  //        two right most children of node4. The code to do this this is the Node constructor that takes a Node4 reference as input.
  //        std::unique_ptr<Node> larger_2node{std::make_unique<Node>(node4)}; 
  //
  pnode->convertTo2Node(std::move(node4)); 

  // 2. Create an entirely new 2-node that contains the largest value in node4, node4.keys_values[2].key(), and whose children are the two right most children of node4
  //    the children of pnode. This is what the Node constructor that takes a Node4 does.
  std::unique_ptr<Node> larger_2node{std::make_unique<Node>(node4)}; 
  
  if (pnode == root.get()) {

       // We pass node4.keys_values[1].key() and node4.keys_values[1].value() as the Key and Value for the new root.
       // pnode == root.get(), and pnode is now a 2-node. larger_2node is the 2-node holding node4.keys_values[2].key().
        
       CreateNewRoot(node4.keys_values[1].key(), node4.keys_values[1].value(), std::move(root), std::move(larger_2node)); 

  } else if (parent->isTwoNode()) { // Since pnode is not the root, its parent is an internal node. If it, too, is a 2-node,

      // we convert it to a 3-node by inserting the middle value into the parent, and passing it the larger 2-node, which it will adopt.
      parent->convertTo3Node(node4.keys_values[1].key(), node4.keys_values[1].value(), std::move(larger_2node));

  } else { // parent is a 3-node, so we recurse.

     // parent now has three items, so we can't insert the middle item. We recurse to split it.
     split(parent, child_indecies, std::move(larger_2node), node4.keys_values[1].key(), node4.keys_values[1].value()); 
  } 

  return;
}
*/
/*
  Requires: currentRoot is the root. tree::root was moved to the parameter currentRoot by the caller. currentRoot has been down sized to a 2-node.
            rightChild is a heap allocated 2-node unique_ptr<Node> holding the largest key (and its associated value) in the formerly 3-node root.   
            new_key is such that pCurrentRoot->keys_values[0].key() < new_key < leftChild->keys_values[0].key(), and will be added above the current root,
            growing the tree upward one level. 
  Promises: A new root is added growing the tree upward one level.
 */
template<class Key, class Value> void tree23<Key, Value>::CreateNewRoot(Key new_key, const Value& new_value, std::unique_ptr<Node> currentRoot, \
                  std::unique_ptr<Node> rightChild) noexcept
{
   // 1. create new root node.
   std::unique_ptr<Node> new_root = std::make_unique<Node>(new_key, new_value);

   // 2. Release the current root, so that it does not inadvertanely get deleted during a move(). It will be the leftChild or new_root.
   std::unique_ptr<Node> leftChild { currentRoot.release() };
 
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
template<class Key, class Value> void tree23<Key, Value>::Node::convertTo2Node(Node4&& node4) noexcept
{ 
  keys_values[0] = std::move(node4.keys_values[0]);

  totalItems = Node::TwoNode; 

  // Take ownership of the two left most children of node4 
  connectChild(0, std::move(node4.children[0]));
  connectChild(1, std::move(node4.children[1]));
} 

/*
 Requires: this must be a 2-node
 Promises: creates a 3-node.
 */
template<class Key, class Value> void tree23<Key, Value>::Node::convertTo3Node(Key new_key, const Value& new_value, std::unique_ptr<Node> newChild) noexcept
{ 
  if (keys_values[0].key() > new_key) {

      keys_values[1] = std::move(keys_values[0]);  

      keys_values[0].key() = new_key;
      keys_values[0].value() = new_value; 

  } else {

      keys_values[1].key() = new_key;
      keys_values[1].value() = new_value;

      // Note: This tells us that newChild will be the right most child, and the existing children do not need to move.
  }

  // Determine where newChild should be inserted.
  int child_index = 0;

  for (; child_index < Node::TwoNodeChildren; ++child_index) {
       
       if (newChild->keys_values[0].key() < children[child_index]->keys_values[0].key()) { // Do I need to worry about children[child_index]->keys_values[1].key() ever?
           break;
       }
  }

  if (child_index == Node::TwoNodeChildren) {
       child_index = 2; 
  }

  // shift children to right as needed 
  for (auto i = Node::TwoNodeChildren - 1; i >= child_index; --i) {

        connectChild(i + 1, std::move(children[i]));
  }
  // insert newChild
  connectChild(child_index, std::move(newChild));

  totalItems = Node::ThreeNode; 
} 

template<class Key, class Value> void  tree23<Key, Value>::Node::connectChild(int childIndex, std::unique_ptr<typename tree23<Key, Value>::Node> child) noexcept 
{
  children[childIndex] = std::move(child); 
  
  if (children[childIndex] != nullptr) { 

       children[childIndex]->parent = this; 
  }
}

template<class Key, class Value> inline void tree23<Key, Value>::Node::connectChild(std::unique_ptr<typename tree23<Key, Value>::Node>& dest,\
                                                                                     std::unique_ptr<typename tree23<Key, Value>::Node> src) noexcept 
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
template<class Key, class Value> inline void tree23<Key, Value>::Node::insertKeyInLeaf(Key key, const Value& new_value)
{
   if (key < keys_values[0].key()) {

       keys_values[1]= std::move(keys_values[0]);

       keys_values[0].key() = key;
       keys_values[0].value() = new_value;

   } else { // key > keys_values[0].key()

       keys_values[1].key() = key;
       keys_values[1].value() = new_value;  
   }

   ++totalItems;
   return;
}
/*
  Requires: this is a 2-node, ie, this->totalItems == Node::TwoNode
  rvalue or universal reference version.
 */
template<class Key, class Value> inline void tree23<Key, Value>::Node::insertKeyInLeaf(Key key, Value&& new_value)
{
   if (key < keys_values[0].key()) {

       keys_values[1] = std::move(keys_values[0]); 

       keys_values[0].key() = key;
       keys_values[0].value() = std::move(new_value);

   } else { // key > keys_values[0].key()

       keys_values[1].key() = key;
       keys_values[1].value() = std::move(new_value);  
   }

   totalItems = Node::ThreeNode; 
   return;
}
/*
 pseudo code for remove(Key key):
 Call findRemovalStartNode. It returns:
      1. Node * of node with key
      2. index of key
      3. an stack<int> that contains the 'history' of child indecies we descended from the root.
 If key does not exist
    return
 else
    save its node pointer and the index within keys_values[].
 If it is an internal node
    swap the key with the key's in order successor's, which will always be in a leaf, enabling deletion to starts at a leaf node.
    
 delete swapped key from leaf node 
 if leaf is now empty
     fixTree(empty_leaf_node)  
 Comment: A stack<int> of indecies records the route taken descending to the node containing 'key'.
 */
template<class Key, class Value> void tree23<Key, Value>::remove(Key key)
{
  if (isEmpty()) {

     return;
  }

  std::stack<int> descent_indecies; 

  int found_index = Node::NotFoundIndex;
  
  Node *premove_start = findRemovalStartNode(key, descent_indecies, found_index);

  if (premove_start == nullptr) return;

  Node *pLeaf;

  if (premove_start->isLeaf()) {
      
      pLeaf = premove_start;  // ...premove_start is a leaf, and the key is in premove_start->keys[found_index]
       
  } else {   // premove_start is an internal node...

      // ...get its in order successor, which will be keys_values[0].key() of a leaf node.
      pLeaf = getSuccessor(premove_start, found_index, descent_indecies); 
          
      /*  
       * Swap the internal key( and its associated value) with its in order successor key and value. The in order successor is always in
       * keys_values[0].key().
       */
      std::swap(premove_start->keys_values[found_index], pLeaf->keys_values[0]); 
   } 
  
 
  pLeaf->removeLeafKey(key); // remove key from leaf         
  
  // We now have reduced the problem to removing the key (and its value) from a leaf node, pLeaf. 
  if (pLeaf->isEmpty()) { 
      
      fixTree(pLeaf, descent_indecies);  
  }

  return;
}
/*
 * Assumes tree is not empty. root is not nullptr.
 */ 
template<class Key, class Value> inline typename tree23<Key, Value>::Node *tree23<Key, Value>::findRemovalStartNode(Key key, std::stack<int>& child_indecies,\
                                                                                                                 int& found_index) const noexcept
{
  found_index = Node::NotFoundIndex;

  Node *premove_start = root.get();
  
  int child_index; 

  while(!premove_start->NodeDescentSearch(key, found_index, child_index)) { // Search for key until found, or we reach a leaf and it is not found when we simply return.

    if (premove_start->isLeaf()) {

        return nullptr;
    } 
        
    child_indecies.push(child_index); // ...remembering which child node branch we took.

    premove_start = premove_start->children[child_index].get();
  }  
 
  return premove_start;
}
/*
 Finds the in order successor of pnode, which must be an internal node.
 
 Input:
 1. pnode must be an internal node.
 2. found_index is the index of the key being removed within pnode->keys[].
 3. reference to child_indecies (see Output). 
 Output:
 1. pointer to leaf node of in order successor
 2. child_indecies traces the descent route to the in order successor.
 3. child_indecies is the stack of indecies into keys[] tracing the descent from the root to the internal node pnode. 
*/

template<class Key, class Value> inline typename tree23<Key, Value>::Node* tree23<Key, Value>::getSuccessor(Node *pnode, int found_index, \
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
template<class Key, class Value> inline void tree23<Key, Value>::Node::removeLeafKey(Key key) noexcept
{
  
  if (isThreeNode() && key == keys_values[0].key()) {

      keys_values[0] = std::move(keys_values[1]);   // removes keys_values[0].key()
      
  }  // ...otherwise, we don't need to overwrite keys_values[0].key(); we just decrease totalItems. 

  --totalItems;
}
/*
 Overview
 ======== 
 fixTree is called when a node has become empty, and the tree therefore needs to be rebalanced. It is initially called when a leaf node becomes empty. It first
 attempts to barrow a key from a 3-node sibling. silbingHasTwoItems() is first called to determine if any 3-node sibling exists. If one does, it calls barrowSiblingKey(),
 which will supply a remove a key/value from sibling, and then shift it left or right so that the tree is re-balanced, and the empty node is filled with a key/value.  
 If no adjacent sibling is a 3-node, a key/value from the parent is brought down and merged with a sibling of pnode. Any non-empty children of pnode are moved to the 
 sibling. Upon return, pnode is deleted from the tree by a calling to unique_ptr<Node>::reset().  
 If the parent of pnode has now become empty (because merge2Nodes was called), a recursive call to fixTree is made.
 Parameters
 ==========
 1. pnode: an empty node, initially a leaf. During recursive calls, pnode is an empty internal 2-node with only one non-nullptr child.  
 2. child_index: The child index in the parent such that 
       pnode->parent->children[child_index] == pnode 
*/

template<class Key, class Value> void tree23<Key, Value>::fixTree(typename tree23<Key, Value>::Node *pnode, std::stack<int>& descent_indecies) noexcept
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

      Node *parent = pnode->parent;

      // child_index is such that: parent->children[child_index] == pnode
      std::unique_ptr<Node> node2Delete;

      if (pnode->parent->isTwoNode()) { 
          /* 
             When the parent is a 2-node, then both pnode's sibling and the parent have one key. We merge the parent's sole key/value with
             pnode's sibling, which is pnode->parent->children[!child_index]. This leaves the parent empty, which we handle recursively below 
             by again calling fixTree(). 
           */
           node2Delete = merge2Nodes(pnode, !child_index); 
    
      } else { 
    
          /* 
           * parent is a 3-node, but has only 2-node children. In this case, we can successfully rebalance the tree. We merge one of the parent keys (and
           * its associated value) with a sibling. This now makes the parent a 2-node. We move the affected children involved appropriately.  We can then
           * safely delete pnode from the tree.
           */
    
         node2Delete = merge3NodeWith2Node(pnode, child_index);
     }
     
     node2Delete.reset(); // delete node2Delete's managed memory.

     if (parent->isEmpty()) { 

          // recurse. parent is an internal empty 2-node with only one non-nullptr child.
          fixTree(parent, descent_indecies);
     }
  }   
}

template<class Key, class Value> inline void tree23<Key, Value>::reassignRoot() noexcept
{
   // The root is a leaf
   if (root->isLeaf()){

      root = nullptr; // also forces the memory held by the unique_ptr<Node> to be deleted.

   } else {
   // recursive remove() case:
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
template<class Key, class Value> void tree23<Key, Value>::barrowSiblingKey(Node *node, int child_index, int sibling_index) noexcept
{
  Node *parent = node->parent; 
  Node *sibling = parent->children[sibling_index].get();

 // If node is an internal node, this implies fixTree() has recursed, and node will have only subtree, one non-nullptr child.
 if (parent->isTwoNode()) {

     // bring down parent key and its associated value. 
     node->keys_values[0] = std::move(parent->keys_values[0]);  

     if (sibling_index < child_index) {  // sibling is to left of node

        parent->keys_values[0] = std::move(sibling->keys_values[1]);

     } else { // sibling is to the right

        parent->keys_values[0] = std::move(sibling->keys_values[0]);

        sibling->keys_values[0] = std::move(sibling->keys_values[1]);
     } 

     node->totalItems = Node::TwoNode;
     sibling->totalItems = Node::TwoNode;

     // Check if leaf node case... 
     if (node->isLeaf()) return;

     // ...or a recursive case 
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

        Node *middleChild = node->parent->children[1].get();

        if (child_index == 0 && sibling_index == 2) {

            // handles two hops left
            node->keys_values[0] = std::move(parent->keys_values[0]);

            node->totalItems = Node::TwoNode;

            parent->keys_values[0]= std::move(middleChild->keys_values[0]);

            middleChild->keys_values[0] = std::move(parent->keys_values[1]); 

            parent->keys_values[1] = std::move(sibling->keys_values[0]);

            sibling->keys_values[0] = std::move(sibling->keys_values[1]);              

            sibling->totalItems = Node::TwoNode;

            // Shift the children appropriately below.
            shiftChildrenLeft(node, middleChild, sibling);
            return;

        } else if (child_index == 2 && sibling_index == 0) {

            // handle two hops
            node->keys_values[0] = std::move(parent->keys_values[1]);
            node->totalItems = Node::TwoNode;

            parent->keys_values[1]= std::move(middleChild->keys_values[0]);
            middleChild->keys_values[0] = std::move(parent->keys_values[0]); 

            parent->keys_values[0] = std::move(sibling->keys_values[1]);

            sibling->totalItems = Node::TwoNode;

            // Shift the children appropriately below.
            shiftChildrenRight(node, middleChild, sibling);
            return;
        }  
     
   // All the one hop causes are handled below, like this one hop example:     
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
           sibling->totalItems = Node::TwoNode;

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
           
            sibling->totalItems = Node::TwoNode;
            break;

         case 2: 

           node->keys_values[0] = std::move(parent->keys_values[1]); 
           parent->keys_values[1] = std::move(sibling->keys_values[0]); 
           sibling->keys_values[0] = std::move(sibling->keys_values[1]);
           sibling->totalItems = Node::TwoNode;
           
           shiftChildrenLeft(node, sibling); 
           break;
    }
  }

  node->totalItems = Node::TwoNode; 
  sibling->totalItems = Node::TwoNode;
  return;
}
/*
 * Does shifting of children left for the one hop case
 * Assumes node and sibling are not leaf nodes. Therefore this is a recurisve call to fixTree()
 */
template<class Key, class Value> void tree23<Key, Value>::shiftChildrenLeft(Node *node, Node *sibling) noexcept
{
  // If node is a leaf, then return.
  if (node->isLeaf()) return;

  // Determine which child of node is non-nullptr.
  int sole_child = node->getSoleChildIndex();

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
template<class Key, class Value> void tree23<Key, Value>::shiftChildrenRight(Node *node, Node *sibling) noexcept
{
  if (node->isLeaf()) return;

  // Determine which child of node is non-nullptr.
  int sole_child = node->getSoleChildIndex();
  
  // If sole_child is 0, then shift it right.
  if (sole_child == 0) {

     node->connectChild(1, std::move(node->children[0]));  
  }

  node->connectChild(0, std::move(sibling->children[2]));  
}
/*
 * Does shifting of children left for the two hop case
 * Assumes node and sibling are not leaf nodes. Therefore this is a recurisve call to fixTree()
 */
template<class Key, class Value> void tree23<Key, Value>::shiftChildrenLeft(Node *node, Node *middleChild, Node *sibling) noexcept
{
  if (node->isLeaf()) return;

  // Determine which child of node is non-nullptr.
  int sole_child = node->getSoleChildIndex();

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
template<class Key, class Value> void tree23<Key, Value>::shiftChildrenRight(Node *node, Node *middleChild, Node *sibling) noexcept
{
  if (node->isLeaf()) return;

  // Determine which child of node is non-nullptr.
  int sole_child = node->getSoleChildIndex();

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
 returns:  the std::unique_ptr<Node> to be deleted.
  
template<class Key, class Value> std::unique_ptr<typename tree23<Key, Value>::Node> tree23<Key, Value>::mergeNodes(Node *pnode, int child_index) noexcept
{
  if (pnode->parent->isTwoNode()) { 
      // 
      // When the parent is a 2-node, then both pnode's sibling and the parent have one key. We merge the parent's sole key/value with
      // pnode's sibling, which is pnode->parent->children[!child_index]. This leaves the parent empty, which we handle recursively by calling
      // fixTree() again. 
      return merge2Nodes(pnode, !child_index); 


  } else { // 

      // 
      // parent is a 3-node, but has only 2-node children. In this case, we can successfully rebalance the tree. We merge one of the parent keys (and
      // its associated value) with a sibling. This now makes the parent a 2-node. We move the effected children involved appropriately.  We can then
      // safely delete pnode from the tree.
      //

    std::unique_ptr<Node> node2Delete = merge3NodeWith2Node(pnode, child_index);
    return node2Delete;
  }
}
 */
/*
 Overview
 ========
 
 Parameters:
 1. pnode is empty 2-node. If it is not a leaf node, it has one child, one subtree; the other child, when pnode is an internal node, is nullptr.
 2. child_index is such that pnode->parent->children[child_index] == pnode
 Requires: The pnode->parent is a 3-node, and all pnode's siblings are 2-nodes.
 Promises: Merges one of the keys/values of pnode->parent with one of pnode's 2-node siblings to rebalance the tree. It shifts the children of the
 effected siblings appropriately, transfering ownership of the sole non-nullptr child of pnode, when pnode is an internal node (which only occurs during
 a recursive call to fixTree()). 
 
 */
template<class Key, class Value> std::unique_ptr<typename tree23<Key, Value>::Node> \
tree23<Key, Value>::merge3NodeWith2Node(Node *pnode, int child_index) noexcept
{
    Node *parent = pnode->parent;

    // If pnode is a leaf, then all children are nullptrs. soleChild is only needed when pnode is an internal node.
    std::unique_ptr<Node> soleChild = (!pnode->isLeaf()) ? std::move(pnode->getNonNullChild()) : nullptr; 

    std::unique_ptr<Node> node2Delete;

    // In all three cases below, we are only moving the parent's grandchildren. We also need to move the immediate children of the
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
          
          parent->children[1]->totalItems = Node::ThreeNode;
          parent->totalItems = Node::TwoNode;

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

          parent->children[0]->totalItems = Node::ThreeNode;
          parent->totalItems = Node::TwoNode;

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

          parent->children[1]->totalItems = Node::ThreeNode;
          parent->totalItems = Node::TwoNode;
    
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
template<class Key, class Value> inline std::unique_ptr<typename tree23<Key, Value>::Node> tree23<Key, Value>::merge2Nodes(Node *pnode, int sibling_index) noexcept
{
  Node *parent = pnode->parent;
   
  Node *sibling = parent->children[sibling_index].get();

  std::unique_ptr<Node> node2Delete = std::move(parent->children[!sibling_index]); 

  if (sibling_index == 1) { // sibling is right child.

      sibling->keys_values[1] = std::move(sibling->keys_values[0]);
      sibling->keys_values[0] = std::move(parent->keys_values[0]);

  } else { // sibling is the left child.

      sibling->keys_values[1] = std::move(parent->keys_values[0]);
  } 

  parent->totalItems = 0;
  
  sibling->totalItems = Node::ThreeNode;

  if (sibling->isLeaf()) {

      return node2Delete; 
  } 

  // Recursive case: This only occurs if fixTreewe adopt the sole child of pnode. The other child was deleted from the tree and so sibling->children[!child_index] == nullptr.
  std::unique_ptr<Node>& nonemptyChild = pnode->getNonNullChild();

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
  Node *pleaf = find(key, index, root.get()); // How do we determine if it wasn't in tree? Should I return pair<bool, Node *>.
  if (!pleaf->isLeaf()) {
       return;  // already exists.
  }
  // Further test that it is not in the leaf
}
*/
#endif
