#ifndef IVL_SIMPLE_TREE
#define IVL_SIMPLE_TREE

# include <vector>
# include <string>
# include <iostream>

using namespace std;

template<typename T>
class simple_tree {
public:
    T root;
    vector<simple_tree<T>*> forest;

    simple_tree(const T s)
        : root(s) { }
    
    simple_tree(const T s, const vector<simple_tree<T>*> own)
        : root(s)
        , forest(own) { }
};

#endif /* IVL_SIMPLE_TREE */
