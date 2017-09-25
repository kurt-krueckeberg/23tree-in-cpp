#include "include/test.h"
#include "include/tree23.h"
#include "include/level-order-display.h"
//#include "include/debug-level-order-printer.h"
#include "include/debug-printer.h"
#include <iostream>
#include <iterator>
#include <algorithm>

using namespace std;

void run_test(const vector<int>& test_case, void (*f)(const std::vector<int>&))
{  
    cout << "Input vector<int> to next case is: \n";

    copy(test_case.begin(), test_case.end(), ostream_iterator<int>(cout, ", "));
           
    cout << endl << flush;
    
    try {
   
        (*f)(test_case);
   
    } catch(...) {

        cout << "An uncaught exception occured:\n"  << endl;
    }
    
    cout << "\n================================" << endl; 
}

void run_tests(const std::vector<std::vector<int>>& other_cases, const std::vector<int>& base_case, void (*f)(const std::vector<int>&))
{
   for( const auto& append_vec : other_cases) {

       vector<int> test_case {base_case};

       auto key_break = test_case.size();

       copy(append_vec.begin(), append_vec.end(), back_inserter(test_case));

       random_shuffle(test_case.begin(), test_case.end());
       
       cout << "Input vector<int> to next case is: \n";

       copy(test_case.begin(), test_case.end(), ostream_iterator<int>(cout, ", "));
              
       cout << endl << flush;
       
       try {
      
           (*f)(test_case);
      
       } catch(...) {

           cout << "An uncaught exception occured:\n"  << endl;
       }
       
       cout << "\n================================" << endl; 
  }

}

tree23<int, int> insert_vec_into_tree(const vector<int>& test_case)
{
  tree23<int, int> tree;

  for (auto& key : test_case) {

      cout << "Inserting: " << key << std::endl;
      
      tree.insert(key, key);
  }

  return tree;
}

void test_insert(const vector<int>& test_case)
{    
  tree23<int, int> tree;
  
   int i = 0;
    
   tree = insert_vec_into_tree(test_case);

   print_tree(tree);  // or to print debug info change to: debug_print_tree(tree);      

   cout << flush << "\nTesting find(v) of all keys v inserted so for. First, reprinting the tree.\n" << flush;

   print_tree(tree);  

   for (auto key : test_case) {
      
        string result_msg = tree.find(key) ? " found " : " not found ";
           
        cout << "Key " << key << " was " << result_msg << endl; 
   } 

   return; 
}

void test_remove(const std::vector<int>& test_case)
{  
   tree23<int, int> tree(insert_vec_into_tree(test_case)); 

   vector<int> removal_vec{test_case}; // make copy of test_case vector

   random_shuffle(removal_vec.begin(), removal_vec.end()); // and scramble it.
   
   cout << "Printing the tree before removals start.\n";
   
   //--debug_print_tree(tree);
   print_tree(tree); //++
   
   for(auto& key : removal_vec) {

       cout << "\nRemoving key: " << key << endl;
       
       tree.remove(key);

       cout << "\nPrinting tree in order after removal: \n";
               
       //--debug_print_tree(tree);   // to do regular print do: print_tree(tree);
       print_tree(tree);   // to do regular print do: print_tree(tree); //++
    }
       
    cout << flush << "\nTesting find(v) of all keys v removed so for. First, reprinting the tree.\n" << flush;

    print_tree(tree);  

    for (auto& key : removal_vec) {
      
         string result_msg = tree.find(key) ? " found " : " not found ";
           
         cout << "Key " << key << " was " << result_msg << endl; 
    } 
}

void test_copy_ctor(const std::vector<int>& input)
{
  tree23<int, int> tree{ insert_vec_into_tree(input) }; 

  tree23<int, int> tree_copy{tree};
  
  auto lambda_closure = [](const pair<int, int>& pr){ cout << pr.first << ", "; };
  
  cout << "input tree first followed by its copy" << endl;

  tree.inOrderTraverse(lambda_closure);
  
  cout << "\n";
}

void test_const_iterator_methods(const std::vector<int>& input)
{
  const tree23<int, int> tree{ insert_vec_into_tree(input)};
  
  tree23<int, int>::const_iterator citer{ tree.begin() };

  tree23<int, int>::const_iterator citerb{ tree.begin() };

  cout << "Testing const_iterator comparison operators == and !=.\n" << endl << flush;

  if (citer == citerb) {
    cout << "citer == citerb\n" << endl << flush;
  }
 
  if (citer != citerb) {
    cout << "citer != citerb\n" << endl << flush;
  }

  cout << "Testing const_iterator move constructor:\ntree23<int, int>::const_iterator citer2 {std::move(citer)};" << endl << flush;

  tree23<int, int>::const_iterator citer2 {std::move(citer)};

  cout << "Testing const_iterator move constructor:\ntree23<int, int>::const_iterator citer3 = citer2;" << endl << flush;

  tree23<int, int>::const_iterator citer3 = citer2; // Invokes the copy constructor.

  auto key_value1 = *--citer3; 
 /* 
  const tree23<int, int>::KeyValue& key_value2 = *++citer3; 
  
  const tree23<int, int>::KeyValue& key_value3 = *++citer3;   
  */

  const pair<const int, int>& key_value2 = *++citer3; 
  
  const pair<const int, int>& key_value3 = *++citer3; 
     
  int debug = 10;
  
  ++debug;
}

void test_nonconst_iterator(const std::vector<int>& input)
{
  tree23<int, int> tree = insert_vec_into_tree(input); 

  tree23<int, int> tree_copy{tree};
  
  auto lambda_closure = [](const pair<const int, int>& key_value ){ cout << key_value.first << ", "; };
  
  cout << "input tree first followed by its copy" << endl;

  tree.inOrderTraverse(lambda_closure);
  
  cout << "\n";

  cout << "Printing with non-const iterator\n";

  using nonconst_tree23 = tree23<int, int>;

  tree23<int, int> nonconst_tree = const_cast<nonconst_tree23&>(tree);

  tree23<int, int>::iterator iter = nonconst_tree.begin();

  for (; iter != nonconst_tree.end(); ++iter) {
 
        //--cout << (*iter).key << ", " << endl << flush;
        cout << (*iter).first << ", " << endl << flush;
  }
  cout << endl;
}

void test_forward_iterator(const std::vector<int>& test_case)
{
  tree23<int, int> tree;
  
   int i = 0;
    
   for (auto& key : test_case) {

      cout << "Inserting: " << key << std::endl;
      
      tree.insert(key, key);
    }

  level_order_print(tree);
  
  cout << "\n\nIn order print of tree:\n";
  
  auto lambda_closure = [](const pair<const int, int>& key_value ){ cout << key_value.first << ", "; };

  tree.inOrderTraverse(lambda_closure);
  
  cout << flush << "\nPrinting tree with iterator:\n";

  try {

    print_with_iterator(tree);

  } catch (std::exception& e) {

    cout << "\nException: " << e.what() << endl; 
  }

  cout << flush; 
  return; 
}

void print_with_iterator(const tree23<int, int>& tree)
{
  for (const auto& pr : tree) 
       cout << pr.first << ", " << flush;

}

void test_backward_iterator(const std::vector<int>& input)
{
  tree23<int, int> tree;
  
   int i = 0;
    
   for (auto& key : input) {

      cout << "Inserting: " << key << std::endl;
                
      tree.insert(key, key);
    }

  level_order_print(tree);
 
  cout << "\n\nPrinting tree with iterator:\n";
  
  print_with_iterator(tree); 
  
  cout << flush << "\nPrinting tree with reverse iterator:\n";

  try {

     print_with_reverse_iterator(tree);

  } catch (std::exception& e) {

     cout << "\nException: " << e.what() << endl; 
  }

  cout << flush; 
  return; 
}

void print_with_reverse_iterator(const tree23<int, int>& tree)
{
 for (auto it = tree.rbegin(); it != tree.rend(); ++it) 
     cout << (*it).first << ", " << flush;
 
}

void test_reverse_iterators(const vector<int>& input)
{
   tree23<int, int> tree{ insert_vec_into_tree(input) };

   print_with_const_reverse_iterator(tree);
}


void print_with_const_reverse_iterator(const tree23<int, int>& tree)
{

 tree23<int, int>::const_reverse_iterator riter = tree.rbegin();
 tree23<int, int>::const_reverse_iterator riter_end = tree.rend();

 for(; riter != riter_end; ++riter) {

   const tree23<int, int>::value_type& key_value = *riter;

   cout << key_value.first << ", " << flush;
 } 
}

void level_order_print(const tree23<int, int>& tree)
{
  cout << flush << "Level order print of tree: ";

  levelOrderDisplay<tree23<int, int>> printFunctor(tree, cout);

  tree.levelOrderTraverse(printFunctor);
   
  cout << flush;
}

void print_tree(const tree23<int, int>& tree)
{
  level_order_print(tree);

  cout << "\n\nIn order print of tree: ";
  
  auto lambda_closure = [](const tree23<int, int>::value_type& key_value ){ cout << key_value.first << ' '; };
/*
  tree.inOrderTraverse(lambda_closure);
 */ 
    
  tree23<int, int>::const_iterator const_iter = tree.begin();
  
  for(const auto& key_value : tree) {
      
      //--cout << key_value.key << flush;
      cout << key_value.first << ", " << flush;
  }
  cout << endl;
}
 
void debug_print_tree(const tree23<int, int> & tree)
{
  level_order_print(tree);
   
  //--debug_levelOrderPrinter<tree23<int,int>> debugprintFunctor(tree, cout);
  DebugPrinter<tree23<int,int>> debug_printer(tree, cout);

  cout << "\nLevel order debug print of tree: \n" << flush;
  
  tree.levelOrderTraverse(debug_printer);
   
  cout << endl;

  cout << "\n\nIn order print of tree: ";

  auto lambda_closure = [](const tree23<int, int>::value_type& key_value ){ cout << key_value.first << ' '; };

  tree.inOrderTraverse(lambda_closure);
  
  cout << endl << flush;
}
