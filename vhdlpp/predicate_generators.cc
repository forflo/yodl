// FM. MA
#include <functional>
#include "root_class.h"
#include "predicate_generators.h"

// Disjunction of two predicates.
std::function<bool (const AstNode* )> operator||(
    const std::function<bool (const AstNode *)> &lhs,
    const std::function<bool (const AstNode *)> &rhs){

    return [lhs, rhs](const AstNode *n) -> bool {
        return (lhs(n) || rhs(n));
    };
}

// Conjugation of two predicates.
std::function<bool (const AstNode* )> operator&&(
    const std::function<bool (const AstNode *)> &lhs,
    const std::function<bool (const AstNode *)> &rhs){

    return [lhs, rhs](const AstNode *n) -> bool {
        return (lhs(n) && rhs(n));
    };
}

// we cannot use && or || or ! operators as they require
// the return type bool
// we cannot use && or || or ! operators as they require
// the return type bool
// Negation
std::function<bool (const AstNode* )> operator!(
    const std::function<bool (const AstNode *)> &rhs){

    return [rhs](const AstNode *n) -> bool {
        return (!rhs(n));
    };
}

// XOR
std::function<bool (const AstNode* )> operator^(
    const std::function<bool (const AstNode *)> &lhs,
    const std::function<bool (const AstNode *)> &rhs){

    return [lhs, rhs](const AstNode *n) -> bool {
        return (rhs(n) != lhs(n));
    };
}
