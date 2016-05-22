// FM. MA

#include <map>
#include <iostream>
#include <string>
#include <vector>

#include "simple_tree.h"
#include "vsignal.h"
#include "enum_overloads.h"
#include "StringHeap.h"

SimpleTree<map<string, string>> *Signal::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "Signal"},
            {"signal name", name_.str()}});

    result->forest.push_back(type_->emit_strinfo_tree());

    if (init_expr_)
        result->forest.push_back(init_expr_->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *Variable::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "Variable"},
            {"variable name", name_.str()}});

    result->forest.push_back(type_->emit_strinfo_tree());

    if (init_expr_)
        result->forest.push_back(init_expr_->emit_strinfo_tree());

    return result;
}
