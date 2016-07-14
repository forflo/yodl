#ifndef IVL_SIMPLE_TREE
#define IVL_SIMPLE_TREE

# include <vector>
# include <map>
# include <string>
# include <iostream>

template<typename T> class SimpleTree;

template<typename T>
bool operator==(const std::vector<SimpleTree<T>*> &thi,
                const std::vector<SimpleTree<T>*> &that);

template<typename T>
class SimpleTree {
public:
    SimpleTree(const T s)
        : root(s) { };

    ~SimpleTree(){
        for (auto &i : forest)
            delete i;
    };

    SimpleTree(const T s, std::vector<SimpleTree<T>*> own)
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
    std::vector<SimpleTree<T>*> forest;
};


SimpleTree<std::map<std::string, std::string>> *empty_simple_tree();

// has to be inline because c++
template<typename T>
bool SimpleTree<T>::operator==(const SimpleTree<T> &that) const {
    return (root == that.root &&
            forest == that.forest);
}

template<typename T>
bool operator==(const std::vector<SimpleTree<T>*> &thi,
                const std::vector<SimpleTree<T>*> &that){
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
    std::cout << "Root:\n";

    for (auto &i : tree->root){
        std::cout << i.first
             << " = "
             << i.second
             << "\n";
    }

    std::cout << "Forest: \n";
    for (auto &i : tree->forest)
        traverse_st(i, depth++);

    return;
}


#endif /* IVL_SIMPLE_TREE */
