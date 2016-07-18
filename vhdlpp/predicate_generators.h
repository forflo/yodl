// FM. MA
// This file contains some useful metafunctions for
// the quick-and-dirty usage of GenericTraverser
////
#ifndef IVL_PREDICATE_GENERATOR
#define IVL_PREDICATE_GENERATOR

#include "root_class.h"
#include "mach7_includes.h"
#include <functional>

////
//Nomenclature: Type predicates are c++-functors that
//return true, if they're called with a pointer that matches
//a certain type.

//Usage (1-ary):
//function<bool (const AstNode *)> tP =
//    makeTypePredicate<ExpInteger>();
//
//Usage (n-ary):
//function<bool (const AstNode *)> tnP =
//    makeNaryTypePredicate<ExpInteger, ExpArithmetic, SequentialStmt...>();

template<typename T> struct makeTypePredicate {
    bool operator()(const AstNode *n){
        Match(n){
            CaseInT(mch::C<T>()){ return true; }
        } EndMatch;

        return false;
    }
};

template<typename T, typename ...J> struct makeNaryTypePredicate;
template<typename T> struct makeNaryTypePredicate<T>;

// terminate template recursion through partial specialization
template<typename T> struct makeNaryTypePredicate<T>{
    bool operator()(const AstNode *n){
        Match(n){
            CaseInT(mch::C<T>()){ return true; }
        } EndMatch;
        return false;
    }
};

// simple recursive (and variadic) template that can be used to generate
// type predicates that check for multiple types.
// WARNING: This should not be used for huge type parameter lists as the
//          created type switching algorithm is slow (O(n), with n being the
//          number of type given to this template)
// TODO: Improve performance
template<typename T, typename ...J> struct makeNaryTypePredicate {
    bool operator()(const AstNode *n){
        if (helper(n)) {
            return true;
        } else {
            return makeNaryTypePredicate<J...>()(n);
            return false;
        }
    }
private:
    bool helper(const AstNode *n){
        Match(n){
            CaseInT(mch::C<T>()){
                return true;
            }
        } EndMatch;
        return false;
    }
};



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

#endif /* IVL_PREDICATE_GENERATOR */
