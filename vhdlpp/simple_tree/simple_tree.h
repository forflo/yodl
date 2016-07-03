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

    bool operator==(const SimpleTree<T> &that) const;

public:
    static void traverse_st(SimpleTree<T> *tree,
                            int depth = 0);

public:
    T root;
    vector<SimpleTree<T>*> forest;
};


SimpleTree<map<string, string>> *empty_simple_tree();

// has to be inline because c++
template<typename T>
bool SimpleTree<T>::operator==(const SimpleTree<T> &that) const {
    return (root == that.root &&
            forest == that.forest);
}

template<typename T>
bool operator==(const vector<SimpleTree<T>*> &thi,
                const vector<SimpleTree<T>*> &that){
    bool result(true);

    if (thi.size() != thi.size()){
        return false;
    }

    for (auto thiBegin = thi.begin(), thiEnd = thi.end(),
             thatBegin = that.begin(), thatEnd = that.end();
         thiBegin != thiEnd && thatBegin != thatEnd;
         ++thiBegin, ++thatBegin) {

        result = result && (**thiBegin == **thatBegin);
    }

    return result;
}

template<typename T>
void SimpleTree<T>::traverse_st(SimpleTree<T> *tree,
                                int depth){
    cout << "Root:\n";

    for (auto &i : tree->root){
        cout << i.first
             << " = "
             << i.second
             << "\n";
    }

    cout << "Forest: \n";
    for (auto &i : tree->forest)
        traverse_st(i, depth++);

    return;
}


#endif /* IVL_SIMPLE_TREE */
