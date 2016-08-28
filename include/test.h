#ifndef test_23493274923
#define test_23493274923
#include <vector>
#include  "tree23.h"

template<class Key, class Value> class tree23; // Forward declaration of template class tree23...

void run_test(const std::vector<int>& vec, void (*function)(const std::vector<int>&));
 
void run_tests(const std::vector<std::vector<int>>& other_cases, const std::vector<int>& base_case, void (*function)(const std::vector<int>&));

tree23<int, int> insert_vec_into_tree(const std::vector<int>& test_case);

void test_nonconst_iterator(const std::vector<int>& v);

void level_order_print(const tree23<int, int>& tree);

void test_insert(const std::vector<int>& v);

void test_remove(const std::vector<int>& input);

void test_copy_ctor(const std::vector<int>& input);

void test_const_iterator_methods(const std::vector<int>& input);

void test_forward_iterator(const std::vector<int>& input);

void print_tree(const tree23<int, int>& tree);

void print_with_nonconst_reverse_iterator(tree23<int, int>& tree);

void debug_print_tree(const tree23<int, int> & tree);

void print_with_forward_iterator(const tree23<int, int>& the_tree);

void test_backward_iterator(const std::vector<int>& input);

void print_with_backward_iterator(const tree23<int, int>& the_tree);

void print_with_reverse_iterator(const tree23<int, int>& tree);

class reverse_iterator_sim {

   tree23<int, int>::iterator current;

   public:

    reverse_iterator_sim(tree23<int, int>& lhs_tree) : current{lhs_tree} {}     

//    tree23<int, int>::iterator base() const;

    tree23<int, int>::iterator base_nonconst();

};

#endif
