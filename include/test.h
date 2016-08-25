#ifndef test_23493274923
#define test_23493274923
#include <vector>

template<class Key, class Value> class tree23; // Forward declaration of template class tree23...

void level_order_print(const tree23<int, int>& tree);

void test_insert(const std::vector<int>& v, int break_key);

void run_test(const std::vector<int>& vec, void (*function)(const std::vector<int>&, int), int break_key = -1);
 
void run_tests(const std::vector<std::vector<int>>& other_cases, const std::vector<int>& base_case, void (*function)(const std::vector<int>&, int), \
        int break_key = -1);

void test_remove(const std::vector<int>& input, int break_key);

void test_copy_ctor(const std::vector<int>& input, int break_key); 

void test_forward_iterator(const std::vector<int>& input, int break_key); 

void print_with_forward_iterator(const tree23<int, int>& the_tree);

void test_backward_iterator(const std::vector<int>& input, int break_key); 

void print_with_backward_iterator(const tree23<int, int>& the_tree);

void print_with_reverse_iterator(const tree23<int, int>& tree);

#endif
