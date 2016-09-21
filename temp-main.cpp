#include <cstdlib>
#include <algorithm>
#include <iterator>
#include <exception>
#include "include/new-tree23.h"

#include <utility> // debug purposes only

using namespace std;

int main(int argc, char** argv) 
{
  tree23<int, int> tree1 { {1, 2}, {2, 2}, {3, 3}, {4, 4} };

  tree23<int, int> tree2 {std::move(tree1) };
  
  
   return 0;
}
