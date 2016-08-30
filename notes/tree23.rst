 include:: ../include/html-entities.txt

.. role:: kurt-code

Implementing a 2 3 Tree in C++14
================================

2 3 Tree Discussions
--------------------

The following sources discuss 2 3 Trees and their algorithms: 

1. `Balanced Trees <http://algs4.cs.princeton.edu/33balanced/>`_ 
2. `Data Structures Balanced Trees <https://www.cse.unr.edu/~mgunes/cs302/Chapter19-BalancedSearchTrees.ppt>`_ 
3. `Balanced Search Trees <http://www.cs.sfu.ca/CourseCentral/225/ahadjkho/lecture-notes/balanced_trees.pdf>`_
4. `2 3 Trees <http://ee.usc.edu/~redekopp/cs104/slides/L19_BalancedBST_23.pdf>`_
5. `Deletion in 2 3 trees <http://www-bcf.usc.edu/~dkempe/CS104/11-19.pdf>`_
6. `Virgina Tech 2 3 Tree slides <http://courses.cs.vt.edu/cs2606/Fall07/Notes/T05B.2-3Trees.pdf>`_
7. `STL's Red-Black Trees <http://www.drdobbs.com/cpp/stls-red-black-trees/184410531>`_

Implementation Overview
-----------------------

Nested Class tree23<Key, Value>::KeyValue
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The key and value are stored in a KeyValue object. KeyValue has a move assignement and move constructor to improve the efficiency of the tree insertion
algorithm, but is does not have.

.. code-block:: cpp 

   class KeyValue { 
     public:
      Key   key;
      Value value;
      KeyValue() = default;
      KeyValue(Key k, Value&& v) : key{k}, value{std::move(v)} {} 
      KeyValue(Key k, const Value& v) : key{k}, value{v} {} 
      KeyValue(const KeyValue& lhs) = default; 
 
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

2 3 tree nodes are of type ``unique_ptr<Node23>``, where Node23 is the nested class shown below that contains two **std::arrays**: ``std::array<KeyValue, 2> keys_values``
and ``std::array<std::unique_ptr<Node23>, 3> children[]``.  When a Node23 object represents a 2-node, the left subtree of smaller keys is rooted at ``children[0]`` and
the right subtree of larger keys is rooted at ``children[1]``. When a Node23 represent a 3-node, ``children[0]`` is the left subtree, ``children[1]`` the middle subtree
that contains keys greater than ``keys_values[0].key`` but less than ``keys_values[2].key``, and ``children[2]`` is the right subtree containing all keys greater than
``keys_values[2].key``.

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
         
             int totalItems; // set using either Node23::TwoNodeItems or Node23::ThreeNodeItems
  
             void connectChild(int childIndex, std::unique_ptr<Node23> child)  noexcept;
             void connectChild(std::unique_ptr<Node23>& dest, std::unique_ptr<Node23> src)  noexcept;
            
             void convertTo2Node(Node4& node4) noexcept; 
  
             void convertTo3Node(Key key, const Value& value, std::unique_ptr<Node23> pnode23) noexcept; 
  
             bool NodeDescentSearch(Key value, int& index, Node23 *next) noexcept;          // called during find()  
             bool NodeDescentSearch(Key value, int& index, int& next_child_index) noexcept; // called during insert()
  
             void insertKeyInLeaf(Key key, const Value& value);
             void insertKeyInLeaf(Key key, Value&& new_value);
        }; 

**Note:** Method ``isLeaf()`` checks that both ``children[0]`` and ``children[1]`` are ``nullptr`` since checking only ``children[0]`` is insufficient during ``remove(Key key)``
when a node might have only one subtree that, for example, is rooted at ``children[1]``, while the subtree at ``children[0]`` is ``nullptr``.
  
Node4 nested class
^^^^^^^^^^^^^^^^^^

The nested Node4 class is used during insertion only. Its two constructors automatically sort the keys of its input parameters. When the input parameters are an internal 3-node, 
this particular constructor is used: 

.. code-block:: cpp

    template<class Key, class Value> tree23<Key, Value>::Node4::Node4(Node23 *p3node, Key key, const Value& value, int child_index, std::unique_ptr<Node23> heap_2node) noexcept;

The constructor takes ownership of both ``p3node``'s children and ``heap_2node``. ``child_index`` is used to determine the indecies of each adopted child,
where ``child_index`` is the index of the prior, lower-level 3-node that was processed in the immediately-prior call to ``split()``.

Methods
-------

test\_invariant
^^^^^^^^^^^^^^^

The ``test_invariant()`` methods test both the ordering of the tree as well as the parent pointer in each node. Any invariant violations result in a message following
the display of the node's keys. It calls several ``test_xxx_invariant()`` subroutines of ``Node23`` each of which tests various invariant properties.
 
find(Key key)
^^^^^^^^^^^^^

An iterative algorithm rather than a recursive algorithm is used to search for a key.

.. code-block:: cpp

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

Iteration of 2 3 Tree
^^^^^^^^^^^^^^^^^^^^^

By Recursion
~~~~~~~~~~~~

Recursive algorithms, like the in-order traversal code below, can be used to traverse the tree in pre order, in order and post order. In the in order traversal method
below a template method that take a functor class instance that overloads the function call operator.

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
 
There is also a level-order traversal template method that also takes a functor as parameter. In this case, the functor's function call operator must take two arguments:
a ``const Node23&`` and an ``int`` that indicates the current level of the tree.

.. code-block:: cpp
 
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

Using External Iterators
~~~~~~~~~~~~~~~~~~~~~~~~
     
Since the predecessor and successor of any key (except the tree's min and maximum) can always be found, an external bidirectional iterator can be supplied. The iterator
maintains a pointer to the current node, the current index into ``keys_values``, and the current iterator state, where state can be ``beg``, ``end``, or ``in_interval``. 
``end`` is a logical sate representing one-past the last element, ``beg`` represents the first key/value pair, and `in_interval` is the state of not being at ``end`` or beg, a sort of
the in-between state.

tree23's ``begin()`` calls a constructor that sets position to ``beg``, and it calls ``seekToSmallest()`` to set ``current`` and ``key_index`` to the first key. ``end()`
likewise calls a constructor that sets position to ``end``, and it calls ``seekToLargest()`` to set ``current`` and ``key_index`` to the last key.

If the ``position`` is 'beg', ``decrement()`` does a no-op, and none of the member varibles changes. If the ``position`` is 'end' and ``increment()`` is called, it, too,
does a no-op, and none of the member varibles changes. 

.. code-block:: cpp

    class iterator : public std::iterator<std::bidirectional_iterator_tag, typename tree23<Key, Value>::KeyValue> { 
                                                 
       friend class tree23<Key, Value>;   
      private:
         tree23<Key, Value>& tree; 

         const typename tree23<Key, Value>::Node23 *current;

         int key_index;  // The parent's child index such that: current == current->parent->children[child_index]

         iterator_position position;

         void initialize(iterator_position pos); // reuseable constructor code. 

         int getChildIndex(const typename tree23<Key, Value>::Node23 *p) const noexcept;

         std::pair<const Node23 *, int> getSuccessor(const Node23 *current, int key_index) const noexcept;

         std::pair<const Node23 *, int> getInternalNodeSuccessor(const typename tree23<Key, Value>::Node23 *pnode, int index_of_key) const noexcept;

         std::pair<const typename tree23<Key, Value>::Node23 *, int>  getLeafNodeSuccessor(const typename tree23<Key, Value>::Node23 *, int) const noexcept;

         std::pair<const Node23 *, int> getPredecessor(const Node23 *current, int key_index) const noexcept;

         std::pair<const Node23 *, int> getInternalNodePredecessor(const typename tree23<Key, Value>::Node23 *pnode, int index) const noexcept;

         std::pair<const Node23 *, int>  getLeafNodePredecessor(const typename tree23<Key, Value>::Node23 *p, int index) const noexcept;

         std::pair<const typename tree23<Key, Value>::Node23 *, int> findLeftChildAncestor() noexcept;

         // called by constructor 
         void seekToSmallest() noexcept;    
         void seekToLargest() noexcept;    

         iterator& increment() noexcept; // called by operator++() 

         iterator& decrement() noexcept; // called by operator--()

      public:

         explicit iterator(tree23<Key, Value>&); 

         iterator(tree23<Key, Value>& lhs, tree23<Key, Value>::iterator_position);  

         iterator(const iterator& lhs); 

         iterator(iterator&& lhs); 
 
         bool operator==(const iterator& lhs) const;
         bool operator!=(const iterator& lhs) const { return !operator==(lhs); }

         // TODO: KeyValue& is wrong. We don't want to change the key. Should we return pair<Key, Value&> instead? 
         typename tree23<Key, Value>::KeyValue&         dereference() noexcept; 
         const typename tree23<Key, Value>::KeyValue&   dereference() const noexcept; 

         //const typename tree23<Key, Value>::KeyValue&  dereference() const noexcept; // KeyValue& is wrong. We don't want to change the key. How about std::pair<Key, Value&>?

         iterator& operator++() noexcept; 
         iterator operator++(int) noexcept;

         iterator& operator--() noexcept;
         iterator operator--(int) noexcept;
        
         // TODO: Change KeyValue& to pair<const Key, Value&> 
         typename tree23<Key, Value>::KeyValue& operator*() noexcept; // KeyValue& is wrong. We don't want to change the key. How about std::pair<Key, Value&>?

         const typename tree23<Key, Value>::KeyValue& operator*() const noexcept; // KeyValue& is wrong. We don't want to change the key. How about std::pair<Key, Value&>?

         typename tree23<Key, Value>::KeyValue *operator->() noexcept;
    };

    class const_iterator : public std::iterator<std::bidirectional_iterator_tag, const typename tree23<Key, Value>::KeyValue> {
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

         const typename tree23<Key, Value>::KeyValue&  operator*() noexcept; // KeyValue& is wrong. We don't want to change the key. How about std::pair<Key, Value&>?
         const typename tree23<Key, Value>::KeyValue&  operator*() const noexcept; // KeyValue& is wrong. We don't want to change the key. How about std::pair<Key, Value&>?
         const typename tree23<Key, Value>::KeyValue *operator->() const noexcept { return &this->operator*(); } // KeyValue& or pair<Key, Value&>????
    };

    iterator begin() noexcept;  
    iterator end() noexcept;  
  
    const_iterator begin() const noexcept;  
    const_iterator end() const noexcept;  

    reverse_iterator rbegin() noexcept;  
    reverse_iterator rend() noexcept;  
  
    reverse_const_iterator rbegin() const noexcept;  
    reverse_const_iterator rend() const noexcept;  


Reverse iterators are also supported by the rbegin() and rend() methods.
  
Insertion
^^^^^^^^^

To best understand the algorithm, it helps to follow the Insertion slides examples at `Data Structures Balanced Trees <https://www.cse.unr.edu/~mgunes/cs302/Chapter19-BalancedSearchTrees.ppt>`_.  
The insertion algorithm is based on the pseudo code in slides 25 and 26, along with the 4-node technique discussed in `Balanced Trees <http://algs4.cs.princeton.edu/33balanced/>`_.

Insertion begins at the leaf node where the insertion search terminates. As the algorithm descends the tree to the leaf node, the index of each child
branch taken is pushed onto a ``stack<int>``.  If the leaf is a 2-node, we simply insert the new key and its associated value into the leaf, and we are done. However, if
the leaf where the insertion is to begin is a 3-node, as is the case in slide #17 of https://www.cse.unr.edu/~mgunes/cs302/Chapter19-BalancedSearchTrees.ppt, when 38 is
inserted. 

To handle this case, we need to split the 3-node.

split method
~~~~~~~~~~~~

``split(Node23 *p3node, Key new_key, const Value& new_value, std::stack<int>& child_indecies, std::unique_ptr<Node23> heap_2node)`` is passed four parameters: 

1. a 3-node leaf pointer (which is always a leaf node when invoked by ``insert()``)
2. the new key and value
3. the stack of child indecies of the child branches taken descending the tree to the leaf node
4. and an rvalue ``unique_ptr<Node23>`` whose underlying pointer is nullptr. 
   
Neither the stack nor the unique_ptr<Node23> are used when the first parameter is a leaf node. 

``split()`` first creates a 4-node, whose constructor automatically sorts the keys of ``p3node`` and ``new_key``. It sets all four children to nullptr:

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
        //...omitted. See below  
      }
         //...omitted. See below
     } 

Next the 4-node is "split" into two 2-nodes: one that contains the smallest key in ``node4`` and that adopts node4's two left most childre; the other will
contains node4's largest key and adopts node4's two right most children. The smaller 2-node is simply pnode downsized from a 3-node to a 2-node.  
The larger 2-node is allocated on the heap:

.. code-block:: cpp

    pnode->convertTo2Node(std::move(node4)); // takes an rvalue: Node4&&

    std::unique_ptr<Node23> larger_2node{std::make_unique<Node23>(node4)}; 
                                                                          
Next, split attempts to "push" or insert the middle key (and its asoociated value) of node4 in the parent node. There are cases it considers:

1. when pnode is the root, ``CreateNewRoot()`` is called to add a new root node above pnode 

.. code-block:: cpp

      if (pnode == root.get()) {
    
           // We pass node4.keys_values[1].key and node4.keys_values[1].value as the key and value for the new root.
           // pnode == root.get(), and p3node is now a 2-node. larger_2node is the 2-node holding node4.keys_values[2].key.
            
           CreateNewRoot(node4.keys_values[1].key, node4.keys_values[1].value, std::move(root), std::move(larger_2node)); 
      } 

2. when pnode->parent is a 2-node, it calls ``convertTo3Node()`` to rebalance the tree:

.. code-block:: cpp

      else if (parent->isTwoNode()) { // Since p3node is not the root, its parent is an internal node. If it, too, is a a 2-node,
    
          // we convert it to a 3-node by inserting the middle value of node4 into the parent, and passing it the larger 2-node, which it will adopt.
          parent->convertTo3Node(node4.keys_values[1].key, node4.keys_values[1].value, std::move(larger_2node));
      }

3. if the parent is a 3-node, we recurse. The recursion terminates when either of the two above cases is encountered, as will eventually always be the
   case.

.. code-block:: cpp

      else { // parent is a 3-node, so we recurse.
    
         // parent now has three items, so we can't insert the middle item. We recurse to split it.
         split(parent, node4.keys_values[1].key, new_value, child_indecies, std::move(larger_2node)); 
      } 
    
      return;
    } // end of split()

See the source code comments for details on the subroutines ``convertTo3Node()`` and ``CreateNewRoot()`` as well as slides #16 through #26 at `Data Structures Balanced Trees <https://www.cse.unr.edu/~mgunes/cs302/Chapter19-BalancedSearchTrees.ppt>`_
and the insertion examples at `Balanced Trees <http://algs4.cs.princeton.edu/33balanced/>`_, where the use the 4-node is explained. 

Deletion
^^^^^^^^

The deletion algorithm is based on the examples in slides # through # and the pseudo code in slide #.   
