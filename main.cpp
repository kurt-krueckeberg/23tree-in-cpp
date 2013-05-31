/* 
 * File:   main.cpp
 * Author: kurt
 *
 * Created on April 22, 2013, 6:56 PM
 */

#include <cstdlib>
#include "tree23.h"
#include "TreePrinter.h"
#include <iostream>

using namespace std;

int main(int argc, char** argv) 
{
    
    int v[] = { 10, 20, 30, 40, 50 ,60, 70 , 80, 90, 37, 36, 35, 34 };
    int size = sizeof(v)/sizeof(int);
    
    Tree23<int> tree;
        
    Node23<int> *inserted_node = 0;

    /* Build this tree:

            (34,  40)
           /    |    \
         20    36    (60,80)  
        / \    / \   /  | \
       10 30 35 37 50 70  90 */
       
    for(int i = 0; i < size; i++) {
        
        inserted_node = tree.insert(v[i]);
    }
    
    TreePrinter tree_printer(cout);
    tree.Traverse(tree_printer); 
    cout << endl;
    
    tree.remove(10);
     /* Leaves this tree:

            (36,     60)
          /      |    \
         34      40    80  
        / \      / \   / \
  (20,30) 35    37 50 70  90 */
         
    
    tree.remove(36);  
    
    tree.Traverse(tree_printer); // This blows up because we do not have a proper tree.
    cout << "------------" << endl;
    cout << endl;
    return 0;
}

