#include <vector>
#include <map>
#include <string>
#include <iostream>

#include "simple_tree.h"

using namespace std;

SimpleTree<map<string, string>> *empty_simple_tree(){
    return new SimpleTree<map<string, string>>(
        map<string, string>{{"node-type", "empty node"}});
}

template<typename T>
void SimpleTree<T>::traverse_st(SimpleTree<T> *tree,
                                int depth){
    cout << "Foot:\n";

    for (auto &i : tree->root){
        cout << i->first
             << " = "
             << i->second
             << "\n";
    }

    cout << "Forest: \n";
    for (auto &i : tree->forest)
        traverse_st(i, depth++);

    return;
}

template<typename T>
bool SimpleTree<T>::operator==(const SimpleTree<T> &that){
    return (this->root == that.root &&
            this->forest == that.forest);
}

template<typename T>
bool operator==(const vector<SimpleTree<T>*> &thi,
                const vector<SimpleTree<T>*> &that){
    bool result(true);

    if (thi.size() != thi.size())
        return false;

    for (auto thiBegin = thi.begin(), thiEnd = thi.end(),
             thatBegin = that.begin(), thatEnd = that.end();
         thiBegin != thiEnd && thatBegin != thatEnd;
         ++thiBegin, ++thatBegin) {

        result = result && (*thiBegin == *thatBegin);
    }
}
