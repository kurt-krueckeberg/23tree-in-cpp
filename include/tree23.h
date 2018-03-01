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
#include <algorithm>
#include <tuple>
#include "debug.h"

template<class Key, class Value> class tree23; // This forward declaration... 

//...is required by these friend functions
template<class Key, class Value> std::ostream& operator<<(std::ostream& ostr, const typename tree23<Key, Value>::Node& node23); 
template<class Key, class Value> std::ostream& operator<<(std::ostream& ostr, const typename tree23<Key, Value>::Node4& node4); 

template<class Key, class Value> class tree23 {
    
  private:

  union KeyValue { // This union is used to hold to two types of pairs, one of has const Key, the other a non-const Key to eliminate...
 
      std::pair<Key, Value>        nc_pair;  // ...constantly casting of const_cast<Key>(p.first) = some_noconst_key;
      std::pair<const Key, Value>  const_pair;  // We return this member of the union when we are not changing the Key in the pair.

     public: 
      KeyValue() {} 
      KeyValue(Key key, const Value& value) : nc_pair{key, value} {}
      
      KeyValue(const KeyValue& lhs) : nc_pair{lhs.nc_pair} {}
      
      KeyValue(Key k, Value&& v) : nc_pair{k, std::move(v)} {} 

      KeyValue(KeyValue&& lhs) :  nc_pair{std::move(lhs.nc_pair)} {}

      KeyValue& operator=(const KeyValue& lhs);  
      KeyValue& operator=(KeyValue&& lhs); 
 
     constexpr Key&  key()  { return nc_pair.first; }
     constexpr const Key& key() const { return const_pair.first; }

     constexpr Value&  value()  { return nc_pair.second; }
     constexpr const Value& value() const { return const_pair.second; }
     
     constexpr std::pair<Key, Value>& pair()  { return nc_pair; }
     
     constexpr const std::pair<const Key, Value>& pair() const { return const_pair; }
          
     friend std::ostream& operator<<(std::ostream& ostr, const KeyValue& key_value)
     {
         ostr << "{" << key_value.key() << ',' <<  key_value.value() <<  "}, ";
	 return ostr;
     }
  };

  class Node4;   // Fwd declaration 

  
   // The tree's heap-allocated Node nodes are managed by std::unique_ptr<Node>s.
    class Node {

        friend class tree23<Key, Value>;             

     public:   
        Node(Key key, const Value& value, Node *ptr2parent=nullptr);
       ~Node()
        { 
           /* For debugging uncomment.
            std::cout << "~Node: [ ";
            for(auto i = 0; i < getTotalItems(); ++i)
                  std::cout << keys_values[i].key() << ',' << std::flush;
            std::cout << ']' << std::flush;
            */ 
         }


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

        constexpr Key& key(int i) { return keys_values[i].key(); }
        constexpr const Key& key(int i) const { return keys_values[i].key(); }

        constexpr Key& value(int i) { return keys_values[i].value(); }
        constexpr const Key& value(int i) const { return keys_values[i].value(); }

        constexpr std::pair<Key, Value>& pair(int i) { return keys_values[i].pair(); }
        constexpr const std::pair<const Key, Value>& pair(int i) const { return keys_values[i].pair(); }

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

        std::pair<bool, int> siblingHasTwoItems(int child_index) const noexcept;

        std::ostream& test_3node_invariant(std::ostream& ostr, const Node *root) const noexcept;

        std::ostream& debug_print(std::ostream& ostr, bool show_addresses=false) const noexcept;

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

           std::tuple<bool, Node *, int> find(Key value) noexcept;    

           void insertKeyInLeaf(Key key, const Value& value);
           void insertKeyInLeaf(Key key, Value&& new_value);
    }; 


  class NodeLevelOrderPrinter {

      std::ostream& ostr;
      int current_level;
      int height;

      void display_level(std::ostream& ostr, int level) const noexcept
      {
        ostr << "\n\n" << "current_level = " <<  current_level << ' '; 
           
        // Provide some basic spacing to tree appearance.
        std::size_t num = height - current_level + 1;
        
        std::string str( num, ' ');
        
        ostr << str; 
      }

      std::ostream& (Node::*pmf)(std::ostream&) const noexcept;

     public: 
        
     NodeLevelOrderPrinter (int hght,  std::ostream& (Node::*pmf_)(std::ostream&) const noexcept, std::ostream& ostr_in): height{hght}, ostr{ostr_in}, current_level{0}, pmf{pmf_} {}

     NodeLevelOrderPrinter (const NodeLevelOrderPrinter& lhs): height{lhs.height}, ostr{lhs.ostr}, current_level{lhs.current_level}, pmf{lhs.pmf} {}

     void operator ()(const Node *pnode, int level)
     { 
         // Did current_level change?
         if (current_level != level) { 
        
             current_level = level;
        
             display_level(ostr, level);       
         }

         (pnode->*pmf)(std::cout);

         std::cout << ' ' << std::flush;
     }
  };

  class Node4 { // Class Node4 is only used to aid the insert() algorithm.

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

      constexpr Key& key(int i) { return keys_values[i].key(); }
      constexpr const Key& key(int i) const { return keys_values[i].key(); }

      constexpr Key& value(int i) { return keys_values[i].value(); }
      constexpr const Key& value(int i) const { return keys_values[i].value(); }

      std::ostream& print(std::ostream& ostr) const noexcept;
      std::ostream& debug_print(std::ostream& ostr) const noexcept;

      constexpr bool isLeaf() const noexcept { return (children[0] == nullptr) ? true : false; } 

      friend std::ostream& operator<<(std::ostream& ostr, const Node4& node4) 
      { 
          return node4.print(ostr); 
      }
  };

    std::unique_ptr<Node> root; 

    int size_; // Number of nodes

    // Subroutines called by insert()
    // called by insert() and remove().
    std::tuple<bool, typename tree23<Key, Value>::Node *, int, std::stack<int>> findNode(Key new_key) const noexcept;

    void CreateNewRoot(Key new_key, const Value& new_value, std::unique_ptr<Node>&& leftChild, std::unique_ptr<Node>&& rightChild) noexcept;  
   
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

    void  merge2Nodes(Node *pnode, int child_index) noexcept;
    void  merge3NodeWith2Node(Node *pnode, int child_index) noexcept;

    void shiftChildren(Node *node, Node *sibling, int node_index, int sibling_index) noexcept;
     
    template<typename Functor> void DoInOrderTraverse(Functor f, const Node *proot) const noexcept;

    template<typename Functor> void DoPostOrderTraverse(Functor f,  const Node *proot) const noexcept;
    template<typename Functor> void DoPreOrderTraverse(Functor f, const Node *proot) const noexcept;

   // Called by copy constructor and copy assignment operators, respectively.
   void CloneTree(const std::unique_ptr<Node>& Node2Copy, std::unique_ptr<Node>& NodeCopy, const Node * parent) noexcept;

   void DestroyTree(std::unique_ptr<Node> &root) noexcept; 

   int  height(const Node *pnode) const noexcept;
   
   int  depth(const Node *pnode) const noexcept;
   bool isBalanced(const Node *pnode) const noexcept;

   bool find_(const Node *pnode, Key key) const noexcept;

   tree23<Key, Value>& clone(tree23<Key, Value>& lhs) noexcept;
   tree23<Key, Value>& move(tree23<Key, Value>&& lhs) noexcept;

  public:
    // For STL compatibility, there are the required container "typedef's"/using.

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

         void initialize(iterator_position pos); // reusable constructor code. 

         int getChildIndex(const typename tree23<Key, Value>::Node *p) const noexcept;

         std::pair<const Node *, int> getSuccessor(const Node *current, int key_index) const noexcept;

         std::pair<const Node *, int> getInternalNodeSuccessor(const typename tree23<Key, Value>::Node *pnode, int index_of_key) const noexcept;

         std::pair<const typename tree23<Key, Value>::Node *, int>  getLeafNodeSuccessor(const typename tree23<Key, Value>::Node *, int) const;

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

         constexpr reference dereference() noexcept 
         { 
             return const_cast<std::pair<const Key, Value>&>(current->keys_values[key_index].pair());  
         }

         constexpr const std::pair<const Key, Value>& dereference() const noexcept { \
                         return const_cast<const std::pair<const Key, Value>&>( current->keys_values[key_index].pair());} 

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

    std::ostream& test_height(std::ostream& ostr) const noexcept;

    tree23(const tree23&) noexcept; 

    tree23& operator=(const tree23&) noexcept; 

    tree23(tree23&&) noexcept; // move constructor

    tree23& operator=(tree23&&) noexcept; // move assignemtn
    
    void insert(Key key, const Value& value);
    bool isEmpty() const noexcept;

    void printlevelOrder(std::ostream&) const noexcept;
    void debug_printlevelOrder(std::ostream& ostr) const noexcept;
    
    void printInOrder(std::ostream& ostr) const noexcept;   

    // TODO: Should emplace() return an iterator?
    //++template<class... Args> void emplace(Key key, Args&&... args); // <-- Implementation not complete.
       
    bool find(Key key) const noexcept;

    const Value& operator[](Key key) const;

    Value& operator[](Key key);

    void remove(Key key);

    // Breadth-first traversal
    template<class Functor> void levelOrderTraverse(Functor f) const noexcept;

    // Depth-first traversals
    template<typename Functor> void inOrderTraverse(Functor f) const noexcept;
    template<typename Functor> void preOrderTraverse(Functor f) const noexcept;
    template<typename Functor> void postOrderTraverse(Functor f) const noexcept;

    int  height() const noexcept;
    bool isBalanced() const noexcept;

    int size() const 
    {
        return size_;
    }

    bool test_invariant() const noexcept;
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
 Only called from tree23<Key,Value>::remove(Key key)'s subroutines when a 2-node being deleted briefly has only one non-nullptr child.
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
template<class Key, class Value> inline bool tree23<Key, Value>::isEmpty() const noexcept
{
  return root == nullptr ? true : false;
}

template<class Key, class Value> std::ostream& tree23<Key, Value>::Node::debug_print(std::ostream& ostr, bool show_addresses) const noexcept
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

    // If we are not in_between...check whether both iterators are at the end...
    if (lhs.position == iterator_position::end && position == iterator_position::end) { 

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

template<class Key, class Value> std::pair<const typename tree23<Key, Value>::Node *, int> tree23<Key, Value>::iterator::getPredecessor(const typename  tree23<Key, Value>::Node *current, int key_index) const noexcept
{
  if (current->isLeaf()) { // If leaf node

     if (current == tree.root.get()) { // root is leaf      

         if (tree.root->isThreeNode() && key_index == 1) {
             
             return {current, 0};
         }
                  
         return {nullptr, 0};
            
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
 const Node *leftChild = pnode->children[key_index].get();

 for (const Node *cursor = leftChild; cursor != nullptr; cursor = cursor->children[cursor->totalItems].get()) {

    pnode = cursor;
 }

 return {pnode, pnode->totalItems - 1}; 
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

      return {pnode, 0}; 
  }

  if (int child_index = getChildIndex(pnode); child_index != 0) { // If pnode is not the left-most child, the predecessor is in the parent

      return  {pnode->parent, child_index - 1}; 

  } else {

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
      const Node *parent = pnode->parent;
      
      // Ascend the parent pointer chain as long as pnode is the left most child of its parent.
      for(; pnode == parent->children[0].get();  parent = parent->parent)  {
      
          // pnode is still the left most child, but if its is the root, we cannot ascend further and there is no predecessor.  
          if (parent == tree.root.get()) {
                
              return {nullptr, 0};  // To indicate this we set current, the member of the pair, to nullptr and key_index, the second member, to 0.
          }
          pnode = parent;
      }
                                                                                                                                                                   
      int index_of_child = pnode == parent->children[1].get() ? 1 : 2; // If parent is a 2-node, then pnode will always be parent->children[1].get().
                                                                    // If parent is a 3-node, we must compare pnode to parent->children[1].get(), because  
                                                                    // if pnode is not the second child, it must be the third child.
                                                                                                                                                          
      // Using index_of_child, we know the key of the next smallest key will be child_index -1.
      return {parent, index_of_child - 1};
  }   
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
    pair<const Node *, int>, first is the node with the next in-order key, and second is the index into keys_values[]. If the last key has already been visited,
    the pointer returned will be nullptr.

 */
template<class Key, class Value> std::pair<const typename tree23<Key, Value>::Node *, int> tree23<Key, Value>::iterator::getSuccessor(const Node *current,\
                                                                                                           int index_of_key) const noexcept
{
  if (current->isLeaf()) { // If leaf node
     
     if (current == tree.root.get()) { // root is leaf      

         if (tree.root->isThreeNode() && key_index == 0) { 

             return {current, 1};
         }
                  
         return {nullptr, 0};
 
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
   The smallest node in the subtree rooted at the right child of the pnode->keys_values[key_index]; i.e., the left most node of the subtree root as the right child.
   Here "right child" means the pnode->children[key_index + 1]

    Note: When a 2 3 tree node is a 3-node, it has two "right" chidren from the point of view of its first key and two "left" children from the point of view of its
    second key.
 */
template<class Key, class Value> std::pair<const typename tree23<Key, Value>::Node *, int> tree23<Key, Value>::iterator::getInternalNodeSuccessor(const typename tree23<Key, Value>::Node *pnode, \
 int key_index) const noexcept	    
{
 const Node *rightChild = pnode->children[key_index + 1].get();

 // Get the smallest node in the subtree rooted at the rightChild, i.e., its left most node...
 for (const Node *cursor = rightChild; cursor != nullptr; cursor = cursor->children[0].get()) {

    pnode = cursor;
 }

 return {const_cast<Node *>(pnode), 0};
}
/*
 Requires:
 1. pnode is a either 2- or 3-node leaf node. 
 2. If pnode is 3-node, then key_index must be one: pnode->keys_values[1].key(), must be 1. It can never be 0, the first key.

 Promises:
  To return the in-order successor represented by the pair { const Node *pnode; int key_index }.
 */
template<class Key, class Value> std::pair<const typename tree23<Key, Value>::Node *, int> tree23<Key, Value>::iterator::getLeafNodeSuccessor(const \
 typename tree23<Key, Value>::Node *pnode, int index_of_key) const 
{
  // If the leaf node is a 3-node and key_index points to the first key, this is trivial: we simply set key_index to 1. 
  if (pnode->isThreeNode() && index_of_key == 0) {

      return {current, 1}; 
  }

  int suc_key_index;

  // Determine child_index such that pnode == pnode->parent->children[child_index]

  /*
   Handle easy cases first:
   1. If child_index is 0, then the successor is pnode->parent->keys_values[0].
   2. If child_index is 1 and parent is a 3-node, the successor is pnode->parent and suc_key_index is 1.
   */
  switch (int child_index = getChildIndex(pnode); child_index) {

      case 0:
           /*
             pnode is either the left most child of either a 2-node or 3-node parent. If pnode is a 3-node, its key_index equals 1 (because if it is was 0,
             this was already handled at the beginning of this method. 
             The possibilities are:

             (a)   [20]       (b) [20]    (c)   [20, 40]       (d) [20,  40]    
                   / \            /  \          /   |  \            /   |  \ 
                [15]  [x]    [15, 18] [x]    [15]  [ ]  [ ]   [15, 18] [ ] [ ]   

            Note: if leaf is a 3-node, key_index is 1. In all four scenarios above, we advance to the first key of the parent. 
          */
         return {pnode->parent, 0};
 
      case 1: 
         /* 
          pnode is either the right child of a 2-node or the middle child of a 3-node. If pnode is a 3-node, key_index equals 1 because if it was a 0, it was
          already handled at the start of this method.  The possibilities look like this

          1.) If parent is a 2-node, there are two possible cases: 

            (a)   [20]    (b)   [20]
                  / \           /  \  
                [x]  [30]    [x]  [30, 32]

           In (a), key_index is 0. In (b), key_index is 1.   

          2.) If parent is a 3-node, there are two possible cases: 

            (c)   [20,  40]    (d)   [20,   40]
                  /   |   \         /    |    \ 
                [x]  [30] [ ]     [x]  [30,32] [ ] 
           
            In (c) above, key_index is 0. In (d), key_index is 1. 
        */ 

         if (pnode->parent->isThreeNode()) { // This is the trivial case, we advance to the 2nd key of the parent 3-node. 

            return {pnode->parent, 1};
         } 

    /* Note: If the parent is a 2-node, we fall through to 'case 2' */

    case 2: 
   /* 
     pnode is the third child of its parent, and pnode is either a 2-node or 3-node; giving us these four possible scenarios: 

       (a)   [20]    (b)   [20]       (c)   [20,   40]     (d)   [20,   40]        
             / \           /  \            /    |    \          /    |    \        
           [x]  [30]    [x]  [30, 32]    [ ]   [ ]   [50]     [ ]   [ ]   [50, 60] 

    In (a), pnode is [30]. In (b), pnode is [30, 32] and key_index is 1. In (c), pnode is [50]. In (d), pnode is [50, 60] and key_index of 1.

    To find the successor in all four cases, we walk up the ancestor chain until we traverse the first left child pointer: the first node that is
    a left child of its parent. That parent contains the successor. However, if we get to the root without finding a node that is a left child, there
    is no successor (because pnode and key_index is the max key).

    Note: In a 2 3 tree, a "left child pointer" isn't always the first child. A "left child pointer" simply means a pointer to a subtree with smaller values than
    the parent. For example, the middle child pointer of a 3-node parent is a "left child pointer" of the 2nd key because all the values of the subtree rooted at
    the middle child are less than the 2nd key of the middle child's parent 3-node. 

    As an example, in the subtree shown below

                  [17,       60]   <-- 3-node
                  /       |     \
                 /        |      \
              [10]       [35]     [70, 100]
             /   \       /  \      /  |  \
           [5]  [15]   [20] [50]  <-- pnode points to leaf node [50]. 
           / \   / \   / \  / \   
          0   0 0   0 0   0 0  0  ... 

      if [50] is the pnode leaf node, the successor of [50] is the second key of the 3-node [17, 60]. When we walk up the parent chain from [50],
      the first left child pointer we encounter is the middle child of the 3-node [17, 60], which is the "left" child of 60, and  [60] is the next largest key.

      The same logic applies to all four possilbe cases (a) through (d) above. For example, for case (b), illustrated in the tree below
 
                  [17,            60]   <-- 3-node
                  /       |         \
                 /        |          \
              [10]       [35]        [70, 100]
             /   \       /  \        /  |  \
           [5]  [15]   [20] [50, 55]             <-- pnode points to key 55 in leaf node [50, 55]. 
           / \   / \   / \  / \   
          0   0 0   0 0   0 0  0  ... 
    
      60 is the successor of 50.
    */
       {
           const Node *parent = pnode->parent;
           
           // Ascend the parent pointer as long as the node continues to be the right most child (of its parent). 
           for(;pnode == parent->getRightMostChild(); parent = parent->parent)  { 
           
               // pnode is still the right most child, but if it is also the root, then, there is no successor (because pnode was the largest node in the tree). 
               if (parent == tree.root.get()) {
                  
                   return {nullptr, 0};  // To indicate "no-successor" we return the pair: {nullptr, 0}.
               }
           
               pnode = parent;
           }
           
           // If pnode is a 3-node, determine if we ascended from the first child, parent->children[0], or the middle child, parent->children[1], and set suc_key_index accordingly. 
           if (parent->isThreeNode()) {

              suc_key_index = (pnode == parent->children[0].get()) ? 0 : 1; 

           } else { // pnode is a 2-node

              suc_key_index = 0;
           }

           return {parent, suc_key_index};
         }

         break;

    default:
       break;

  } // end switch

  throw std::logic_error("An unexpected case occurred in getLeafNodeSuccessor that should never happend");
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

                key_index = pair.second; // current has not changed, but key_index has.
  
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

   case iterator_position::end:
    {
        position = iterator_position::in_between;
        break;
    }

   case iterator_position::in_between: // 'in_between' means current and key_index range from the second key/value in tree and its last key/value.
                                       // 'in_between' corresponds to the inclusive half interval [second key, last key), while 'beg' refers only to
                                       //  first key/value.  
    {      
       if (std::pair<const Node *,int> pair = getPredecessor(current, key_index); pair.first == nullptr) { // If nullptr, there is no successor: current and key_index already point to the first key/value in the tree. 

            // Therefore current doesn't change, nor key_index, but the state becomes 'beg'. 
            position = iterator_position::beg;

       } else {
 
           current = pair.first;
           key_index = pair.second;
       }
    }
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

 Returns std::pair<bool, child_index>, where bool is true is there is a 3-node sibling, and child_index is such that

    pnode->parent->children[child_index] == p3node_sibling
 
 */
template<class Key, class Value> std::pair<bool, int> tree23<Key, Value>::Node::siblingHasTwoItems(int child_index) const noexcept
{
 
 if (parent->isTwoNode()) { // In a recursive case, the parent has 0 totalItems, and it has only one non-nullptr child.

     if (int sibling_index = !child_index; parent->children[sibling_index]->isThreeNode()) { // toggle between 0 or 1

        return {true, sibling_index};

     } else {

        return {false, 0};
     } 
 } 

 /* 
   3-node parent cases below. Determine if any immediate sibling is a 3-node. There will only be two children to inspect when the parent is a 3-node and child_index
   is 1.
   */
  switch (child_index) {
      case 0:

        if (parent->children[1]->isThreeNode()) {
  
            return {true, 1};  
  
        } else if (parent->children[2]->isThreeNode()) {
  
            return {true, 2};  
  
        } 
        break;

      case 1:
        if (parent->children[0]->isThreeNode()) {
  
            return {true, 0};  
  
        } else if (parent->children[2]->isThreeNode()) {
  
            return {true, 2};  
  
        } 
        break;
  
      case 2:
        if (parent->children[1]->isThreeNode()) {
  
            return {true, 1};  
  
        } else if (parent->children[0]->isThreeNode()) {
  
            return {true, 0};  
  
        } 
        break;

      default:
       break; 
  }
  return {false, 0};
}

template<class Key, class Value> inline tree23<Key, Value>::tree23() noexcept : root{nullptr}, size_{0}
{

}   

template<class Key, class Value> inline tree23<Key, Value>::tree23(std::initializer_list<typename tree23<Key, Value>::value_type> list) : size_{0}
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
                              
   destNode = std::make_unique<Node>(srcNode->keys_values, const_cast<Node *>(parent), srcNode->totalItems);

   switch (srcNode->totalItems) {

      case 1: // two node
      {    
            CloneTree(srcNode->children[0], destNode->children[0], destNode.get()); 
            
            CloneTree(srcNode->children[1], destNode->children[1], destNode.get()); 

            break;

      }   
      case 2: // three node
      {
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
template<class Key, class Value> inline tree23<Key, Value>::tree23(tree23<Key, Value>&& lhs) noexcept 
{
    move(std::move(lhs));
}   

// Copy assignment
template<class Key, class Value> tree23<Key, Value>& tree23<Key, Value>::operator=(const tree23<Key, Value>& lhs) noexcept
{
  if (this == &lhs)  {
      
      return *this;
  }
  
  DestroyTree(root); // free all the nodes of the current tree 

  return clone(lhs);
}

template<class Key, class Value> inline tree23<Key, Value>& tree23<Key, Value>::clone(tree23<Key, Value>& lhs) noexcept
{
  size_ = lhs.size_;
    
  CloneTree(lhs.root, root);
  return *this;
}

template<class Key, class Value> inline tree23<Key, Value>& tree23<Key, Value>::move(tree23<Key, Value>&& lhs) noexcept 
{
  root = std::move(lhs.root);  
  size_ = lhs.size_;

  return *this;
}

// Move assignment
template<class Key, class Value> inline tree23<Key, Value>& tree23<Key, Value>::operator=(tree23<Key, Value>&& lhs) noexcept
{
  if (this == &lhs)  {
      
     return *this;
  }

  return move(std::move(lhs));   
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

           keys_values[dest] = std::move(p3node->keys_values[src]); 
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

    child_index is such that p3node->children[child_index].get() == the prior lower level 3-node that was downsized to a 2-node in the immediately-prior call to split.

    child_index is used to:

    1.) determine the index to use in inserting key into Node4::keys[], and 

    2.) to maintain the same general child relationships in the 4-node that existed within the 3-node. We know, for example, that heap_2node
        will always be to the right the previous p3node. We also use child_index to determine where heap_2node should be placed in Node4::children[]. 
        heap_2node is the 2-node allocated on the heap in the prior call to split when the 4-node created on the stack was split into two 2-nodes.
        heap_2node is the larger of those two 2-nodes. 
*/

template<class Key, class Value> tree23<Key, Value>::Node4::Node4(Node *p3node, Key in_key, const Value& in_value, int child_index, \
  std::unique_ptr<Node> heap_2node) noexcept : parent{p3node->parent} 
{
  switch(child_index) {
 
      case 0: // key was pushed up from the 4-node constructed from the child at p3node->children[0] (and another key). It will therefore become the
	     // smallest value in the 4-node.

      {
        key(0) = in_key; // key is the smallest value, so we put in the first position... 
        value(0) = in_value;

        //...followed by the current p3node's keys and values
        for(auto i = 0; i < Node::ThreeNode; ++i) {
 
            keys_values[i + 1] = std::move(p3node->keys_values[i]); 
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
 
        key(1) = in_key;
        value(1) = in_value; 
        
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
    
         key(2) = in_key; // key is the largest value in 4-node
         value(2) = in_value;
    
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
                         the parent pointer does not need to be properly set--I believe--because Node4 only owns the children temporarily, until it is split
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
  since 'this' is of type 'Node4 *'. 
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

       ostr << "{ " << key(i) << ", " << value(i) << "}, "; 
   }
   
   ostr << "] children:  [ ";  
   
   for (auto& pChild : children) {
       
       if (pChild == nullptr) {
           
           ostr << "nullptr, ";   
           
       } else {

         for (auto i = 0; i < pChild->totalItems; ++i) {  
            
            ostr << "{ " << pChild->key(i) << ", " << pChild->value(i) << "}, ";
         } 

       }
   }
   ostr << "]" << std::endl;
   return ostr;
}

template<class Key, class Value> template<typename Functor> inline void tree23<Key, Value>::inOrderTraverse(Functor f) const noexcept
{
   DoInOrderTraverse(f, root.get());
}

template<class Key, class Value> template<typename Functor> inline void tree23<Key, Value>::preOrderTraverse(Functor f) const noexcept
{
   PreInOrderTraverse(f, root.get());
}

template<class Key, class Value> template<typename Functor> inline void tree23<Key, Value>::postOrderTraverse(Functor f) const noexcept
{
   DoPostOrderTraverse(f, root.get());
}

template<class Key, class Value> template<typename Functor> void tree23<Key, Value>::DoInOrderTraverse(Functor f, const Node *current) const noexcept
{
   if (current == nullptr) {

      return;
   }

   switch (current->getTotalItems()) {

      case 1: // two node
            DoInOrderTraverse(f, current->children[0].get());
   
            f(current->pair(0));   

            DoInOrderTraverse(f, current->children[1].get());
            break;

      case 2: // three node
            DoInOrderTraverse(f, current->children[0].get());

            f(current->pair(0));

            DoInOrderTraverse(f, current->children[1].get());
 
            f(current->pair(1));

            DoInOrderTraverse(f, current->children[2].get());
            break;
   }
}
template<class Key, class Value> template<typename Functor> void tree23<Key, Value>::DoPreOrderTraverse(Functor f, const Node *current) const noexcept
{
   if (current == nullptr) {

      return;
   }

   switch (current->getTotalItems()) {

      case 1: // two node
            f(current->pair(0));   

            DoPreOrderTraverse(f, current->children[0].get());
 
            DoPreOrderTraverse(f, current->children[1].get());
            break;

      case 2: // three node
            f(current->pair(0));

            DoPreOrderTraverse(f, current->children[0].get());

            DoPreOrderTraverse(f, current->children[1].get());
 
            f(current->pair(1));

            break;
   }
}

template<class Key, class Value> template<typename Functor> void tree23<Key, Value>::DoPostOrderTraverse(Functor f, const Node *current) const noexcept
{
   if (current == nullptr) {

      return;
   }

   switch (current->getTotalItems()) {

      case 1: // two node
            DoPostOrderTraverse(f, current->children[0].get());

            DoPostOrderTraverse(f, current->children[1].get());
 
            f(current->pair(0));   
            break;

      case 2: // three node
            DoPostOrderTraverse(f, current->children[0].get());

            DoPostOrderTraverse(f, current->children[1].get());

            f(current->pair(0));

            DoPostOrderTraverse(f, current->children[2].get());
 
            f(current->pair(1));
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
   queue.push({proot, initial_level});

   while (!queue.empty()) {

        auto[current, current_tree_level] =  queue.front(); // Use of C++17 unpacking

        f(current, current_tree_level);  
        
        if (current != nullptr && !current->isLeaf()) {

            if (current->totalItems == 0) { // This can happen only during remove() when an internal 2-node becomes empty temporarily  ...

                   //...when only and only one of the empty 2-node's children will be nullptr. 
                   queue.push( { (current->children[0] == nullptr) ? nullptr : current->children[0].get(), current_tree_level + 1 }); 
                   queue.push( { (current->children[1] == nullptr) ? nullptr : current->children[1].get(), current_tree_level + 1 }); 

	    } else {
            
                for(auto i = 0; i < current->getChildCount(); ++i) {
    
                   queue.push({current->children[i].get(), current_tree_level + 1});  
                }
	    }
        }

        queue.pop(); 
   }
}

template<class Key, class Value> bool tree23<Key, Value>::find(Key key) const noexcept
{
  return find_(root.get(), key);
} 

template<class Key, class Value> bool tree23<Key, Value>::find_(const Node *pnode, Key key) const noexcept
{
   if (pnode == nullptr) return false;
   
   auto i = 0;
   
   for (; i < pnode->getTotalItems(); ++i) {

      if (key < pnode->key(i)) 
         return find_(pnode->children[i].get(), key); 
    
      else if (key == pnode->key(i)) 
         return true;
   }

   return find_(pnode->children[i].get(), key);
}

template<typename Key, typename Value> inline void tree23<Key, Value>::printlevelOrder(std::ostream& ostr) const noexcept
{
  NodeLevelOrderPrinter tree_printer(height(), &Node::print, ostr);  
  
  levelOrderTraverse(tree_printer);
  
  ostr << std::flush;

}

template<typename Key, typename Value> void tree23<Key, Value>::debug_printlevelOrder(std::ostream& ostr) const noexcept
{
  NodeLevelOrderPrinter tree_printer(height(), &Node::debug_print, ostr);  
  
  levelOrderTraverse(tree_printer);
  
  ostr << std::flush;
}


template<typename Key, typename Value> inline void tree23<Key, Value>::printInOrder(std::ostream& ostr) const noexcept
{
  auto lambda = [&](const std::pair<Key, Value>& pr) { ostr << pr.first << ' '; };
  inOrderTraverse(lambda); 
}

template<class Key, class Value> inline void tree23<Key, Value>::CreateRoot(Key key, const Value& value) noexcept
{
   root = std::make_unique<Node>(key, value);
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
      ++size_;
      return;
  }

/*
  new_key will be inserted between the next largest value in the tree and the prior next smallest: predecessor_key < new_key < successor_key

   "stack<int> child_indecies" tracks each branch taken when descending to the leaf into which the new_key will be inserted. This stack aids in creating 4-nodes
   from internal 3-nodes. child_indecies tells us the branches taken from the root to pinsert_start. 

   Thus, for example, in code like that below, which converts the descent branches contained in the stack<int> named child_indecies into a deque<int> named
   branches, branches can be used to duplicate the exact descent branches taken from the root to the leaf where the insertion of new_key should begin:

       // Convert stack to deque
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

  auto result_tuple= findNode(new_key);

  if (std::get<0>(result_tuple)) { // new_key already exists, so we overwrite its associated value with the new value.

       std::get<1>(result_tuple)->value(std::get<2>(result_tuple)) = new_value;
       return;  
  }
  
  Node *pinsert_start = std::get<1>(result_tuple);
  
  // new_key was not found in the tree; therefore we know pinsert_start is a leaf.
  if (pinsert_start->isThreeNode()) { 
    
      // split() converts first parameter from a 3-node to a 2-node.
      split(std::get<1>(result_tuple), std::get<3>(result_tuple), std::unique_ptr<Node>{nullptr}, new_key, new_value); 

  } else { // else we have room to insert new_new_key/new_value into leaf node.
      
     pinsert_start->insertKeyInLeaf(new_key, new_value);
  }
  ++size_;
}
/*
 New code
 Returns:

   bool -- found or not
   Node * -- the Node where found or leaf node where search ended.
   int -- the index into keys_values where found, or 0 if not found.
   statck<int> -- stack of child indecies taken--if key not found.
 */
template<class Key, class Value> std::tuple<bool, typename tree23<Key, Value>::Node *, int, std::stack<int>> tree23<Key, Value>::findNode(Key key) const noexcept
{
  std::stack<int> indecies;

  Node *current = root.get();

  // Search for new_key until found or if we search a leaf node and didn't find the key.
  while(true) {

    //auto result_tuple = current->find(key);
    auto [bool_found, pnode, index] = current->find(key);

    if (bool_found) { // found
            
        //return std::tuple_cat(std::move(result_tuple), std::tuple{std::move(indecies)}  ); //{std::move(indecies)}); 
        return {bool_found, pnode, index, std::move(indecies)}; //{std::move(indecies)}); 

    } else if (current->isLeaf()) { // not in tree. If leaf, return current.

        return {false, current, 0, std::move(indecies)};

    } else {

       indecies.push(index); // Remember which child node branch we took. 

       current = pnode;
    } 
  }
}
/*
 Advances cursor next if key not found in current node. If found sets found_index.
 TODO: Change this to return a tuple or struct and not use references params to do this.
 */
template<class Key, class Value> std::tuple<bool, typename tree23<Key, Value>::Node *, int> tree23<Key, Value>::Node::find(Key key_in) noexcept
{
  auto i = 0;
  
  for(; i < getTotalItems(); ++i) {

     if (key_in < key(i)) {
            
         return {false, children[i].get(), i};

     } else if (key_in == key(i)) {

         return {true, this, i};
     }
  }

  // It must be greater than the last key (because it is not less than or equal to it).
  return {false, children[totalItems].get(), i};
}

// Can I rename this CreateRoot()?

template<class Key, class Value> template<class... Args> inline void tree23<Key, Value>::EmplaceRoot(Key key, Args&&... args) noexcept
{
   root = {key, std::forward<Args>(args)...}; // Uses Node's variadic template constructor.
}

// See split() variadic template method in ~/test/main.cpp
/*++
// TODO: Rewrite to work like ::insert() with new Node::find() and new findInsertNode().

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
  
  if (int found_index = findInsertNode(key, child_indecies, pinsert_start); found_index != Node::NotFoundIndex) { // if key already exists, overwrite its associated value.

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
  
  if (std::unique_ptr<Node> larger_2node{std::make_unique<Node>(node4)}; pnode == root.get()) {

      // We pass node4.keys_values[1].key() and node4.keys_values[1].value() as the Key and Value for the new root.
      // pnode == root.get(), and pnode is now a 2-node. larger_2node is the 2-node holding node4.keys_values[2].key().
        
      CreateNewRoot(node4.key(1), node4.value(1), std::move(root), std::move(larger_2node)); 

  } else if (parent->isTwoNode()) { // Since pnode is not the root, its parent is an internal node. If it, too, is a 2-node, ...

      // ...we convert the parent to a 3-node by inserting the middle value into the parent, and passing it the larger 2-node, which it will adopt.
      parent->convertTo3Node(node4.key(1), node4.value(1), std::move(larger_2node));

  } else { // parent is a 3-node, so we recurse.

     // parent now has three items, so we can't insert the middle item. We recurse to split the parent.
     split(parent, child_indecies, std::move(larger_2node), node4.key(1), node4.value(1)); 
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
template<class Key, class Value> void tree23<Key, Value>::CreateNewRoot(Key new_key, const Value& new_value, std::unique_ptr<Node>&& currentRoot, \
                  std::unique_ptr<Node>&& rightChild) noexcept
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

      key(0) = new_key;
      value(0) = new_value; 

  } else {

      key(1) = new_key;
      value(1) = new_value;

      // Note: This tells us that newChild will be the right most child, and the existing children do not need to move.
  }

  // Determine where newChild should be inserted.
  int child_index = 0;

  for (; child_index < Node::TwoNodeChildren; ++child_index) {
       
       if (newChild->key(0) < children[child_index]->key(0)) { 
           break;
       }
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
template<class Key, class Value> inline void tree23<Key, Value>::Node::insertKeyInLeaf(Key in_key, const Value& new_value)
{
   if (in_key < keys_values[0].key()) {

       keys_values[1]= std::move(keys_values[0]);

       key(0) = in_key;
       value(0) = new_value;

   } else { // key > keys_values[0].key()

       key(1) = in_key;
       value(1) = new_value;  
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

       key(0) = key;
       value(0) = std::move(new_value);

   } else { // key > keys_values[0].key()

       key(1) = key;
       value(1) = std::move(new_value);  
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

  // Note: findNode() by value a stack<int> as 4th value in tuple. But RVO should eliminate copies/moves--right?
  // But I can simply use auto &, I think.
  auto [bool_found, premove, remove_index, descent_indecies] = findNode(key); // We don't want a copy of descent_indecies, just a reference.

  if (!bool_found) return;
  
  Node *pLeaf;

  if (premove->isLeaf()) {
      
      pLeaf = premove;  // ...premove is a leaf, and the key is in premove->keys[remove_index]
       
  } else {   // premove is an internal node...

      // We will swap the internal node key/value with its leaf node in order successor.
      // getSuccessor() will update its third parameter. 
      pLeaf = getSuccessor(premove, remove_index, descent_indecies); 

      std::swap(premove->keys_values[remove_index], pLeaf->keys_values[0]); 
  } 
 
  pLeaf->removeLeafKey(key); // Remove key from leaf         
  
  // We now have reduced the problem to removing the key (and its value) from a leaf node, pLeaf. 
  if (pLeaf->isEmpty()) { 
      
      fixTree(pLeaf, descent_indecies); 
  }

  --size_;  
  return;
}

/*
 Finds the in order successor of internal node pnode, which is the left most child of pnode's immediate right subtree.
 
 Input:
 1. pnode must be an internal node.
 2. found_index is the index of the key being removed within pnode->keys[].
 3. reference to child_indecies that hold the descent path from the root to pnode. 
 Output:
 1. pointer to leaf node of in order successor
 2. child_indecies is updated so it now holds the descent path from the root to pnode and then from pnode to its in order successor.
*/
template<class Key, class Value> typename tree23<Key, Value>::Node * tree23<Key, Value>::getSuccessor(Node *pnode, int found_index, \
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
 fixTree() is initially called when a leaf node becomes empty, and therefore the tree needs to be rebalanced. It first attempts to barrow a key from a 3-node sibling and calls 
 silbingHasTwoItems() is called. If a 3-node sibling exits, barrowSiblingKey() returns its child_index such that

   p3node_sibling ==  pnode->parent->children[child_index]

 and passes child_index to then shift it left or right so that the tree is re-balanced, and the empty node is filled with a key/value.  
 If no adjacent sibling is a 3-node, a key/value from the parent is brought down and merged with a sibling of pnode. Any non-empty children of pnode are moved to the 
 sibling. Upon return, pnode is deleted from the tree by a calling to unique_ptr<Node>::reset().  
 If the parent of pnode has now become empty (because merge2Nodes was called), a recursive call to fixTree is made.
 Parameters
 ==========
 1. pnode: an empty node, initially a leaf. During recursive calls to fixTree, pnode is an empty internal 2-node with only one non-nullptr child.  
 2. child_index: The child index in the parent such that: pnode->parent->children[child_index] == pnode 
*/
template<class Key, class Value> void tree23<Key, Value>::fixTree(typename tree23<Key, Value>::Node *pnode, std::stack<int>& descent_indecies) noexcept
{
  if (pnode == root.get()) {

     // make the merged sibling the root.
     reassignRoot(); // This decrease the tree height--I believe.

     // delete the root and make its only non-empty child the new root.
     return;
  }

  int pnode_child_index = descent_indecies.top();

  descent_indecies.pop();

  // case 1. If the empty node has a sibling with two keys, then we can shift keys and barrow a key for pnode from its parent. 

  if (auto [has_two_items, sibling_index] = pnode->siblingHasTwoItems(pnode_child_index); has_two_items) { 

      barrowSiblingKey(pnode, pnode_child_index, sibling_index);
     
  } else  { // No sibling has two items, so we merge a key/value from pnode's parent with the appropriate sibling. 

     Node *parent = pnode->parent;
      
     if (pnode->parent->isThreeNode()) { 
         
         // parent is a 3-node, but has only 2-node children. In this case, we can successfully rebalance the tree. We merge one of the parent keys (and
         // its associated value) with a sibling. This now makes the parent a 2-node. We move the children affected by the merge appropriately, and then we can
         // safely delete pnode from the tree.
    
         merge3NodeWith2Node(pnode, pnode_child_index);
                    
      } else { 
    
          // When the parent is a 2-node, then both pnode's sibling and the parent have one key. We merge the parent's sole key/value with
          // pnode's sibling at pnode->parent->children[!pnode_child_index]. This leaves the parent empty, which we handle recursively below 
          // by again calling fixTree(). 
          merge2Nodes(pnode, !pnode_child_index); 

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
   // The root has a sole non-empty child, make it the new root. unique_ptr's assignment operator will first delete the current empty root
   // node pointer before doing the assignment.
      root = std::move(root->getNonNullChild());  
      root->parent = nullptr;   
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
template<class Key, class Value> void tree23<Key, Value>::barrowSiblingKey(Node *pnode, int child_index, int sibling_index) noexcept
{
  Node *parent = pnode->parent; 
  Node *sibling = parent->children[sibling_index].get();

 // If node is an internal node, this implies fixTree() has recursed, and node will have only subtree, one non-nullptr child.
 if (parent->isTwoNode()) {

     // bring down parent key and its associated value. 
     pnode->keys_values[0] = std::move(parent->keys_values[0]);  

     if (sibling_index < child_index) {  // sibling is to left of node

        parent->keys_values[0] = std::move(sibling->keys_values[1]);

     } else { // sibling is to the right

        parent->keys_values[0] = std::move(sibling->keys_values[0]);

        sibling->keys_values[0] = std::move(sibling->keys_values[1]);
     } 

     pnode->totalItems = Node::TwoNode;
     sibling->totalItems = Node::TwoNode;

     // Check if leaf node case... 
     if (pnode->isLeaf()) return;

     // ...or if fixTree() has been called recursively 
     if (sibling_index < child_index) {  // If sibling is to left of node...

         // ...detemine if the left child is the non-nullptr child, make it the right child; otherwise, 
	 // leave it alone.
         if (pnode->children[0] != nullptr) {
 
            pnode->connectChild(1, std::move(pnode->children[0])); // Shift the non-nullptr child, the sole child of node, to be the right child of
	                                                         // node.
         }

         pnode->connectChild(0, std::move(sibling->children[2])); // Make the left sibling's right child the left child of node. 

     } else { // it is to the right of node

	 // ...determine if the right child of node is the sole child. If so, make it the left child.    
         if (pnode->children[1] != nullptr) {
		 
            // Shift the non-nullptr child, the sole child of node, to position 0.
            pnode->connectChild(0, std::move(pnode->children[1]));
         }   

         pnode->connectChild(1, std::move(sibling->children[0])); // Make the right sibling's left child the right child of node.

         // Shift its two right most children left one slot.
         sibling->connectChild(0, std::move(sibling->children[1]));
         sibling->connectChild(1, std::move(sibling->children[2]));
     }

  } else { // parent is a 3-node
     /*
        First, determine if there are two hops to the sibling from which we will barrow a key. Two hops can only occurs when child_index is 0, and
       	the sibling_index is 2, or vice versa. 
      */

        Node *middleChild = pnode->parent->children[1].get();

        if (child_index == 0 && sibling_index == 2) {

            // handles two hops left
            pnode->keys_values[0] = std::move(parent->keys_values[0]);

            pnode->totalItems = Node::TwoNode;

            parent->keys_values[0]= std::move(middleChild->keys_values[0]);

            middleChild->keys_values[0] = std::move(parent->keys_values[1]); 

            parent->keys_values[1] = std::move(sibling->keys_values[0]);

            sibling->keys_values[0] = std::move(sibling->keys_values[1]);              

            sibling->totalItems = Node::TwoNode;

            // Shift the children appropriately below.
            shiftChildrenLeft(pnode, middleChild, sibling);
            return;

        } else if (child_index == 2 && sibling_index == 0) {

            // handle two hops
            pnode->keys_values[0] = std::move(parent->keys_values[1]);
            pnode->totalItems = Node::TwoNode;

            parent->keys_values[1]= std::move(middleChild->keys_values[0]);
            middleChild->keys_values[0] = std::move(parent->keys_values[0]); 

            parent->keys_values[0] = std::move(sibling->keys_values[1]);

            sibling->totalItems = Node::TwoNode;

            // Shift the children appropriately below.
            shiftChildrenRight(pnode, middleChild, sibling);
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
           
           pnode->keys_values[0] = std::move(parent->keys_values[0]); 
           parent->keys_values[0] = std::move(sibling->keys_values[1]); 
           sibling->totalItems = Node::TwoNode;

           shiftChildrenRight(pnode, sibling);
           break;      

         case 1: // If sibling_index == 1, then child_index == 0 or child_index == 2/ 

           if (child_index == 0) {

               pnode->keys_values[0] = std::move(parent->keys_values[0]); 
               parent->keys_values[0] = std::move(sibling->keys_values[0]); 
               
               shiftChildrenLeft(pnode, sibling); 

            } else { // child_index == 2

               pnode->keys_values[0]= std::move(parent->keys_values[1]);  
               parent->keys_values[1]= std::move(sibling->keys_values[1]); 
               
               shiftChildrenRight(pnode, sibling); 
            }
           
            sibling->totalItems = Node::TwoNode;
            break;

         case 2: 

           pnode->keys_values[0] = std::move(parent->keys_values[1]); 
           parent->keys_values[1] = std::move(sibling->keys_values[0]); 
           sibling->keys_values[0] = std::move(sibling->keys_values[1]);
           sibling->totalItems = Node::TwoNode;
           
           shiftChildrenLeft(pnode, sibling); 
           break;
    }
  }

  pnode->totalItems = Node::TwoNode; 
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

  // If sole_child is 1, then shift it left.
  if (int sole_child = node->getSoleChildIndex(); sole_child == 1) {

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

  // Determine which child of node is non-nullptr. If the sole child is 0, then shift it right.
  if (node->getSoleChildIndex() == 0) {

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

  // If sole child is 1, then shift it left.
  if (node->getSoleChildIndex()== 1) {

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

  // If first child is sole child

  if (node->getSoleChildIndex()== 0) {

     node->connectChild(1, std::move(node->children[0]));  
  }

  node->connectChild(0, std::move(middleChild->children[1]));

  middleChild->connectChild(1 , std::move(middleChild->children[0]));
  middleChild->connectChild(0 , std::move(sibling->children[2]));
}
/*
 Overview
 ========
 
 Parameters:
 1. pnode is empty 2-node. If it is not a leaf node, it has one child, one subtree; the other child, when pnode is an internal node, is nullptr.
 2. child_index is such that pnode->parent->children[child_index] == pnode
 Requires: The pnode->parent is a 3-node, and all pnode's siblings are 2-nodes.
 Promises: Merges one of the keys/values of pnode->parent with one of pnode's 2-node siblings to rebalance the tree. It shifts the children of the
 effected siblings appropriately, transfering ownership of the sole non-nullptr child of pnode, when pnode is an internal node, which only occurs during
 a recursive call to fixTree(). 
 
 */
template<class Key, class Value> void  tree23<Key, Value>::merge3NodeWith2Node(Node *pnode, int child_index) noexcept
{
    Node *parent = pnode->parent;

    // If pnode is a leaf, then all children are nullptrs. The non-null child is only needed when pnode is an internal node.
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
template<class Key, class Value> void tree23<Key, Value>::merge2Nodes(Node *pnode, int sibling_index) noexcept
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

      return; 
  } 

  // Recursive case: This only occurs if fixTree adopted the sole child of pnode. The other child was deleted from the tree and so sibling->children[!child_index] == nullptr.
  std::unique_ptr<Node>& nonemptyChild = pnode->getNonNullChild();

  // Is sibling to the left? 
  if (sibling_index == 0) {
        
      sibling->connectChild(2, std::move(nonemptyChild));    

  } else { // no, it is to the right.

      sibling->connectChild(2, std::move(sibling->children[1]));
      sibling->connectChild(1, std::move(sibling->children[0]));
      sibling->connectChild(0, std::move(nonemptyChild));    
  }
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
              ostr << "error: children[0]->key(" << i << " is not less than " << key(0) << ".\n";
           }  
       break; 

       case 1:
 
       // Test middle child's keys, key, are such that: keys_values.key() [0] < key < keys_values.key()[1]
           if (!(children[1]->keys_values[i].key() > keys_values[0].key() && children[1]->keys_values[i].key() < keys_values[1].key())) {
     
              // problem
              ostr << "error: children[1]->key(" << i << " = " << children[1]->key(i) << " is not between " << key(0) << " and " << key(1) << ".\n";
           }

       break;

      case 2:     
       // Test right child's keys are all greater than nodes sole key
     
           if (children[2]->keys_values[i].key() <= keys_values[1].key()) { // If any are less than or equal to keys_values.key()[1], it is an error.
     
              // problem
              ostr << "error: children[2]->key(" << i << ") = " << children[2]->key(2) << " is not greater than " << key(1) << ".\n";
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

/*
 * Returns -1 is pnode not in tree
 * Returns: 0 for root
 *          1 for level immediately below root
 *          2 for level immediately below level 1
 *          3 for level immediately below level 2
 *          etc. 
 */
template<class Key, class Value> int tree23<Key, Value>::depth(const Node *pnode) const noexcept
{
    if (pnode == nullptr) return -1;

    int depth = 0;
      
    for (const Node *current = root; current != nullptr; ++depth) {

      if (current->key() == pnode->key()) {

          return depth;

      } else if (pnode->key() < current->key()) {

          current = current->left;

      } else {

          current = current->right;
      }
    }

    return -1; // not found
}

template<class Key, class Value> inline int tree23<Key, Value>::height() const noexcept
{
   return height(root.get());
}

template<class Key, class Value> int tree23<Key, Value>::height(const Node* pnode) const noexcept
{
   if (pnode == nullptr) {

       return -1;

   } else {
       
      std::array<int, Node::ThreeNodeChildren> heights;
      
      int num_children = pnode->getChildCount();
      
      for (auto i = 0; i < num_children; ++i) {
          
         heights[i] = height(pnode->children[i].get());
      }
      
      int max = *std::max_element(heights.begin(), heights.begin() + num_children);
      
      return 1 + max;
   }
}

/*
  Input: pnode must be in tree
 */
template<class Key, class Value> bool tree23<Key, Value>::isBalanced(const Node* pnode) const noexcept
{
    if (pnode == nullptr) return false; 

    std::array<int, Node::ThreeNodeChildren> heights; // four is max number of children.
    
    int child_num = pnode->getChildCount();
    
    for (auto i = 0; i < child_num; ++i) {

         heights[i] = height(pnode->children[i].get());
    }
    
    int minHeight = *std::min_element(heights.begin(), heights.begin() + child_num);
    
    int maxHeight = *std::max_element(heights.begin(), heights.begin() + child_num);

    // Get absolute value of difference between max height and min of height of children.
    int diff = std::abs(maxHeight - minHeight);

    return (diff == 1 || diff ==0) ? true : false; // return true is absolute value is 0 or 1.
}

// Visits each Node in level order, testing whether it is balanced. Returns false if any node is not balanced.
template<class Key, class Value> bool tree23<Key, Value>::isBalanced() const noexcept
{
    std::queue<const Node *> nodes;

    nodes.push(root.get());

    while (!nodes.empty()) {

       const Node *current = nodes.front();
       
       nodes.pop(); // remove first element
       
       if (isBalanced(current) == false)  return false; 

       // push its children onto the stack 
       for (auto i = 0; i < current->getChildCount(); ++i) {
          
           if (current->children[i] != nullptr) {
               
               nodes.push(current->children[i].get());
           }   
       }
    }
    return true; // All Nodes were balanced.

}

template<class Key, class Value> bool tree23<Key, Value>::test_invariant() const noexcept
{
  // Compare size with a count of the number of nodes from traversing the tree.
  auto end_iter = end();

  auto  count = 0;
  for (auto iter : *this)  {

      ++count; 
  }

  if (size_ != count) {

      std::cout << "The manual node count is " << count << ", and the value of size_ is " << size_  << '.' << std::endl;
  }

  return (size_ == count) ? true : false;

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

