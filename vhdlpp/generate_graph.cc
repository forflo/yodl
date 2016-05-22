// FM. MA
//
#include "generate_graph.h"
#include "simple_tree/simple_tree.h"

#include <sstream>
#include <cmath>
#include <string>
#include <map>
#include <iostream>
#include <vector>

using namespace std;

static const char * NODEID = "NODEID";
static const char * NODE_ATTRIBS = "node [shape=record, color=indigo];\n";
static const char * EDGE_ATTRIBS = "edge [arrowhead=vee, color=black];\n";

static int path_to_num(vector<int> &path){
    int result = 0;
    int exp = path.size() - 1;

    for (auto &i : path)
        result += i * pow(2, exp--);

    return result;
}

int emit_edges(ostream &out, 
        SimpleTree<map<string, string>> * ast){
    
    for (auto &i : ast->forest){
        out << ast->root[NODEID]
            << ":s"
            << " -> "
            << i->root[NODEID]
            << ":n;\n";

        emit_edges(out, i);
    }

    return 0;
}

int emit_vertices(ostream &out, 
        const SimpleTree<map<string, string>> * ast, 
        vector<int> &path, 
        int depth = 0){
    int pcount = 0;
    map<string, string> root_copy(ast->root);

    out << root_copy[NODEID] << " [label=\"{";
    
    out << "{" << root_copy["node-type"] << "}";
    root_copy.erase("node-type");
    root_copy.erase(NODEID);

    for (auto &i : root_copy)
        out << "|{" << i.first << "|" << i.second << "}";

    out << "}\"];\n";

    for (auto &i : ast->forest) {
        path.push_back(pcount++);
        emit_vertices(out, i, path, ++depth);
    }

    return 0;
}

int add_nodeids(SimpleTree<map<string, string>> *ast, 
        vector<int> &path,
        int depth = 0){
    int pcount = 0;

    stringstream buffer;
    buffer << "node_"
        << path_to_num(path)
        << "_"
        << depth;

    ast->root[NODEID] = buffer.str();

    for (auto &i : ast->forest){
        path.push_back(pcount++);
        add_nodeids(i, path, ++depth);
    }

    return 0;
}

int emit_dotgraph(ostream &out, 
        string name, 
        SimpleTree<map<string, string>> *ast){
    vector<int> path;
    add_nodeids(ast, path);

    out << "digraph " << name << "{\n";
    out << EDGE_ATTRIBS << NODE_ATTRIBS;
    
    emit_vertices(out, ast, path);
    out << '\n';
    emit_edges(out, ast);

    out << "}\n";

    return 0;
}
