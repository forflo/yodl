#ifndef IVL_SIMPLE_TREE
#define IVL_SIMPLE_TREE

# include <vector>
# include <map>
# include <string>
# include <iostream>

using namespace std;

template<typename T>
class SimpleTree {
public:
    T root;
    vector<SimpleTree<T>*> forest;

    SimpleTree(const T s)
        : root(s) { };

    SimpleTree(const T s, vector<SimpleTree<T>*> own)
        : root(s)
        , forest(own) { };

    SimpleTree(const T s, SimpleTree<T> *own)
        : root(s) {
        forest.push_back(own);
    };

    ~SimpleTree(){
        for (auto &i : forest)
            delete i;
    };
};

void traverse_st(SimpleTree<map<string, string>> *tree, int depth = 0);

SimpleTree<map<string, string>> *empty_simple_tree();

#endif /* IVL_SIMPLE_TREE */
