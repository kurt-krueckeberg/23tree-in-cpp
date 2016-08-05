#ifndef debug_h_1826381263683
#define debug_h_1826381263683

#include <iterator>
#include <iostream>
#include <string>
#include <stack>

namespace debug {

template<class Node> void dump(const std::string& msg, const Node& node)  noexcept
{
  std::cout << msg << "\n";

  node.debug_print(std::cout);

  std::cout << std::endl;
}

using int_stack = std::stack<int>;

template<> void dump<int_stack>(const std::string& msg,  const int_stack& indecies)  noexcept;

} // end namespace. No semicolon

#endif
