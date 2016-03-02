#ifndef  TREE23_H // new version
#define	TREE23_H

#include <utility>
#include <iostream>
#include <exception>
#include <array>
#include <memory>

// fwd declarations
template<typename T> class Tree23;    

class duplicatekey :  public std::exception {
public:
    
  virtual const char* what() const throw()
  {
    return "Attempting to insert duplicate key ";
  }
};

template<typename K> class Tree23 {
    
protected:       
    
   class Node34; // fwd declaration of protected class
   
   class Node23 {
      public: 
           friend class Tree23<K>;             
           friend class Node34;

           explicit Node23(bool three_node=false);
           
           Node23& operator=(Node23& rhs);
           explicit Node23(K key);
    
           Node23(K small, K large);
           Node23(K small, Node23 *pParent, Node23 *pleftChild, Node23 *prightChild);
           Node23(const Node23& n);
    
           Node23& makeTwoNode(K value, Node23 *pParent, Node23 *left, Node23 *right);                
    
           bool isLeafNode(); 
           
           bool isThreeNode();
           
           void setThreeNode(bool flag);
          
           K operator[](int index) const { return keys[index]; }

      private:
           Node23 *parent;
               
           K keys[2]; 

           Node23 *children[3]; 
                   
           bool isThreeNodeFlag; // or could use totalItems 
   };  
  
  
  protected:
    /* 
     * This class' constructor sorts the values of the Node23 and new_value into small, middle and large. For a leaf node, all child pointers will be zero.
     * For an internal node, they are assigned into two left-most and two right-most child pointers.
     */
     class Node34 {
         
      friend class Tree23<K>; 
      
       private:
          
          std::array<K, 3> keys;
   
          std::array<Node23 *, 4> children;
          
          /* Note: Their is no parent node pointer. */
       public:
    
	 /* Using default values allows us to generalize both leaf node and internal node cases. */
	 Node34(Node23 *threeNode, K new_value, Tree23<K>::Node23 *leftChildOfNewValue=nullptr,
                                                  Tree23<K>::Node23 *rightChildOfNewValue=nullptr);

        K operator[](int index) const { return keys[index]; } 

        Tree23<K>::Node23 *getChild(int index) { return children[index]; }
    };

   protected:

    Node23 *root; 
    
    bool DoSearch(K key, Node23 *root, Node23 *&location);

public:

   typedef Node23 Node;
 
     explicit Tree23() { root = nullptr; } 
    ~Tree23();
     template<typename Functor> void traverse(Functor f);

     // TODO: move ctor and move assignment
     bool Search(K key, Node23 *&location);
     bool remove(K key, Node23 *location=nullptr);
           
     Tree23<K>::Node23 *insert(K key, Node23 *location=nullptr) throw(duplicatekey);
};
/* 
 * It is sufficient to just check the leaf pointer. Internal nodes will have at minimun bot children[0]
 * and children[2] set.
 */   
template<typename K> inline  bool Tree23<K>::Node23::isLeafNode()  
{ 
   return !children[0] ? true : false;
}

template<typename K> inline bool Tree23<K>::Node23::isThreeNode() 
{ 
   return this->isThreeNodeFlag; 
}

template<typename K> inline void Tree23<K>::Node23::setThreeNode(bool flag) 
{
  this->isThreeNodeFlag = flag; 
}

template<typename K> inline Tree23<K>::Node23::Node23(K key) : isThreeNodeFlag(false)
{ 
   keys[0] = key; 
   children[0] = 0;
   children[1] = 0; 
}
/*
template<typename K> inline Tree23<K>::Node23::Node23(bool three_node) : isThreeNodeFlag(three_node), parent(0) 
{ 
     children[0] = 0;
     children[1] = 0;
}
*/
template<typename K> inline Tree23<K>::Node23::Node23(K small, K large) : isThreeNodeFlag(false), parent(0)
{ 
    keys[0] = small;
    keys[1] = large; 
   
    children[0] = nullptr;
    children[1] = nullptr;
    children[2] = nullptr;
}

template<typename K> inline Tree23<K>::Node23::Node23(K small, Tree23<K>::Node23 *pParent, Tree23<K>::Node23 *pleftChild,
              Tree23<K>::Node23 *prightChild) : isThreeNodeFlag(false), parent(pParent)
{ 
    keys[0] = small; 
    children[0] = pleftChild;
    children[1] = prightChild; // Change this to be: children[1] = rightChild;
} 

template<typename K> inline Tree23<K>::Node23::Node23(const Tree23<K>::Node23& n) : isThreeNodeFlag(n.isThreeNodeFlag), parent(n.parent) 
{ 
   children[0] = n.children[0];
   children[1] = n.children[1];
}


template<typename K> typename Tree23<K>::Node23& Tree23<K>::Node23::makeTwoNode(K value, Tree23<K>::Node23 *pParent,
        Tree23<K>::Node23 *left, Tree23<K>::Node23 *right)               
{
  keys[0] = value;
  children[0] = left;
  children[1] = right;
  isThreeNodeFlag = false; 
  parent =  pParent;
  return *this;
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

        DestroyTree(p->children[0]);

        DestroyTree(p->children[1]);

        DestroyTree(p->children[2]);

        delete p; 

  } else { // descend two node

        DestroyTree(p->children[0]);

        DestroyTree(p->children[1]);

        delete p; 
  }
}

template<typename K> template<typename Functor>  inline void Tree23<K>::traverse(Functor f) 
{
    return traverse(f, root); 
}

template<typename K> template<typename Functor>  void Tree23<K>::traverse(Functor f, Node23 *p)
{
  // Since the tree is always balanced, it is sufficient to check just one p
  if (p == nullptr) {
	return;
  }
	 
  if (p->isThreeNode()) { // descend three node

        traverse(f, p->children[0]);

        f(p->keys[0]);

        traverse(f, p->children[1]);

        f(p->keys[1]);

        traverse(f, p->children[2]);

  } else { // descend two node

        traverse(f, p->children[0]);

        f(p->keys[0]);

        traverse(f, p->children[2]);
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
    bool bRc;
    /*
     * Is key in current?
     */
    if (current->keys[0] == key || (current->isThreeNode() && current->keys[1] == key)) {
        
	location = current;
        return true; 
                
    } else if ((current->isLeafNode())) { 
        
        location = current;
	return false; 
     /*
      *  Does current have two keys?
      */ 
    } else if (current->isThreeNode()){
        
        if (key < current->keys[0]) {
            
            bRc = DoSearch(key, current->children[0], location); 
            
        }  else if (key < current->keys[1]) {
            
            bRc = DoSearch(key, current->children[1], location); 
            
        } else {
            
            bRc = DoSearch(key, current->children[2], location); 
        }
      
    } else { // ...or only one?
        
         if (key < current->keys[0]) {
            
            bRc = DoSearch(key, current->children[0], location); 
            
        }  else {
            
            bRc = DoSearch(key, current->children[1], location);  
        }    
    }
    return bRc;
}
#endif
