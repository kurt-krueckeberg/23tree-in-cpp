#ifndef debug_level_order_printer_13123
#define debug_level_order_printer_13123

#include <utility>
#include <string>
#include <ostream>  
#include "level-order-display.h"

template<class Tree>
class debug_levelOrderPrinter : public levelOrderDisplay<Tree> {

  public:
     
     debug_levelOrderPrinter(const Tree& in_tree, std::ostream& ostr);
     debug_levelOrderPrinter(debug_levelOrderPrinter&); 
     debug_levelOrderPrinter(debug_levelOrderPrinter&&) = delete;
    ~debug_levelOrderPrinter() {}
    
   virtual void display_level(std::ostream& ostr, int level) noexcept override;
   virtual void display_node(std::ostream& ostr, const typename Tree::Node23& node) noexcept override; 
};

template<class Tree>
inline debug_levelOrderPrinter<Tree>::debug_levelOrderPrinter(const Tree& in_tree, std::ostream& ostr) : levelOrderDisplay<Tree>{in_tree, ostr} 
{
}

template<class Tree> 
inline debug_levelOrderPrinter<Tree>::debug_levelOrderPrinter(debug_levelOrderPrinter& lhs) : levelOrderDisplay<Tree>{lhs} 
{
}

template<class Tree>
inline void debug_levelOrderPrinter<Tree>::display_level(std::ostream& ostr, int level) noexcept
{
   levelOrderDisplay<Tree>::display_level(ostr, level);
}

template<class Tree>
inline void debug_levelOrderPrinter<Tree>::display_node(std::ostream& ostr, const typename Tree::Node23& node) noexcept
{
   if (&node == nullptr) {

      ostr << "[nullptr]";

   } else {

     node.debug_print(ostr);
   }

   ostr << std::flush;
}
#endif
