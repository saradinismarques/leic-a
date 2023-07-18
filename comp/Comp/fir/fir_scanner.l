%option c++ prefix="fir_scanner_" outfile="fir_scanner.cpp"
%option stack noyywrap yylineno 8bit
%option debug
%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <cstring>
#include <sstream>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "fir_parser.tab.h"


std::string str;


// don't change this
#define yyerror LexerError
%}

SPACE     ([ \t\r]|\n)
INT       [[:digit:]]+

LETTER  [A-Za-z_]
IDENTIFIER   {LETTER}({LETTER}|[[:digit:]])*

%x X_COMMENT
%x X_STRING X_OCTAL
%%

  /* { yydebug = 1; set_debug(1); } */

"(*"                   yy_push_state(X_COMMENT);
<X_COMMENT>.|\n        ;
<X_COMMENT>"*)"        yy_pop_state();

"!!".*                 ; /* ignore comments */

">="                   return tGE; 
"<="                   return tLE; 
"=="                   return tEQ; 
"!="                   return tNE;

"null"                 return tNULL;

"->"                   return tARROW;
"@"                    return tPROLOGUE;
">>"                   return tEPILOGUE;

"void"                 return tTYPE_VOID;
"int"                  return tTYPE_INTEGER;
"float"                return tTYPE_FLOAT;
"string"               return tTYPE_STRING;

"while"                return tWHILE; 
"if"                   return tIF; 
"else"                 return tELSE; 
"write"                return tWRITE;
"writeln"              return tWRITELN;
"@"                    return tREAD;
"do"                   return tDO;
"finally"              return tFINALLY;
"leave"                return tLEAVE;
"restart"              return tRESTART;
"return"               return tRETURN;
"then"                 return tTHEN;


[A-Za-z][A-Za-z0-9_]*  yylval.s = new std::string(yytext); return tIDENTIFIER;

\'                     yy_push_state(X_STRING); yylval.s = new std::string("");
<X_STRING>\'           yy_pop_state(); return tSTRING;
<X_STRING>\\\'         *yylval.s += yytext + 1;
<X_STRING>.            *yylval.s += yytext;
<X_STRING>\n           yyerror("newline in string");

0                   { yylval.i = strtoul(yytext, NULL, 10); return tINTEGER; }
[1-9][[:digit:]]*   {
						yylval.i = strtoul(yytext, NULL, 10);
						// std::cerr << "\n\n\n\n doin decimal thingy " << yylval.i << "\n\n\n\n" << std::endl;
						return tINTEGER;
					}

0[0-7]+             {
						yylval.i = strtoul(yytext, NULL, 8);
						// std::cerr << "\n\n\n\n doin octal thingy " << yylval.i << "\n\n\n\n" << std::endl;
						return tINTEGER;
					}

([0-9]*\.[0-9]+|[0-9]+\.[0-9]*)([eE][-+]?[0-9]+)?   { yylval.f = strtod(yytext, NULL); return tFLOAT; }
[0-9]+[.][0-9]*[.]+[0-9|.]*                         yyerror("error. Number with wrong");

[-()<>=~+*/%;{}?.,@]       return *yytext;

[ \t\n]+               ; /* ignore whitespace */

.                      std::cerr << "enhhh " << yytext << std::endl; yyerror("Unknown caracter");

{IDENTIFIER}  yylval.s = new std::string(yytext); return tIDENTIFIER;

%%