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

static SimpleTree<map<string, string>> *clone(
    const SimpleTree<map<string, string>> *ast);

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
            << (arrowFrom != compassPoint::NIL ? ":" : "")
            << arrowFrom
            << " -> "
            << i->root[NODEID]
            << (arrowTo != compassPoint::NIL ? ":" : "")
            << arrowTo << ";\n";

        emit_edges(out, i);
    }

    return 0;
}

int DotGraphGenerator::emit_vertices(ostream &out,
                                     SimpleTree<map<string, string>> * ast,
                                     int depth){
    map<string, string> root_copy(ast->root);

    out << root_copy[NODEID] << " [label=\"{";

    out << "{" << root_copy["node-type"] << "}";
    root_copy.erase("node-type");
    root_copy.erase(NODEID);

    for (auto &i : root_copy){
        if (!keyBlacklist.empty()){
            for (auto &j : keyBlacklist){
                if (j != i.first){
                    out << "|{" << i.first << "|" << i.second << "}";
                }
            }
        } else {
            out << "|{" << i.first << "|" << i.second << "}";
        }
    }

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

int DotGraphGenerator::emit_ascii_tree(ostream &out,
                                       const SimpleTree<map<string, string>> *ast,
                                       const int indent = 0){

    for (int i = 0; i < indent * indentMult; i++){
        out << " ";
    }

    auto i1 = ast->root.find("node-type");
    auto i2 = ast->root.find("node-pointer");

    out << "[" << (i1 != ast->root.end() ? i1->second : "")
        << "@" << (i2 != ast->root.end() ? i2->second : "")
        << " | ";

    for (auto &i : ast->root){
        if (i.first == "node-pointer"){
            continue;
        }
        if (!keyBlacklist.empty()){
            for (auto &j : keyBlacklist){
                if (j != i.first){
                    out << i.first << "=" << i.second << " "
                        << (i.first == (std::prev(ast->root.end(), 1)->first)
                            ? ""
                            : ", ");
                }
            }
        } else {
            out << i.first << "=" << i.second
                << (i.first == (std::prev(ast->root.end(), 1)->first)
                    ? ""
                    : ", ");
        }

    }
    out << "]" << endl;

    for (auto &i : ast->forest){
        emit_ascii_tree(out, i, indent + 1);
    }

    return 0;
}

//TODO: Try to make this more general by using Concepts TS
//      and logic inside a copy constructor of SimpleTree...
static SimpleTree<map<string, string>> *clone(
    const SimpleTree<map<string, string>> *tree){

    auto rootCopy = map<string, string>();
    auto forestCopy = vector<SimpleTree<map<string, string>> *>();

    for (auto &i : tree->root)
        rootCopy.insert(std::pair<string, string>(i.first, i.second));

    for (auto &i : tree->forest)
        forestCopy.push_back(clone(i));

    return new SimpleTree<map<string, string>>(rootCopy, forestCopy);
}

DotGraphGenerator::DotGraphGenerator(){}

int DotGraphGenerator::operator()(ostream &out, string name,
                                  const SimpleTree<map<string, string>> *ast){
    auto treeCopy = clone(ast);
    emit_dotgraph(out, name, treeCopy);
    delete treeCopy;
    return 0;
}

int DotGraphGenerator::operator()(
    std::string name,
    const SimpleTree<std::map<std::string, std::string>> *ast){

    auto treeCopy = clone(ast);
    emit_dotgraph(std::cout, name, treeCopy);
    delete treeCopy;
    return 0;
}

int DotGraphGenerator::operator()(
    std::ostream & out ,
    const SimpleTree<std::map<std::string, std::string>> *ast){

    return emit_ascii_tree(out, ast);
}

int DotGraphGenerator::operator()(
    const SimpleTree<std::map<std::string, std::string>> *ast){

    return emit_ascii_tree(std::cout, ast);
}

// TODO: make that work
//int operator()(std::ostream &out, std::string name, AstNode *ast){
//    return emit_dotgraph(out, name, ast);
//}
