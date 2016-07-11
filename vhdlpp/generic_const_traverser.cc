//FM. MA
//
// This class contains boilerplate code for
// generic AST traversal. A lot of it is redundant, but
// because of the very nature of c++ and it's lacking
// introspection facilities, this can't be auto generated
// easily (i'm not saying that it can't be done).
// I made heavy use of s///g command and other test
// manipultion methods of my editor
////
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <cerrno>
#include <limits>
#include <vector>
#include <map>
#include <inttypes.h>
#include <iostream>
#include <typeinfo>
#include <math.h>

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

#pragma clang diagnostic ignored "-Wshadow"

#if defined(HAVE_GETOPT_H)
# include <getopt.h>
#endif
// MinGW only supports mkdir() with a path. If this stops working because
// we need to use _mkdir() for mingw-w32 and mkdir() for mingw-w64 look
// at using the autoconf AX_FUNC_MKDIR macro to figure this all out.
#if defined(__MINGW32__)
# include <io.h>
# define mkdir(path, mode)    mkdir(path)
#endif

using namespace mch;
using namespace std;

//TODO:
// ExpAggregate::choice_t -- inherit AstNode done
// ExpAggregate::choice_element -- inherit AstNode done
// ExpAggregate::element_t -- inherit AstNode done
// ExpConditional::case_t -- inherit AstNode done
// ExpName::index_t -- inherit AstNode done

// IfSequential::Elsif -- inherit AstNode done
// CaseSeqStmt::CaseStmtAlternative -- inherit AstNode done
// BlockStatement::BlockHeader -- inherit AstNode

// VTypeArray::range_t -- inherit AstNode
// VTypeRecord::element_t -- inherit AstNode

// named_expr_t -- inherit AstNode

void GenericTraverser::traverseConst(const AstNode *n){
    traversalMessages.push_back("Entering AstNode switch");

    Match(n){
        Case(C<Entity>()){
            traverseConst(static_cast<const Entity*>(n));
        }
        Case(C<Architecture>()) {
            traverseConst(static_cast<const Architecture*>(n));
        }
        Case(C<VType>()){
            traverseConst(static_cast<const VType*>(n));
        }
        Case(C<SequentialStmt>()){
            traverseConst(static_cast<const SequentialStmt*>(n));
        }
        Case(C<Architecture::Statement>()){
            traverseConst(static_cast<const Architecture::Statement*>(n));
        }
        Case(C<Expression>()){
            traverseConst(static_cast<const Expression*>(n));
        }
        Case(C<SigVarBase>()){
            traverseConst(static_cast<const SigVarBase*>(n));
        }
        Otherwise() {
            errorFlag = true;
            traversalErrors.push_back("Unknown type in node switch");
        }
    } EndMatch;
}

void GenericTraverser::traverseConst(const ComponentBase *n){
    traversalMessages.push_back("Entering Entity switch");

    // For Entity
    var<map<perm_string, Architecture *>> entityArchs;
    var<Architecture*> entityBound;
    var<map<perm_string, VType::decl_t>> entityDecls;

    // For ComponentBase
    var<perm_string> compName;
    var<vector<InterfacePort*>> compParams, compPorts;

    Match(n){
        Case(C<ComponentBase>(compName, compParams, compPorts)) {
            Match(n){
                Case(C<Entity>(entityArchs, entityBound, entityDecls)){
                    traversalMessages.push_back("Entity detected");

                    // run visitor
                    if(runVisitorConst(n) == false){ return; }

                    for (auto &i : entityArchs)
                        traverseConst(i.second);
                }
                Otherwise(){
                    traversalMessages.push_back("ComponentBase detected");

                    // run visitor
                    if(runVisitorConst(n) == false){ return; }
                }
            } EndMatch;
        }
        Otherwise() {
            errorFlag = true;
            traversalErrors.push_back("Impossible condition in traverse "
                                      "for Component Base");
        }
    } EndMatch;
}

void GenericTraverser::traverseConst(const Architecture *n){
    currentPathConst.insert(currentPathConst.begin(), n);
    traversalMessages.push_back("Entering Architecture switch");

    var<list<Architecture::Statement *>> statements;
    var<ComponentInstantiation *> componentInst;
    var<perm_string> name;
    // propably not needed
    //var<ProcessStatement *> currenProcess;

    Match(n){
        Case(C<Architecture>(statements, componentInst, name)){
            traversalMessages.push_back(string("Architecture ")
                                        + name.str()
                                        + string(" detected"));

                    if(runVisitorConst(n) == false){ return; }

            for (auto &i : statements)
                traverseConst(i);

            if (componentInst)
                traverseConst(componentInst);
        }
        Otherwise(){
            errorFlag = true;
            traversalErrors.push_back("Impossible condition in "
                                      "traverse Architecture");
        }
    } EndMatch;
}

void GenericTraverser::traverseConst(const Architecture::Statement *n){
    currentPathConst.insert(currentPathConst.begin(), n);
    traversalMessages.push_back("Entering Statement switch");

    var<perm_string> name, label;
    var<list<Architecture::Statement*>> stmts;
    var<list<Architecture::Statement*>*> stmts_ptr;
    var<BlockStatement::BlockHeader*> header;
    var<Expression&> cond;

    // for SignalAsignment and CondSignalAssignment
    var<ExpName*> lval;
    var<list<Expression*>> rval;
    var<list<ExpConditional::case_t*>> options;
    var<list<const ExpName*>> senslist;

    // for ComponentInstanciation
    var<perm_string> iname, cname;
    var<map<perm_string, Expression*>> genmap, portmap;

    // for StatementList subtree
    var<list<SequentialStmt*>> seqStmts;

    if (n == NULL) {
        errorFlag = true;
        traversalErrors.push_back("Statement traverser called with Nullptr!");
        return;
    }

    Match(n){
        Case(C<GenerateStatement>(name, stmts)){
            traversalMessages.push_back("GenerateStatement detected");
            var<Expression *> cond, msb, lsb;
            var<perm_string> genvar;

            Match(n){
                Case(C<ForGenerate>(genvar, msb, lsb)){
                    traversalMessages.push_back("ForGenerate detected");

                    // run visitor
                    if(runVisitorConst(n) == false){ return; }

                    // descent
                    traverseConst(msb);
                    traverseConst(lsb);

                    for (auto &i : stmts)
                        traverseConst(i);
                }
                Case(C<IfGenerate>(cond)){
                    traversalMessages.push_back("IfGenerate detected");

                    // run visitor
                    if(runVisitorConst(n) == false){ return; }

                    traverseConst(cond);

                    for (auto &i : stmts)
                        traverseConst(i);
                }
                Otherwise(){
                    traversalMessages.push_back(
                        "GenerateStatement detected [error]");

                    // run visitor
                    if(runVisitorConst(n) == false){ return; }

                    // descent
                    for (auto &i : stmts)
                        traverseConst(i);
                }
            } EndMatch;

            return;
        }

        Case(C<SignalAssignment>(lval, rval)){
            traversalMessages.push_back("SignalAssignment detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // descent
            traverseConst(lval);

            for (auto &i : rval)
                traverseConst(i);
        }

        Case(C<CondSignalAssignment>(lval, options, senslist)){
            traversalMessages.push_back("CondSignalAssignment detected");


            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // descent
                traverseConst(lval);

                for (auto &i : senslist)
                    traverseConst(i);

            for (auto &i : options)
                traverseConst(i);
        }

        Case(C<ComponentInstantiation>(iname, cname, genmap, portmap)){
            traversalMessages.push_back("ComponentInstantiation detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // descent
                for (auto &i : genmap)
                    traverseConst(i.second);

                for (auto &i : portmap)
                    traverseConst(i.second);
        }

        Case(C<StatementList>(seqStmts)){
            traversalMessages.push_back("StatementList detected");

            var<perm_string> name;
            var<list<Expression *>> procSensList;

            Match(n){
                Case(C<FinalStatement>()) {
                    traversalMessages.push_back("Found final Statement");
                    // run visitor
                    if(runVisitorConst(n) == false){ return; }

                    // descent
                    for (auto &i : seqStmts)
                        traverseConst(i);
                }
                Case(C<InitialStatement>()) {
                    traversalMessages.push_back("Found initial Statement");
                    // run visitor
                    if(runVisitorConst(n) == false){ return; }

                    // descent
                    for (auto &i : seqStmts)
                        traverseConst(i);
                }
                Case(C<ProcessStatement>(name, procSensList)) {
                    traversalMessages.push_back("Found process Statement");

                    // run visitor
                    if(runVisitorConst(n) == false){ return; }

                    // descent
                        for (auto &i : procSensList)
                            traverseConst(i);

                    for (auto &i : seqStmts)
                        traverseConst(i);
                }
                Otherwise() {
                    errorFlag = true;
                    traversalErrors.push_back("Error in Statementlist switch: "
                                              "Raw StatementList");

                    return ;
                }
            } EndMatch;
        }

        Case(C<BlockStatement>(label, header, stmts_ptr)){
            traversalMessages.push_back("BlockStatement detected");

                    // run visitor
                        if(runVisitorConst(n) == false){ return; }

            // descent
            for (auto &i : *static_cast<list<Architecture::Statement*>*>(
                     stmts_ptr)){
                traverseConst(i);
            }

            //TODO: descent into header too
        }

        Otherwise() {
            errorFlag = true;
            traversalErrors.push_back("Raw ArchitectureStatement detected");
            //cout << "Wildcard pattern for traverseConst(const Arch::Stmt &s)\n";
            return;
        }
    } EndMatch;
}

void GenericTraverser::traverseConst(const Expression *n){
    currentPathConst.insert(currentPathConst.begin(), n);

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

    if (n == NULL) { return; }
    Match(n){
        Case(C<ExpUnary>(op1)){
            traversalMessages.push_back("ExpUnary detected");
            var<ExpEdge::fun_t> edgeSpec;

            Match(n){
                Case(C<ExpEdge>(edgeSpec)){
                    traversalMessages.push_back("ExpEdge detected");

                    // run visitor
                    if(runVisitorConst(n) == false){ return; }

                    // descent
                    traverseConst(op1);
                }
                Case(C<ExpUAbs>()){
                    traversalMessages.push_back("ExpUAbs detected");

                    // run visitor
                    if(runVisitorConst(n) == false){ return; }

                    // descent
                    traverseConst(op1);
                }
                Case(C<ExpUNot>()){
                    traversalMessages.push_back("ExpUNot detected");

                    // run visitor
                    if(runVisitorConst(n) == false){ return; }

                    // descent
                    traverseConst(op1);
                }
            } EndMatch;
        }

        Case(C<ExpBinary>(op1, op2)){
            traversalMessages.push_back("ExpBinary detected");
            var<ExpArithmetic::fun_t> arithOp;
            var<ExpLogical::fun_t> logOp;
            var<ExpRelation::fun_t> relOp;
            var<ExpShift::shift_t> shiftOp;

            Match(n){
                Case(C<ExpArithmetic>(arithOp)){
                    traversalMessages.push_back("ExpArithmetic detected");

                    // run visitor
                    if(runVisitorConst(n) == false){ return; }

                    // descent
                    traverseConst(op1);
                    traverseConst(op2);
                }
                Case(C<ExpLogical>(logOp)){
                    traversalMessages.push_back("ExpLogical detected");

                    // run visitor
                    if(runVisitorConst(n) == false){ return; }

                    // descent
                    traverseConst(op1);
                    traverseConst(op2);
                }
                Case(C<ExpRelation>(relOp)){
                    traversalMessages.push_back("ExpRelation detected");

                    // run visitor
                    if(runVisitorConst(n) == false){ return; }

                    // descent
                    traverseConst(op1);
                    traverseConst(op2);
                }
                Case(C<ExpShift>(shiftOp)){
                    traversalMessages.push_back("ExpShift detected");

                    // run visitor
                    if(runVisitorConst(n) == false){ return; }

                    // descent
                    traverseConst(op1);
                    traverseConst(op2);
                }
                Otherwise() {
                    errorFlag = true;
                    traversalErrors.push_back("Raw ExpBinary detected");
                    /*error*/
                }
            } EndMatch;
        }

        Case(C<ExpAggregate>(elements, aggregate)){
            traversalMessages.push_back("ExpAggregate detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // descent
            // TODO: implement descents
        }

        Case(C<ExpAttribute>(attribName, attribArgs)){
            traversalMessages.push_back("ExpAttribute detected");
            var<ExpName *> attribBase;
            var<const VType *> attribTypeBase;

            Match(n){
                Case(C<ExpObjAttribute>(attribBase)){
                    traversalMessages.push_back("ExpObjAttribute detected");

                    // run visitor
                    if(runVisitorConst(n) == false){ return; }

                    // descent
                    for (auto &i : *static_cast<list<Expression*> *>(attribArgs))
                        traverseConst(i);

                    traverseConst(attribBase);
                }
                Case(C<ExpTypeAttribute>(attribTypeBase)){
                    traversalMessages.push_back("ExpTypeAttribute detected");

                    // run visitor
                    if(runVisitorConst(n) == false){ return; }

                    // descent
                    for (auto &i : *static_cast<list<Expression*> *>(attribArgs))
                        traverseConst(i);

                    traverseConst(attribTypeBase);
                }
                Otherwise(){
                    errorFlag = true;
                    traversalErrors.push_back("Raw ExpAttribute detected");
                }
            } EndMatch;
        }

        Case(C<ExpBitstring>(bitString)){
            traversalMessages.push_back("ExpBitstring detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // no descent, because leaf node
        }

        Case(C<ExpCharacter>(charValue)){
            traversalMessages.push_back("ExpCharacter detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // no descent, because leaf node
        }

        Case(C<ExpConcat>(concLeft, concRight)){
            traversalMessages.push_back("ExpConcat detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // descent
            traverseConst(concLeft);
            traverseConst(concRight);
        }

        Case(C<ExpConditional>(condOptions)){
            traversalMessages.push_back("ExpConditional detected");
            Match(n){
                Case(C<ExpSelected>(selector)){
                    traversalMessages.push_back("ExpSelected detected");

                    // run visitor
                    if(runVisitorConst(n) == false){ return; }

                    // descent
                    // TODO: implement descent
                }

                Otherwise(){
                    //TODO: Just base class
                }
            } EndMatch;
            traversalMessages.push_back("ExpConditional");
        }

        Case(C<ExpFunc>(funcName, definition, argVector)){
            traversalMessages.push_back("ExpFunc detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // descent
            // TODO: descent into definition
            for (auto &i : argVector)
                traverseConst(i);
        }

        Case(C<ExpInteger>(intValue)){
            traversalMessages.push_back("ExpInteger detected");
            //TODO: implement

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // no descent, because leaf node
        }

        Case(C<ExpReal>(dblValue)){
            traversalMessages.push_back("ExpReal detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // no descent, because leaf node
        }

        Case(C<ExpName>(nameName, indices)){
            traversalMessages.push_back("ExpName detected");

            Match(n){
                Case(C<ExpNameALL>()){
                    traversalMessages.push_back("ExpNameALL detected");

                    // run visitor
                    if(runVisitorConst(n) == false){ return; }

                    // descent
                    for (auto &i : *static_cast<list<Expression*>*>(indices))
                        traverseConst(i);
                }
                Otherwise(){
                    traversalMessages.push_back("ExpName detected");

                    // run visitor
                    if(runVisitorConst(n) == false){ return; }

                    // descent
                    for (auto &i : *static_cast<list<Expression*>*>(indices))
                        traverseConst(i);
                }
            } EndMatch;
        }

        Case(C<ExpScopedName>(scopeName, scope, scopeNameName)){
            traversalMessages.push_back("ExpScopedName detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // descent
            // TODO: scope
            traverseConst(scopeNameName);
        }

        Case(C<ExpString>(strValue)){
            traversalMessages.push_back("ExpString detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // no descent, because leaf node
        }

        Case(C<ExpCast>(castExp, castType)){
            traversalMessages.push_back("ExpCast detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // descent
            traverseConst(castExp);
            traverseConst(castType);
        }

        Case(C<ExpNew>(newSize)){
            traversalMessages.push_back("ExpNew detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // descent
            traverseConst(newSize);
        }

        Case(C<ExpTime>(timeAmount, timeUnit)){
            traversalMessages.push_back("ExpTime detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // no descent, because leaf node
        }

        Case(C<ExpRange>(rangeLeft, rangeRight,
                         /*direction,*/ rangeExpr,
                         rangeBase/*, rangeReverse*/)){
            traversalMessages.push_back("ExpRange detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // descent
            traverseConst(rangeLeft);
            traverseConst(rangeRight);
            traverseConst(rangeBase);
        }

        Case(C<ExpDelay>(delayExpr, delayDelay)){
            traversalMessages.push_back("ExpDelay detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // descent
            traverseConst(delayExpr);
            traverseConst(delayDelay);
        }

        Otherwise(){
            errorFlag = true;
            traversalErrors.push_back("Raw Expression detected");
        }
    } EndMatch;
}

void GenericTraverser::traverseConst(const SequentialStmt *n){
    currentPathConst.insert(currentPathConst.begin(), n);
    var<perm_string> loopName;
    var<list<SequentialStmt*>> loopStmts;

    // For IfSequential
    var<Expression *> ifCond;
    var<list<SequentialStmt*>> ifPath, elsePath;
    var<list<IfSequential::Elsif*>> elsifPaths;
    // For ReturnStmt
    var<Expression*> retValue;

    // For SignalSeqAssignment
    var<Expression*> assignLval;
    var<list<Expression *>> waveform;

    // For CaseStmtAlternative
    var<Expression *> caseCond;
    var<list<CaseSeqStmt::CaseStmtAlternative>> caseAlternatives;
    // For ProcedureCall
    var<perm_string> procName;
    var<list<named_expr_t*> *> procParams;
    var<SubprogramHeader *> procDef;

    // For VariableSeqAssignment
    var<Expression *> varLval, varRval;

    // For ReportStmt
    var<Expression *> reportMsg, assertCond;
    var<ReportStmt::severity_t> reportSeverity;

    // For WaitStmt
    var<WaitStmt::wait_type_t> waitType;
    var<Expression *> waitExpr;
    var<set<ExpName*>> waitSens;

    Match(n){
        Case(C<LoopStatement>(loopName, loopStmts)){
            traversalMessages.push_back("LoopStatement detected");
            var<Expression *> whileCond;
            var<perm_string> iterVar;
            var<ExpRange*> iterRange;

            Match(n){
                Case(C<WhileLoopStatement>(whileCond)){
                    traversalMessages.push_back("WhileLoopStatement detected");

                    // run visitor
                    if(runVisitorConst(n) == false){ return; }

                    // descent
                    traverseConst(whileCond);

                    for (auto &i : loopStmts)
                        traverseConst(i);
                }

                Case(C<ForLoopStatement>(iterVar, iterRange)){
                    traversalMessages.push_back("ForLoopStatement detected");

                    // run visitor
                    if(runVisitorConst(n) == false){ return; }

                    // descent
                    traverseConst(iterRange);

                    for (auto &i : loopStmts)
                        traverseConst(i);
                }

                Case(C<BasicLoopStatement>()){
                    traversalMessages.push_back("BasicLoopStatement detected");

                    // run visitor
                    if(runVisitorConst(n) == false){ return; }

                    // descent
                    for (auto &i : loopStmts)
                        traverseConst(i);
                }

                Otherwise(){
                    errorFlag = true;
                    traversalErrors.push_back("Raw LoopStatement detected");
                }
            } EndMatch;
        }

        Case(C<IfSequential>(ifCond, ifPath, elsifPaths, elsePath)){
            traversalMessages.push_back("IfSequential detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // descent
            traverseConst(ifCond);

            for (auto &i : ifPath)
                traverseConst(i);

            for (auto &i : elsifPaths)
                traverseConst(i);

            for (auto &i : elsePath)
                traverseConst(i);
        }

        Case(C<ReturnStmt>(retValue)){
            traversalMessages.push_back("ReturnStmt detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // descent
            traverseConst(retValue);
        }

        Case(C<SignalSeqAssignment>(assignLval, waveform)){
            traversalMessages.push_back("SignalSeqAssignment detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // descent
            traverseConst(assignLval);

            for (auto &i : waveform)
                traverseConst(i);
        }

        //FIXME: Ruines build. Don't know why
//        Case(C<CaseSeqStmt>(caseCond, caseAlternatives)){
//            traversalMessages.push_back("CaseSeqStmt detected");
//            //TODO:
//        }

        Case(C<ProcedureCall>(procName, procParams, procDef)){
            traversalMessages.push_back("ProcedureCall detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // descent
            for (auto &i : *static_cast<list<named_expr_t*>*>(procParams))
                traverseConst(i);
        }

        Case(C<VariableSeqAssignment>(varLval, varRval)){
            traversalMessages.push_back("VariableSeqAssignment detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // descent
            traverseConst(varLval);
            traverseConst(varRval);
        }

        Case(C<ReportStmt>(reportMsg, reportSeverity)){
            Match(n){
                Case(C<AssertStmt>(assertCond)){
                    traversalMessages.push_back("AssertStmt detected");

                    // run visitor
                    if(runVisitorConst(n) == false){ return; }

                    // descent

                        traverseConst(assertCond);
                        traverseConst(reportMsg);
                }
                Otherwise(){
                    traversalMessages.push_back("ReturnStmt detected");

                    // run visitor
                    if(runVisitorConst(n) == false){ return; }

                    // descent
                    traverseConst(reportMsg);
                }
            } EndMatch;
        }

        Case(C<WaitForStmt>()){
            traversalMessages.push_back("WaitForStmt detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // nothing to descent
        }

        Case(C<WaitStmt>(waitType, waitExpr, waitSens)){
            traversalMessages.push_back("WaitStmt detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // descent
            traverseConst(waitExpr);
            for (auto &i : waitSens)
                traverseConst(i);
        }

        Otherwise(){
            errorFlag = true;
            traversalErrors.push_back("Raw SequentialStat detected");
        }
    } EndMatch;
}

void GenericTraverser::traverseConst(const VType *n){
    currentPathConst.insert(currentPathConst.begin(), n);

    // For VTypePrimitive
    var<VTypePrimitive::type_t> primType;
    var<bool> primPacked;

    // For VTypeArray
    var<const VType *> arrEtype;
    var<vector<VTypeArray::range_t>> arrRanges;
    var<bool> arrSigFlag;
    var<const VTypeArray *> arrParent;

    // For VTypeRange
    var<const VType *> rangeBase;

    // For VTypeRangeConst
    var<int64_t> cRangeStart, cRangeEnd;

    // For VTypeRangeExpr
    var<Expression *> eRangeStart, eRangeEnd;
    var<bool> eDownto;

    // For VTypeRecord
    var<vector<VTypeRecord::element_t *>> recordElements;

    // For VTypeEnum
    var<vector<perm_string>> enumNames;

    // For VTypeDef
    var<perm_string> typeName;
    var<const VType *> typeType;

    Match(n){
        Case(C<VTypeERROR>()){
            traversalMessages.push_back("VTypeERROR detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // nothing to descent
        }

        Case(C<VTypePrimitive>(primType, primPacked)){
            traversalMessages.push_back("VTypePrimitive detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // nothing to descent
        }

        Case(C<VTypeArray>(arrEtype, arrRanges, arrSigFlag, arrParent)){
            traversalMessages.push_back("VTypeArray detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // descent
            traverseConst(arrEtype);

            for (auto &i : arrRanges)
                traverseConst(&i); // working??

            traverseConst(arrParent);
        }

        Case(C<VTypeRange>(rangeBase)){
            traversalMessages.push_back("VTypeRange detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // descent
            traverseConst(rangeBase);
        }

        Case(C<VTypeRangeConst>(cRangeStart, cRangeEnd)){
            traversalMessages.push_back("VTypeRangeConst detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // nothing to descent
        }

        Case(C<VTypeRangeExpr>(eRangeStart, eRangeEnd, eDownto)){
            traversalMessages.push_back("VTypeRangeExpr detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // descent
            traverseConst(eRangeStart);
            traverseConst(eRangeEnd);
        }

        Case(C<VTypeEnum>(enumNames)){
            traversalMessages.push_back("VTypeEnum detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // nothing to descent
        }

        Case(C<VTypeRecord>(recordElements)){
            traversalMessages.push_back("VTypeRecord detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // descent
            for (auto &i : recordElements)
                traverseConst(i);
        }

        Case(C<VTypeDef>(typeName, typeType)){
            traversalMessages.push_back("VTypeDef detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // descent
            traverseConst(typeType);
        }

        Case(C<VSubTypeDef>()){
            traversalMessages.push_back("VSubTypeDef detected");

            // run visitor
            if(runVisitorConst(n) == false){ return; }

            // nothing to descent
        }
    } EndMatch;
}

void GenericTraverser::traverseConst(const SigVarBase *n){
    currentPathConst.insert(currentPathConst.begin(), n);
    Match(n) {
        Case(C<Signal>()){
            traversalMessages.push_back("Signal detected");

            if(runVisitorConst(n) == false){ return; }

            // nothing to descent
        }
        Case(C<Variable>()){
            traversalMessages.push_back("Variable detected");

            // run visitor
            if (runVisitorConst(n) == false){ return; }

            // nothing to descent
        }
        Otherwise(){
            errorFlag = true;
            traversalErrors.push_back("Raw SigVarBase detected");
        }
    } EndMatch;
}

bool GenericTraverser::runVisitorConst(const AstNode *n){
    if (predicate(n)){
        constVisitor(n);

        return true;
    } else {
        return false;
    }
}

bool GenericTraverser::runVisitorMutating(AstNode *n){
    if (predicate(n)){
        mutatingVisitor(n);

        return true;
    } else {
        return false;
    }
}
