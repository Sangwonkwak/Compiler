/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ELSE = 259,
    IF = 260,
    INT = 261,
    RETURN = 262,
    VOID = 263,
    WHILE = 264,
    NUM = 265,
    ID = 266,
    PLUS = 267,
    MINUS = 268,
    TIMES = 269,
    OVER = 270,
    LT = 271,
    LE = 272,
    GT = 273,
    GE = 274,
    EQ = 275,
    NE = 276,
    ASSIGN = 277,
    SEMI = 278,
    COMMA = 279,
    LPAREN = 280,
    RPAREN = 281,
    LBRACE = 282,
    RBRACE = 283,
    LCURLY = 284,
    RCURLY = 285,
    LCOM = 286,
    RCOM = 287,
    ERROR = 288
  };
#endif
/* Tokens.  */
#define ELSE 259
#define IF 260
#define INT 261
#define RETURN 262
#define VOID 263
#define WHILE 264
#define NUM 265
#define ID 266
#define PLUS 267
#define MINUS 268
#define TIMES 269
#define OVER 270
#define LT 271
#define LE 272
#define GT 273
#define GE 274
#define EQ 275
#define NE 276
#define ASSIGN 277
#define SEMI 278
#define COMMA 279
#define LPAREN 280
#define RPAREN 281
#define LBRACE 282
#define RBRACE 283
#define LCURLY 284
#define RCURLY 285
#define LCOM 286
#define RCOM 287
#define ERROR 288

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 23 "cminus.y" /* yacc.c:1909  */

	int val;
	char* str;
	struct treeNode* node;
	int Type;

#line 125 "y.tab.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
