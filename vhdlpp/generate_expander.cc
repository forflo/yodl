#include "generic_traverser.h"
#include "architec.h"
#include "stateful_lambda.h"
#include "exp_name_replacer.h"
#include "generate_expander.h"
#include "mach7_includes.h"

int GenerateExpander::expandForGenerate(AstNode *node){
    auto forGenerate = dynamic_cast<ForGenerate*>(node);

    perm_string genvar = forGenerate->genvar_;
    perm_string label = forGenerate->name_;
    Expression *lower = forGenerate->lsb_;
    Expression *upper = forGenerate->msb_;
    ExpRange *range = forGenerate->range_;


    int64_t leftVal, rightVal;
    bool staticEvaluable = true;

    staticEvaluable = staticEvaluable &&
        lower->evaluate(currentEntity, currentScope, leftVal);
    staticEvaluable = staticEvaluable &&
        upper->evaluate(currentEntity, currentScope, rightVal);

    // non static range => semantic error
    if (!staticEvaluable){
        return 1;
    }

    // VHDL Standard says, that we have to translate each generate statement
    // into an according block statement... (see 14.5.3 of IEEE 1076-2008)
    BlockStatement *b = new BlockStatement(
        0, label, *currentScope,
        new std::list<Architecture::Statement*>(forGenerate->statements_));

    switch(range->direction_) {
    case ExpRange::range_dir_t::DOWNTO:
        if (leftVal < rightVal) { /* SEMANTIC ERROR */ return 1; }

        for (int i = leftVal; i >= rightVal; i--){
            accumulator.push_back(b->clone());
        }
        break;
    case ExpRange::range_dir_t::TO:
        if (leftVal > rightVal) { /* SEMANTIC ERROR */ return 1; }

        for (int i = leftVal; i <= rightVal; i++){
            ExpNameReplacer replacer(ExpInteger{i}, ExpName(genvar));

            GenericTraverser replacerT(
                [](const AstNode *n) -> bool {
                    Match(n){Case(mch::C<ExpName>()){return true;}} EndMatch;
                    return false;
                },
                replacer,
                GenericTraverser::NONRECUR);

            Architecture::Statement *temp = b->clone();
            replacerT(temp);

            accumulator.push_back(temp);
        }
        break;
    default:
        delete b;
        return 1; //ERROR
    }

    delete b;
    return 0;
}

// FIXME: Limitation of Parser: else and elseif not implemented!
int GenerateExpander::expandIfGenerate(AstNode *node){
    auto ifGenerate = dynamic_cast<IfGenerate*>(node);

    perm_string label = ifGenerate->name_;
    Expression *condition = ifGenerate->cond_;
    std::list<Architecture::Statement *> statements = ifGenerate->statements_;

    bool isStaticallyEvaluable = false;
    int64_t value;

    isStaticallyEvaluable = condition->evaluate(currentEntity, currentScope, value);

    // semantic error
    if (!isStaticallyEvaluable){
        return 1;
    }

    if (value){
        // VHDL Standard says, that we have to translate each generate statement
        // into an according block statement... (see 14.5.3 of IEEE 1076-2008)
        BlockStatement *b = new BlockStatement(
            0, label, *currentScope,
            new std::list<Architecture::Statement*>(statements));

        accumulator.push_back(b);
    }

    return 0;
}

bool GenerateExpander::containsGenerateStmt(
    const std::list<Architecture::Statement *> &sList){
    using namespace mch;

    StatefulLambda<int> genStmtCounter(
        0, static_cast<function<int (const AstNode *, int &)>>(
            [](const AstNode *, int &env) -> int {
                env++;
                return 0;}));

    GenericTraverser traverser(
        [](const AstNode *n) -> bool {
            Match(n){
                Case(C<ForGenerate>()){ return true;}
                Case(C<IfGenerate>()){ return true;}
            } EndMatch;
            return false;
        },
        static_cast<function<int (const AstNode *)>>(
            [&genStmtCounter](const AstNode *n) -> int {
                return genStmtCounter(n);
            }),
        GenericTraverser::RECUR);

    for (auto &i : sList){
        traverser(const_cast<const Architecture::Statement*>(i));

        if (genStmtCounter.environment > 0){
            return true;
        }
    }

    return false;
}

int GenerateExpander::expandGenerate(Architecture::Statement *g){
    using namespace mch;
    Match(g){
        Case(C<ForGenerate>()){
            return expandForGenerate(g);
        }
        Case(C<IfGenerate>()){
            return expandIfGenerate(g);
        }
    } EndMatch;
    return 0;
}

bool GenerateExpander::isBlockStmt(Architecture::Statement *g){
    using namespace mch;
    Match(g){
        Case(C<BlockStatement>()){return true;}
    } EndMatch;
    return false;
}

bool GenerateExpander::isGenerate(Architecture::Statement *g){
    using namespace mch;
    Match(g){
        Case(C<ForGenerate>()){return true;}
        Case(C<IfGenerate>()){return true;}
    } EndMatch;
    return false;
}

int GenerateExpander::modify(std::list<Architecture::Statement *> &statements){
    using namespace mch;
    std::list<Architecture::Statement *> unfolded;

    while (containsGenerateStmt(statements)){
        for (auto &i : statements){

            //TODO: replace with exception
            if (i == NULL) {
                std::cout << "Nullptr in modifyArch" << endl;
                return 1;
            }

            if (isGenerate(i)){
                bool expandRc = expandGenerate(i);

                if (expandRc){
                    std::cout << "error in expander" << endl;
                    return 1; //error in expander
                }

                for (auto &j : accumulator)
                    unfolded.push_back(j);

                accumulator.clear();
            } else if (isBlockStmt(i)) {
                modifyBlock(i);
                unfolded.push_back(i);
            } else {
                unfolded.push_back(i);
            }
        }

        statements = unfolded;
        unfolded.clear();
    }


    return 0;
}

int GenerateExpander::modifyArch(AstNode *a){
    Architecture *arch = dynamic_cast<Architecture*>(a);
    return modify(arch->statements_);
}

int GenerateExpander::modifyBlock(AstNode *b){
    BlockStatement *block = dynamic_cast<BlockStatement*>(b);

    return modify(*block->concurrent_stmts_);
}

// We need to make sure that unroll and modify
// have access to the most recent scope.
int GenerateExpander::operator()(AstNode *n){
    using namespace mch;

    Match(n){
        Case(C<Architecture>()){
            currentScope = dynamic_cast<ScopeBase *>(n);
            return modifyArch(n);
        }
        Case(C<BlockStatement>()){
            currentScope = dynamic_cast<ScopeBase *>(n);
            return modifyBlock(n);
        }
        Case(C<Entity>()){
            currentEntity = dynamic_cast<Entity*>(n);
        }
        Case(C<ScopeBase>()){
            currentScope = dynamic_cast<ScopeBase *>(n);
        }
    } EndMatch;

    return 0;
}
