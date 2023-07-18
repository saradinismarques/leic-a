%option c++ prefix="l22_scanner_" outfile="l22_scanner.cpp"
%option stack noyywrap yylineno 8bit debug
%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <sstream>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "l22_parser.tab.h"

static bool restart_functions = false;

static int lLevelsToPop = 0, lIndex = 0;
static std::vector<int> indents;   // known indentation levels
static std::vector<int> deferred;  // deferred tokens


static std::ostringstream strlit;

inline bool valid(int indent) {
    lLevelsToPop = lIndex = -1;
    bool indentOk = false;
    for (size_t ix = 0; ix < indents.size(); ix++) {
        if (indent == indents[ix]) {
            lLevelsToPop = indents.size() - ix - 1;
            lIndex = ix;
            indentOk = true;
        }
    }
    return indentOk;
}

// don't change this
#define yyerror LexerError
%}


SPACE     [ \t]
INT       [[:digit:]]+

LETTER  [A-Za-z]
IDENTIFIER   {LETTER}({LETTER}|"-"|[:digit:])*

%x X_STRING X_COMMENT X_BACKSLASH
%x X_NEWLINE

%%
  {
        //std::cerr << "################" << std::endl;
        set_debug(true);
        if (restart_functions) {
          std::cerr << "POPPING FUNCTIONS (" << indents.size() << ")" << std::endl;
          indents.pop_back();
          if (indents.size() == 0) restart_functions = false;
          return '}';
        }
        else {
          if (lLevelsToPop > 0) {
            std::cerr << "POPPING LEVELS (" << lLevelsToPop << ")" << std::endl;
            lLevelsToPop--;
            indents.pop_back();
            return '}';
          }
        }
        yydebug=1;
        
  }


  /* ====================================================================== */
  /* ====[                      2.2 - INDENTATION                     ]==== */
  /* ====================================================================== */

\n                   yy_push_state(X_NEWLINE); /* delay decision */

<X_NEWLINE>^{SPACE}+  {
    yy_pop_state();

    if (valid(yyleng)) {
          std::cerr << "CASE 1" << std::endl;
          if (lLevelsToPop == 0) {
            // at the same level: ignore index
            // do not return tINDENT;
            std::cerr << "CASE 1a (levels to pop = 0)" << std::endl;
            return ';';
          }
          else {
            // nothing is done here: the scanner will ignore the input while levels > 0
            // and send block-end tokens to the parser
            std::cerr << "CASE 1b (levels to pop = " << lLevelsToPop << ")" << std::endl;
            // at least one has to be returned here...
            if (lLevelsToPop > 0) {
              std::cerr << "POPPING LEVELS (" << lLevelsToPop << ")" << std::endl;
              lLevelsToPop--;
              indents.pop_back();
              return '}';
            }
          }
    }
    else if (indents.size() == 0) {
          // first block to open
          std::cerr << "CASE 2" << std::endl;
          indents.push_back(yyleng);
          return '{';
    }
    else if (yyleng > indents.back()) {
          // this is like the one before, but not the first
          std::cerr << "CASE 3" << std::endl;
          indents.push_back(yyleng);
          return '{';
    }
    else {
          // something wrong: bad indent: communicate to user
          std::cerr << "CASE 4" << std::endl;
          std::cerr << "bad indent at line " << yylineno;
    }

}
<X_NEWLINE>\n      ; // ignore successive newline chars
<X_NEWLINE>.       {
  // in this case, we have a new function starting
  // first, return whatever we are seeing to the input
  yyless(0);
  yy_pop_state();
  if (indents.size() > 0) {
    std::cerr << "should restart " << indents.size() << " functions at " << yylineno << std::endl;
    indents.pop_back();
    if (indents.size() > 0) { restart_functions = true; }
    return '}';
  }
  else return ';'; // top-level declarations
}

  /* ====================================================================== */
  /* ====[                        OPERADORES                          ]==== */
  /* ====================================================================== */


">="                   return tGE; 
"<="                   return tLE; 
"=="                   return tEQ; 
"!="                   return tNE; 
"not"                  return tNOT;
"and"                  return tAND;
"or"                   return tOR;
"->"                   return tARROW;



  /* ====================================================================== */
  /* ====[                         DECLARACOES                        ]==== */
  /* ====================================================================== */

"foreign"              return tFOREIGN; 
"use"				           return tUSE;
"public"			         return tPUBLIC;


  /* ====================================================================== */
  /* ====[                        INSTRUCOES                          ]==== */
  /* ====================================================================== */

"if"                   return tIF; 
"then:"                return tTHEN;
"elif"                 return tELIF;
"else:"                return tELSE; 
"while"                return tWHILE; 
"do:"				           return tDO;
"stop"                 return tSTOP;
"again"                return tAGAIN;  
"write"                return tWRITE; 
"writeln"              return tWRITELN; 
"return"               return tRETURN; 



  /* ====================================================================== */
  /* ====[                        EXPRESSOES                          ]==== */
  /* ====================================================================== */

"input"               return tINPUT;
"null"                return tNULL;



  /* ====================================================================== */
  /* ====[                         KEYWORDS                           ]==== */
  /* ====================================================================== */

"begin"               return tBEGIN;
"end"                 return tEND;
"sizeof"              return tSIZEOF;
"var"                 return tVAR;
"int"                 return tINT_TYPE;
"double"              return tDOUBLE_TYPE;
"text"                return tSTRING_TYPE;
"void"                return tVOID;

  /* ====================================================================== */
  /* ====[                         TP                                 ]==== */
  /* ====================================================================== */

"with"                return tWITH;
"change"              return tCHANGE;
".."                  return tDOTS;

  /* ====================================================================== */
  /* ====[                     IDENTIFICADORES                        ]==== */
  /* ====================================================================== */

[A-Za-z][A-Za-z0-9_]*  yylval.s = new std::string(yytext); return tIDENTIFIER;


  /* ====================================================================== */
  /* ====[                        COMENTARIOS                         ]==== */
  /* ====================================================================== */


";".*\n                 ; /* ignore comments */


"(*"                     yy_push_state(X_COMMENT);
<X_COMMENT>"(*"          yy_push_state(X_COMMENT);
<X_COMMENT>"*)"          yy_pop_state();
<X_COMMENT>.|\n          ; /* ignore content */

  /* ====================================================================== */
  /* ====[                         INTEIROS                           ]==== */
  /* ====================================================================== */
0[0-6]+	                      { 
                                  yylval.i = strtol(yytext, nullptr, 7); 
                                  if (errno == ERANGE) yyerror("ERANGE");
                                  return tINTEGER;
                              }

[0-9]+                        {  
                                  yylval.i = strtol(yytext, nullptr, 10);
                                  if (errno == ERANGE) yyerror("ERANGE");
                                  return tINTEGER;
                              }
                              

  /* ====================================================================== */
  /* ====[                           REAIS                            ]==== */
  /* ====================================================================== */

[0-9]+"."[0-9]+  { yylval.d = std::stod(yytext, NULL); 
                         return tDOUBLE; 
                 }

[0-9]+"."        {  yylval.d = std::stod(yytext, NULL); 
                         return tDOUBLE; 
                 }

"."[0-9]+        {  yylval.d = std::stod(yytext, NULL); 
                         return tDOUBLE; 
                 }

[0-9]+("."[0-9]+)?[Ee][-+]?[0-9]+  { yylval.d = std::stod(yytext, NULL); 
                                           return tDOUBLE; 
                                   }

  /* ====================================================================== */
  /* ====[                       	STRINGS                             ]==== */
  /* ====================================================================== */

"\""                          yy_push_state(X_STRING);
<X_STRING>"\\"                yy_push_state(X_BACKSLASH);
<X_STRING>"\""                {
                                yylval.s = new std::string(strlit.str());
                                strlit.str("");
                                yy_pop_state();
                                return tSTRING;
                              }
<X_STRING>.                   strlit << *yytext;
<X_STRING>\n                  yyerror("Error in string");

<X_BACKSLASH>n              { strlit << '\n'; yy_pop_state(); }
<X_BACKSLASH>r              { strlit << '\r'; yy_pop_state(); }
<X_BACKSLASH>t              { strlit << '\t'; yy_pop_state(); }
<X_BACKSLASH>"\""           { strlit << '"'; yy_pop_state(); }
<X_BACKSLASH>"\\"           { strlit << "\\"; yy_pop_state(); }
<X_BACKSLASH>[0-6]{1,3} {
                                 strlit << (char)(unsigned char)std::stoul(yytext, NULL, 7);
                                 yy_pop_state();
                               }

<X_BACKSLASH>0[7-9][A-Za-z0-9]* {
                                 strlit << '\0';
                                 yy_pop_state();
                               }
<X_BACKSLASH>.			         { strlit << *yytext; yy_pop_state(); }



  /* ====================================================================== */
  /* ====[                        OUTROS CHARS                        ]==== */
  /* ====================================================================== */

[-()<>=+*/%;{}:[\],@?]       return *yytext;

[ \t]+               ; /* ignore whitespace */

<<EOF>>                {
  std::cout << "AT <<EOF>> OPEN INDENTS ARE " << indents.size() << std::endl;
  if (indents.size() > 0) {
    indents.pop_back();
    return '}';
  }
  return 0;  // nothing left to do: tell parser to proceed
}

.                      yyerror(yytext);    /* all other chars: error! */


%%
