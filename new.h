#ifndef  TREE23_H
#define	TREE23_H

#include <utility>
#include <iostream>
#include <exception>

// fwd declarations
template<typename T> class Tree23;    
//--template<typename T> class Tree23<T>::Node23;    
//--template<typename K> class Node34; 

class duplicatekey :  public std::exception {
public:
    
  virtual const char* what() const throw()
  {
    return "Attempting to insert duplicate key ";
  }
};

template<typename K> class Tree23 {
    
protected:       
    
   class Node34; // fwd declaration of protected class.
  
public:
   
   class Node23 {
      private: 
           friend class Tree23<K>;             
           friend class Node34;
           Node23(bool three_node=false) : isThreeNodeFlag(three_node), leftChild(0), rightChild(0), parent(0) { }
           
           Node23& operator =(Node23& rhs);
           
           Node23(K key);
    
           Node23(K small, K large);
    
           Node23(K small, Node23 *pParent, Node23 *pleftChild, Node23 *prightChild);
           
           Node23(const Node23& n);
    
           Node23& makeTwoNode(K value, Node23 *pParent, Node23 *left, Node23 *right);                
    
           bool isLeafNode(); 
           
           bool isThreeNode();
           
           void setThreeNode(bool flag);
           
           K getSmallValue() { return smallValue; }
           K getLargeValue() { return largeValue; }
    
           Node23 *leftChild;
           Node23 *rightChild;
           Node23 *middleChild;
           
           Node23 *parent;
               
           K smallValue;
           K largeValue;
                              
           bool isThreeNodeFlag;
   };  

   typedef Node23 Node;
   
  protected:
    /* 
     * This class' constructor sorts the values of the Node23 and new_value into small, middle and large. For a leaf node, all child pointers will be zero.
     * For an internal node, they are assigned into two left-most and two right-most child pointers.
     */
     class Node34 {
         
      friend class Tree23<K>; 
      
       private:
          K smallValue;
          K middleValue;
          K largeValue;
    
          Tree23<K>::Node23 *leftChild;    
          Tree23<K>::Node23 *leftMiddleChild;	
    
          Tree23<K>::Node23 *rightMiddleChild;
          Tree23<K>::Node23 *rightChild;
          
          /* Note: Their is no parent node pointer. */
       public:
    
	 /* Using default values allows us to generalize both leaf node and internal node cases. */
	 Node34(Node23 *threeNode, K new_value, Tree23<K>::Node23 *leftChildOfNewValue=nullptr,
                                                  Tree23<K>::Node23 *rightChildOfNewValue=nullptr);
         
         K  getSmallValue()  { return smallValue; }
         K  getMiddleValue() { return middleValue; }
         K  getLargeValue()  { return largeValue;  }
    	 Tree23<K>::Node23 *getLeftChild()        { return leftChild; }
	 Tree23<K>::Node23 *getLeftMiddleChild()  { return leftMiddleChild; }	
	 Tree23<K>::Node23 *getRightMiddleChild() { return rightMiddleChild; }	
	 Tree23<K>::Node23 *getRightChild()   { return rightChild; } 
    };

   protected:

    Node23 *root; 
    
    bool DoSearch(K key, Node23 *root, Node23 *&location);

    // Called by insert to split three nodes receiving a new value 
    Node23 *Split(Node23 *p, K key, Node23 *pLeftChildOfNewKey = nullptr, Node23 *pRightChildOfNewKey = nullptr);

    template<typename Functor> void DoTraverse(Functor f, Node23 *root);
    void DestroyTree(Node23 *root);
    
     // Find in order successor
    Node23 *FindNextLargest(K key, Node23 *location);
    void fix(Node23 *location, Node23 *pAdoptee=nullptr);
    bool Redistribute(Node23 *node, int& situation_designator);
    void ReassignChildren(Node23 *node, Node23* pChildOfNode, int situation);

  public:    
     Tree23() { root = nullptr; } 
    ~Tree23();
     template<typename Functor> void Traverse(Functor f);
     bool Search(K key, Node23 *&location);
     bool remove(K key, Node23 *location=nullptr);
           
     Tree23<K>::Node23 *insert(K key, Node23 *location=nullptr) throw(duplicatekey);
};
/* 
 * It is sufficient to just check the leaf pointer. Internal nodes will have at minimun bot leftChild
 * and rightChild set.
 */   
template<typename K> inline  bool Tree23<K>::Node23::isLeafNode()  
{ 
   return !leftChild ? true : false;
}

template<typename K> inline bool Tree23<K>::Node23::isThreeNode() 
{ 
   return this->isThreeNodeFlag; 
}

template<typename K> inline void Tree23<K>::Node23::setThreeNode(bool flag) 
{
  this->isThreeNodeFlag = flag; 
}

template<typename K> inline Tree23<K>::Node23::Node23(K key) : isThreeNodeFlag(false), leftChild(0), rightChild(0) 
{ 
   smallValue = key; 
}

template<typename K> inline Tree23<K>::Node23::Node23(K small, K large) : isThreeNodeFlag(false), leftChild(0), rightChild(0) 
{ 
    smallValue = small;
    largeValue = large; 
}

template<typename K> inline Tree23<K>::Node23::Node23(K small, Tree23<K>::Node23 *pParent, Tree23<K>::Node23 *pleftChild,
              Tree23<K>::Node23 *prightChild) : isThreeNodeFlag(false), parent(pParent), leftChild(pleftChild), rightChild(prightChild) 
{ 
    smallValue = small; 
} 

template<typename K> inline Tree23<K>::Node23::Node23(const Tree23<K>::Node23& n) : isThreeNodeFlag(n.isThreeNodeFlag),
        leftChild(n.leftChild), rightChild(n.rightChild), parent(n.parent) 
{ 
}


template<typename K> typename Tree23<K>::Node23& Tree23<K>::Node23::makeTwoNode(K value, Tree23<K>::Node23 *pParent,
        Tree23<K>::Node23 *left, Tree23<K>::Node23 *right)                
{
  smallValue = value;
  leftChild = left;
  rightChild = right;
  isThreeNodeFlag = false; 
  parent =  pParent;
  return *this;
}

template<typename K> Tree23<K>::Node34::Node34(Node23 *threeNode, K new_key, Node23 *leftChildOfNewValue, Node23 *rightChildOfNewValue)
{
    /*
     * Sort to find smallValue, middleValue and largeValue. Assign child pointers appropriately.
     */
    if (new_key < threeNode->smallValue) { // new_key is smallest value
     	
 	smallValue = new_key;
 	middleValue = threeNode->smallValue;
 	largeValue = threeNode->largeValue;

        leftChild = leftChildOfNewValue;
        leftMiddleChild = rightChildOfNewValue;
        rightMiddleChild = threeNode->middleChild;
     	rightChild = threeNode->rightChild;
             		
     } else if (new_key > threeNode->largeValue) { // new_key is largest value
     
 	smallValue = threeNode->smallValue;
 	middleValue = threeNode->largeValue;
 	largeValue = new_key;
     
        leftChild = threeNode->leftChild;
        leftMiddleChild = threeNode->middleChild;
        rightMiddleChild = leftChildOfNewValue;
     	rightChild = rightChildOfNewValue;

     } else { // new_key is the middle value
     
 	smallValue = threeNode->smallValue;		    
 	middleValue = new_key;
 	largeValue = threeNode->largeValue;		    

        leftChild = threeNode->leftChild;
        leftMiddleChild = leftChildOfNewValue;
        rightMiddleChild = rightChildOfNewValue;
     	rightChild = threeNode->rightChild;
     }
}


	
template<typename K>  inline Tree23<K>::~Tree23()
{
  DestroyTree(root);
}
/*
 * Post order Traversal, deleting nodes.
 */ 
template<typename K> void Tree23<K>::DestroyTree(Node23 *p)
{
  if (p == nullptr) {
	return;
  }
	 
  if (p->isThreeNode()) { // descend three node

        DestroyTree(p->leftChild);

        DestroyTree(p->middleChild);

        DestroyTree(p->rightChild);

        delete p; 

  } else { // descend two node

        DestroyTree(p->leftChild);

        DestroyTree(p->rightChild);

        delete p; 
  }

}

template<typename K> template<typename Functor>  inline void Tree23<K>::Traverse(Functor f) 
{
    return DoTraverse(f, root); 
}

template<typename K> template<typename Functor>  void Tree23<K>::DoTraverse(Functor f, Node23 *p)
{
  // Since the tree is always balanced, it is sufficient to check just one p
  if (p == nullptr) {
	return;
  }
	 
  if (p->isThreeNode()) { // descend three node

        DoTraverse(f, p->leftChild);

        f(p->smallValue);

        DoTraverse(f, p->middleChild);

        f(p->largeValue);

        DoTraverse(f, p->rightChild);

  } else { // descend two node

        DoTraverse(f, p->leftChild);

        f(p->smallValue);

        DoTraverse(f, p->rightChild);
  }
}

template<typename K> bool Tree23<K>::Search(K key, Node23 *&location)
{
    // make sure tree has at least one element    
    if (root == nullptr) {
    
        location = nullptr;
        return false;
        
    } else {
          
        return DoSearch(key, root, location);
    }
}   

template<typename K>  bool Tree23<K>::DoSearch(K key, Node23 *current, Node23 *&location)
{
    /*
     * Is key in current?
     */
    if (current->smallValue == key || (current->isThreeNode() && current->largeValue == key)) {
        
	location = current;
        return true; 
                
    } else if ((current->isLeafNode())) { 
        
        location = current;
	return false; 
     /*
      *  Does current have two keys?
      */ 
    } else if (current->isThreeNode()){
        
        if (key < current->smallValue) {
            
            DoSearch(key, current->leftChild, location); 
            
        }  else if (key < current->largeValue) {
            
            DoSearch(key, current->middleChild, location); 
            
        } else {
            
            DoSearch(key, current->rightChild, location); 
        }
      
    } else { // ...or only one?
        
         if (key < current->smallValue) {
            
            DoSearch(key, current->leftChild, location); 
            
        }  else {
            
            DoSearch(key, current->rightChild, location);  
        }    
    }
}

template<typename K> typename Tree23<K>::Node23 *Tree23<K>::insert(K key, Tree23<K>::Node23 *location)  throw(duplicatekey)
{
    // root case
    if (root == nullptr) {

         root = new Node23(key);
	 return root;
    } 

    // Test that location is 0, which suggests the root, or if it truly is a leaf.
    if (location == nullptr || !location->isLeafNode()) {
        
        Node23 *leaf; 
        bool bRc = DoSearch(key, root, leaf); 
	return insert(key, leaf);
    }
    
    Node23 *result = nullptr;
    
    // If the leaf node is a two node, insert the key, and we are done.
    if (!location->isThreeNode()) {
        
        result = location;             
        
        if (key > location->smallValue) {
            
            location->largeValue = key;
            location->setThreeNode(true);
            
        } else if (key < location->smallValue) {
            
            location->largeValue = location->smallValue;
            location->smallValue = key;
            location->setThreeNode(true);
                    
        } else {   
            // throw duplication found. They are equal, so throw exception.
            throw duplicatekey(); 
        }
        
    } else {
       
        // ... else it is a three node and cannot contain three items, so split it into two two-nodes.
        result = Split(location, key); 
    }
    
    return result;
}  

/*
 * Some of the logic is the same. Just used a sheet a paper and work through a example by hand, using the pseudo code at
 * http://www.cs.ucr.edu/cs14/cs14_06win/slides/2-3_trees_covered.pdf
split(Node23<K>* n, K new_key)
{
 if (n is the root) {

    create a new node p

 } else {

    let p be the parent of n
 }

 Replace n with two nodes n1 and n2, so that p is their parent, giving n1 the smallest value
 and n2 the largest value. (Pass in Node34?)
 
 if (n is not a leaf) {
   // n had three children since it was a three node. One of its children was split in the prior call to split, so
   // there are four children. 
    
	Make n1 the parent of the two leftmost children. 
	Make n2 the parent of the two rightmost children. 
  }

  middle = be the item in n that has the middle value

  if (adding middle to p would cause it to has three values)
           split(p, middle)
  else 
	   add middle to p
} 
*/
template<typename K> typename Tree23<K>::Node23* Tree23<K>::Split(Node23 *location, K new_key, Node23 *pLeftChildOfNewKey, Node23 *pRightChildOfNewKey)
{
    Node23* parent = nullptr;

    bool isLeaf = location->isLeafNode();
    bool bRootIsNew = false;
    
    if (location == root) { // This is one case: splitting the root.

	parent = new Node23;
        bRootIsNew = true;

    } else  {
        
        parent = location->parent;
    }    
    
    /*
     We pass the current node, location, plus the middle value to the four node ctor. It automatically set the leftChild, leftMiddleChild, rightMiddleChild, and the 
     rightChild. We then build two two-nodes, one with the smallest value, which will have its leftChild and rightChild set to the four node's leftChild and 
     leftMiddleChild, respectively; the other two node will contain the largest value in the four node, and it twill have leftChild and rightChild set to
     leftMiddleChild and rightChild, respectively. We will allocated smallest on the heap, and convert location, the original three node, to be largest.  
    */ 
    Node34 node34(location, new_key, pLeftChildOfNewKey, pRightChildOfNewKey); // defaults handles leaf node case. 
    
    Node23 *pSmallest = new Node23(node34.getSmallValue(), parent, node34.getLeftChild(), node34.getLeftMiddleChild());

    location->makeTwoNode(node34.getLargeValue(), parent, node34.getRightMiddleChild(), node34.getRightChild());

    Node23 *pLargest = location; 
   
    if (!isLeaf) { 
        
        // Make pSmallest the parent of the two leftmost children. 
        node34.getLeftChild()->parent  = pSmallest; 
        node34.getLeftMiddleChild()->parent  = pSmallest;

	// Make pLargest the parent of the two rightmost children. 
        node34.getRightMiddleChild()->parent  = pLargest; 
        node34.getRightChild()->parent  = pLargest;
    }

    K middle = node34.getMiddleValue(); 

    // add middle value to parent. But we must decide if a new root was allocated, to which we are adding the middle value.
    if (bRootIsNew) { 
  
	// If we allocated a new root, then it is empty and it will be two node.
	parent->smallValue = middle;
        parent->leftChild = pSmallest; 
        parent->rightChild = pLargest; 
        root = parent;

    } else if (parent->isThreeNode()) { // if middle value would cause parent to have three values, then split it.

	Split(parent, middle, pSmallest, pLargest);

    } else { // otherwise it is a two node and can accept the middle value
        
        parent->setThreeNode(true); 
        
        // We compare middle to parent->smallValue
        if (middle < parent->smallValue) {

		parent->largeValue = parent->smallValue;
		parent->smallValue = middle;
		parent->leftChild = pSmallest; 
		parent->middleChild = pLargest;
                // Note: parent->rightChild does not need to be changed.

        }  else { 

		parent->largeValue = middle;
		parent->middleChild = pSmallest;
		// parent->rightChild =  pLargest; This is redundant because pLargest is location, which already is the rightChild.
        }   
    } 
    
    return parent; 
}
/*
 * pseudo code from: http://www.cs.ucr.edu/cs14/cs14_06win/slides/2-3_trees_covered.pdf
 *
 * if (node is not 0) {
 *
 *      if (item is not in leaf node) {  
 *
 *            swap item with in-order successor (always leaf)
 *            leafNode = new location of item to delete

 *       } else {
 *             leafNode = node  
 *       }
 *
 *       delete item from leaf  
 *            
 *       if (leafNode now contains no items) {
 *           fix(leafNode) // may need other parameters
 * }
 */
template<typename K> bool Tree23<K>::remove(K key, Node23 *location)
{
  if (location == nullptr) {
      
        bool bFound = DoSearch(key, root, location); 

        if (bFound == false) {

           // if not found, do what?
            return false;
        } 
        
        bool bRc = remove(key, location);
        return bRc;
        
      // check that key is in location; if not, return false?
  } else if (key != location->smallValue && key != location->largeValue) {
      
       return false;
  }

  /*
   * If item is not a leaf, swap item with inorder successor (always leaf)
   */
  Node23 *leafNode = nullptr;

  bool isInternalNode = !location->isLeafNode();
  
  if (isInternalNode) { // internal node

      // find in order successor
      leafNode = FindNextLargest(key, location);
          
      /*  
       * Swap value with in order successor node. The in order successor will always be
       * smallValue.
       */

        std::swap(location->smallValue, leafNode->smallValue);
        
  } else {
      
      leafNode = location;
  }

  // "delete" item from leafNode.

  if (leafNode->isThreeNode()) { // Simply make three node into a two node to delete. 
           
       if (key == leafNode->smallValue) {

            leafNode->smallValue = leafNode->largeValue;
       }   
       leafNode->setThreeNode(false);

  } else { 
         // leaf is empty two node.
         fix(leafNode);
  }
  
  return true;
}
/*
 *  Precondition: location is an internal node, not a leaf, and key is in the node.
 *  Returns: Node of in-order successor node, always a leaf. This nodes' smallValue is the in-order successor to key.
 */
template<typename K> typename Tree23<K>::Node23 *Tree23<K>::FindNextLargest(K key, Tree23<K>::Node23 *location)
{
 Node23 *next = nullptr;

 // When this method is called, key will equal smallValue or largeValue, and we must do a comparison.
 // We check if location is a three node and, if it is, we compare key to smallValue. If equal, go down middleChild.
 if (location->isThreeNode() && location->smallValue == key) {
   
          next = location->middleChild;
   
 } else { 
 // If it is a two node, there is never a need to do a comparison. We always go down right child.  Likewise, if it is a three node and key
 // is not smallValue, go down right child (because it is equal to the largeValue).
   
          next = location->rightChild;
 } 
 // Continue down left branches until we encounter a leaf.
 while (!next->isLeafNode()) {
     
      next = next->leftChild;
 }
 
 return next;
}
/*
 * preconditions: node points to an empty node.
 * returns: true if it could successfully redistributed values; otherwise, returns false.
 */ 
template<typename K> bool Tree23<K>::Redistribute(Node23 *node, int& situation)
{ 
bool bRc = true;
Node23 *parent = node->parent;

  if (parent->isThreeNode()) {

    /* parent is three node. Look for a three node sibling. */
    if (node == parent->leftChild) { // left

         if (parent->middleChild->isThreeNode()) { // case 1
            /* 
              (20,      40)          (30,     40)
             /      |     \    =>    /     |    \
          Hole   (30,35)   50       20    35     50
            |    /  | \    /\       |    / | \   /\
            C   29 32  37 60 70     C  29 32 37 45 70
             */
              node->smallValue = parent->smallValue; 
              parent->smallValue = parent->middleChild->smallValue; 
              parent->middleChild->smallValue = parent->middleChild->largeValue; 
              parent->middleChild->setThreeNode(false);

              situation = 1;

         } else if (parent->rightChild->isThreeNode()) {
           /*
               (10,  20)                  (15,  30)    
              /    |    \          =>    /    |    \
            Hole   15    (30, 40)      10    20     40 */

              node->smallValue = parent->smallValue; 
              parent->smallValue = parent->middleChild->smallValue; 
              
              parent->middleChild->smallValue = parent->largeValue; 
              parent->largeValue = parent->rightChild->smallValue;
              parent->rightChild->smallValue = parent->rightChild->largeValue; 
              parent->rightChild->setThreeNode(false);
              situation = 2;

         } else {
	     bRc = false; 
	 }

    } else if (node == parent->middleChild) { // middle

         if (parent->leftChild->isThreeNode()) {
           /* 
               (10, 20)                   (5, 20)
              /    |  \             =>   /   |  \
           (1, 5) Hole (30, 40)        1    10  (30, 40) */

            node->smallValue = parent->smallValue;
            parent->smallValue = parent->leftChild->largeValue;
            parent->leftChild->setThreeNode(false);
		
            situation = 3;

         } else if (parent->rightChild->isThreeNode()) {
           /*
              (10, 20)                 (10,  30)
             /    |  \           =>    /    |   \
           1     Hole  (30, 40)       1    20   40
           */
            node->smallValue = parent->largeValue;
            parent->largeValue = parent->rightChild->smallValue;
            parent->rightChild->setThreeNode(false);
            
            situation = 4;
         } else {
	   bRc = false;
         } 

    } else /*(node == parent->rightChild)*/ {  //rightChild is the only case left 

         if (parent->middleChild->isThreeNode()) {
           /* We redistribute value of siblings, but we do not reassign here their children 
              (10,       25)              (10,       19)
             /       |     \       =>    /      |       \
            5    (15, 19)   Hole        5       15       25     
           / \    /  |  \    /         / \     / | \     /      
          1   7  12 17  22  60        1   7  12  17 18  60       */
              
              node->smallValue = parent->largeValue; 
              parent->largeValue = parent->middleChild->largeValue; 
              parent->middleChild->setThreeNode(false);

              situation = 5;

         } else if (parent->leftChild->isThreeNode()) {

           /*  Redistribute() redistributes node values but not their child. 
           (20,       40)               (18,   30)   
           /       |     \      =>     /     |   \   
       (10, 18)   30     Hole        10     20    40 
        /  | \    /  \    /         / | \   / \   /  
       5  12  19 25  32  55        5  7 19 25 32 55      */
 
             node->smallValue =  parent->largeValue;
              parent->largeValue = parent->middleChild->smallValue;
              parent->middleChild->smallValue =  parent->smallValue;
              parent->smallValue = parent->leftChild->largeValue;
              parent->leftChild->setThreeNode(false);
              situation = 6;

         } else {
	      bRc = false;
         } 
    } // end else 
    
  } else { // parent is a two node

    if (parent->leftChild == node) {
   
        if (parent->rightChild->isThreeNode()) {    

	      node->smallValue = parent->smallValue;
              parent->smallValue = parent->rightChild->smallValue;
              parent->rightChild->smallValue = parent->rightChild->largeValue; // Is this necessary if parent is a two node?
              parent->rightChild->setThreeNode(false);  
              situation = 7;
              
        }  else {

	   bRc = false;	
        }
        
     } else {  // node is rightChild
        
     
        if (parent->leftChild->isThreeNode()) {    

	      node->smallValue = parent->smallValue;
              parent->smallValue = parent->leftChild->largeValue;
              parent->rightChild->setThreeNode(false);  
              situation = 8;

        } else {
	      bRc = false;	
        } 
    } 
  }
  return bRc;
}

/*
 * pseudo code base on: http://www.cs.mtsu.edu/~jhankins/files/3110/presentations/2-3Trees.ppt
 * similar pseudo code: http://www.cs.ucr.edu/cs14/cs14_06win/slides/2-3_trees_covered.pdf
 * Delete applet at: http://slady.net/java/bt/view.php applet for delete operation.
 *
fix(Node23 *node, Node23 *pChildOfNode)
{
 if (node is the root) {

      delete root
      set new root pointer to pChildOfNode 

 } else {

     Let p be the parent of node

     if ( some sibling of node has two items ) { 

        redistribute the items appropriately among node, the sibling and the parent.

        if (node is internal) { // See http://www.cs.mtsu.edu/~jhankins/files/3110/presentations/2-3Trees.ppt

	   Move the appropriate child from sibling to node. What this means is, after we distribute values, if the node
           is internal, some silbings, along with node, no longer have the proper parent because the values in the parent
           have changed. 
         }

     } else {

	  To merge nodes...

          Choose an adjacent sibling s of node.
          Bring the appropriate item down from p into s 

          if (node is internal) {
                Move node's child to s 
          }

          Remove node

          if (p is now empty) {

              fix(p, s)
          }
     }
}
*/
template<typename K> void Tree23<K>::fix(Node23 *node, Node23 *pChildOfNode)
{
 if (node  == root) {
      /* delete root */ 
      delete root;
      root = pChildOfNode; 

 } else {

     Node23 *parent = node->parent;

     int situation;
  
     // try to redistribute.. 
     if (Redistribute(node, situation)) { 

        // We could redistribute. If node is a internal node, we need to reassign the children appropriately among the siblings. 	   
	if (!node->isLeafNode()) {  

              ReassignChildren(node, pChildOfNode, situation);
	 }

     } else { // ...we could not redistribute because all siblings are two nodes, so we merge the parent with a sibling. 

	 Node23 *pSibling;

         Node23 *middleChild;
         Node23 *Node23::*Ptr2newParentOfChild; // Pointer to member variable (of type Node23 *)
         bool parentIsEmpty = false; // If parent is a two node, it will be flagged empty after merge.

	 /* Merge nodes by bringing down parent's value to sibling of node.  */ 
                  
          if (parent->isThreeNode()) {

              if (parent->rightChild == node) {

                  pSibling = parent->middleChild; 
                  pSibling->largeValue = parent->largeValue;

                  middleChild = pSibling->rightChild;
		  Ptr2newParentOfChild = &Node23::rightChild;
                  
              } else if (parent->middleChild == node) {

                  pSibling = parent->rightChild; 

                  pSibling->largeValue = pSibling->smallValue;
                  pSibling->smallValue = parent->largeValue;

                  middleChild = pSibling->leftChild;
		  Ptr2newParentOfChild = &Node23::leftChild;

              } else { // node is leftChild

                  pSibling = parent->middleChild; 

                  pSibling->largeValue = pSibling->smallValue;
                  pSibling->smallValue = parent->largeValue;

                  middleChild = pSibling->leftChild;   
		  Ptr2newParentOfChild = &Node23::leftChild;
              } 

              // parent is now a two node
              parent->setThreeNode(false);

          }  else { /* parent is a two node and its other child is a two node  */ 
              
                if (parent->rightChild == node) {

		       pSibling = parent->leftChild;

                       pSibling->largeValue = parent->smallValue;

                       middleChild = pSibling->rightChild;
                       Ptr2newParentOfChild =  &Node23::rightChild;
           
                 } else {

			pSibling = parent->rightChild;

	                pSibling->largeValue = pSibling->smallValue;
        	        pSibling->smallValue = parent->smallValue;

                        middleChild = pSibling->leftChild;
			Ptr2newParentOfChild = &Node23::leftChild;
                 }      

                 parentIsEmpty = true;
          } 

          pSibling->setThreeNode(true);  

	  if (!node->isLeafNode()) {
	           
              pSibling->middleChild = middleChild;
              
              // Move node's child pChildOfNode to sibling 
	      pSibling->*Ptr2newParentOfChild  =  pChildOfNode;
	  }

	  // Remove node...
	  delete node;

	  if (parentIsEmpty) { 

	      fix(parent, pSibling); 
	  }
     }
   } // end main if/else
}

template<typename K> void Tree23<K>::ReassignChildren(Node23 *node, Node23* pChildOfNode, int situation)
{
    Node23 *parent = node->parent;
    
       switch (situation) {

            case 1:
            /*  Redistribute did (a). We must do (b)
              (20,      40)    (a)   (30,     40)     (b)   (30,     40)        
             /      |     \     =>   /     |    \     =>    /     |    \     
          Hole   (30,35)   50       20    35     50        20    35    50      
            |    /  | \    /\       |    / | \   /\        /\    / \   /\      
            C   29 32  37 60 70     C  29 32 37 45 70     C 29 32  37 45 70 */
                 node->leftChild = pChildOfNode;
                 node->rightChild = parent->middleChild->leftChild; 
                 parent->middleChild->leftChild = parent->middleChild->middleChild; 

                 break;
            case 2:

           /*      (10,       20)     Redist.    (15,   30)                                                                                     
                   /       |     \       =>     /     |    \                   (15,   30)   
                 Hole     15    (30,40)        10    20     40      Reassign    /    |  \   
                   |      / \    / | \          |    / \   / | \       =>     10   20     40                                                  
                   5    12 17  32 35 70         5   12 17 32 35 70   Children / \   / \   / \                                                  
                                                                             5  12 17 32 35 70      */                                                      
                   node->leftChild = pChildOfNode;

                   node->rightChild = parent->middleChild->leftChild;

                   parent->middleChild->leftChild = parent->middleChild->rightChild;
                   parent->middleChild->rightChild = parent->rightChild->leftChild;
                   parent->rightChild->leftChild = parent->rightChild->middleChild; 
                   break;
                   
            case 3: 
             

           /* 
               (10,  20)          Redist.     (10,  30)       Reassign  (10,    30)          
              /     |  \             =>      /     |   \      Children  /     |   \         
             7    Hole (30, 40)             7     20    40       =>    7     20    40  
            / \     |   /  |  \            / \     |   / | \          / \    /\    / \
           1   5   17  32 35  70          1   5   17  32 35  70      1   5 17  32 35  70 */

            case 4: // case 4 is identical to case 3.
              /*     (10,  20)          Redist.    (10,  30)       Reassign      (10,     30)  
                   /    |    \             =>      /    |    \     Children      /      |   \      
               (5,8)   Hole  (30, 40)         (5,8)    20    40      =>        (5,8)    20    40   
              /  | \    |   /   |  \          /  | \    |   / | \              / | \    /\    / \  
             1   7  9  17  32  35  70        1   7  9  17  32 35  70          1  7  9 17  32 35  70 */

                    node->leftChild = pChildOfNode;
                    node->rightChild = parent->rightChild->leftChild;
                    parent->rightChild->leftChild = parent->rightChild->middleChild;
                    break;

            case 5:
       /* We redistribute value of siblings, but we do not reassign here their children 
          (10,       25)              (10,      19)    Reassign      (10,       19)
         /       |     \       =>    /      |      \   Children     /      |       \                              
        5    (15, 19)   Hole        5       15      25    =>       5       15       25     
       / \    /  |  \    |         / \     / | \     |            / \     /  \     /  \    
      1   7  12 17  22  60        1   7  12  17 18  60           1   7  12    17  22   60   */
            
                   // If the sole child of node is the leftChild, move it to the rightChild.
                   node->rightChild = pChildOfNode;
                   node->leftChild = parent->middleChild->rightChild;
                   parent->middleChild->rightChild = parent->middleChild->middleChild;
                   

              case 6:

       /*  Redistribute() redistributed node values (a) but did not reassign their childa(b)
                  (20,       40)               (18,   30)           (18,    30)    
                  /       |     \    Redist   /     |   \   Shift   /     |    \                
              (10, 18)   30     Hole   =>   10     20    40   =>   10     20     40                         
               /  | \    /  \    /    (a)  / | \   / \   /   (b)  /  \   / \    / \
              5  12  19 25  32  55        5 12 19 25 32 55       5   12 19 25  32 55  
              */

                   // If the sole child of node is the leftChild, move it to the rightChild.
                   node->rightChild = (node->leftChild = pChildOfNode) ? node->leftChild : node->rightChild;

                   node->leftChild = parent->middleChild->rightChild;

                   parent->middleChild->rightChild = parent->middleChild->leftChild;
                   parent->middleChild->leftChild = parent->leftChild->rightChild;
                   parent->leftChild->rightChild = parent->leftChild->middleChild; 
                   break;

            case 7:
             /* parent is a two node and right Child is three node
                    20                            30                    30
                   /  \         Redistribute     /  \      Reassign    /   \               
                Hole  (30, 40)     =>           20    40     =>       20     40    
                  |    /  |  \                  |    / | \           / \    / \
                  5  25  35  50                 5  25  35  50       5  25 35  50 */


                  node->leftChild = pChildOfNode;
                  node->rightChild = parent->rightChild->leftChild; 
                  parent->rightChild->leftChild = parent->rightChild->middleChild;
                  break;


            case 8: /* parent is a two node and right Child is three node
                       50        After              40                        40        
                      /   \      Redistribute      /   \      Reassign       /  \     
                  (30, 40) Hole     =>           30     50       =>         30   50   
                  /  |  \   |                   / | \    |    Children     / \   / \  
                25  35  45  60                 25 35 45  60               25 35 45 60   */ 

                  node->rightChild = pChildOfNode;
                  node->leftChild = parent->leftChild->rightChild; 
                  parent->leftChild->rightChild = parent->rightChild->middleChild;
                  break;
       }
}
#endif	