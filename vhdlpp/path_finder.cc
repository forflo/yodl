// FM. MA
#include <vector>
#include <map>

////
// code base specific includes
#include "vhdlpp_config.h"
#include "generate_graph.h"
#include "version_base.h"
#include "simple_tree.h"
#include "scope.h"
#include "StringHeap.h"
#include "compiler.h"
#include "sequential.h"
#include "library.h"
#include "std_funcs.h"
#include "std_types.h"
#include "architec.h"
#include "parse_api.h"
#include "generic_traverser.h"
#include "vtype.h"
#include "root_class.h"
#include "mach7_bindings.h"
#include "path_finder.h"

#pragma clang diagnostic ignored "-Wshadow"

int PathFinder::findPath(AstNode *startNode){
    if (!paths.empty()) { paths.clear(); }

    if (arity <= 0){ return 1; }

    if (arity == 1){
        paths = {{startNode}};
        return 0;
    }

    std::vector<AstNode *> accumulator(arity);
    accumulator[0] = startNode;

    return getNaryPaths(1, getListOfChilds(startNode), accumulator);
}

int PathFinder::findPath(const AstNode *){
    if (!paths.empty()) { paths.clear(); }

    return 0;
}

// return as const, so no copy has to be made
const std::vector<std::vector<AstNode *>> PathFinder::getPaths(){
    return paths;
}

std::ostream &operator<<(std::ostream &out, PathFinder &us){
    int pfad = 0, elem = 0;
    out << "Path finder with arity: [" << us.getArity() << "]" << endl;
    out << endl;

    for (auto &i : us.getPaths()){
        out << "Pfad [" << (pfad++) << "] = [";
        for (auto &j : i)
            out << "(" << (elem++) << ") " << j << " ";
        out << "]" << endl;
        elem = 0;
    }

    return out;
}


int PathFinder::getNaryPaths(size_t depth, const std::list<AstNode *> &childs,
                             std::vector<AstNode *> &accumulator){
    if (accumulator.size() != arity){
        return 1;
    }

    cout << "NaryPaths called!\n";
    cout << "Depth:" << depth << endl;
    cout << "Arity:" << arity << endl;

    if (depth < arity){
        for (auto &i : childs){
            cout << "child: " << i << endl;
            accumulator[depth] = i;
            getNaryPaths(depth + 1, getListOfChilds(i), accumulator);
        }
    } else {
        cout << "push";
        for (auto &i : accumulator){
            cout << i << " ";
        }
        cout <<endl;
        paths.push_back(accumulator);
    }

    return 0;
}

const std::list<AstNode *> PathFinder::getListOfChilds(AstNode *e){
    using namespace mch;

    var<VType *> type;
    var<Expression *> op1, op2;

    var<vector<ExpAggregate::element_t*>> elements;
    var<vector<ExpAggregate::choice_element>> aggregate;

    var<perm_string> attribName;
    var<list<Expression *> *> attribArgs;

    // for ExpBitstring
    var<vector<char>> bitString;

    // for ExpCharacter
    var<char> charValue;

    // For ExpConcat
    var<Expression*> concLeft, concRight;

    // For ExpConditional
    var<list<ExpConditional::case_t*>> condOptions;
    var<Expression*> selector;

    // For ExpFunc
    var<perm_string> funcName;
    var<SubprogramHeader *> definition;
    var<vector<Expression*>> argVector;

    // For ExpInteger
    var<int64_t> intValue;

    // For ExpReal
    var<double> dblValue;

    // For ExpName
    var<perm_string> nameName;
    var<list<Expression*>*> indices;

    //For ExpScopedName
    var<perm_string> scopeName;
    var<ScopeBase *> scope;
    var<ExpName*> scopeNameName;

    // For ExpCast
    var<Expression*> castExp;
    var<const VType *> castType;

    // For ExpRange
    var<Expression*> rangeLeft, rangeRight;
    var<ExpRange::range_dir_t> direction;
    var<bool> rangeExpr, rangeReverse;
    var<ExpName *> rangeBase;

    // For ExpNew
    var<Expression*> newSize;

    // For ExpTime
    var<uint64_t> timeAmount;
    var<ExpTime::timeunit_t> timeUnit;

    // For ExpString
    var<string> strValue;

    // For ExpDelay
    var<Expression *> delayExpr, delayDelay;

    std::list<AstNode *> result;

    if (e == NULL) { return result; }
    Match(e){
        Case(C<ExpUnary>(op1)){
            var<ExpEdge::fun_t> edgeSpec;

            Match(e){
                Case(C<ExpEdge>(edgeSpec)){
                    return result;
                }
                Case(C<ExpUAbs>()){
                    return result;
                }
                Case(C<ExpUNot>()){
                    return result;
                }
            } EndMatch;
        }

        Case(C<ExpBinary>(op1, op2)){

            var<ExpArithmetic::fun_t> arithOp;
            var<ExpLogical::fun_t> logOp;
            var<ExpRelation::fun_t> relOp;
            var<ExpShift::shift_t> shiftOp;

            Match(e){
                Case(C<ExpArithmetic>(arithOp)){
                    return {op1, op2};
                }
                Case(C<ExpLogical>(logOp)){
                    return {op1, op2};
                }
                Case(C<ExpRelation>(relOp)){
                    return {op1, op2};
                }
                Case(C<ExpShift>(shiftOp)){
                    return {op1, op2};
                }
                Otherwise() {
                    return result;
                }
            } EndMatch;
        }

        Case(C<ExpAggregate>(elements, aggregate)){
            for (auto &i : elements)
                result.push_front(i);

            //TODO: Fix the fact that the list contains
            //      values rather that pointers
            /*for (auto &i : aggregate)
              result.push_front(i);*/

            return result;
        }

        Case(C<ExpAttribute>(attribName, attribArgs)){

            var<ExpName *> attribBase;
            var<const VType *> attribTypeBase;

            for (auto &i : *static_cast<std::list<Expression*> *>(attribArgs))
                result.push_front(i);

            Match(e){
                Case(C<ExpObjAttribute>(attribBase)){
                    result.push_back(attribBase);
                    return result;
                }
                Case(C<ExpTypeAttribute>(attribTypeBase)){
                    //attribTypeBase is const so do nothing
                }
                Otherwise(){
                    return result;
                }
            } EndMatch;
        }

        Case(C<ExpBitstring>(bitString)){
            return result;
        }

        Case(C<ExpCharacter>(charValue)){
            return result;
        }

        Case(C<ExpConcat>(concLeft, concRight)){
            return {concLeft, concRight};
        }

        Case(C<ExpConditional>(condOptions)){
            for (auto &i : condOptions)
                result.push_back(i);

            Match(e){
                Case(C<ExpSelected>(selector)){
                    result.push_back(selector);
                    return result;
                }

                Otherwise(){
                    return result;
                }
            } EndMatch;

        }

        Case(C<ExpFunc>(funcName, definition, argVector)){
            for (auto &i : argVector)
                result.push_back(i);
            //TODO: result.push_back(definition);

            return result;
        }

        Case(C<ExpInteger>(intValue)){
            return result;
        }

        Case(C<ExpReal>(dblValue)){
            return result;
        }

        Case(C<ExpName>(nameName, indices)){
            for (auto &i : *static_cast<list<Expression*>*>(indices))
                result.push_back(i);

            Match(e){
                Case(C<ExpNameALL>()){
                    return result;
                }
                Otherwise(){
                    return result;
                }
            } EndMatch;
        }

        Case(C<ExpScopedName>(scopeName, scope, scopeNameName)){
            return {scope, scopeNameName};
        }

        Case(C<ExpString>(strValue)){
        }

        Case(C<ExpCast>(castExp, castType)){
        }

        Case(C<ExpNew>(newSize)){
            return {newSize};
        }

        Case(C<ExpTime>(timeAmount, timeUnit)){
        }

        Case(C<ExpRange>(rangeLeft, rangeRight,
                         /*direction,*/ rangeExpr,
                         rangeBase/*, rangeReverse*/)){
            return {rangeLeft, rangeRight, rangeBase};
        }

        Case(C<ExpDelay>(delayExpr, delayDelay)){
            return {delayExpr, delayDelay};
        }

        Otherwise(){
            return result;
        }
    } EndMatch;

    return result;
}
