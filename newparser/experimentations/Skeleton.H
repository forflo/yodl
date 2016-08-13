#ifndef SKELETON_HEADER
#define SKELETON_HEADER
/* You might want to change the above name. */

#include "Absyn.H"


class Skeleton : public Visitor
{
public:
  void visitName(Name *p);
  void visitFunction_call(Function_call *p);
  void visitAssociation_element(Association_element *p);
  void visitActual_part(Actual_part *p);
  void visitAcutal_part(Acutal_part *p);
  void visitExpression(Expression *p);
  void visitActual_designator(Actual_designator *p);
  void visitAcutal_designator(Acutal_designator *p);
  void visitPrefix(Prefix *p);
  void visitSelected_name(Selected_name *p);
  void visitSuffix(Suffix *p);
  void visitAttribute_name(Attribute_name *p);
  void visitSlice_name(Slice_name *p);
  void visitIndexed_name(Indexed_name *p);
  void visitCharacter_literal(Character_literal *p);
  void visitGraphic_character(Graphic_character *p);
  void visitOperator_symbol(Operator_symbol *p);
  void visitSimple_name(Simple_name *p);
  void visitNameSimple(NameSimple *p);
  void visitNameOp(NameOp *p);
  void visitNameChar(NameChar *p);
  void visitNameSelected(NameSelected *p);
  void visitNameIndexed(NameIndexed *p);
  void visitNameSlice(NameSlice *p);
  void visitNameAttr(NameAttr *p);
  void visitFunctionCallNyadic(FunctionCallNyadic *p);
  void visitFunctionCallPolyadic(FunctionCallPolyadic *p);
  void visitAssocElementPositional(AssocElementPositional *p);
  void visitAssocElementNamedParam(AssocElementNamedParam *p);
  void visitActualPartD(ActualPartD *p);
  void visitActualFoo(ActualFoo *p);
  void visitExpressionName(ExpressionName *p);
  void visitExpressionNumber(ExpressionNumber *p);
  void visitExpressionDesignator(ExpressionDesignator *p);
  void visitNameDesignator(NameDesignator *p);
  void visitNamePrefix(NamePrefix *p);
  void visitFunctionPrefix(FunctionPrefix *p);
  void visitSelectedName(SelectedName *p);
  void visitAllSuffix(AllSuffix *p);
  void visitAllSuffixfoo(AllSuffixfoo *p);
  void visitNameSuffix(NameSuffix *p);
  void visitAttributeName(AttributeName *p);
  void visitSliceName(SliceName *p);
  void visitIndexedName(IndexedName *p);
  void visitCharLiteral(CharLiteral *p);
  void visitGraphicChar(GraphicChar *p);
  void visitOperatorSymbol(OperatorSymbol *p);
  void visitSimpleName(SimpleName *p);
  void visitListAssociation_element(ListAssociation_element *p);
  void visitListExpression(ListExpression *p);

  void visitDecimal_Literal(Decimal_Literal x);
  void visitBase_Spec(Base_Spec x);
  void visitInteger(Integer x);
  void visitChar(Char x);
  void visitDouble(Double x);
  void visitString(String x);
  void visitIdent(Ident x);

};


#endif
