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
            accumulator.push_back(b->clone());
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
        delete b;
    }

    return 0;
}

bool GenerateExpander::containsGenerateStmt(std::list<Architecture::Statement *> &sList){
    using namespace mch;

    StatefulLambda<int> genStmtCounter(
        0, static_cast<function<int (const AstNode *, int &)>>(
            [](const AstNode *, int &env) -> int {
                env++;
                return 0;
            }));

    for (auto &i : sList){
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
            i, GenericTraverser::RECUR);

        traverser.traverse();

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

    while (containsGenerateStmt(statements)){
        for (std::list<Architecture::Statement *>::iterator i = statements.begin();
             i != statements.end();
             ++i){

            //TODO: replace with exception
            if (*i == NULL) {
                std::cout << "Nullptr in modifyArch" << endl;
                return 1;
            }

            if (isGenerate(*i)){
                if (expandGenerate(*i)){
                    statements.splice(i, accumulator);
                    statements.erase(i);
                    std::advance(i, accumulator.size() - 1);
                    accumulator.clear();
                } else {
                    return 1; //error in expander
                }
            }
        }
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
        Case(C<Entity>()){
            currentEntity = dynamic_cast<Entity*>(n);
        }
        Case(C<ScopeBase>()){
            currentScope = dynamic_cast<ScopeBase *>(n);
        }
        Case(C<Architecture>()){
            return modifyArch(n);
        }
        Case(C<BlockStatement>()){
            return modifyBlock(n);
        }
    } EndMatch;

    return 0;
}
