#ifndef level_order_invariant_report_23434 
#define level_order_invariant_report_23434 

#include <utility>
#include <string>
#include <ostream>  
#include "level-order-display.h" 
  
template<class Tree>
class levelOrderInvariantReport : public levelOrderDisplay<Tree> {
  public:
     
     levelOrderInvariantReport(const Tree& in_tree, std::ostream& ostr_lhs); 
     
     levelOrderInvariantReport(levelOrderInvariantReport& in_report) : levelOrderDisplay<Tree>{in_report} {}
     levelOrderInvariantReport(levelOrderInvariantReport&& in) = delete;
    ~levelOrderInvariantReport() {};
    
   virtual void display_level(std::ostream& ostr, int level) noexcept override;
   virtual void display_node(std::ostream& ostr, const  typename Tree::Node23& node) noexcept override; 
};

template<class Tree>
inline levelOrderInvariantReport<Tree>::levelOrderInvariantReport(const Tree& in_tree, std::ostream& ostr_lhs) : levelOrderDisplay<Tree>(in_tree, ostr_lhs)
{
}

template<class Tree>
inline void levelOrderInvariantReport<Tree>::display_level(std::ostream& ostr, int level) noexcept
{
   levelOrderDisplay<Tree>::display_level(ostr, level);
}

template<class Tree>
inline void levelOrderInvariantReport<Tree>::display_node(std::ostream& ostr, const typename Tree::Node23& node) noexcept
{
   std::string errors;

   if (&node == nullptr) {

      ostr << "[nullptr]";
   }
   else {

      node.print(ostr);
      const Tree& tree = levelOrderDisplay<Tree>::getTree();
      ostr << tree.test_invariant(node) << ", ";
   }

   ostr << std::flush;
}
#endif
