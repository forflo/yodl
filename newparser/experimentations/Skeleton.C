/*** BNFC-Generated Visitor Design Pattern Skeleton. ***/
/* This implements the common visitor design pattern.
   Note that this method uses Visitor-traversal of lists, so
   List->accept() does NOT traverse the list. This allows different
   algorithms to use context information differently. */

#include "Skeleton.H"



void Skeleton::visitName(Name *t) {} //abstract class
void Skeleton::visitFunction_call(Function_call *t) {} //abstract class
void Skeleton::visitAssociation_element(Association_element *t) {} //abstract class
void Skeleton::visitActual_part(Actual_part *t) {} //abstract class
void Skeleton::visitAcutal_part(Acutal_part *t) {} //abstract class
void Skeleton::visitExpression(Expression *t) {} //abstract class
void Skeleton::visitActual_designator(Actual_designator *t) {} //abstract class
void Skeleton::visitAcutal_designator(Acutal_designator *t) {} //abstract class
void Skeleton::visitPrefix(Prefix *t) {} //abstract class
void Skeleton::visitSelected_name(Selected_name *t) {} //abstract class
void Skeleton::visitSuffix(Suffix *t) {} //abstract class
void Skeleton::visitAttribute_name(Attribute_name *t) {} //abstract class
void Skeleton::visitSlice_name(Slice_name *t) {} //abstract class
void Skeleton::visitIndexed_name(Indexed_name *t) {} //abstract class
void Skeleton::visitCharacter_literal(Character_literal *t) {} //abstract class
void Skeleton::visitGraphic_character(Graphic_character *t) {} //abstract class
void Skeleton::visitOperator_symbol(Operator_symbol *t) {} //abstract class
void Skeleton::visitSimple_name(Simple_name *t) {} //abstract class

void Skeleton::visitNameSimple(NameSimple *name_simple)
{
  /* Code For NameSimple Goes Here */

  name_simple->simple_name_->accept(this);

}

void Skeleton::visitNameOp(NameOp *name_op)
{
  /* Code For NameOp Goes Here */

  name_op->operator_symbol_->accept(this);

}

void Skeleton::visitNameChar(NameChar *name_char)
{
  /* Code For NameChar Goes Here */

  name_char->character_literal_->accept(this);

}

void Skeleton::visitNameSelected(NameSelected *name_selected)
{
  /* Code For NameSelected Goes Here */

  name_selected->selected_name_->accept(this);

}

void Skeleton::visitNameIndexed(NameIndexed *name_indexed)
{
  /* Code For NameIndexed Goes Here */

  name_indexed->indexed_name_->accept(this);

}

void Skeleton::visitNameSlice(NameSlice *name_slice)
{
  /* Code For NameSlice Goes Here */

  name_slice->slice_name_->accept(this);

}

void Skeleton::visitNameAttr(NameAttr *name_attr)
{
  /* Code For NameAttr Goes Here */

  name_attr->attribute_name_->accept(this);

}

void Skeleton::visitFunctionCallNyadic(FunctionCallNyadic *function_call_nyadic)
{
  /* Code For FunctionCallNyadic Goes Here */

  function_call_nyadic->name_->accept(this);

}

void Skeleton::visitFunctionCallPolyadic(FunctionCallPolyadic *function_call_polyadic)
{
  /* Code For FunctionCallPolyadic Goes Here */

  function_call_polyadic->name_->accept(this);
  function_call_polyadic->listassociation_element_->accept(this);

}

void Skeleton::visitAssocElementPositional(AssocElementPositional *assoc_element_positional)
{
  /* Code For AssocElementPositional Goes Here */

  assoc_element_positional->actual_part_->accept(this);

}

void Skeleton::visitAssocElementNamedParam(AssocElementNamedParam *assoc_element_named_param)
{
  /* Code For AssocElementNamedParam Goes Here */

  assoc_element_named_param->name_->accept(this);
  assoc_element_named_param->actual_part_->accept(this);

}

void Skeleton::visitActualPartD(ActualPartD *actual_part_d)
{
  /* Code For ActualPartD Goes Here */

  actual_part_d->actual_designator_->accept(this);

}

void Skeleton::visitActualFoo(ActualFoo *actual_foo)
{
  /* Code For ActualFoo Goes Here */

  actual_foo->name_->accept(this);
  actual_foo->actual_designator_->accept(this);

}

void Skeleton::visitExpressionName(ExpressionName *expression_name)
{
  /* Code For ExpressionName Goes Here */

  expression_name->name_->accept(this);

}

void Skeleton::visitExpressionNumber(ExpressionNumber *expression_number)
{
  /* Code For ExpressionNumber Goes Here */

  visitInteger(expression_number->integer_);

}

void Skeleton::visitExpressionDesignator(ExpressionDesignator *expression_designator)
{
  /* Code For ExpressionDesignator Goes Here */

  expression_designator->expression_->accept(this);

}

void Skeleton::visitNameDesignator(NameDesignator *name_designator)
{
  /* Code For NameDesignator Goes Here */

  name_designator->name_->accept(this);

}

void Skeleton::visitNamePrefix(NamePrefix *name_prefix)
{
  /* Code For NamePrefix Goes Here */

  name_prefix->name_->accept(this);

}

void Skeleton::visitFunctionPrefix(FunctionPrefix *function_prefix)
{
  /* Code For FunctionPrefix Goes Here */

  function_prefix->function_call_->accept(this);

}

void Skeleton::visitSelectedName(SelectedName *selected_name)
{
  /* Code For SelectedName Goes Here */

  selected_name->prefix_->accept(this);
  selected_name->suffix_->accept(this);

}

void Skeleton::visitAllSuffix(AllSuffix *all_suffix)
{
  /* Code For AllSuffix Goes Here */


}

void Skeleton::visitAllSuffixfoo(AllSuffixfoo *all_suffixfoo)
{
  /* Code For AllSuffixfoo Goes Here */


}

void Skeleton::visitNameSuffix(NameSuffix *name_suffix)
{
  /* Code For NameSuffix Goes Here */

  name_suffix->simple_name_->accept(this);

}

void Skeleton::visitAttributeName(AttributeName *attribute_name)
{
  /* Code For AttributeName Goes Here */

  attribute_name->prefix_->accept(this);
  attribute_name->name_->accept(this);

}

void Skeleton::visitSliceName(SliceName *slice_name)
{
  /* Code For SliceName Goes Here */

  slice_name->prefix_->accept(this);
  slice_name->expression_->accept(this);

}

void Skeleton::visitIndexedName(IndexedName *indexed_name)
{
  /* Code For IndexedName Goes Here */

  indexed_name->prefix_->accept(this);
  indexed_name->listexpression_->accept(this);

}

void Skeleton::visitCharLiteral(CharLiteral *char_literal)
{
  /* Code For CharLiteral Goes Here */

  visitChar(char_literal->char_);

}

void Skeleton::visitGraphicChar(GraphicChar *graphic_char)
{
  /* Code For GraphicChar Goes Here */

  visitChar(graphic_char->char_);

}

void Skeleton::visitOperatorSymbol(OperatorSymbol *operator_symbol)
{
  /* Code For OperatorSymbol Goes Here */

  visitString(operator_symbol->string_);

}

void Skeleton::visitSimpleName(SimpleName *simple_name)
{
  /* Code For SimpleName Goes Here */

  visitIdent(simple_name->ident_);

}


void Skeleton::visitListAssociation_element(ListAssociation_element *list_association_element)
{
  for (ListAssociation_element::iterator i = list_association_element->begin() ; i != list_association_element->end() ; ++i)
  {
    (*i)->accept(this);
  }
}

void Skeleton::visitListExpression(ListExpression *list_expression)
{
  for (ListExpression::iterator i = list_expression->begin() ; i != list_expression->end() ; ++i)
  {
    (*i)->accept(this);
  }
}


void Skeleton::visitDecimal_Literal(Decimal_Literal x)
{
  /* Code for Decimal_Literal Goes Here */
}

void Skeleton::visitBase_Spec(Base_Spec x)
{
  /* Code for Base_Spec Goes Here */
}

void Skeleton::visitInteger(Integer x)
{
  /* Code for Integer Goes Here */
}

void Skeleton::visitChar(Char x)
{
  /* Code for Char Goes Here */
}

void Skeleton::visitDouble(Double x)
{
  /* Code for Double Goes Here */
}

void Skeleton::visitString(String x)
{
  /* Code for String Goes Here */
}

void Skeleton::visitIdent(Ident x)
{
  /* Code for Ident Goes Here */
}



