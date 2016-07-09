// Traversal Skeleton for the Expression class tree

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

    Match(e){
        Case(C<ExpUnary>(op1)){

            var<ExpEdge::fun_t> edgeSpec;

            Match(e){
                Case(C<ExpEdge>(edgeSpec)){

                }
                Case(C<ExpUAbs>()){

                }
                Case(C<ExpUNot>()){

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

                }
                Case(C<ExpLogical>(logOp)){


                }
                Case(C<ExpRelation>(relOp)){

                }
                Case(C<ExpShift>(shiftOp)){

                }
                Otherwise() {
                }
            } EndMatch;
        }

        Case(C<ExpAggregate>(elements, aggregate)){
        }

        Case(C<ExpAttribute>(attribName, attribArgs)){

            var<ExpName *> attribBase;
            var<const VType *> attribTypeBase;

            Match(e){
                Case(C<ExpObjAttribute>(attribBase)){
                }
                Case(C<ExpTypeAttribute>(attribTypeBase)){

                }
                Otherwise(){
                }
            } EndMatch;
        }

        Case(C<ExpBitstring>(bitString)){

        }

        Case(C<ExpCharacter>(charValue)){
        }

        Case(C<ExpConcat>(concLeft, concRight)){
        }

        Case(C<ExpConditional>(condOptions)){

            Match(e){
                Case(C<ExpSelected>(selector)){

                }

                Otherwise(){

                }
            } EndMatch;

        }

        Case(C<ExpFunc>(funcName, definition, argVector)){

        }

        Case(C<ExpInteger>(intValue)){
        }

        Case(C<ExpReal>(dblValue)){
        }

        Case(C<ExpName>(nameName, indices)){


            Match(e){
                Case(C<ExpNameALL>()){
                }
                Otherwise(){

                }
            } EndMatch;
        }

        Case(C<ExpScopedName>(scopeName, scope, scopeNameName)){
        }

        Case(C<ExpString>(strValue)){
        }

        Case(C<ExpCast>(castExp, castType)){
        }

        Case(C<ExpNew>(newSize)){
        }

        Case(C<ExpTime>(timeAmount, timeUnit)){
        }

        Case(C<ExpRange>(rangeLeft, rangeRight,
                         /*direction,*/ rangeExpr,
                         rangeBase/*, rangeReverse*/)){
        }

        Case(C<ExpDelay>(delayExpr, delayDelay)){

        }

        Otherwise(){

        }
    } EndMatch;
