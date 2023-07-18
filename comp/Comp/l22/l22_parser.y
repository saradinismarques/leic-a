%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <algorithm>
#include <memory>
#include <cstring>
#include <cdk/compiler.h>
#include <cdk/types/types.h>
#include ".auto/all_nodes.h"
#define LINE                         compiler->scanner()->lineno()
#define yylex()                      compiler->scanner()->scan()
#define yyerror(compiler, s)         compiler->scanner()->error(s)
//-- don't change *any* of these --- END!
%}

%parse-param {std::shared_ptr<cdk::compiler> compiler}

%union {
  //--- don't change *any* of these: if you do, you'll break the compiler.
  YYSTYPE() : type(cdk::primitive_type::create(0, cdk::TYPE_VOID)) {}
  ~YYSTYPE() {}
  YYSTYPE(const YYSTYPE &other) { *this = other; }
  YYSTYPE& operator=(const YYSTYPE &other) { type = other.type; return *this; }

  std::shared_ptr<cdk::basic_type> type;        /* expression type */
  std::vector<std::shared_ptr<cdk::basic_type>> types;
  //-- don't change *any* of these --- END!

  int                   i;	/* integer value */
  double                d;    /* double value */
  std::string          *s;	/* symbol name or string literal */
  cdk::basic_node      *node;	/* node pointer */
  cdk::sequence_node   *sequence;
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;
  l22::block_node       *block;
  cdk::string_node      *string; 
};


%token <s> tIDENTIFIER tSTRING
%token <d> tDOUBLE
%token <i> tINTEGER
%token tAGAIN tSTOP tRETURN
%token tDOUBLE_TYPE tSTRING_TYPE tINT_TYPE tVOID
%token tVAR tPUBLIC tUSE tFOREIGN
%token tARROW
%token tWHILE tBEGIN tEND tDO tIF tWRITE tWRITELN tINPUT
%token tNULL
%token tSIZEOF
%token tPRIVATE

%nonassoc tIFX
%nonassoc tELIF
%nonassoc tTHEN
%nonassoc tELSE 

%right '='
%left tOR
%left tAND
%nonassoc tNOT
%left tEQ tNE
%left tGE tLE '>' '<'
%left '+' '-'l
%left '*' '/' '%'
%nonassoc tUNARY
%nonassoc '(' '['

%type <node> declaration instruction program ifcontent blktion
%type <sequence> declarations instructions file arguments exprs blktions
%type <expression> expr
%type <lvalue> lval
%type <block> block
%type <type> type func-type return_type
%type <types> types
%type <s> string

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

file      : program                     { compiler->ast($$ = new cdk::sequence_node(LINE, $1)); }
          | declarations                { compiler->ast($$ = new cdk::sequence_node(LINE, $1)); }
          | declarations program        { compiler->ast($$ = new cdk::sequence_node(LINE, $2, $1)); }
          ;

declarations   : declaration ';'                     { $$ = new cdk::sequence_node(LINE, $1); }
               | declaration                      { $$ = new cdk::sequence_node(LINE, $1); }
               | declarations declaration ';'     { $$ = new cdk::sequence_node(LINE, $2, $1); }
               ; 


declaration    : type tIDENTIFIER '=' expr             {$$ = new l22::variable_declaration_node(LINE, tPRIVATE, $1, *$2, $4); delete $2;} 
               | type tIDENTIFIER                      {$$ = new l22::variable_declaration_node(LINE, tPRIVATE, $1, *$2, nullptr); delete $2;}
               | tVAR tIDENTIFIER '=' expr             {$$ = new l22::variable_declaration_node(LINE, tPRIVATE, nullptr, *$2, $4); delete $2;}
               | tUSE type tIDENTIFIER                       {$$ = new l22::variable_declaration_node(LINE, tUSE, $2, *$3, nullptr); delete $3;}
               | tFOREIGN type tIDENTIFIER                      {$$ = new l22::variable_declaration_node(LINE, tFOREIGN, $2, *$3, nullptr); delete $3;}
               | tPUBLIC tIDENTIFIER                      {$$ = new l22::variable_declaration_node(LINE, tPUBLIC, nullptr, *$2, nullptr); delete $2;}
               | tPUBLIC tIDENTIFIER '=' expr                     {$$ = new l22::variable_declaration_node(LINE, tPUBLIC, nullptr, *$2, $4); delete $2;}
               | tPUBLIC type tIDENTIFIER                      {$$ = new l22::variable_declaration_node(LINE, tPUBLIC, $2, *$3, nullptr); delete $3;}
               | tPUBLIC type tIDENTIFIER '=' expr                     {$$ = new l22::variable_declaration_node(LINE, tPUBLIC, $2, *$3, $5); delete $3;}
               ;

program	: tBEGIN block tEND { $$ = new l22::program_node(LINE, $2); }
	     ;

return_type         : type  {$$ = $1;}
                    | tVOID {$$ = cdk::primitive_type::create(0,cdk::TYPE_VOID);}
                    ;

type : tINT_TYPE                   {$$ = cdk::primitive_type::create(4,cdk::TYPE_INT);}
     | tDOUBLE_TYPE                {$$ = cdk::primitive_type::create(4,cdk::TYPE_DOUBLE);}
     | tSTRING_TYPE                {$$ = cdk::primitive_type::create(4,cdk::TYPE_STRING);}
     | '[' type ']'                {$$ = cdk::reference_type::create(4,$2);}
     | func-type                   {$$ = $1;}
     ;

func-type : return_type '<' types '>'     { $$ = cdk::functional_type::create($3, $1); }
          | return_type '<' '>'           { $$ = cdk::functional_type::create($1); }
          ;

types      : type             { std::vector<std::shared_ptr<cdk::basic_type>> arg_types;
                                arg_types.push_back($1); $$ = arg_types;
                              }
           | types type       { $1.push_back($2); $$ = $1; }
           ; 


block     : '{' declarations instructions '}'    { $$ = new l22::block_node(LINE, $2, $3); }
          | '{' declarations '}'               { $$ = new l22::block_node(LINE, $2, nullptr); }
          | '{' instructions '}'               { $$ = new l22::block_node(LINE, nullptr, $2); }
          | '{' '}'                              { $$ = new l22::block_node(LINE, nullptr, nullptr); }
          ;

instructions   : instruction            { $$ = new cdk::sequence_node(LINE, $1);     }
               | blktions                   { $$ = new cdk::sequence_node(LINE, $1);     }
               | instructions ';' instruction    { $$ = new cdk::sequence_node(LINE, $3, $1); }
               | blktions instruction   { $$ = new cdk::sequence_node(LINE, $2, $1); }
               ;

blktions       : blktion                      { $$ = new cdk::sequence_node(LINE, $1);     }
               | instructions ';' blktion     { $$ = new cdk::sequence_node(LINE, $3, $1);     }
               | blktions blktion             { $$ = new cdk::sequence_node(LINE, $2, $1);     }


blktion        : tIF ifcontent                   { $$ = $2; }
               | block                           { $$ = $1; }
               | tWHILE '(' expr ')' tDO block                        { $$ = new l22::while_node(LINE, $3, $6); }
               ;

instruction    : expr                                                 { $$ = new l22::evaluation_node(LINE, $1); }
               | tWRITE exprs                                         { $$ = new l22::write_node(LINE, $2, false); }
               | tWRITELN exprs                                       { $$ = new l22::write_node(LINE, $2, true); }
               | tAGAIN                                               { $$ = new l22::again_node(LINE); }  
               | tSTOP                                                { $$ = new l22::stop_node(LINE); }  
               | tRETURN expr                                         { $$ = new l22::return_node(LINE, $2); }  
               | tRETURN                                              { $$ = new l22::return_node(LINE, nullptr); }  
               | tWITH expr tCHANGE expr tARROW expr tDOTS expr       { $$ = new l22::change_node(LINE, $2, $4, $6, $8); }
               ;

exprs     : expr                   { $$ = new cdk::sequence_node(LINE, $1); }
          | exprs ',' expr         { $$ = new cdk::sequence_node(LINE, $3, $1); }
          ;

ifcontent : '(' expr ')' tTHEN block %prec tIFX             { $$ = new l22::if_node(LINE, $2, $5); }
          | '(' expr ')' tTHEN block tELIF ifcontent        { $$ = new l22::if_else_node(LINE, $2, $5, $7); }
          | '(' expr ')' tTHEN block tELSE block            { $$ = new l22::if_else_node(LINE, $2, $5, $7); }


arguments      : declaration                    { $$ = new cdk::sequence_node(LINE, $1); } //how to pass argms type and seq nodes with args
               | arguments declaration         { $$ = new cdk::sequence_node(LINE, $2, $1); }
               ; 

string    : tSTRING                {$$ = $1;}
          | string tSTRING         {$$ = new std::string(*$1 + *$2); delete $1; delete $2;}
          ;


expr : tINTEGER                { $$ = new cdk::integer_node(LINE, $1); }
	| string                { $$ = new cdk::string_node(LINE, $1); }
     | tDOUBLE                { $$ = new cdk::double_node(LINE, $1); }
     | tNULL                   { $$ = new l22::nullptr_node(LINE); }
     | '-' expr %prec tUNARY       { $$ = new cdk::neg_node(LINE, $2); }
     | '+' expr %prec tUNARY       { $$ = new l22::identity_node(LINE, $2); }
     | expr '+' expr	         { $$ = new cdk::add_node(LINE, $1, $3); }
     | expr '-' expr	         { $$ = new cdk::sub_node(LINE, $1, $3); }
     | expr '*' expr	         { $$ = new cdk::mul_node(LINE, $1, $3); }
     | expr '/' expr	         { $$ = new cdk::div_node(LINE, $1, $3); }
     | expr '%' expr	         { $$ = new cdk::mod_node(LINE, $1, $3); }
     | expr '<' expr	         { $$ = new cdk::lt_node(LINE, $1, $3); }
     | expr '>' expr	         { $$ = new cdk::gt_node(LINE, $1, $3); }
     | expr tGE expr	         { $$ = new cdk::ge_node(LINE, $1, $3); }
     | expr tLE expr           { $$ = new cdk::le_node(LINE, $1, $3); }
     | expr tNE expr	         { $$ = new cdk::ne_node(LINE, $1, $3); }
     | expr tEQ expr	         { $$ = new cdk::eq_node(LINE, $1, $3); }
     | tNOT expr                 { $$ = new cdk::not_node(LINE, $2); }
     | expr tAND expr          { $$ = new cdk::and_node(LINE, $1, $3); }
     | expr tOR expr          { $$ = new cdk::or_node(LINE, $1, $3); }
     | '(' expr ')'            { $$ = $2; }
     | '[' expr ']'            { $$ = new l22::stack_alloc_node(LINE, $2);}
     | lval                    { $$ = new cdk::rvalue_node(LINE, $1); }  //FIXME
     | lval '=' expr           { $$ = new cdk::assignment_node(LINE, $1, $3); }
     | lval '?'                 { $$ = new l22::address_of_node(LINE, $1); }
     | '(' ')' tARROW return_type ':' block              { $$ = new l22::function_definition_node(LINE, 0, nullptr, nullptr, nullptr, $6); } //change function types
     | '(' arguments ')' tARROW return_type ':' block    { $$ = new l22::function_definition_node(LINE, 0, nullptr, nullptr, $2, $7); }      //change function types
     | expr '(' arguments ')'     { $$ = new l22::function_call_node(LINE, $3, $1, nullptr); }      //change function types
     | expr '(' ')'     { $$ = new l22::function_call_node(LINE, nullptr, $1, nullptr); }      //change function types
     ;



lval : tIDENTIFIER             { $$ = new cdk::variable_node(LINE, $1); }
     | expr '[' expr ']'       { $$ = new l22::index_node(LINE, $1, $3); }
     ;


%%
