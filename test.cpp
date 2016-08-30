#include "include/test.h"
#include "include/tree23.h"
#include "include/level-order-display.h"
#include "include/debug-level-order-printer.h"
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
    
   for (auto& key : test_case) {

      cout << "Inserting: " << key << std::endl;
      
      tree.insert(key, key);

      print_tree(tree);  // or to print debug info change to: debug_print_tree(tree);      
      
    }

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
   
   debug_print_tree(tree);
   
   for(auto& key : removal_vec) {

       cout << "\nRemoving key: " << key << endl;
       
       tree.remove(key);

       cout << "\nPrinting tree in order after removal: \n";
               
       debug_print_tree(tree);   // to do regular print do: print_tree(tree);
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
  
  auto lambda_closure = [](const tree23<int, int>::KeyValue& key_value ){ cout << key_value.key << ", "; };
  
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

  auto pair1 = *--citer3; 
  
  std::pair<const int, const int&> pair2 = *++citer3; 
  
  std::pair<const int, const int&> pair3 = *++citer3; 
     
  int debug = 10;
  
  ++debug;
}

void test_nonconst_iterator(const std::vector<int>& input)
{
  tree23<int, int> tree = insert_vec_into_tree(input); 

  tree23<int, int> tree_copy{tree};
  
  auto lambda_closure = [](const tree23<int, int>::KeyValue& key_value ){ cout << key_value.key << ", "; };
  
  cout << "input tree first followed by its copy" << endl;

  tree.inOrderTraverse(lambda_closure);
  
  cout << "\n";

  cout << "Printing with non-const iterator\n";

  using nonconst_tree23 = tree23<int, int>;

  tree23<int, int> nonconst_tree = const_cast<nonconst_tree23&>(tree);

  tree23<int, int>::iterator iter = nonconst_tree.begin(); // This calls the non-const version of begin()

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
  
  auto lambda_closure = [](const tree23<int, int>::KeyValue& key_value ){ cout << key_value.key << ", "; };

  tree.inOrderTraverse(lambda_closure);
  
  cout << flush << "\nPrinting tree with iterator:\n";

  try {

    print_with_forward_iterator(tree);

  } catch (std::exception& e) {

    cout << "\nException: " << e.what() << endl; 
  }

  cout << flush; 
  return; 
}

void print_with_forward_iterator(const tree23<int, int>& tree)
{ 
  auto citer = tree.begin();
  auto cend_iter = tree.end();

  for( ; citer != cend_iter; ++citer) {

       cout << (*citer).first << ", ";
       cout << flush;
  } 
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
 
  cout << "\n\nIn order print of tree:\n";
  
  auto lambda_closure = [](const tree23<int, int>::KeyValue& key_value ){ cout << key_value.key << ", "; };

  tree.inOrderTraverse(lambda_closure);
  
  cout << flush << "\nPrinting tree with backward iterator:\n";

  try {

     print_with_backward_iterator(tree);

  } catch (std::exception& e) {

     cout << "\nException: " << e.what() << endl; 
  }

  cout << flush; 
  return; 
}

void print_with_backward_iterator(const tree23<int, int>& tree)
{
 auto iter = tree.end();

 auto iter_begin = tree.begin();

 --iter;

 for(; iter != iter_begin; --iter) {

   auto _pair = *iter;

   cout << _pair.first << ", " << flush;
 } 

}

void test_reverse_iterators(const vector<int>& input)
{
   tree23<int, int> tree{ insert_vec_into_tree(input) };

   print_with_nonconst_reverse_iterator(tree);

   print_with_const_reverse_iterator(tree);
}

void print_with_nonconst_reverse_iterator(tree23<int, int>& tree)
{
 using tree23_int = tree23<int, int>;

 tree23<int, int>& tree23 = const_cast<tree23_int&>(tree);

 auto riter = tree.rbegin();
 auto riter_end = tree.rend();
  
 for(; riter != riter_end; ++riter) {

    auto pair_ = *riter; // causes compiler error.

    cout << pair_.first << ", " << flush;
 }
}

void print_with_const_reverse_iterator(const tree23<int, int>& tree)
{

 tree23<int, int>::const_reverse_iterator criter = tree.rbegin();
 tree23<int, int>::const_reverse_iterator criter_end = tree.rend();

 for(; criter != criter_end; ++criter) {

   pair<const int, const int&> pair_ = *criter; //error: conversion from ‘const tree23<int, int>::KeyValue’ to non-scalar type ‘std::pair<const int, const int&>’ requested

   cout << pair_.first << ", " << flush;
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
  
  auto lambda_closure = [](const tree23<int, int>::KeyValue& key_value ){ cout << key_value.key << ' '; };
/*
  tree.inOrderTraverse(lambda_closure);
 */ 
    
  tree23<int, int>::const_iterator const_iter = tree.begin();
  
  for(const auto& pair_ : tree) {
      
      cout << pair_.first << flush;
  }
  cout << endl;
}
 
void debug_print_tree(const tree23<int, int> & tree)
{
  level_order_print(tree);
   
  debug_levelOrderPrinter<tree23<int,int>> debugprintFunctor(tree, cout);

  cout << "\nLevel order debug print of tree: \n" << flush;
  
  tree.levelOrderTraverse(debugprintFunctor);
   
  cout << endl;

  cout << "\n\nIn order print of tree: ";

  auto lambda_closure = [](const tree23<int, int>::KeyValue& key_value ){ cout << key_value.key << ' '; };

  tree.inOrderTraverse(lambda_closure);
  
  cout << endl << flush;
}
