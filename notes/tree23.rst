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

Overview
--------

Nested Class tree23<Key, Value>::KeyValue
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
^^^^^^^^^^^^^^^^^^^^

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
^^^^^^^^^^^^^^^^^^

The nested Node4 class is used to aid insertion of new keys. Its constructor 

<show here its ctor>

automatically sorts the keys of a 3-node and a new key being insert into the 3-node. The Node4 is later "split" during `split(...)`.

test_invariant
^^^^^^^^^^^^^^

The test_invariant() methods test both the ordering of the tree, but also the ordering of the keys within 3^nodes, as well as the parent pointer in
each node. Any violations result in a message following the display of the node's keys. It call severl `test_xxx_invariant()` methods of Node23.
 
Search
^^^^^^

An iterative algorithm is used for search.

    template<class Key, class Value> inline bool tree23<Key, Value>::find(Key key) const noexcept
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


Methods
-------
     
Traversal Algorithms
^^^^^^^^^^^^^^^^^^^^

Recursive algorithms are used to traverse the tree in pre order, in order and post order. Each is a template method that take a functor that overloads the function
call operator, and each is an inline method that calls a private method to do the actual work. 

The algorithm is nearly identical to the in order recursive algorithm for a binary tree except when a 3-node is encountered, when the middle child is 
descended after the left child and before its right child. 

Only the in order travesal algorithm is shown below

.. code-block:: cpp


    template<class Key, class Value> template<typename Functor> inline void tree23<Key, Value>::inOrderTraverse(Functor f) const noexcept
    {
       DoInOrderTraverse(f, root);
    }
    
    template<class Key, class Value> template<typename Functor> void tree23<Key, Value>::DoInOrderTraverse(Functor f, const std::unique_ptr<Node23>& current) const noexcept
    {
       if (current == nullptr) { // base case for recursion
    
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
 
There is also a level order traversal template method that takes a functor as parameter. The functor's function call operator must take two arguments:
a `const Node23&` and an `int`, indicating the current level of the tree.
 
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
    
                if (current->totalItems == 0) { // This can happen only during remove() when an internal 2-node can become empty temporarily...
    
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
    
     
Insertion
^^^^^^^^^
    
Insertion begins at the leaf node where the insertion search terminates. As the tree is descended to the leaf, the index of each child branches taken
is pushed onto a stack. 

If the leaf is a 2-node, we simply insert the new key and its associated value into the leaf, and we are done. This is what happens if 39 is inserted 
into the tree below in figues XXX (a). The search terminates at the 2-node containing 40 into which 39 is inserted. 

<insert here a scanned figure with four subfigures showing a showing a working example of insert>

If we next insert 38 figure XXX (b.), the insertion search again terminates at the same leaf node, but now it is a 3-node. To handle this case, we call
split() and pass it four paraemeters: the 3-node leaf pointer, the new key and value, the stack of child indecies of the child branches taken descending
the tree and an rvalue unique_ptr<Node23> whose underlying pointer is nullptr. Neither the stack nor the unique_ptr<Node23> are used when the first
parameter is a leaf node.


split()
~~~~~~~

When ``split`` is first called by ``remove``. p3node is a leaf. split first creates a 4-node, whose constructor automatically sorts the keys of p3node and new_key.
It sets all four children to nullptr:

.. code-block:: cpp

    template<class Key, class Value> void tree23<Key, Value>::split(Node23 *p3node, Key new_key, const Value& new_value, std::stack<int>& child_indecies, \
        std::unique_ptr<Node23> heap_2node)  noexcept
    {
      // get the actual parent              
      Node23 *parent = p3node->parent;
      
      // Create 4-node on stack that will aid in splitting the 3-node that receives new_key (and new_value).
      Node4 node4;
    
      int child_index;
     
      if (p3node->isLeaf()) { 
    
          node4 = Node4{p3node, new_key, new_value}; // We construct a 4-node from the 3-node leaf.
      } else { 
        //...omitted  
      }
      //...omitted
     } 

Next the 4-node is "split" into two 2-nodes: a 2-node that contains the smallest key in node4, and a 2-node that contains node4's largest key. The smaller 2-node
is simply pnode downsized from a 3-node to a 2-node by calling `

     p3node->convertTo2Node(node4); // takes a rvalue: Node4&&

convertTo2Node also connects node4's two left most children as the left and right children of the downsized pnode. The other larger
2-node is allocated on the heap:
    
    std::unique_ptr<Node23> larger_2node{std::make_unique<Node23>(node4)}; 

Its two children are the two right most children of node4. Next, split tests if whether pnode is the root    

      if (pnode == root.get()) {
    
           // We pass node4.keys_values[1].key and node4.keys_values[1].value as the key and value for the new root.
           // pnode == root.get(), and p3node is now a 2-node. larger_2node is the 2-node holding node4.keys_values[2].key.
            
           CreateNewRoot(node4.keys_values[1].key, node4.keys_values[1].value, std::move(root), std::move(larger_2node)); 
    
      } 

and if it is, it allocates a new root by calling CreateNodeRoot(); otherwise, it tests if pnode->parent is a 2-node. If it is, the tree can be rebalanced,
which is done by convertTo3Node.
      
      else if (parent->isTwoNode()) { // Since p3node is not the root, it has a parent that is an internal node. We check if is a 2-node.
    
          // If it is, we convert it to a 3-node by inserting the middle value into the parent, and passing it its new third child.
          parent->convertTo3Node(node4.keys_values[1].key, node4.keys_values[1].value, std::move(larger_2node));
    
      }

Finally, if the parent is not a 2-node, we recurse....      

      else { // parent is a 3-node, so we recurse.
    
         // parent now has three items, so we can't insert the middle item. We recurse to split it.
         split(parent, node4.keys_values[1].key, new_value, child_indecies, std::move(larger_2node)); 
      } 
    
      return;
    }

    
The code to downsize the leaf node is the convertTo2Node
    
<show convertTo2Node here>
    
Only the large 2-node, however, is allocated from the heap. The other, smaller 2-node is simply the leaf node downsized from a 3-node to a 2-node.
    
<show 3-node downsize method here>


Next split() considers three cases...If the parent is a 2-node, we convert it to a 3-node. This rebalances the tree

<show the convertTo3Node() method here>

If the parent is an internal 3-node (as will be the case as long a the parent is not the root), we recurse by calling split() again, passing...

[describe parameters passed to split() here...describe the use use of the descend stack as well

During split() recursion, if an internal node is a 3-node, a 4-node is created on the stack that takes ownership of the 3-node's three children. This
version of the 4-node constructor is only called if `split(....)` recurses and... is a 3-node. The constructor also adopts a unique_ptr<Node23> passed to the constructor.  

<show multi-parameter 4-node ctor here>

Finally, there is one other case: when the parent is the root. In this case CreateNewRoot() is called.

[describe CreateNewRoot here]

The middle value from the 4-node is next pushed up to the parent node. If it is a 3-node 
