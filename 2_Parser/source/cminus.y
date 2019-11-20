/****************************************************/
/* File: tiny.y                                     */
/* The TINY Yacc/Bison specification file           */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static char * savedName; /* for use in various case */
static char * savedType;
static int savedLineNo;  /* ditto */
static TreeNode * savedTree; /* stores syntax tree for later return */
static int yylex(void); // added 11/2/11 to ensure no conflict with lex

%}
/* YYSTYPE redefine */
%union {
	int val;
	char* str;
	struct treeNode* node;
	int Type;
};

/* For dangling else problem */
%nonassoc "then"
%nonassoc ELSE

%type <Type> type_specifier
%type <node> program declaration_list declaration var_declaration fun_declaration params param_list param compound_stmt local_declarations statement_list statement expression_stmt selection_stmt iteration_stmt return_stmt expression var simple_expression relop additive_expression addop term mulop factor call args arg_list
%token <str> IF ELSE INT RETURN VOID WHILE 
%token <val> NUM 
%token <str> ID PLUS MINUS TIMES OVER LT LE GT GE EQ NE ASSIGN SEMI COMMA LPAREN RPAREN LBRACE RBRACE LCURLY RCURLY LCOM RCOM
%token <str> ERROR 

%% /* Grammar for CMINUS */

program				: declaration_list
						{ savedTree = $1; }
					;

declaration_list	: declaration_list declaration
						{ TreeNode* t = $1;
							if(t != NULL){
							while(t->sibling != NULL)
								t = t ->sibling;
							t->sibling = $2;
							$$ = $1; }
							else $$ = $2;
						}
					| declaration { $$ = $1; }
					;

declaration			: var_declaration { $$ = $1; }
					| fun_declaration { $$ = $1; }
					;

var_declaration		: type_specifier ID SEMI
					{ $$ = newDecNode(VarK);
						$$->attr.name = $2;
						$$->type = $1;
					}
					| type_specifier ID LBRACE NUM RBRACE SEMI
					{ TreeNode* temp = newExpNode(ConstK);
						$$ = newDecNode(VararrK);
						$$->attr.name = $2;
						$$->type = $1;
						$$->child[0] = temp;
						temp->attr.val = $4;
					}
					;

type_specifier		: INT { $$ = Int; }
					| VOID { $$ = Void; }
					;

fun_declaration		: type_specifier ID LPAREN params RPAREN compound_stmt
						{ $$ = newDecNode(FuncK);
							$$->attr.name = $2;
							$$->type = $1;
							$$->child[0] = $4;
							$$->child[1] = $6;
						}
					;

params				: param_list { $$ = $1; }
					| VOID
						{ $$ = newParNode(SinpK);
							$$->type = Void;
							$$->attr.name = copyString("null");
						}
					;

param_list			: param_list COMMA param
						{ TreeNode* t = $1;
							if( t != NULL){
							while(t->sibling != NULL)
								t = t->sibling;
							t->sibling = $3;
							$$ = $1; }
							else $$ = $3;
						}
					| param { $$ = $1; }
					;

param				: type_specifier ID
						{ $$ = newParNode(SinpK);
							$$->attr.name = $2;
							$$->type = $1;
						}
					| type_specifier ID LBRACE RBRACE
						{ $$ = newParNode(ArrpK);
							$$->attr.name = $2;
							$$->type = $1;
						}
					;

compound_stmt		: LCURLY local_declarations statement_list RCURLY
						{ $$ = newStmtNode(CmstmtK);
							$$->child[0] = $2;
							$$->child[1] = $3;
						}
					;

local_declarations	: local_declarations var_declaration
						{ TreeNode* t = $1;
							if( t != NULL){
							while(t->sibling != NULL)
								t = t->sibling;
							t->sibling = $2;
							$$ = $1; }
							else $$ = $2;
						}
					| { $$ = NULL; }
					;

statement_list		: statement_list statement
						{ TreeNode* t = $1;
							if( t != NULL){
							while(t->sibling != NULL)
								t = t->sibling;
							t->sibling = $2;
							$$ = $1; }
							else $$ = $2;
						}
					| { $$ = NULL; }
					;

statement			: expression_stmt { $$ = $1; }
					| compound_stmt { $$ = $1; }
					| selection_stmt { $$ = $1; }
					| iteration_stmt { $$ = $1; }
					| return_stmt { $$ = $1; }
					;

expression_stmt		: expression SEMI { $$ = $1; }
					| SEMI { $$ = NULL; }
					;


selection_stmt		: IF LPAREN expression RPAREN statement		%prec "then"
						{ $$ = newStmtNode(IfK);
							$$->child[0] = $3;
							$$->child[1] = $5;
						}
					| IF LPAREN expression RPAREN statement ELSE statement
						{ $$ = newStmtNode(IfelseK);
							$$->child[0] = $3;
							$$->child[1] = $5;
							$$->child[2] = $7;
						}
					;

iteration_stmt		: WHILE LPAREN expression RPAREN statement
						{ $$ = newStmtNode(WhileK);
							$$->child[0] = $3;
							$$->child[1] = $5;
						}
					;

return_stmt			: RETURN SEMI
						{ $$ = newStmtNode(ReturnK); }
					| RETURN expression SEMI
						{ $$ = newStmtNode(ReturnK);
							$$->child[0] = $2;
						}
					;
expression			: var ASSIGN expression
						{ $$ = newExpNode(AssignK);
							$$->child[0] = $1;
							$$->child[1] = $3;
						}
					| simple_expression { $$ = $1; }
					;

var					: ID
						{ $$ = newExpNode(IdK);
							$$->attr.name = $1;
						}
					| ID LBRACE expression RBRACE
		  				{ $$ = newExpNode(IdK);
							$$->attr.name = $1;
							$$->child[0] = $3;
						}
					;

simple_expression	: additive_expression relop additive_expression
						{ $$ = $2;
							$$->child[0] = $1;
							$$->child[1] = $3;
						}
					| additive_expression { $$ = $1; }
					;

relop				: LT { $$ = newExpNode(OpK); $$->attr.op = LT; }
					| LE { $$ = newExpNode(OpK); $$->attr.op = LE; }
					| GT { $$ = newExpNode(OpK); $$->attr.op = GT; }
					| GE { $$ = newExpNode(OpK); $$->attr.op = GE; }
					| EQ { $$ = newExpNode(OpK); $$->attr.op = EQ; }
					| NE { $$ = newExpNode(OpK); $$->attr.op = NE; }
					;

additive_expression	: additive_expression addop term
						{ $$ = $2;
							$$->child[0] = $1;
							$$->child[1] = $3;
						}
					| term { $$ = $1; }
					;

addop				: PLUS { $$ = newExpNode(OpK); $$->attr.op = PLUS; }
					| MINUS { $$ = newExpNode(OpK); $$->attr.op = MINUS; }
					;

term				: term mulop factor
						{ $$ = $2;
							$$->child[0] = $1;
							$$->child[1] = $3;
						}
					| factor { $$ = $1; }
					;

mulop				: TIMES { $$ = newExpNode(OpK); $$->attr.op = TIMES; }
					| OVER { $$ = newExpNode(OpK); $$->attr.op = OVER; }
					;

factor				: LPAREN expression RPAREN { $$ = $2; }
					| var { $$ = $1; }
					| call { $$ = $1; }
					| NUM { $$ = newExpNode(ConstK); $$->attr.val = $1; }
					;

call				: ID LPAREN args RPAREN
						{ $$ = newExpNode(CallK);
							$$->attr.name = $1;
							$$->child[0] = $3;
						}
					;

args				: arg_list { $$ = $1; }
					| { $$ = NULL; }
					;

arg_list			: arg_list COMMA expression
						{ TreeNode* t = $1;
							if( t != NULL){
							while(t->sibling != NULL)
								t = t->sibling;
							t->sibling = $3;
							$$ = $1; }
							else $$ = $3;
						}
					| expression { $$ = $1; }
					;
		
%%

int yyerror(char * message)
{ fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
  fprintf(listing,"Current token: ");
  printToken(yychar,tokenString);
  Error = TRUE;
  return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
 */
static int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}

