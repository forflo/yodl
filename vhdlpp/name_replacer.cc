// FM. MA
// Simple constant propagator
////
#include "name_replacer.h"

int NameReplacer::operator()(AstNode *n, const std::vector<AstNode *> &parents){
    using namespace mch;

    std::cout << "Replacer called with: " <<
        dynamic_cast<const ExpInteger*>(&replacement)->value_ << " " <<
        name.name_ << endl;


    if (n == 0) { return 1; }
    if (parents.size() == 0){ return 0; }

    ExpName *current = dynamic_cast<ExpName *>(n);
    if (current->name_ != name.name_){
        return 0;
    }

    Match(parents[0]){
        Case(C<ExpBinary>()){
            ExpBinary *toMod = dynamic_cast<ExpBinary*>(parents[0]);

            if (toMod->operand1_ == current){
                delete toMod->operand1_;
                toMod->operand1_ = replacement.clone();
            } else {
                delete toMod->operand2_;
                toMod->operand2_ = replacement.clone();
            }

            break;
        }
        Case(C<ExpRange>()){
            ExpRange *toMod = dynamic_cast<ExpRange*>(parents[0]);
            std::cout << "expRange REPLACER" << std::endl;

            if (toMod->left_ == current){
                delete toMod->left_;
                toMod->left_ = replacement.clone();
            } else if (toMod->right_ == current) {
                delete toMod->right_;
                toMod->right_ = replacement.clone();
            } else {
                std::cout << "This should not have happened!" << std::endl;
                return 1;
            }

            break;
        }
        Case(C<SignalAssignment>()){
            std::list<Expression *> newRvals;
            std::list<Expression *> obsoleteRvals;
            SignalAssignment *assign =
                dynamic_cast<SignalAssignment*>(parents[0]);

            for (auto &i : assign->rval_){

                Match(i){
                    Case(C<ExpName>()){
                        newRvals.push_back(replacement.clone());
                        obsoleteRvals.push_back(i);
                        break;
                    }
                    Otherwise(){
                        newRvals.push_back(i);
                        break;
                    }
                } EndMatch;
            }

            cout << "deletion of" << obsoleteRvals.size() << endl;
            for (auto &i : obsoleteRvals)
                delete i;

            assign->rval_ = newRvals;

            if (assign->lval_ == current){
                // TODO: Only indices of ExpNames may be
                // replaced by replacement
                //    assign->lval_ = replacement; is not correct (and possible)
            }

            break;
        }
        Otherwise(){
            std::cout << "Name Replacement not successful!" << endl;
            break;
        }
    } EndMatch;

    return 0;
}
