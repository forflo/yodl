// FM. MA
////

#include <fstream>
#include <iomanip>
#include <typeinfo>
#include <set>

#include "sequential.h"
#include "expression.h"

using namespace std;

SimpleTree<map<string, string>> *WaitStmt::emit_strinfo_tree(void) const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "WaitStmt"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"wait-type", "TODO"}});

    if (expr_)
        result->forest = { expr_->emit_strinfo_tree()};

    for (auto &i : sens_list_)
        result->forest.push_back(i->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *WaitForStmt::emit_strinfo_tree(void) const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"node-type", "WaitForStmt"}});

    result->forest = { delay_->emit_strinfo_tree()};

    return result;
}

SimpleTree<map<string, string>> *AssertStmt::emit_strinfo_tree(void) const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "AssertStmt"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"default-msg", string(default_msg_)}});

    result->forest = { cond_->emit_strinfo_tree()};

    return result;
}

SimpleTree<map<string, string>> *ReportStmt::emit_strinfo_tree(void) const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ReportStmt"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"severity", "TODO"}});

    result->forest = { msg_->emit_strinfo_tree()};

    return result;
}

SimpleTree<map<string, string>> *BasicLoopStatement::emit_strinfo_tree(void) const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "BasicLoopStatement"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"label", (name_.str() ? name_.str() : "")}});

    for (auto &i : stmts_)
        result->forest.push_back(i->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *ForLoopStatement::emit_strinfo_tree(void) const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ForLoopStatement"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"label", (name_.str() ? name_.str() : "") },
            {"loop-iterator", (it_.str() ? it_.str() : "")}});

    result->forest = { range_->emit_strinfo_tree() };

    for (auto &i : stmts_)
        result->forest.push_back(i->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *WhileLoopStatement::emit_strinfo_tree(void) const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "WhileLoopStatement"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"label", (name_.str() ? name_.str() : "")}});

    result->forest = { cond_->emit_strinfo_tree() };

    return result;
}

SimpleTree<map<string, string>> *VariableSeqAssignment::emit_strinfo_tree(void) const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"node-type", "VariableSeqAssignment"}});

    result->forest = {
        lval_->emit_strinfo_tree(),
        rval_->emit_strinfo_tree()};

    return result;
}

SimpleTree<map<string, string>> *ProcedureCall::emit_strinfo_tree(void) const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ProcedureCall"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"label", name_.str()}});

    for (auto &i : *param_list_)
        result->forest.push_back(i->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *CaseSeqStmt::emit_strinfo_tree(void) const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"node-type", "CaseSeqStmt"}});

    result->forest = { cond_->emit_strinfo_tree() };

    for (auto &i : alt_)
        result->forest.push_back(i->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *CaseSeqStmt::CaseStmtAlternative::emit_strinfo_tree(void) const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"node-type", "CaseStmtAlternative"}});

    for (auto &e : *exp_)
        result->forest.push_back(e->emit_strinfo_tree());

    for (auto &s : stmts_)
        result->forest.push_back(s->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *SignalSeqAssignment::emit_strinfo_tree(void) const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"node-type", "SignalSeqAssignment"}});


    result->forest = { lval_->emit_strinfo_tree() };

    for (auto &i : waveform_)
        result->forest.push_back(i->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *ReturnStmt::emit_strinfo_tree(void) const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"node-type", "ReturnStmt"}});


    result->forest = { val_->emit_strinfo_tree() };

    return result;
}

SimpleTree<map<string, string>> *IfSequential::emit_strinfo_tree(void) const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"node-type", "IfSequential"}});

    result->forest = { cond_->emit_strinfo_tree() };

    for (auto &i : if_)
        result->forest.push_back(i->emit_strinfo_tree());

    for (auto &i : elsif_)
        result->forest.push_back(i->emit_strinfo_tree());

    for (auto &i : else_)
        result->forest.push_back(i->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *IfSequential::Elsif::emit_strinfo_tree(void) const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"node-type", "Elsif"}});

    result->forest = { cond_->emit_strinfo_tree() };

    for (auto &i : if_)
        result->forest.push_back(i->emit_strinfo_tree());

    return result;
}
