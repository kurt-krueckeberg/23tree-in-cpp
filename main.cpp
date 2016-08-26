#include <cstdlib>
#include <vector>
#include <algorithm>
#include <iterator>
#include <exception>
#include "include/tree23.h"
#include "include/test.h"

using namespace std;

int main(int argc, char** argv) 
{
  tree23<int, int> tree;

  vector<int> test_case {50, 39, 15, 65, 69, 150, 125, 20, 70, 100, 40, 34, 37, 30, 10, 33, 36, 38, 85, 90, 60, 35, 80, 89};
  vector<int> v2;

  transform(test_case.begin(), test_case.end(), back_inserter(v2), [=](int element) { return element * -1; });

  copy(v2.begin(), v2.end(), back_inserter(test_case));

  random_shuffle(test_case.begin(), test_case.end());
       
  vector<int> base_case { 10, 20, 30, 40, 50, 60, 70 , 80, 90, 100, 39 };

  // The last sub-vector combines all the others.
  vector<vector<int>> other_cases { vector<int>{ 15 }, vector<int>{ 89, 85 }, vector<int>{ 69, 65 }, vector<int>{ 150, 125 } ,\

  vector<int>{ 38, 37, 36, 35, 34, 33}, vector<int> { 15, 89, 85, 69, 65, 150, 125, 38, 37, 36, 35, 34, 33} };

  cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nRemove Tests\n";

  for( const auto& append_vec : other_cases) {

       vector<int> test_case {base_case};

       auto key_break = test_case.size();

       copy(append_vec.begin(), append_vec.end(), back_inserter(test_case));

       random_shuffle(test_case.begin(), test_case.end());
 
       cout << "Input to next case is: ";

       copy(test_case.begin(), test_case.end(), ostream_iterator<int>(cout, ", "));

       cout << endl;
       
       try {
      
          run_tests(other_cases, base_case, &test_nonconst_iterator);

              
       } catch(std::exception& e) { 
      
           cerr << "An exception occured:\n" << e.what() << endl;
       }
  }
  
   return 0;
}
