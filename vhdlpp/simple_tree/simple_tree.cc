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

