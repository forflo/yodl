// FM. MA
//
#include "generate_graph.h"
#include "simple_tree.h"
#include "enum_overloads.h"

#include <sstream>
#include <cmath>
#include <string>
#include <map>
#include <iostream>
#include <vector>

using namespace std;

string DotGraphGenerator::path_to_string(vector<int> &path){
    string result = "";
    for (auto &i : path)
        result += to_string(i);

    return result;
}

int DotGraphGenerator::emit_edges(ostream &out,
        SimpleTree<map<string, string>> * ast){

    for (auto &i : ast->forest){
        out << ast->root[NODEID]
            << arrowFrom
            << " -> "
            << i->root[NODEID]
            << arrowTo << ";\n";

        emit_edges(out, i);
    }

    return 0;
}

int DotGraphGenerator::emit_vertices(ostream &out,
        const SimpleTree<map<string, string>> * ast,
        int depth){
    map<string, string> root_copy(ast->root);

    out << root_copy[NODEID] << " [label=\"{";

    out << "{" << root_copy["node-type"] << "}";
    root_copy.erase("node-type");
    root_copy.erase(NODEID);

    for (auto &i : root_copy)
        out << "|{" << i.first << "|" << i.second << "}";

    out << "}\"];\n";

    for (auto &i : ast->forest) {
        emit_vertices(out, i, ++depth);
    }

    return 0;
}

int DotGraphGenerator::add_nodeids(SimpleTree<map<string, string>> *ast,
        vector<int> path,
        int depth){
    int pcount = 0;

    stringstream buffer;
    buffer << "node_"
        << path_to_string(path)
        << "_"
        << depth;

    ast->root[NODEID] = buffer.str();

    for (auto &i : ast->forest){
        vector<int> tpath(path);
        tpath.push_back(pcount++);
        add_nodeids(i, tpath, depth + 1);
    }

    return 0;
}

int DotGraphGenerator::emit_dotgraph(ostream &out,
        string name,
        SimpleTree<map<string, string>> *ast){
    vector<int> path;
    add_nodeids(ast, path);

    out << "digraph " << name << "{\n";
    out << GRAPH_ATTRIBS
        << "node [shape=" << nodeShape
        << ", color=" << nodeColor << "];" << endl
        << "edge [arrowhead=" << arrowShape
        << ", color=" << edgeColor << "];" << endl;

    emit_vertices(out, ast);
    out << '\n';
    emit_edges(out, ast);

    out << "}\n";

    return 0;
}

int DotGraphGenerator::operator()(ostream &out, string name,
                                  SimpleTree<map<string, string>> *ast){
    return emit_dotgraph(out, name, ast);
}
