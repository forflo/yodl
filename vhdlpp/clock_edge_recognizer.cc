#include <clock_edge_recognizer.h>
#include <root_class.h>
#include <mach7_includes.h>
#include <iostream>

namespace mch {
    template <> struct bindings<ExpUNot> {
        Members(ExpUNot::operand1_);
    };

    template <> struct bindings<ExpCharacter> {
        Members(ExpCharacter::value_);
    };

    template <> struct bindings<ExpAttribute> {
        Members(ExpAttribute::name_);
    };

    template <> struct bindings<ExpLogical> {
        Members(ExpLogical::operand1_,
                ExpLogical::operand2_,
                ExpLogical::fun_);
    };

    template <> struct bindings<ExpFunc> {
        Members(ExpFunc::name_,
                ExpFunc::argv_);
    };

    template <> struct bindings<ExpName> {
        //TODO: add prefix
        Members(//ExpName::prefix_, // unique_ptr<ExpName>
                ExpName::name_,
                ExpName::indices_);
    };

    template <> struct bindings<ExpRelation> {
        Members(ExpRelation::operand1_,
                ExpRelation::operand2_,
                ExpRelation::fun_);
    };
};

void ClockEdgeRecognizer::reset(void){
    containsClockEdge = false;
    numberClockEdges = 0;
    direction = NetlistGenerator::edge_spec::UNDEF;

    clockNameExp = NULL;
    clockFuncExp = NULL;
}

int ClockEdgeRecognizer::operator()(const Expression *n){
    using namespace mch;

    auto checkHelper = [this](var<perm_string> attrName,
                              var<char> charVal,
                              const ExpName *tmp,
                              const char *checkAttrName,
                              var<ExpLogical::fun_t> logOp,
                              var<ExpRelation::fun_t> relOp)
        -> void {
        if (tmp == NULL)
            std::cout << "ClockEdgeRecognizer checkHelper. Impossible!"
                      << std::endl;

        if ((charVal == '0' || charVal == '1') &&
            (relOp == ExpRelation::fun_t::EQ) &&
            (logOp == ExpLogical::fun_t::AND) &&
            (!strcmp(attrName, checkAttrName))){

            clockNameExp = tmp;
            containsClockEdge = true;
            numberClockEdges++;
            if (charVal == '0')
                direction = NetlistGenerator::edge_spec::FALLING;
            else
                direction = NetlistGenerator::edge_spec::RISING;
        }
    };

    var<perm_string> attrName, name, funcName;
    var<char> charVal;
    var<vector<Expression*>> params;
    var<ExpLogical::fun_t> logOp;
    var<ExpRelation::fun_t> relOp;

    Match(n){
        Case(C<ExpFunc>(funcName, params)){
            if (params.size() == 1 &&
                (!strcmp(funcName, "falling_edge") ||
                 !strcmp(funcName, "rising_edge" ))) {
                containsClockEdge = true;
                numberClockEdges++;
            }

            clockFuncExp = dynamic_cast<const ExpFunc*>(n);

            if (!strcmp(funcName, "falling_edge"))
                direction = NetlistGenerator::edge_spec::FALLING;
            if (!strcmp(funcName, "rising_edge"))
                direction = NetlistGenerator::edge_spec::RISING;

            break;
        }

        // case 1
        Case(C<ExpLogical>(
                 C<ExpRelation>(
                     C<ExpName>(name),
                     C<ExpCharacter>(charVal),
                     relOp),
                 C<ExpAttribute>(attrName),
                 logOp)){

            checkHelper(attrName, charVal,
                        dynamic_cast<const ExpName *>(
                            dynamic_cast<const ExpRelation *>(
                                dynamic_cast<const ExpLogical *>(n)
                                ->operand1_)
                            ->operand1_),
                        "event", logOp, relOp);
            break;
        }
        Case(C<ExpLogical>(
                 C<ExpRelation>(
                     C<ExpCharacter>(charVal),
                     C<ExpName>(name),
                     relOp),
                 C<ExpAttribute>(attrName),
                 logOp)){

            checkHelper(attrName, charVal,
                        dynamic_cast<const ExpName *>(
                            dynamic_cast<const ExpRelation *>(
                                dynamic_cast<const ExpLogical *>(n)
                                ->operand1_)
                            ->operand2_),
                        "event", logOp, relOp);
            break;
        }

        // case 2
        Case(C<ExpLogical>(
                 C<ExpRelation>(
                     C<ExpName>(name),
                     C<ExpCharacter>(charVal),
                     relOp),
                 C<ExpUNot>(
                     C<ExpAttribute>(attrName)),
                 logOp)){

            checkHelper(attrName, charVal,
                        dynamic_cast<const ExpName *>(
                            dynamic_cast<const ExpRelation *>(
                                dynamic_cast<const ExpLogical *>(n)
                                ->operand1_)
                            ->operand1_),
                        "stable", logOp, relOp);
            break;
        }
        Case(C<ExpLogical>(
                 C<ExpRelation>(
                     C<ExpCharacter>(charVal),
                     C<ExpName>(name),
                     relOp),
                 C<ExpUNot>(
                     C<ExpAttribute>(attrName)),
                 logOp)){

            checkHelper(attrName, charVal,
                        dynamic_cast<const ExpName *>(
                            dynamic_cast<const ExpRelation *>(
                                dynamic_cast<const ExpLogical *>(n)
                                ->operand1_)
                            ->operand2_),
                        "stable", logOp, relOp);
            break;
        }

        // mirror cases for case 1
        Case(C<ExpLogical>(
                 C<ExpAttribute>(attrName),
                 C<ExpRelation>(
                     C<ExpName>(name),
                     C<ExpCharacter>(charVal),
                     relOp),
                 logOp)){

            checkHelper(attrName, charVal,
                        dynamic_cast<const ExpName *>(
                            dynamic_cast<const ExpRelation *>(
                                dynamic_cast<const ExpLogical *>(n)
                                ->operand2_)
                            ->operand1_),
                        "event", logOp, relOp);
            break;
        }
        Case(C<ExpLogical>(
                 C<ExpAttribute>(attrName),
                 C<ExpRelation>(
                     C<ExpCharacter>(charVal),
                     C<ExpName>(name),
                     relOp),
                 logOp)){

            checkHelper(attrName, charVal,
                        dynamic_cast<const ExpName *>(
                            dynamic_cast<const ExpRelation *>(
                                dynamic_cast<const ExpLogical *>(n)
                                ->operand2_)
                            ->operand2_),
                        "event", logOp, relOp);
            break;
        }

        // mirror cases for case 2
        Case(C<ExpLogical>(
                 C<ExpUNot>(
                     C<ExpAttribute>(attrName)),
                 C<ExpRelation>(
                     C<ExpName>(name),
                     C<ExpCharacter>(charVal),
                     relOp),
                 logOp)){

            checkHelper(attrName, charVal,
                        dynamic_cast<const ExpName *>(
                            dynamic_cast<const ExpRelation *>(
                                dynamic_cast<const ExpLogical *>(n)
                                ->operand2_)
                            ->operand1_),
                        "stable", logOp, relOp);
            break;
        }
        Case(C<ExpLogical>(
                 C<ExpUNot>(
                     C<ExpAttribute>(attrName)),
                 C<ExpRelation>(
                     C<ExpCharacter>(charVal),
                     C<ExpName>(name),
                     relOp),
                 logOp)){

            checkHelper(attrName, charVal,
                        dynamic_cast<const ExpName *>(
                            dynamic_cast<const ExpRelation *>(
                                dynamic_cast<const ExpLogical *>(n)
                                ->operand2_)
                            ->operand2_),
                        "stable", logOp, relOp);
            break;
        }
        Otherwise(){ break; }
    } EndMatch;
    return 0;
}
