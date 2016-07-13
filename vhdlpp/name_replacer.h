#ifndef IVL_NAME_REPLACER
#define IVL_NAME_REPLACER

#include <vector>
#include <iostream>

#include "root_class.h"
#include "mach7_includes.h"
#include "expression.h"
#include "architec.h"

class NameReplacer {
public:
    NameReplacer(const Expression &e, const ExpName &n)
        : replacement(e)
        , name(n) {}

    ~NameReplacer() {

    }

    int operator()(AstNode *n, const std::vector<AstNode *> &parents);

private:
    const Expression &replacement;
    const ExpName &name;
};

#endif /* IVL_NAME_REPLACER */
