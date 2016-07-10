#include "generate_expander.h"
#include "mach7_includes.h"

int GenerateExpander::expandForGenerate(AstNode *node){
    auto forGenerate = dynamic_cast<ForGenerate*>(node);

    perm_string genvar = forGenerate->genvar_;
    perm_string label = forGenerate->name_;
    Expression *lower = forGenerate->lsb_;
    Expression *upper = forGenerate->msb_;
    ExpRange *range = forGenerate->range_;
    std::list<Architecture::Statement *> statements = forGenerate->statements_;
    std::list<Architecture::Statement *> tempAccu(accumulator);

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

    BlockStatement *b = new BlockStatement(
        0, label, *currentScope,
        new std::list<Architecture::Statement*>(statements));

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

    for (auto &i : sList){
        Match(i){
            Case(C<ForGenerate>()){ return true;}
            Case(C<IfGenerate>()){ return true;}
        } EndMatch;
    }

    return false;
}

void GenerateExpander::switchGenerateType(Architecture::Statement *g){
    using namespace mch;
    Match(g){
        Case(C<ForGenerate>()){ expandForGenerate(g); }
        Case(C<IfGenerate>()){ expandIfGenerate(g); }
    } EndMatch;
}

int GenerateExpander::modifyArch(AstNode *a){
    Architecture *arch = dynamic_cast<Architecture*>(a);

    using namespace mch;

    for (std::list<Architecture::Statement *>::iterator i = arch->statements_.begin();
         i != arch->statements_.end();
         ++i){

        //TODO: replace with exception
        if (*i == NULL) { std::cout << "Nullptr in modifyArch" << endl; }
        switchGenerateType(*i);
    }

    return 0;
}


int GenerateExpander::modifyBlock(AstNode *b){
    BlockStatement *block = dynamic_cast<BlockStatement*>(b);

    using namespace mch;

    for (std::list<Architecture::Statement *>::iterator i = block->concurrent_stmts_->begin();
         i != block->concurrent_stmts_->end();
         ++i){

        //TODO: replace with exception
        if (*i == NULL) { std::cout << "Nullptr in modifyArch" << endl; }
        switchGenerateType(*i);
    }

    return 0;
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
