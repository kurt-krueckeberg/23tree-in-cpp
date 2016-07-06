#include "include/debug.h"
#include "include/tree23.h"
#include <iostream>

using namespace std;

// Display the stack in "pop" order
template<> void debug::dump<stack<int>>(const string& msg,  const stack<int>& indecies)  noexcept
{
    cout << msg;
    
    struct Hack : public stack<int> {

        static int item(int i, const stack<int>& stack_ref)
        {
            return (stack_ref.*&Hack::c)[i];
        }
    };
    
    for(int i = indecies.size() - 1; i >= 0;  --i) {
      //    for(int i = 0;  i < indecies.size();  ++i) {

        cout << Hack::item(i, indecies) << ", ";
    }

    cout << endl;
}
