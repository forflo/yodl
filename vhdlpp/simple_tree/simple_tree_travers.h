
#ifndef IVL_SIMPLE_TREE_TRAV
#define IVL_SIMPLE_TREE_TRAV

# include <map>
# include <vector>
# include <string>
# include <iostream>

using namespace std;

void traverse_st(simple_tree<map<string, string>> *tree, int depth){
    cout << "traverse depth: " << depth << '\n';
    cout << "root:\n";

    for (map<string, string>::iterator i = tree->root.begin();
            i!= tree->root.end();
            ++i){
        cout << "first: " << i->first << " second: " << i->second << "\n";
    }

    cout << "forest: \n";
    for (vector<simple_tree<map<string, string>> *>::iterator i = tree->forest.begin();
            i != tree->forest.end();
            ++i){
        traverse_st(*i, depth++); 
    }

    return;
}

#endif /* IVL_SIMPLE_TREE_TRAV */
