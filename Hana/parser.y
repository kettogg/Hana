%code requires {

# define YYLTYPE_IS_DECLARED 1

}

%{
    #include "Ast/AstNode.h"
    #include "CodeGen/FunctionDeclaration.h"
    #include "CodeGen/ClassDeclaration.h"
    #include "CodeGen/Return.h"
    #include "CodeGen/FunctionDeclaration.h"
    #include "CodeGen/ClassDeclaration.h"
    #include "CodeGen/Conditional.h"
    #include "CodeGen/UnaryOperator.h"
    #include "CodeGen/BinaryOperator.h"
    #include "CodeGen/CompareOperator.h"
    #include "CodeGen/Assignment.h"
    #include "CodeGen/MethodCall.h"
    #include "CodeGen/Declaration.h"
    #include "CodeGen/WhileLoop.h"
    #include "CodeGen/Array.h"
    #include "CodeGen/Range.h"

    #include <stdio.h>
    #include <stack>
    hana::Block *programBlock; /* Root node of our final AST */

    extern int yylex();
    int yyerror(char const * s );
    #define YYERROR_VERBOSE
    #define YYDEBUG 1

    extern std::stack<std::string> fileNames;

    # define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
          (Current).file_name = fileNames.top();            \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
          (Current).file_name = fileNames.top();            \
        }                                                               \
    while (0)

%}

/* Represents the many different ways we can access our data */
%union {
    hana::Node *node;
    hana::Block *block;
    hana::Expression *expr;
    hana::Statement *stmt;
    hana::Identifier *ident;
    hana::VariableDeclaration *var_decl;
    std::vector<hana::VariableDeclaration*> *varvec;
    std::vector<hana::Expression*> *exprvec;
    std::string *string;
    long long integer;
    double number;
    int boolean;
    int token;
}

/* Define our terminal symbols (tokens). This should
   match our tokens.l lex file. We also define the node type
   they represent.
 */
%token <string> TIDENTIFIER TSTR
%token <integer> TINTEGER
%token <number> TDOUBLE
%token <boolean> TBOOL
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE
%token <token> TLTLT "<<"
%token <token> TRANGE
%token <token> TPLUS TMINUS TMUL TDIV
%token <token> TNOT TAND TOR
%token <token> TIF TELSE TWHILE
%token <token> TDEF TRETURN TVAR
%token <token> INDENT UNINDENT 

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (Identifier*). It makes the compiler happy.
 */
%type <ident> ident
%type <expr> literals expr boolean_expr binop_expr unaryop_expr array_expr array_access  range_expr
%type <varvec> func_decl_args
%type <exprvec> call_args array_elemets_expr 
%type <block> program stmts block
%type <stmt> stmt var_decl func_decl conditional return while class_decl array_add_element
%type <token> comparison 

/* Operator precedence for mathematical operators */
%left TPLUS TMINUS
%left TMUL TDIV
%left TAND TNOT

%start program
%debug 
%verbose 
%locations /* track locations: @n of component N; @$ of entire range */
/*
%define parse.lac full
%define lr.type ielr
*/

%%

program : %empty { programBlock = new hana::Block(); }
        | stmts { programBlock = $1; }
        ;

stmts : stmt { $$ = new hana::Block(); $$->statements.push_back($<stmt>1); }
      | stmts stmt { $1->statements.push_back($<stmt>2); }
      ;

stmt : var_decl
     | func_decl
     | class_decl
     | conditional 
     | return
     | while
     | array_add_element
     | expr { $$ = new hana::ExpressionStatement($1); }
     ;

block : INDENT stmts UNINDENT { $$ = $2; }
      | INDENT UNINDENT { $$ = new hana::Block(); }
      ;

conditional : TIF expr block TELSE block {$$ = new hana::Conditional($2,$3,$5);}
            | TIF expr block {$$ = new hana::Conditional($2,$3);}
            ; 

while : TWHILE expr block TELSE block {$$ = new hana::WhileLoop($2,$3,$5);}
      | TWHILE expr block {$$ = new hana::WhileLoop($2,$3);}
      ; 

var_decl : ident ident { $$ = new hana::VariableDeclaration($1, $2, @$); }
         | ident ident '=' expr { $$ = new hana::VariableDeclaration($1, $2, $4, @$); }
         | TVAR ident { $$ = new hana::VariableDeclaration($2, @$); }
         | TVAR ident '=' expr { $$ = new hana::VariableDeclaration($2, $4, @$); }
         ;

func_decl : TDEF ident '(' func_decl_args ')' ':' ident block { $$ = new hana::FunctionDeclaration($7, $2, $4, $8, @$); }
          | TDEF ident '(' func_decl_args ')' block { $$ = new hana::FunctionDeclaration($2, $4, $6, @$); }
          ;

func_decl_args : %empty  { $$ = new hana::VariableList(); }
          | var_decl { $$ = new hana::VariableList(); $$->push_back($<var_decl>1); }
          | func_decl_args ',' var_decl { $1->push_back($<var_decl>3); }
          ;

class_decl: TDEF ident block {$$ = new hana::ClassDeclaration($2, $3); }
          ;

return : TRETURN { $$ = new hana::Return(@$); }
       | TRETURN expr { $$ = new hana::Return(@$, $2); }
       ;

expr : ident '=' expr { $$ = new hana::Assignment($<ident>1, $3, @$); }
     | ident '(' call_args ')' { $$ = new hana::MethodCall($1, $3, @$);  }
     | ident { $<ident>$ = $1; }
     | literals
     | boolean_expr 
     | binop_expr
     | unaryop_expr
     | '(' expr ')' { $$ = $2; }
     | range_expr
     | array_expr
     | array_access
     ;

ident : TIDENTIFIER { $$ = new hana::Identifier(*$1, @1); delete $1; }
      | TIDENTIFIER '.' TIDENTIFIER { $$ = new hana::Identifier(*$1,*$3, @$); delete $1; delete $3;}
      ;

literals : TINTEGER { $$ = new hana::Integer($1); }
         | TDOUBLE { $$ = new hana::Double($1); }
         | TSTR { $$ = new hana::String(*$1); delete $1; }
         | TBOOL { $$ = new hana::Boolean($1); }
         ;

/* have to write it explicit to have the right operator precedence */
binop_expr : expr TAND expr { $$ = new hana::BinaryOp($1, $2, $3, @$); }
           | expr TOR expr { $$ = new hana::BinaryOp($1, $2, $3, @$); }
           | expr TPLUS expr { $$ = new hana::BinaryOp($1, $2, $3, @$); }
           | expr TMINUS expr { $$ = new hana::BinaryOp($1, $2, $3, @$); }
           | expr TMUL expr { $$ = new hana::BinaryOp($1, $2, $3, @$); }
           | expr TDIV expr { $$ = new hana::BinaryOp($1, $2, $3, @$); }
           ;

unaryop_expr : TNOT expr { $$ = new hana::UnaryOperator($1, $2); }
             ;

boolean_expr : expr comparison expr { $$ = new hana::CompOperator($1, $2, $3); }
             ;

call_args : %empty  { $$ = new hana::ExpressionList(); }
          | expr { $$ = new hana::ExpressionList(); $$->push_back($1); }
          | call_args ',' expr  { $1->push_back($3); }
          ;
 
comparison : TCEQ | TCNE | TCLT | TCLE | TCGT | TCGE
           ;
          
array_elemets_expr: %empty {$$ = new hana::ExpressionList(); }
                 | expr {$$ = new hana::ExpressionList(); $$->push_back($1);}
                 | array_elemets_expr ',' expr {$$->push_back($3); }
                 ; 
                 
array_expr : '[' array_elemets_expr ']' {$$ = new hana::Array($2, @$);}
          ;
          
array_add_element: ident "<<" expr { $$ = new hana::ArrayAddElement($1, $3, @$); }
                ;
                
array_access: ident '[' TINTEGER ']' { $$ = new hana::ArrayAccess($1, $3, @$); }
           | array_access '[' TINTEGER ']' { $$ = new hana::ArrayAccess($1, $3, @$); }
           ;

range_expr : '[' expr TRANGE expr ']' {$$ = new hana::Range($2, $4, @$);}
           ;

%%
