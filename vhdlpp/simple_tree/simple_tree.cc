#include "simple_tree.h"

# include <vector>
# include <map>
# include <string>
# include <iostream>

using namespace std;

SimpleTree<map<string, string>> *empty_simple_tree(){
    return new SimpleTree<map<string, string>>(
        map<string, string>{{"node-type", "empty node"}});
}


void traverse_st(SimpleTree<map<string, string>> *tree, int depth){
    cout << "root:\n";

    for (map<string, string>::iterator i = tree->root.begin();
            i!= tree->root.end();
            ++i){
        cout << i->first << " = " << i->second << "\n";
    }

    cout << "forest: \n";
    for (vector<SimpleTree<map<string, string>> *>::iterator i = tree->forest.begin();
            i != tree->forest.end();
            ++i){
        traverse_st(*i, depth++);
    }

    return;
}
