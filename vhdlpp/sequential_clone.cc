// FM. MA

#include <iostream>
#include <cstdio>
#include <typeinfo>
#include <limits>
#include <ivl_assert.h>

#include "sequential.h"
#include "expression.h"
#include "architec.h"
#include "package.h"
#include "compiler.h"
#include "subprogram.h"
#include "std_types.h"

IfSequential::Elsif *IfSequential::Elsif::clone() const {
    list<SequentialStmt*> *copy_elsif = new list<SequentialStmt*>();

    for (auto &i : if_)
        copy_elsif->push_back(i->clone());

    return new IfSequential::Elsif(cond_->clone(), copy_elsif);
}

/*
ForLoopStatement *ForLoopStatement::clone() const {
    list<SequentialStmt *> *copy = new list<SequentialStmt *>();

    for (auto &i : stmts_)
        copy->push_back(i->clone());

    return new ForLoopStatement(
        name_,
        it_,
        static_cast<ExpRange*>(range_->clone()),
        copy);
}

*/
