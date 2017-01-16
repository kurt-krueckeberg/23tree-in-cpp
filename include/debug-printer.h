#ifndef debug_printer_13123
#define debug_printer_13123

#include <utility>
#include <string>
#include <ostream>  
#include "level-order-display.h"

template<class Tree>
class debug_printer : public levelOrderDisplay<Tree> {

  public:
     
     debug_printer(const Tree& in_tree, std::ostream& ostr);
     debug_printer(debug_printer&); 
     debug_printer(debug_printer&&) = delete;
    ~debug_printer() {}
    
   virtual void display_level(std::ostream& ostr, int level) noexcept override;
   virtual void display_node(std::ostream& ostr, const typename Tree::Node23& node) noexcept override; 
};

template<class Tree>
inline debug_printer<Tree>::debug_printer(const Tree& in_tree, std::ostream& ostr) : levelOrderDisplay<Tree>{in_tree, ostr} 
{
}

template<class Tree> 
inline debug_printer<Tree>::debug_printer(debug_printer& lhs) : levelOrderDisplay<Tree>{lhs} 
{
}

template<class Tree>
inline void debug_printer<Tree>::display_level(std::ostream& ostr, int level) noexcept
{
   levelOrderDisplay<Tree>::display_level(ostr, level);
}

template<class Tree>
inline void debug_printer<Tree>::display_node(std::ostream& ostr, const typename Tree::Node23& node) noexcept
{
   if (&node == nullptr) {

      ostr << "[nullptr]";

   } else {

     node.debug_print(ostr);
   }

   ostr << std::flush;
}
#endif
