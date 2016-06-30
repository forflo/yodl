#ifndef IVL_SIMPLE_TREE
#define IVL_SIMPLE_TREE

# include <vector>
# include <map>
# include <string>
# include <iostream>

using namespace std;

template<typename T>
class SimpleTree {
private:
    T root;
    vector<SimpleTree<T>*> forest;

public:
    SimpleTree(const T s)
        : root(s) { };

    SimpleTree(const T s, vector<SimpleTree<T>*> own)
        : root(s)
        , forest(own) { };

    SimpleTree(const T s, SimpleTree<T> *own)
        : root(s) {
        forest.push_back(own);
    };

    bool operator==(const vector<SimpleTree<T>*> &this,
                    const vector<SimpleTree<T>*> &that);

    bool operator==(const SimpleTree &that){
        return (this.root == that.root &&
                this.forest == that.forest);
    }

    ~SimpleTree(){
        for (auto &i : forest)
            delete i;
    };
};

bool operator==(const vector<SimpleTree<T>*> &this,
                const vector<SimpleTree<T>*> &that){
    bool result(true);

    if (this.size() != this.size())
        return false;

    for (auto thisBegin = this.begin(), thisEnd = this.end(),
             thatBegin = that.begin(), thatEnd = that.end();
         thisBegin != thisEnd && thatBegin != thatEnd;
         ++thisBegin, ++thatBegin) {

        result = result && (*thisBegin == *thatBegin);
    }
}

void traverse_st(SimpleTree<map<string, string>> *tree, int depth = 0);

SimpleTree<map<string, string>> *empty_simple_tree();

#endif /* IVL_SIMPLE_TREE */
