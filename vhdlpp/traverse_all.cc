#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <cerrno>
#include <limits>
#include <vector>
#include <map>
#include <iostream>
#include <math.h>

////
// mach7 setup
// Support for N-ary Match statement on patterns
#include <type_switchN-patterns.hpp>
#include <address.hpp>      // Address and dereference combinators
#include <bindings.hpp>     // Mach7 support for bindings on arbitrary UDT
#include <constructor.hpp>  // Support for constructor patterns
#include <equivalence.hpp>  // Equivalence combinator +

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
#include "traverse_all.h"
#include "vtype.h"
#include "root_class.h"
#include "mach7_bindings.h"

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

void traverse(AstNode *root){
    cout << "ast root switch\n";

    Match(root){
        Case(C<Entity>()){
            traverse(static_cast<Entity*>(root));
            return ;
        }
        Case(C<Architecture>()) {
            traverse(static_cast<Architecture*>(root));
            return;
        }
        Otherwise() { return; }
    } EndMatch
}

void traverse(Entity *top){
    cout << "entity switch\n";

    var<map<perm_string, Architecture *>> archs;
    var<Architecture*> bound_arch;
    var<map<perm_string, VType::decl_t>> decls;
    var<perm_string> name;

    Match(top){
        Case(C<Entity>(archs, bound_arch, decls, name)){
            cout << "Entity detected: " << name << endl;
            for (auto &i : archs)
                traverse(i.second);
            return;
        }
        Otherwise() {
            cout << "No Entity!" << endl;
            return;
        }
    } EndMatch
}

void traverse(Architecture *arch){
    cout << "arch switch\n";

    var<list<Architecture::Statement *>> stmts;
    var<ComponentInstantiation *> comps;
    var<ProcessStatement *> procs;
    var<perm_string> name;

    Match(arch){
        Case(C<Architecture>(stmts, comps, procs, name)){
            cout << "Architecture detected: " << name << endl;
            for (auto &i : stmts)
                traverse(*i);
            return;
        }
        Otherwise(){
            cout << "No Architecture!" << endl;
            return;
        }
    } EndMatch
}

void traverse(const Architecture::Statement &s){
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

    Match(s){
        Case(C<GenerateStatement>(name, stmts)){
            var<Expression *> cond, msb, lsb;
            var<perm_string> genvar;

            Match(s){
                Case(C<ForGenerate>(genvar, msb, lsb)){
                    //TODO: traverse further
                }
                Case(C<IfGenerate>(cond)){
                    //TODO: traverse further
                }
                Otherwise(){ return; } //TODO: Error log
            } EndMatch

            return;
        }

        Case(C<SignalAssignment>(lval, rval)){
            //TODO: traverse(lval);
            //TODO:
            //for (auto &i : rval)
            //    traverse(i);
            cout << "Found SignalAssignment" << endl;
        }

        Case(C<CondSignalAssignment>(lval, options, senslist)){
            //TODO: traverse(lval);
            //TODO:
            //for (auto &i : options)
            //    traverse(i);
            //for (auto &i : senslist)
            //    traverse(i);
            cout << "Found CondSignalAssignment" << endl;
        }

        Case(C<ComponentInstantiation>(iname, cname, genmap, portmap)){
            //TODO: traverse further
            cout << "Found Component Instantiation" << endl;
        }

        Case(C<StatementList>(seqStmts)){
            var<perm_string> name;
            var<list<Expression *>> procSensList;
            Match(s){
                Case(C<FinalStatement>()) {
                    cout << "Found final Statement" << endl;
                }
                Case(C<InitialStatement()) {
                    cout << "Found initial Statement" << endl;
                }
                Case(C<ProcessStatement>(name, procSensList)) {
                    //TODO: traverse stmts, procSensList;
                    cout << "Found process: " << name << endl;
                }
                Otherwise() {
                    //TODO: Error msg!
                    return ;
                }
            } EndMatch
        }

        Case(C<BlockStatement>(label, header, stmts_ptr)){
            cout << "Found Block statement: " << label << "\n";
            for (auto &i : static_cast<list<Architecture::Statement*>*>(stmts_ptr)){
                traverse(*i);
            }
        }

        Otherwise() {
            //cout << "Wildcard pattern for traverse(const Arch::Stmt &s)\n";
            return;
        }
    } EndMatch
}

void traverse(Expression *e){
    var<const VType *> type;
    var<Expression *> op1, op2;

    Match(e){
        Case(C<ExpUnary>(op1)){
            var<ExpEdge::fun_t> edgeSpec;

            Match(e){
                Case(C<ExpEdge>(edgeSpec)){
                    //TODO: implement
                }
                Case(C<ExpUAbs>()){
                    //TODO: implement
                }
                Case(C<ExpUNot>()){
                    //TODO: implement
                }
            } EndMatch
        }
        Case(C<ExpBinary>(op1, op2)){
            var<ExpArithmetic::fun_t> arithOp;
            var<ExpLogical::fun_t> logOp;
            var<ExpRelation::fun_t> relOp;
            var<ExpShift::shift_t> shiftOp;

            Match(e){
                Case(C<ExpArithmetic>(arithOp)){
                    //TODO:
                }
                Case(C<ExpLogical>(logOp)){
                    //TODO:
                }
                Case(C<ExpRelation>(relOp)){
                    //TODO:
                }
                Case(C<ExpShift>(shiftOp)){
                    //TODO:
                }
                Otherwise() {/*error*/}
            } EndMatch
        }

        var<vector<element_t*>> elements;
        var<vector<choice_element>> aggregate;
        Case(C<ExpAggregate>(elements, aggregate)){
            //TODO: implement
            cout << "ExpAggregate" << endl;
        }

        var<perm_string> attribName;
        var<list<Expression *> *> arribArgs;
        Case(C<ExpAttribute>(attribName, attribArgs)){
            var<ExpName *> attribBase;
            var<const VType *> attribTypeBase;
            Match(e){
                Case(C<ExpObjAttribute>(attribBase)){
                    //TODO: Implement
                }
                Case(C<ExpTypeAttribute>(attribTypeBase)){
                    //TODO: Implement
                }
                Otherwise(){
                    //TODO: No error. Just base class
                }
            } EndMatch
            //TODO: implement
            cout << "ExpAttribute" << endl;
        }

        var<vector<char>> bitString;
        Case(C<ExpBitstring>(bitString)){
            //TODO: implement
            cout << "ExpBitstring" << endl;
        }

        var<char> charValue;
        Case(C<ExpCharacter>(charValue)){
            //TODO: implement
            cout << "ExpCharacter" << endl;
        }

        var<Expression*> concLeft, concRight;
        Case(C<ExpConcat>(concLeft, concRight)){
            //TODO: implement
            cout << "ExpConcat" << endl;
        }

        var<list<case_t*>> condOptions;
        var<Expressoin*> selector;
        Case(C<ExpConditional>(condOptions)){
            Match(e){
                Case(C<ExpSelected>(selector)){
                    //TODO: implement
                }
                Otherwise(){
                    //TODO: Just base class
                }
            } EndMatch
            //TODO: implement
            cout << "ExpConditional" << endl;
        }

        var<perm_string> funcName;
        var<SubprogramHeader *> definition;
        var<vector<Expression*>> argVector;
        Case(C<ExpFunc>(funcName, definition, argVector)){
            //TODO: implement
            cout << "ExpFunc" << endl;
        }

        var<int64_t> intValue;
        Case(C<ExpInteger>(intValue)){
            //TODO: implement
            cout << "ExpInteger" << endl;
        }

        var<double> dblValue;
        Case(C<ExpReal>(dblValue)){
            //TODO: implement
            cout << "ExpReal" << endl;
        }

        var<perm_string> nameName;
        var<list<Expression*>*> indices;
        Case(C<ExpName>(nameName, indices)){
            Match(e){
                Case(C<ExpNameAll>()){
                    //TODO: Implement
                }
                Otherwise(){
                    //TODO: Here just the base class
                }
            } EndMatch
            //TODO: implement
            cout << "ExpName" << endl;
        }

        var<perm_string> scopeName;
        var<ScopeBase *> scope;
        var<ExpName*> nameName;
        Case(C<ExpScopedName>(scopeName, scope, nameName)){
            //TODO: implement
            cout << "ExpScopedName" << endl;
        }

        var<string> strValue
        Case(C<ExpString>(strValue)){
            //TODO: implement
            cout << "ExpString" << endl;
        }

        var<Expression*> castExp;
        var<const VType *> castType;
        Case(C<ExpCast>(castExp, castType)){
            //TODO: implement
            cout << "ExpCast" << endl;
        }

        var<Expression*> newSize;
        Case(C<ExpNew>(newSize)){
            //TODO: implement
            cout << "ExpNew" << endl;
        }

        var<uint64_t> timeAmount;
        var<ExpTime::timeunit_t> timeUnit;
        Case(C<ExpTime>(timeAmount, timeUnit)){
            //TODO: implement
            cout << "ExpTime" << endl;
        }

        var<Expression*> rangeLeft, rangeRight;
        var<ExpRange::range_dir_t> direction;
        var<bool> rangeExpr, rangeReverse;
        var<ExpName *> rangeBase;
        Case(C<ExpRange>(rangeLeft, rangeRight,
                         direction, rangeExpr,
                         rangeBase, rangeReverse)){
            //TODO: implement
            cout << "ExpRange" << endl;
        }

        var<Expression *> delayExpr, delayDelay;
        Case(C<ExpDelay>(delayExpr, delayDelay)){
            //TODO: implement
            cout << "ExpDelay" << endl;
        }

        Otherwise(){
            //TODO: error message
        }
    } EndMatch
}

void traverse(SequentialStmt *seq){
    var<perm_string> loopName;
    var<list<SequentialStmt*>> loopStmts;

    Match(seq){
        Case(C<LoopStatement>(loopName, loopStmts)){
            var<Expression *> whileCond;
            var<perm_string> iterVar;
            var<ExpRange*> iterRange;

            Match(seq){
                Case(C<WhileLoopStatement>(whileCond)){
                    //TODO:
                }

                Case(C<ForLoopStatement>(iterVar, iterRange)){
                    //TODO:
                }

                Case(C<BasicLoopStatement>()){
                    //TODO:
                }

                Otherwise(){
                    //TODO: Error!
                }
            } EndMatch
        }

        var<Expression *> ifCond;
        var<list<SequentialStmt*>> ifPath, elsePath;
        var<list<IfSequential::Elsif*>> elsifPaths;

        Case(C<IfSequential>(ifCond, ifPath,
                             elsifPaths, elsePath)){
            //TODO:
        }

        var<Expression*> retValue;
        Case(C<ReturnStmt>(retValue)){
            //TODO:
        }

        var<Expression*> assignLval;
        var<list<Expression *>> waveform;
        Case(C<SignalSeqAssignment>(assignLval, waveform)){
            //TODO:
        }

        var<Expression *> caseCond;
        var<list<CaseStmtAlternative>> caseAlternatives;
        Case(C<CaseSeqStmt>(caseCond, caseAlternatives)){
            //TODO:
        }

        var<perm_string> procName;
        var<list<named_expr_t*> *> procParams;
        var<SubprogramHeader *> procDef;
        Case(C<ProcedureCall>(procName, procParams, procDef)){
            //TODO:
        }

        var<Expression *> varLval, varRval;
        Case(C<VariableSeqAssignment>(varLval, varRval)){
            //TODO:
        }

        var<Expression *> reportMsg, assertCond;
        var<ReportStmt::severity_t> reportSeverity;
        Case(C<ReportStmt>(reportMsg, reportSeverity)){
            Match(seq){
                Case(C<AssertStmt>(assertCond)){
                    //TODO:
                }
                Otherwise(){
                    //TODO: Just ReportStmt
                }
            } EndMatch
        }

        var<> ;
        Case(C<WaitForStmt>()){
            //TODO:
        }

        var<WaitStmt::wait_type_t> waitType;
        var<Expression *> waitExpr;
        var<set<ExpName*>> waitSens;
        Case(C<WaitStmt>(waitType, waitExpr, waitSens)){
            //TODO:
        }

        Otherwise(){
            //TODO: error message
        }
    } EndMatch
}
