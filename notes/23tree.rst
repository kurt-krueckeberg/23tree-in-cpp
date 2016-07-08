.. include:: ../include/html-entities.txt

.. role:: kurt-code

Implementing a 2 3 Tree in C++14
================================

2 3 Tree Discussions
--------------------

The following sources discuss 2 3 Trees: 

1. `Balanced Trees <http://algs4.cs.princeton.edu/33balanced/>`_ 
2. `Data Structures Balanced Trees <https://www.cse.unr.edu/~mgunes/cs302/Chapter19-BalancedSearchTrees.ppt>`_ 
3. `Deletion in 2 3 trees <http://www-bcf.usc.edu/~dkempe/CS104/11-19.pdf>`_
4. `Virgina Tech 2 3 Tree slides <http://courses.cs.vt.edu/cs2606/Fall07/Notes/T05B.2-3Trees.pdf>`_

The insertion algorithm is based on the 4-node technique discussed in #1. The delete algorithm is based on #2 and #3.

Nested Class tree23<Key, Value>::KeyValue
-------------------------------------------

The key and value are stored in a KeyValue object. KeyValue has both a move assignement and move constructor to improve the efficiency of the tree insertion
algorithm.

.. code-block:: cpp 

   class KeyValue { 
     public:
      Key   key;
      Value value;
      KeyValue() = default;
      KeyValue(Key k, Value&& v) : key{k}, value{std::move(v)} {} 
      KeyValue(Key k, const Value& v) : key{k}, value{v} {} 
 
      KeyValue(KeyValue&& lhs) : key{lhs.key}, value{std::move(lhs.value)} {} 
 
      KeyValue& operator=(KeyValue&& lhs)
      {
         key = lhs.key;
         value = std::move(lhs.value);
      }
  
      KeyValue& operator=(KeyValue& lhs) = default; 
    };
 
 
Node23 nested class
--------------------

2 3 tree nodes are of type `unique_ptr<Node23>`, where Node23 is a nested class that contains two stdLLarrays: `std::array<KeyValue, 2> keys_values` and
`std::array<std::unique_ptr<Node23>, 3> children`.  When a Node23 object represents a 2-node, the left subtree of smaller keys is rooted at 
`children[0]` and the right subtree of larger keys is rooted at `children[1]`. When a Node23 represent a 3-node, `children[0]` is the left subtree, `children[1]` is the middle subtree
containing keys greater than `keys_values[0].key` but less than `keys_values[2].key`, and `children[2]` is the right subtree containing all keys
greater than `keys_values[2].key`.

.. code-block:: cpp 
 
     class Node23 {
  
          friend class tree23<Key, Value>;             
  
       public:   
          Node23(Key key, const Value& value, Node23 *ptr2parent=nullptr);
          Node23(Node4&);
          Node23(const Node23&) = delete;// All nodes in the tree are of type unique_ptr<Node23>; therefore, we disallow assignment and copy construction...
          Node23& operator=(const Node23&) = delete; 
  
          Node23(Node23&&); // ...but we allow move assignment and move construction.
          Node23& operator=(Node23&&);
  
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
            
             void convertTo2Node(Node4& node4) noexcept; 
  
             void convertTo3Node(Key key, const Value& value, std::unique_ptr<Node23> pnode23) noexcept; 
  
             bool NodeDescentSearch(Key value, int& index, Node23 *next) noexcept;          // called during find()  
             bool NodeDescentSearch(Key value, int& index, int& next_child_index) noexcept; // called during insert()
  
             void insertKeyInLeaf(Key key, const Value& value);
             void insertKeyInLeaf(Key key, Value&& new_value);
        }; 

isLeaf() compares both children[0] and children[1] for nullptr. Both comparisons are needed because during the deletion algorithm, a node might have
only one subtree, with the other child being set to nullptr.
  
Node4 nested class
------------------

The nested Node4 class is used to aid insertion of new keys. Its constructor 

<show here its ctor>

automatically sorts the keys of a 3-node and a new key being insert into the 3-node. The Node4 is later "split" during `split(...)`.

test_invariant
--------------

The test_invariant() methods test both the ordering of the tree, but also the ordering of the keys within 3-nodes, as well as the parent pointer in
each node. Any violations result in a message following the display of the node's keys. It call severl `test_xxx_invariant()` methods of Node23.
 
Search
------

An iterative algorithm is used for search.

<show search code>
 
 
Traversal
---------

Recursive algorithms are used to search in pre order, in order and post order. They are template methods that take a functor that overloads the function
call operator.

<show code>

There is also a level order traversal teampltate methods

<show code>
 
Insertion
---------

Make it short an sweet
 
TODO: Use a working example that is taken from printing the output of insert during various stages to better illustrate the algorithm. Also, mention
that is relies on the 4-node technique described by Sedgwich at <link here>

Insertion begins at the leaf node where the search for the new key terminated. 

<example>


If the leaf is a 2-node, we simply insert the new key and its associated value into the leaf, and we are done. If the leaf node is a 3-node, we create
a 4-node on the stack using a constructor that take the 3-node leaf and the new key as input. The 4-node ctor automatically sorts
all three keys.

<show 4-node ctor here>

We then "split" the 4-node into two 2-nodes: the smaller node holding the keys_values[0], the larger holding kyes_values[2]. This is done in 
split(...).

<show split code here>


The code to downsize the leaf node is the convertTo2Node

<show convertTo2Node here>

Only the large 2-node, however, is allocated from the heap. The other, smaller 2-node is simply the leaf node downsized from a 3-node to a 2-node.

<show 3-node downsize method here>


Next split() considers three cases...If the parent is a 2-node, we convert it to a 3-node. This rebalances the tree

<show the convertTo3Node() method here>

If the parent is an internal 3-node (as will be the case as long a the parent is not the root), we recurse by calling split() again, passing...

[describe parameters passed to split() here...describe the use use of the descend stack as well

Finally, there is one other case: when the parent is the root. In this case CreateNewRoot() is called.

[describe CreateNewRoot here]

The middle value from the 4-node is next pushed up to the parent node. If it is a 3-node 



Deletion
--------


Complete Code
-------------

.. code-block:: cpp

    #include <array>
    #include <memory>
    #include <queue>
    #include <utility>
    #include <stack>
    #include <sstream>
    #include <ostream>  
    #include <exception>
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
    
      class KeyValue { 
        public:
         Key   key;
         Value value;
         KeyValue() = default;
         KeyValue(Key k, Value&& v) : key{k}, value{std::move(v)} {} 
         KeyValue(Key k, const Value& v) : key{k}, value{v} {} 
    
         KeyValue(KeyValue&& lhs) : key{lhs.key}, value{std::move(lhs.value)} {} 
    
         KeyValue& operator=(KeyValue&& lhs)
         {
            key = lhs.key;
            value = std::move(lhs.value);
         }
     
         KeyValue& operator=(KeyValue& lhs) = default; 
       };
     
       class Node23 {
    
            friend class tree23<Key, Value>;             
    
         public:   
            Node23(Key key, const Value& value, Node23 *ptr2parent=nullptr);
            Node23(Node4&);
            Node23(const Node23&) = delete;// All nodes in the tree are of type unique_ptr<Node23>; therefore, we disallow assignment and copy construction...
            Node23& operator=(const Node23&) = delete; 
    
            Node23(Node23&&); // ...but we allow move assignment and move construction.
            Node23& operator=(Node23&&);
    
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
              
               void convertTo2Node(Node4& node4) noexcept; 
    
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
    
            /* Constructor for a leaf node, all child pointers will be zero. */
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
    
        // insert() subroutines
        int findInsertNode(Key new_key, std::stack<int>& descent_indecies, Node23 *&pinsert_start) const noexcept;
    
        void CreateNewRoot(Key new_key, const Value& new_value, std::unique_ptr<Node23> leftChild, std::unique_ptr<Node23> rightChild) noexcept;  
       
        void CreateRoot(Key key, const Value& value) noexcept;
    
        void split(Node23 *current, Key new_key, const Value& new_value, std::stack<int>& child_indecies, \
                std::unique_ptr<Node23> heap_2node) noexcept;
    
        // remove() subroutines
        Node23* findRemovalStartNode(Key key, std::stack<int>& child_indecies, int& found_index) const noexcept;
    
        Node23 *getSuccessor(Node23 *pnode, int found_index, std::stack<int>& child_indecies) const noexcept;
    
        void fixTree(Node23 *pnode, std::stack<int>& child_indecies) noexcept;
    
        void reassignRoot() noexcept;
    
        void barrowSiblingKey(Node23 *pnode, int child_index, int sibling_index) noexcept;
     
        // called by barrowSiblingKey
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
    
      public:
        // Implement this later
        /*
        class iterator { // in order iterator
             const tree<Key, Value>& tree;
             //std::stack<Node23 *>  stack;
             iterator(const tree<Key, Value)& lhs) : tree{lhs} {}
             Node23<Key, Value> *current;
          public:
             bool operator==(const iterator& lhs) const;
             bool operator!=(const iterator& lhs) const;
             iterator& operator++();
             iterator operator++(int);
             Value& operator*();
        };
        */
        tree23() noexcept;
    
        void test_invariant() const noexcept;
    
        std::ostream& test_height(std::ostream& ostr) const noexcept;
    
        std::string test_invariant(const Node23& p) const noexcept; // new
    
        tree23(const tree23&) noexcept; // TODO: Not implemented
        tree23& operator=(const tree23&) noexcept; // TODO: Implemented?
    
        tree23(tree23&&);
    
        tree23& operator=(tree23&&);
    
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
    
    // move constructor
    template<class Key, class Value> tree23<Key, Value>::Node23::Node23(Node23&& node23) : parent{node23.parent}, totalItems{node23.totalItems}
    {
      move_keys_values(node23);
      
      move_children(node23); 
    }
    // move assignment operator
    template<class Key, class Value> typename tree23<Key, Value>::Node23& tree23<Key, Value>::Node23::operator=(Node23&& node23)
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
    
                ostr << keys_values[i].key;
    
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
    
        
             ostr << "{" << keys_values[i].key << "}, ";
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
    
                ostr << keys_values[i].key;
    
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
    Parameter requirements:
    
    1. p3node is an internal node created during a recursive call to split(), not during the initial call to split when a leaf node is passed.
    
    2. child_index is such that
    
       p3node->children[child_index].get() == "the lower prior 3-node that was downsized to a 2-node (holding the smallest value of the paralell 4-node).
    
    3. new_key (and new_value) pushed up from the prior call to split.
    
       new_key == node4.keys_values[1].key
       new_value == node4.keys_values[1].value
    
    4. heap_2node is the 2-node allocated on the heap in the prior call to split. 
    
    Overview:
    
    child_index is such that
    
       p3node->children[child_index].get() == "the lower prior 3-node that was downsized to a 2-node (holding the smallest value of the paralell 4-node).
    
    p3node is an internal 3-node that is the parent of the prior 3-node--initially either a leaf 3-node or later another internal 3-node child in the chain of 3-nodes 
    children already processed by split(). p3node is an internal node, and child_index is used to:
    
    1.) determine the index to use in inserting new_key into Node4::keys[], and 
    2.) to maintain the same general child relationships in the 4-node that existed within the 3-node. We know, for example, that heap_2node will always be to the 
    right the previous p3node. We also use child_index to determine where heap_2node should be placed in Node4::children[]. 
    
    new_key (and new_value) are pushed these values
    
       new_key == node4.keys_values[1].key
       new_value == node4.keys_values[1].value
    
    pushed up from the prior call to split()
    
    heap_2node is the 2-node allocated on the heap in the prior call to split when the 4-node created on the stack was split into two 2-nodes. heap_2node is the
    larger of those two 2-nodes. 
    */
    
    template<class Key, class Value> tree23<Key, Value>::Node4::Node4(Node23 *p3node, Key new_key, const Value& new_value, int child_index, \
      std::unique_ptr<Node23> heap_2node) noexcept : parent{p3node->parent} 
    {
      switch(child_index) {
     
          case 0:
    
          {
            keys_values[0].key = new_key; // new_key is the smallest value, so we put in the first position... 
            keys_values[0].value = new_value;
    
            //...followed by the current p3node's keys and values
            for(auto i = 0; i < Node23::ThreeNodeItems; ++i) {
     
                  keys_values[i + 1] = p3node->keys_values[i]; // TODO: Can move assignment be used safely to increase code efficiency? 
            } 
      
            connectChild(0, std::move(p3node->children[0])); // This is still the smallest child. It is the converted 3-node downsize to a 2-node
                                                                
            connectChild(1, std::move(heap_2node));   // This is the next smallest child.
    
            connectChild(2, std::move(p3node->children[1])); // The others just sort of shift into the final two slots.  
            connectChild(3, std::move(p3node->children[2]));   
    
          }
          break;
          
          case 1: // If child_index = 1, then new_key < p3node.keys_values[1].key && new_key > p3node.keys_values[0].key
    
          {  
            keys_values[0] = p3node->keys_values[0];
     
            keys_values[1].key = new_key;
            keys_values[1].value = new_value; 
            
            keys_values[2] = p3node->keys_values[1];
    
            // children get moved in this manner to maintain the same relationships as those that existed in p3node
            connectChild(0, std::move(p3node->children[0]));
            connectChild(1, std::move(p3node->children[1]));
            connectChild(3, std::move(p3node->children[2]));
        
            connectChild(2, std::move(heap_2node)); // heap_2node's key is larger than the downsized 3-node in p3node->children[0], but less than its next child.
          }
          break;
    
          case 2: // If child_index == 2, then new_key > p3node->keys_values[1].key and so...
    
          { 
             for(auto i = 0; i < Node23::ThreeNodeItems; ++i) {   
                                   
                   keys_values[i] = p3node->keys_values[i]; 
             } 
        
             keys_values[2].key = new_key; // new_key is the largest value in 4-node
             keys_values[2].value = new_value;
        
             for(auto i = 0; i < Node23::ThreeNodeChildren; ++i) { // connect p3node's current children in the same order 
        
                connectChild(i, std::move(p3node->children[i]));
             }
        
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
            
            //--if (!current->isLeaf()) {
            if (current != nullptr && !current->isLeaf()) {
    
                if (current->totalItems == 0) { // This can happen only during remove() when an internal 2-node becomes empty temporarily...
    
                       //...when only and only one of the empty 2-nodes children will be nullptr. 
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
    
    template<class Key, class Value> inline bool tree23<Key, Value>::find(Key key) const noexcept
    {
      if (root == nullptr) {
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
    /*
      int found_index = Node23::NotFoundIndex;
      Node23 *pnode; 
      Node23 *next = root.get();
    
      do { // Search for new_key until we encounter a leaf
    
        pnode = next;
    
        if (pnode->NodeDescentSearch(key, found_index, next)) 
            return true;
    
      } while (!pnode->isLeaf());
    
      return false;
    */
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
    
      if (found_index != Node23::NotFoundIndex) {
    
          // The new_key already exists. Overwrite its associated value with the new value.
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
    Pseudo code: From:  http://staff.columbiacollege.bc.ca/neng/CSC%20225/Chapter19-BalancedSearchTrees.ppt
    and http://algs4.cs.princeton.edu/33balanced/
    
    Parameters:
    
    1. child_indecies holds the indecies use by the 4-node constructor
    
        Node4::Node4(Node23 *p3node, Key new_key, const Value& new_value, int child_index, std::unique_ptr<Node23> heap_2node)
    
    where child_child is such that
    
        p3node->children[child_index].get() == "the prior 3-node into which a value was inserted."
    
    It is now a 2-node holding the smallest value of its paralell 4-node, node4.keys_values[0].key.
     
    2. heap_2node is either nullptr or a newly allocated 2-node created in a prior call to split(). On the initial call it will hold nullptr
       and will not be used.  
    
    Algorithm overview:
    
    split() is a recursive method. It terrminates when a 2-node is encountered, or a 3-node root is eventually split, causing the tree to grow upward
    one level.  We split a 3-node by creating a 4-node on the stack. It holds both the 3-node's keys and values and the new key and value inserted. If
    p3node is a leaf, node4's children are all nullptrs. If p3node is an internal node, we pop its child index from the child_indecies stack<in>. "child
    index" is the index such that 
      
        p3node->children[child_index].get() == "the previous level's 3-node that was just processed by split into which a new key and value were also
                                                inserted."
       
    After creating the 4-node, we "split" it into two 2-nodes:  
    
      1. We convert the 3-node p3node into a 2-node holding only node4.keys_values[0].key and node4.keys_values[0].value, and whose children were the
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
            Let p be the parent of p3node
         
           Create a 4-node node4 on the stack representing the inserted key and value and transferred ownership of pnode's three children. 
     
           From the 4-node create two nodes n1 and n2, so that p is the parent.
     
           Downsize p3node from a 3-node to a 2-node. It will hold the smallest key in node4.
           Give n2 the item  wth the large value in node4. n2 is allocated on the heap.
     
           Connect the two left most children of node4 to n1
           Connect the two right most children of node4 to n2
         
           Move the item with the middle value in p4node up to p
     
           if (p now has three items) {
     
              split(p);
           }
     }
    */
    template<class Key, class Value> void tree23<Key, Value>::split(Node23 *p3node, Key new_key, const Value& new_value, std::stack<int>& child_indecies, \
        std::unique_ptr<Node23> heap_2node)  noexcept
    {
      // get the actual parent              
      Node23 *parent = p3node->parent;
      
      // Debug only next line:
      Node23 *pheap_2node = heap_2node.get(); // debug-only line?
          
      // Create 4-node on stack that will aid in splitting the 3-node that receives new_key (and new_value).
      Node4 node4;
    
      int child_index;
     
      if (p3node->isLeaf()) { // p3node->isLeaf() if and only if heap_2node == nullptr
    
          node4 = Node4{p3node, new_key, new_value}; // We construct a 4-node from the 3-node leaf. The = invokes move assignment--right? 
    
      } else { // It is an internal leaf, so we need to get its child_index such that:
               // p3node == p3node->parent->children[child_index].
    
          child_index = child_indecies.top();
          child_indecies.pop();
    
          node4 = Node4{p3node, new_key, new_value, child_index, std::move(heap_2node)}; 
      }
       
      /* 
         Next we check if the 3-node, p3node, is the root. If so, we create a new top level Node23 and make it the new root.
         If not, we use node4 to 
        
          1.) We downsize p3node to a 2-node, holding the smallest value in the 4-node, node4.keys_values[0], and we connect the two left most
              children of node4 as the two children of p3node. The code to do this is
        
                 p3node->convertTo2Node(node4); 
        
          2.) We allocate a new Node23 2-node on the heap that will hold the largest value in node4, nod4.keys_values[2]. Its two children will be the
              two right most children of node4. The code to do this this is the Node23 constructor that takes a Node4 reference as input.
    
              std::unique_ptr<Node23> larger_2node{std::make_unique<Node23>(node4)}; 
       */
      p3node->convertTo2Node(node4); 
    
      // 2. Create an entirely new 2-node that contains the largest value in node4, node4.keys_values[2].key, and whose children are the two right most children of node4
      //    the children of p3node. This is what the Node23 constructor that takes a Node4 does.
      std::unique_ptr<Node23> larger_2node{std::make_unique<Node23>(node4)}; 
      
      if (p3node == root.get()) {
    
           // We pass node4.keys_values[1].key and node4.keys_values[1].value as the key and value for the new root.
           // p3node == root.get(), and p3node is now a 2-node. larger_2node is the 2-node holding node4.keys_values[2].key.
            
           CreateNewRoot(node4.keys_values[1].key, node4.keys_values[1].value, std::move(root), std::move(larger_2node)); 
    
      } else if (parent->isTwoNode()) { // Since p3node is not the root, it has a parent that is an internal node. We check if is a 2-node.
    
          // If it is, we convert it to a 3-node by inserting the middle value into the parent, and passing it its new third child.
          parent->convertTo3Node(node4.keys_values[1].key, node4.keys_values[1].value, std::move(larger_2node));
    
      } else { // parent is a 3-node, so we recurse.
    
         // parent now has three items, so we can't insert the middle item. We recurse to split it.
         split(parent, node4.keys_values[1].key, new_value, child_indecies, std::move(larger_2node)); 
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
    
       //2. Release the current root, so that it does not inadvertanely get deleted during a move(). It will be the leftChild or new_root.
       std::unique_ptr<Node23> leftChild { currentRoot.release() };
     
       //3. connect left and right children.
       new_root->connectChild(0, std::move(leftChild));
       new_root->connectChild(1, std::move(rightChild));
    
       //4. Make new_root the actual root.
       root = std::move(new_root);
    
       //5. increase tree's height
       ++height;
        
      // DEBUG
      /*
      std::cout << "Doing printlevelOrder after splitting of the root. Should see at least two levels to tree\n";
      printlevelOrder(std::cout);
      std::cout << std::flush; 
      */ 
    }
    
    /*
      This method converts a 3-node into a 2-node
      Note: parent node is already correct and does not need to be set.
    */
    template<class Key, class Value> void tree23<Key, Value>::Node23::convertTo2Node(Node4& node4) noexcept
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
          pLeaf = getSuccessor(premove_start, found_index, descent_indecies); 
              
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
    
    template<class Key, class Value> inline typename tree23<Key, Value>::Node23* tree23<Key, Value>::getSuccessor(Node23 *pnode, int found_index, \
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
    
          //DEBUG START
          
           std::cout << "\ntest_invariant() called. mergeNodes() will be called.\n";
           test_invariant();
           std::cout << std::flush;
           
           //END DEBUG   
    
          // child_index is such that: parent->children[child_index] == pnode
    
          std::unique_ptr<Node23> node2Delete = mergeNodes(pnode, child_index); 
    
          node2Delete.reset(); 
    
           if (parent->isEmpty()) { 
    
              //DEBUG START
              
              std::cout << "\n\nCalling test_invariant() after return from mergeNodes() because node's parent is now empty, too.\n" << std::endl;
    
              test_invariant(); 
              std::cout << std::flush;
              
              //DEBUG END     
    
              // recurse. parent is an internal empty 2-node with only one non-nullptr child.
              fixTree(parent, descent_indecies);
          }
      }   
    }
    // TODO: Does this method also need to adopt a sole child of the now empty root, as is done in barrowSilbingKey() and merge3NodeWith2Node()? 
    template<class Key, class Value> inline void tree23<Key, Value>::reassignRoot() noexcept
    {
       // case 1: If the root has a sole non-empty child, make it the new root. unique_ptr's assignment operator will first delete the current empty root
       // node pointer.
       // In the case that the root is a leaf node, we simply assign
       if (root->isLeaf()){
    
          root = nullptr;
    
       } else {
    
          root = std::move(root->getNonNullChild());  // recursive remove() case
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
     // TODO: If node is an internal node, this implies fixTree has recursed, and node will have one non-nullptr child--right?
     // Question: There if node is an internal node, doesn't its sole non-nullptr child need to adopted?
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
         // TODO: Double check the 3-node parent case code below. 
         /*
            Determine if there are two hops to the sibling from which we will barrow a key. Two hops can only occurs when child_index is 0, and the sibling_index is 2,
            or vice versa. 
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
     requires: 
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
             When the parent is a 2-node, then both pnode's sibling and the parent have one key. We merge the sole parent key/value with
             pnode's sibling, which is in pnode->parent->children[!child_index]. This leaves the parent empty, which we solve recursively,
             by calling fixTree() again. 
           */
           return merge2Nodes(pnode, !child_index); 
    
    
      } else { // parent is a 3-node, but has only 2-node children. 
    
          /* 
             In this case, we can successfully rebalance the tree. We merge one of the parent keys (and its associated value) with a sibling. This now makes the parent
             into a 2-node. We move the effected children involved appropriately.  We can then safely delete pnode from the tree.
             
             There are three cases to consider in choosing which sibling to chose with which the parent will merge one of its keys( and the key's associated value). 
           */
    
        // The  "the sole child of the previously empty 2-node". In the recursive case, the previously empty node was a 2-node
    
        // The line below if only needed if pnode is not a leaf, i.e., this is not a recursive case. 
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


