#ifndef DebugPrinter_13123
#define DebugPrinter_13123

#include <utility>
#include <string>
#include <ostream>  
#include "level-order-display.h"

template<class Tree>
class DebugPrinter : public levelOrderDisplay<Tree> {

  public:
     
     DebugPrinter(const Tree& in_tree, std::ostream& ostr);
     DebugPrinter(DebugPrinter&); 
     DebugPrinter(DebugPrinter&&) = delete;
    ~DebugPrinter() {}
    
   virtual void display_level(std::ostream& ostr, int level) noexcept override;
   virtual void display_node(std::ostream& ostr, const typename Tree::node_type& node) noexcept override; 
};

template<class Tree>
inline DebugPrinter<Tree>::DebugPrinter(const Tree& in_tree, std::ostream& ostr) : levelOrderDisplay<Tree>{in_tree, ostr} 
{
}

template<class Tree> 
inline DebugPrinter<Tree>::DebugPrinter(DebugPrinter& lhs) : levelOrderDisplay<Tree>{lhs} 
{
}

template<class Tree>
inline void DebugPrinter<Tree>::display_level(std::ostream& ostr, int level) noexcept
{
   levelOrderDisplay<Tree>::display_level(ostr, level);
}

template<class Tree>
inline void DebugPrinter<Tree>::display_node(std::ostream& ostr, const typename Tree::node_type& node) noexcept
{
   if (&node == nullptr) {

      ostr << "[nullptr]";

   } else {

     node.debug_print(ostr);
   }

   ostr << std::flush;
}
#endif
