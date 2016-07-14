// FM. MA

#include <list>
#include <map>
#include <sstream>

#include "StringHeap.h"
#include "LineInfo.h"
#include "simple_tree/simple_tree.h"
#include "sequential.h"
#include "scope.h"
#include "architec.h"

using namespace std;

SimpleTree<map<string, string>> *ForGenerate::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ForGenerate"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"generator variable", genvar_.str()},
            {"name", name_.str()}});

    for (auto &i : statements_)
        result->forest.push_back(i->emit_strinfo_tree());

    result->forest.push_back(lsb_->emit_strinfo_tree());
    result->forest.push_back(msb_->emit_strinfo_tree());

    if (range_)
        result->forest.push_back(range_->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *Architecture::emit_strinfo_tree() const{
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "Architecture"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"name", name_.str()}});

    for (auto &i : statements_)
        result->forest.push_back(i->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *IfGenerate::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "IfGenerate"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"name", name_.str()}});

    for (auto &i : statements_)
        result->forest.push_back(i->emit_strinfo_tree());

    result->forest.push_back(cond_->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *CondSignalAssignment::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"node-type", "CondSignalAssignment"}});

    if(lval_)
        result->forest.push_back(lval_->emit_strinfo_tree());

    for (auto &i : options_)
        result->forest.push_back(i->emit_strinfo_tree());

    for (auto &i : sens_list_)
        result->forest.push_back(i->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *SignalAssignment::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{{"node-type", "SignalAssignment"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()}});

    result->forest.push_back(lval_->emit_strinfo_tree());

    for (auto &i : rval_)
        result->forest.push_back(i->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *ComponentInstantiation::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ComponentInstanciation"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"cname", cname_.str()},
            {"label", iname_.str()}});

    return result;
}

SimpleTree<map<string, string>> *StatementList::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{{"node-type", "StatementList"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()}});

    for (auto &i : statements_)
        result->forest.push_back(i->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *InitialStatement::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{{"node-type", "InitialStatement"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()}});

    return result;
}

SimpleTree<map<string, string>> *FinalStatement::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "FinalStatement"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()}});

    return result;
}

SimpleTree<map<string, string>> *ProcessStatement::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ProcessStatement"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"label", iname_.str()}});

    // From base StatementList
    for (auto &i : sensitivity_list_)
        result->forest.push_back(i->emit_strinfo_tree());

    for (auto &i : statements_)
        result->forest.push_back(i->emit_strinfo_tree());


    // From base Scope
    for (auto &i : old_signals_)
        result->forest.push_back(i.second->emit_strinfo_tree());

    for (auto &i : new_signals_)
        result->forest.push_back(i.second->emit_strinfo_tree());
    for (auto &i : old_variables_)
        result->forest.push_back(i.second->emit_strinfo_tree());
    for (auto &i : new_variables_)
        result->forest.push_back(i.second->emit_strinfo_tree());
    for (auto &i : old_components_)
        result->forest.push_back(i.second->emit_strinfo_tree());
    for (auto &i : new_components_)
        result->forest.push_back(i.second->emit_strinfo_tree());
    for (auto &i : use_types_)
        result->forest.push_back(i.second->emit_strinfo_tree());
    for (auto &i : cur_types_)
        result->forest.push_back(i.second->emit_strinfo_tree());

    // TODO: implement
//  for (auto &i : use_constans_)
//      result->forest.push_back(i.second->emit_strinfo_tree());
//  for (auto &i : cur_constans_)
//      result->forest.push_back(i.second->emit_strinfo_tree());

    // TODO: implement
//    for (auto &i : use_subprograms_)
//        result->forest.push_back(i.second->emit_strinfo_tree());
//    for (auto &i : cur_subprograms_)
//        result->forest.push_back(i.second->emit_strinfo_tree());
    for (auto &i : use_enums_)
        result->forest.push_back(i->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *BlockStatement::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "BlockStatement"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"label", label_.str()}});

    if (header_)
        result->forest.push_back(header_->emit_strinfo_tree());

    if (concurrent_stmts_)
        for (auto &i : *concurrent_stmts_)
            result->forest.push_back(i->emit_strinfo_tree());

    // from base scope
    for (auto &i : new_signals_)
        result->forest.push_back(i.second->emit_strinfo_tree());
    for (auto &i : new_variables_)
        result->forest.push_back(i.second->emit_strinfo_tree());
    for (auto &i : new_components_)
        result->forest.push_back(i.second->emit_strinfo_tree());
    for (auto &i : cur_types_)
        result->forest.push_back(i.second->emit_strinfo_tree());
    for (auto &i : cur_constants_)
        result->forest.push_back(i.second->emit_strinfo_tree());

    return result;
};
