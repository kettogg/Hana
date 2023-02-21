#include "VisitorPrettyPrint.h"
#include "../AST/AstNode.h"
#include "../CodeGen/Return.h"
#include "../CodeGen/FunctionDeclaration.h"
#include "../CodeGen/ClassDeclaration.h"
#include "../CodeGen/Conditional.h"
#include "../CodeGen/UnaryOperator.h"
#include "../CodeGen/BinaryOperator.h"
#include "../CodeGen/Assignment.h"
#include "../CodeGen/MethodCall.h"
#include "../CodeGen/Declaration.h"
#include "../CodeGen/WhileLoop.h"
#include "../CodeGen/Array.h"
#include "../CodeGen/CompareOperator.h"
#include "../CodeGen/Range.h"

namespace hana {

static inline std::string indent_spaces(size_t indent)
{
   return std::string(indent * 2u, ' ');
}

void VisitorPrettyPrint::VisitExpression( Expression* expr )
{
   out << indent_spaces(indent) << "Create " << expr->toString() << std::endl;
}  

void VisitorPrettyPrint::VisitStatement( Statement* stmt )
{
   out << indent_spaces(indent) << "Create " << stmt->toString() << std::endl;

}

void VisitorPrettyPrint::VisitReturnStatement( Return* retstmt )
{
   out << indent_spaces(indent) << "Create " << retstmt->toString() << std::endl;
   if( retstmt->getRetExpression() != nullptr ) {
      ++indent;
      retstmt->getRetExpression()->Accept(*this);
      --indent;
   }
}

void VisitorPrettyPrint::VisitFunctionDeclaration( FunctionDeclaration* fndecl )
{
   out << indent_spaces(indent) << "Create " << fndecl->toString() << std::endl;
   ++indent;
   auto parameter = fndecl->getParameter();
   if(parameter->size()) {
      out << indent_spaces(indent) << "Parameters :" << std::endl;
      ++indent;
      for(auto decl : *parameter) {
         out << indent_spaces(indent) << decl->getVariablenTypeName() << " " << decl->getVariablenName() << std::endl;
      }
      --indent;
   }

   auto body = fndecl->getBody();
   for( auto stmt : body->statements ) {
      stmt->Accept( *this );
   }
   --indent;
}

void VisitorPrettyPrint::VisitConditional( Conditional* cmp )
{
   out << indent_spaces(indent) << "Create " << cmp->toString() << std::endl;
   ++indent;
   cmp->getCompOperator()->Accept(*this);
   if( cmp->getThen() ) {
      cmp->getThen()->Accept( *this );
   }
   if( cmp->getElse() ) {
      cmp->getElse()->Accept( *this );
   }
   --indent;
}

void VisitorPrettyPrint::VisitInteger( Integer* expr )
{
   out << indent_spaces(indent) << "Create " << expr->toString() << std::endl;
}

void VisitorPrettyPrint::VisitDouble( Double* expr )
{
   out << indent_spaces(indent) << "Create " << expr->toString() << std::endl;
}

void VisitorPrettyPrint::VisitString( String* expr )
{
   out << indent_spaces(indent) << "Create " << expr->toString() << std::endl;
}

void VisitorPrettyPrint::VisitBoolean( Boolean* expr )
{
   out << indent_spaces(indent) << "Create " << expr->toString() << std::endl;
}

void VisitorPrettyPrint::VisitIdentifier( Identifier* expr )
{
   out << indent_spaces(indent) << "Create " << expr->toString() << std::endl;
}

void VisitorPrettyPrint::VisitUnaryOperator( UnaryOperator* expr )
{
   out << indent_spaces(indent) << "Create " << expr->toString() << std::endl;
   ++indent;
   expr->getRHS()->Accept(*this);
   --indent;
}

void VisitorPrettyPrint::VisitBinaryOp( BinaryOp* expr )
{
   out << indent_spaces(indent) << "Create " << expr->toString() << std::endl;
   ++indent;
   expr->getLHS()->Accept(*this);
   expr->getRHS()->Accept(*this);
   --indent;
}

void VisitorPrettyPrint::VisitCompOperator( CompOperator* expr )
{
   out << indent_spaces(indent) << "Create " << expr->toString() << std::endl;
   expr->getLHS()->Accept(*this);
   expr->getRHS()->Accept(*this);
}

void VisitorPrettyPrint::VisitBlock( Block* expr )
{
   out << indent_spaces(indent) << "Create " << expr->toString() << std::endl;
   ++indent;
   for(auto stmt : expr->statements) {
      stmt->Accept( *this );
   }
   --indent;
   out << indent_spaces(indent) << "End " << expr->toString() << std::endl;
}

void VisitorPrettyPrint::VisitExpressionStatement( ExpressionStatement* expr )
{
   out << indent_spaces(indent) << "Create " << expr->toString() << std::endl;
   ++indent;
   expr->getExpression()->Accept(*this);
   --indent;
}

void VisitorPrettyPrint::VisitAssigment( Assignment* expr )
{
   out << indent_spaces(indent) << "Create " << expr->toString() << std::endl;
   ++indent;
   expr->getExpression()->Accept(*this);
   --indent;
}

void VisitorPrettyPrint::VisitMethodCall( MethodCall* expr )
{
   out << indent_spaces(indent) << "Create " << expr->toString() << std::endl;
   ++indent;
   auto args = expr->getArguments();
   if(args->size()) {
      out << indent_spaces(indent) << "Arguments are:\n";
      for(auto arg : *args) {
         arg->Accept(*this);
      }
   }
   --indent;
}

void VisitorPrettyPrint::VisitVariablenDeclaration( VariableDeclaration* expr )
{
   out << indent_spaces(indent) << "Create " << expr->toString() << std::endl;
   if(expr->hasAssignmentExpr()) {
      ++indent;
      expr->getAssignment()->Accept(*this);
      --indent;
   }
}

void VisitorPrettyPrint::VisitWhileLoop( WhileLoop* expr )
{
   out << indent_spaces(indent) << "Create " << expr->toString() << std::endl;
   ++indent;
   expr->getCondition()->Accept(*this);
   out << indent_spaces(indent) << "Create Loop Body" << std::endl;
   expr->getLoopBlock()->Accept(*this);
   auto elseBlock = expr->getElseBlock();
   if(elseBlock) {
      out << indent_spaces(indent) << "Create Else Body" << std::endl;
      elseBlock->Accept(*this);
   }
   --indent;
}

void VisitorPrettyPrint::VisitClassDeclaration( ClassDeclaration* expr )
{
   out << indent_spaces(indent) << "Create " << expr->toString() << std::endl;
   ++indent;
   auto block = expr->getBlock();
   if(block) {
      block->Accept(*this);
   }
   --indent;
}

void VisitorPrettyPrint::VisitArray(Array* expr)
{
   out << indent_spaces(indent) << "Create array " << expr->toString() << std::endl;
   ++indent;
   for( auto e : *expr->getExpressions() ) {
      e->Accept(*this);
   }
   --indent;
}

void VisitorPrettyPrint::VisitArrayAccess(ArrayAccess* expr)
{
   out << indent_spaces(indent) << "Create " << expr->toString() << " to element " << expr->index << std::endl;
   ++indent;
   if( expr->other != nullptr ) {
      expr->other->Accept(*this);
   }
   --indent;
}

void VisitorPrettyPrint::VisitArrayAddElement(ArrayAddElement* expr)
{
   out << indent_spaces(indent) << "Create " << expr->toString() << " of " << expr->getExpression()->toString() << " to array " << expr->ident->getName() << std::endl;
   ++indent;
   if( expr->getExpression() ) {
      expr->getExpression()->Accept(*this);
   }
   --indent;
}

void VisitorPrettyPrint::VisitRange(Range* expr)
{
   out << indent_spaces(indent) << "Create " << expr->toString() << std::endl;
   ++indent;
   if( expr->begin != nullptr ) {
      expr->begin->Accept(*this);
   }
   if( expr->end != nullptr ) {
      expr->end->Accept(*this);
   }
   --indent;
}

}
