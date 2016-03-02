/* 
 * File:   main.cpp
 * Author: kurt
 *
 * Created on April 22, 2013, 6:56 PM
 */

#include <cstdlib>
//#include "include/new-tree23.h"
#include "include/tree23.h"
#include <iostream>
#include <vector>
#include <functional>
using namespace std;


int main(int argc, char** argv) 
{
   
    vector<int> v { 10, 20, 30, 40, 50 ,60, 70 , 80, 90, 37, 36, 35, 34 };
    
    int size = sizeof(v)/sizeof(int);
    
    Tree23<int> tree;
        
    Tree23<int>::Node *inserted_node = nullptr;

    /* Build this tree:

            (34,  40)
           /    |    \
         20    36    (60,80)  
        / \    / \   /  | \
       10 30 35 37 50 70  90 */
  
    for(auto iter = v.begin(); iter != v.end(); ++iter) {
        
        inserted_node = tree.insert(*iter);
    }
      
    /* lambda code fails to compile, be sure -std=c++11 is being used, or
     * use 
     *  void print_int(int x) { cout << x << ' '; }
     *  //...snip
     *  tree.traverse(print_int);
     * instead.
     */ 
    tree.traverse([](int x){ cout << x << ' '; }); 
            
    tree.remove(10);
     /* Leaves this tree:

            (36,     60)
          /      |    \
         34      40    80  
        / \      / \   / \
  (20,30) 35    37 50 70  90 */
         
    
   tree.remove(36);  
   
   tree.traverse([](int x){ cout << x << ' '; }); // This blows up because we do not have a proper tree.
     
   cout << "\n------------\n" << endl;
   cout << endl;
   return 0;
}

