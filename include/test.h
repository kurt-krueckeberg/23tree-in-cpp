#ifndef test_23493274923
#define test_23493274923
#include <vector>

void test_insert(const std::vector<int>& v, int break_key);

void run_tests(const std::vector<std::vector<int>>& other_cases, const std::vector<int>& base_case, void (*function)(const std::vector<int>&, int), \
        int break_key = -1);
 
//void run_insert_tests(const std::vector<std::vector<int>>& other_cases, const std::vector<int>& base_case);

void test_remove(const std::vector<int>& input, int break_key);

void test_copy_ctor(const std::vector<int>& input, int break_key); 

#endif
