#ifndef IVL_SIMPLE_TREE
#define IVL_SIMPLE_TREE

# include <vector>
# include <map>
# include <string>
# include <iostream>

using namespace std;

template<typename T> class SimpleTree;

template<typename T>
bool operator==(const vector<SimpleTree<T>*> &thi,
                const vector<SimpleTree<T>*> &that);

template<typename T>
class SimpleTree {
public:
    SimpleTree(const T s)
        : root(s) { };

    ~SimpleTree(){
        for (auto &i : forest)
            delete i;
    };

    SimpleTree(const T s, vector<SimpleTree<T>*> own)
        : root(s)
        , forest(own) { };

    SimpleTree(const T s, SimpleTree<T> *own)
        : root(s) {
        forest.push_back(own);
    };

    bool operator==(const SimpleTree &that);

public:
    static void traverse_st(SimpleTree<T> *tree,
                            int depth = 0);

public:
    T root;
    vector<SimpleTree<T>*> forest;
};



SimpleTree<map<string, string>> *empty_simple_tree();

#endif /* IVL_SIMPLE_TREE */
