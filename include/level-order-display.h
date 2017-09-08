#ifndef level_order_display_82394u9324
#define level_order_display_82394u9324

#include <string>

/* 
 *  "template design pattern" functor class for displaying tree23<Key, Value>::node_type instances in level order. Its function call operator

    void operator()(const Tree::node_type& node)

   calls two virtual methods that derived classes override: 

     1. display_level(ostream& ostr, int level), which provides a default implementation.
     2. display_node(ostream& ostr, const Tree::Node& node), which simply calls node.print(ostr).

   A default implementation is provided for display_level().

   The constructor takes two parameters: a const Tree& and  an ostream&. Its copy constructor queries and sets the height by calling tree.getHeight(). Is also sets/
   resets the input copy constructors input parameter's height. This acts like a sort of reset() method, when the functor is passed by value, that re-queries the
   tree's height. 
 * 
 */

template<class Tree>
class levelOrderDisplay  {

  private:
     const Tree& tree;
     int height;
     int current_level;

     std::ostream& ostr;
      
  public:
    levelOrderDisplay(const Tree& tree, std::ostream& ostr_lhs ); 
    levelOrderDisplay(const levelOrderDisplay&);
    levelOrderDisplay(levelOrderDisplay&&) = delete;

    const Tree& getTree() const noexcept { return tree; }

    virtual ~levelOrderDisplay() = default;  
    virtual void initializeTraverse() noexcept;

    // template method, which invokes display methods below.
    void operator()(const typename  Tree::node_type& node, int level) noexcept;
    void operator()(const typename  Tree::node_type* pnode, int level) noexcept;

    virtual void display_node(std::ostream& ostr, const typename Tree::node_type& node) noexcept; 
    virtual void display_level(std::ostream& ostr,  int level) noexcept;
    
};
template<class Tree>
inline levelOrderDisplay<Tree>::levelOrderDisplay(const Tree& in_tree, std::ostream& ostr_lhs) : tree{in_tree}, ostr{ostr_lhs}
{
   initializeTraverse();	
}

template<class Tree>
inline void levelOrderDisplay<Tree>::initializeTraverse() noexcept
{
   height = tree.getHeight();	
   current_level = 0;
}

template<class Tree>
inline levelOrderDisplay<Tree>::levelOrderDisplay(const levelOrderDisplay& lhs) : tree{lhs.tree}, ostr{lhs.ostr}, height{lhs.height}
{
  current_level = 0;
}


template<class Tree> void levelOrderDisplay<Tree>::operator()(const typename Tree::node_type& node, int level) noexcept
{
   // Did current_level change?
   if (current_level != level) { 

         current_level = level;

         display_level(ostr, level);       
   }
   
   display_node(ostr, node); 
}

template<class Tree> void levelOrderDisplay<Tree>::operator()(const typename Tree::node_type* pnode, int level) noexcept
{
   // Did current_level change?
   if (current_level != level) { 

         current_level = level;

         display_level(ostr, level);       
   }
   
   display_node(ostr, *pnode); 
}

// Default implementation that derived class can use if they so desire to display current level of the tree that will next be displayed.
template<class Tree>
void levelOrderDisplay<Tree>::display_level(std::ostream& ostr, int level) noexcept
{
  ostr << "\n\n" << "current_level = " <<  current_level; 
     
  // Provide some basic spacing to tree appearance.
  std::size_t num = height - current_level + 1;
  
  std::string str( num, ' ');
  
  ostr << str;
}

// Default implementation streams the node 
template<class Tree> inline
void  levelOrderDisplay<Tree>::display_node(std::ostream& ostr, const typename Tree::node_type& node) noexcept
{
    if (&node == nullptr) {
        
       ostr << "[nullptr]";   

    } else {
        
     ostr << node << ", "; 
    }
}
#endif
