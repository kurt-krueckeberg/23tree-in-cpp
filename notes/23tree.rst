Nested Class tree23<Key, Value>::KeyValue
-------------------------------------------

The key and value data is stored a class KeyValue, which has a move assignement and move constructor to improve the efficiency of the tree insertion
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

The tree nodes are of `unique_ptr<Node23>`, where Node23 is a nested class, which contains two the two arrays `std::array<KeyValue, 2> keys_values` and
`std::array<std::unique_ptr<Node23>, 3> children`.

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
  
  
Node4 nested class
------------------

The nested Node4 class is used during the insertion algorithm

.. code-block:: cpp

    class Node4 { // Class Node4 is used during insert().

       // Always hold three keys and four children. 
      friend class tree23<Key, Value>; 
     
      private:
         std::array<KeyValue, 3> keys_values;

         // Takes ownership of four 2 3-nodes 
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

 
Search
------
 
Insertion
---------
 
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

