## Possible Bug

The reverse\_iterator prints--I think-- the last element twice when used in a loop like this:

.. code-block:: cpp

    template<class Key, class Value> void rprint(const tree234<Key, Value>& tree, ostream& ostr)
    {
       // This has a  bug. 
       auto end = tree.rend();
    
       for(auto iter = tree.rbegin(); iter != end; ++iter) {
     
          auto& pr = *iter;
    
          ostr << pr.first << ", " << flush;
       }
       
       ostr << endl;
    }


## Thoughts on emplace()

The 4-node is used to combine a 3-node and a new key, initially the key to be inserted. The question is, what do we do with the value associated with the key, or in the
case of emplace(), what do we do with the constructor arguments associated with a value that will be created in place from existing memory? See http://algs4.cs.princeton.edu/33balanced. 

### Understanding Variadic Templates and How to Use Them

Read these articles to understand variadic template functions. Examine the examples in them and understand them. 

- [Lecture Slides on Variadic templates by jarne Stroutrup](https://parasol.tamu.edu/people/bs/622-GP/variadic-templates-and-tuples.pdf) 
- [C++ Templates What is a variadic template function?](https://www.ibm.com/developerworks/community/blogs/5894415f-be62-4bc0-81c5-3956e82276f3/entry/c_templates_what_is_a_variadic_template_function41?lang=en)
- [Introduction to C++ Variadic Tempaltes](http://kevinushey.github.io/blog/2016/01/27/introduction-to-c++-variadic-templates/)
- [Practical Uses of Variadic Templates](https://crascit.com/2015/03/21/practical-uses-for-variadic-templates/)
- [Variadic Templates in C++](http://eli.thegreenplace.net/2014/variadic-templates-in-c/)
- [How vector emplace() and emplace\_back() use Variadic Templates](http://enki-tech.blogspot.com/2012/08/c11-vector-improved-how-it-works.html)
- [Variadic Templates for the Doubters](http://lbrandy.com/blog/2013/03/variadic_templates/)
- [Can't Understand Variadic templates in C++](http://stackoverflow.com/questions/30937379/cant-understand-variadic-templates-in-c)

## Technical References on Trees and Their Iterators 

### Implementation Links and Discusssion

1. [odu] - Traversing Trees with Iterator, an STL-compatible iterator Q&A teaching discussion
2. [geeksforgeeks] - Conceptual Discussion with C code implementation using a stack.
3. [stackoverflow] - Stackoverflow Discussion with a link to SGI STL implmentation 
4. [FSU] - FSU lecture that discusses external bidirectional tree iterator in C++
5. [cmu] - See Non-Recursive Traversals section that discuss forward iteration in Java using a stack
6. [csohio] - Discusses trees and has an implementation in Java

[odu]: <https://secweb.cs.odu.edu/~zeil/cs361/web/website/Lectures/treetraversal/page/treetraversal.html> 
[geeksforgeeks]: <http://www.geeksforgeeks.org/inorder-tree-traversal-without-recursion/>
[stackoverflow]: <http://stackoverflow.com/questions/12684191/implementing-an-iterator-over-binary-or-arbitrary-tree-using-c-11>
[FSU]: <http://www.cs.fsu.edu/~lacher/courses/COP4530/lectures/binary_search_trees3/index.html?$$$slide05i.html$$$>
[cmu]: <https://www.cs.cmu.edu/~adamchik/15-121/lectures/Trees/trees.html>
[csohio]: <http://grail.cba.csuohio.edu/~matos/notes/cis-265/lecture-notes/11-26slide.pdf>

