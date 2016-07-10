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
     
Traversal Algorithms
--------------------

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
---------
    
Insertion begins at the leaf node where the search for the key to be inserted terminated. As the tree is descended to the leaf, the index of child branches taken
as the tree is descended are pushed onto `std::stack<int> child_indecies`, which is later passed to `split(..put in parameteres..)` should it be called. 

<use slide 17 of walls and mirrors ppt>

If the leaf is a 2-node, we simply insert the new key and its associated value into the leaf, and we are done. This is what happens if 39 is inserted 
into the tree below. The search will terminate at the 2-node containing 40 and into which 39 is then inserted. 

<scan 1. insert 39>

If we next attempt to insert 38 into the tree, the search again terminates at the same leaf node, but now it is a 3-node.

<scan 2. insert 38>

To handle this case, we call split(), and pass it the 3-node leaf pointer, the new key and value, the stack of child indecies of the child branches taken descending the tree and 
a rvalue unique_ptr<Node23> whose underlying pointer is nullptr:

      split(pinsert_start, new_key, new_value, child_indecies, std::unique_ptr<Node23>{nullptr}); 

<TODO next here: Now describe split>


The 4-node constructor takes the 3-node leaf and the new key as input and automatically sorts the keys. 
.  If the leaf node is a 3-node, we call split() to "split" the 4-node into two 2-nodes: the
smaller node holding the keys_values[0], the larger holding kyes_values[2].


split()
+++++++

Parameters to split: the leaf node, the new key and its associated value, the stack of child indecies, and the rvalue `unique_ptr<Node23>{nullptr}`.

.. code-block:: cpp

    /*
    Parameters:
    
    1. child_index holds the index into p3node->children[] such that

        p3node->children[child_index].get() == "the lower level prior 3-node in tree that was just downsized to a 2-node (holding the smallest value of
        the paralell 4-node)."
          
        the 4-node constructor uses to determine which child heap_2node should be.
    
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
