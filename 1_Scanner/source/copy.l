/****************************************************/
/* File: cminus.l                                     */
/* Lex specification for C-MINUS                       */
/* Compiler Construction: Principles and Practice   */
/* 2015004302 Kwaksangwon                                */
/****************************************************/

%{
#include "globals.h"
#include "util.h"
#include "scan.h"
/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];
%}

digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}+
newline     \n
whitespace  [ \t]+

%%

"if"            {return IF;}
"else"          {return ELSE;}
"while"         {return WHILE;}
"void"          {return VOID;}
"return"        {return RETURN;}
"int"           {return INT;}
"=="            {return EQ;}
"!="            {return NE;}
"<="            {return LE;}
">="            {return GE;}
"="             {return ASSIGN;}
"<"             {return LT;}
">"             {return GT;}
"+"             {return PLUS;}
"-"             {return MINUS;}
"*"             {return TIMES;}
"/"             {return OVER;}
"("             {return LPAREN;}
")"             {return RPAREN;}
"["             {return LBRACE;}
"]"             {return RBRACE;}
"{"             {return LCURLY;}
"}"             {return RCURLY;}
";"             {return SEMI;}
","             {return COMMA;}
{number}        {return NUM;}
{identifier}    {return ID;}
{newline}       {lineno++;}
{whitespace}    {/* skip whitespace */}
"/*"            {
				  char c;
                  while(1){
					  c = input();
					  printf("%c\n",c);
				      if(c == EOF) return ENDFILE;
					  else if(c == '\n') lineno++;
					  else if(c == '*'){
						  c = input();
						  if( c == '/') break;
						  else if(c == EOF) return ENDFILE;
						  else if(c == '\n') lineno++;
						  else unput(yytext[yyleng-1]);
                      }
                      else continue;
				  }
                }
<<EOF>>			{return ENDFILE;}
.               {return ERROR;}

%%

TokenType getToken(void)
{ static int firstTime = TRUE;
  TokenType currentToken;
  if (firstTime)
  { firstTime = FALSE;
    lineno++;
    yyin = source;
    yyout = listing;
  }
  currentToken = yylex();
  strncpy(tokenString,yytext,MAXTOKENLEN);
  if (TraceScan) {
    fprintf(listing,"\t%d: ",lineno);
    printToken(currentToken,tokenString);
  }
  return currentToken;
}

