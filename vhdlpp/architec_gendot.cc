// FM. MA

#include <list>
#include <map>

#include "../libmisc/StringHeap.h"
#include "../libmisc/LineInfo.h"
#include "simple_tree/simple_tree.h"
#include "sequential.h"
#include "scope.h"
#include "architec.h"

simple_tree<map<string, string>> *ForGenerate::emit_strinfo_tree(void) const {
    auto result = new simple_tree<map<string, string>>(
        map<string, string>{
            {"node-type", "ForGenerate"},
            {"generator variable", genvar_.str()},
            {"name", name_.str()}});

    for (auto &i : statements_)
        result->forest.push_back(i->emit_strinfo_tree());

    result->forest.push_back(lsb_->emit_strinfo_tree());
    result->forest.push_back(msb_->emit_strinfo_tree());

    return result;
}

simple_tree<map<string, string>> *Architecture::emit_strinfo_tree(void) const{
    auto result = new simple_tree<map<string, string>>(
        map<string, string>{
            {"node-type", "Architecture"},
            {"name", name_.str()}});

    for (auto &i : statements_)
        result->forest.push_back(i->emit_strinfo_tree());

    return result;
}

simple_tree<map<string, string>> *IfGenerate::emit_strinfo_tree(void) const {
    auto result = new simple_tree<map<string, string>>(
        map<string, string>{
            {"node-type", "IfGenerate"},
            {"name", name_.str()}});

    for (auto &i : statements_)
        result->forest.push_back(i->emit_strinfo_tree());

    result->forest.push_back(cond_->emit_strinfo_tree());

    return result;
}

simple_tree<map<string, string>> *CondSignalAssignment::emit_strinfo_tree(void) const {
    auto result = new simple_tree<map<string, string>>(
        map<string, string>{
            {"node-type", "CondSignalAssignment"}});

    result->forest.push_back(lval_->emit_strinfo_tree());

    for (auto &i : options_)
        result->forest.push_back(i->emit_strinfo_tree());

    for (auto &i : sens_list_)
        result->forest.push_back(i->emit_strinfo_tree());

    return result;
}

simple_tree<map<string, string>> *SignalAssignment::emit_strinfo_tree(void) const {
    auto result = new simple_tree<map<string, string>>(
        map<string, string>{{"node-type", "SignalAssignment"}});

    result->forest.push_back(lval_->emit_strinfo_tree());

    for (auto &i : rval_)
        result->forest.push_back(i->emit_strinfo_tree());

    return result;
}

simple_tree<map<string, string>> *ComponentInstantiation::emit_strinfo_tree(void) const {
    auto result = new simple_tree<map<string, string>>(
        map<string, string>{
            {"node-type", "ComponentInstanciation"},
            {"cname", cname_.str()},
            {"label", iname_.str()}});

    return result;
}

simple_tree<map<string, string>> *StatementList::emit_strinfo_tree(void) const {
    auto result = new simple_tree<map<string, string>>(
        map<string, string>{{"node-type", "StatementList"}});

    for (auto &i : statements_)
        result->forest.push_back(i->emit_strinfo_tree());

    return result;
}

simple_tree<map<string, string>> *InitialStatement::emit_strinfo_tree(void) const {
    auto result = new simple_tree<map<string, string>>(
        map<string, string>{{"node-type", "InitialStatement"}});

    return result;
}

simple_tree<map<string, string>> *FinalStatement::emit_strinfo_tree(void) const {
    auto result = new simple_tree<map<string, string>>(
        map<string, string>{{"node-type", "FinalStatement"}});

    return result;
}

simple_tree<map<string, string>> *ProcessStatement::emit_strinfo_tree(void) const {
    auto result = new simple_tree<map<string, string>>(
        map<string, string>{
            {"node-type", "ProcessStatement"},
            {"label", iname_.str()}});

    for (auto &i : sensitivity_list_)
        result->forest.push_back(i->emit_strinfo_tree());

    return result;
}
