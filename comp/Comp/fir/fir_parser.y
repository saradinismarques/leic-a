%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <algorithm>
#include <memory>
#include <cstring>
#include <cdk/compiler.h>
#include <cdk/types/types.h>
#include "ast/all.h"
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
  //-- don't change *any* of these --- END!

  int                   i;	/* integer value */
  float			f;	/* float value */
  std::string          *s;	/* symbol name or string literal */
  cdk::basic_node      *node;	/* node pointer */
  cdk::sequence_node   *sequence;
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;
  fir::block_node      *block;
  fir::body_node       *body;
};

%token <i> tINTEGER
%token <f> tFLOAT
%token <s> tIDENTIFIER tSTRING
%token <expression> tNULL

%token tPROLOGUE tEPILOGUE tRETURN tARROW
%token tWHILE tIF tWRITE tWRITELN tREAD tDO tFINALLY tLEAVE tRESTART tTHEN
%token tTYPE_VOID tTYPE_INTEGER tTYPE_FLOAT tTYPE_STRING
%token tSIZEOF

%token tMUL

%nonassoc tIFX
%nonassoc tELSE

%right '='
%left tGE tLE tEQ tNE '>' '<'
%left '+' '-'
%left '*' '/' '%' '~'
%nonassoc tUNARY

//FIXME HEEEELLLLPPP WHAT TO DO ---> literal?
%type <node> stmt
%type <sequence> file

%type <sequence> exprs instructions
%type <lvalue> lval
%type <expression> expr literal
%type <block> block
%type <body> body
%type <expression> default_ret_val
%type <node> declaration variable function_decl function_def
%type <sequence> declarations variables
%type <s> string

%type <type> data_type


%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

file: /*empty*/ 				{ compiler->ast($$ = new cdk::sequence_node(LINE)); }
	| declarations 				{ compiler->ast($$ = $1); }
	;

declarations: declaration 			{ $$ = new cdk::sequence_node(LINE, $1); }
	| declarations declaration 		{ $$ = new cdk::sequence_node(LINE, $2, $1); }
	;

declaration: variable ';' 			{ $$ = $1; }
	| function_decl				{ $$ = $1; }
	| function_def				{ $$ = $1; }
	;

variable: data_type tIDENTIFIER 			{ $$ = new fir::var_decl_node(LINE, 0, $1, *$2, nullptr); }
	| data_type tIDENTIFIER	'=' expr		{ $$ = new fir::var_decl_node(LINE, 0, $1, *$2, $4); }
	| data_type '*' tIDENTIFIER 		{ $$ = new fir::var_decl_node(LINE, 1, $1, *$3, nullptr); }
	| data_type '?' tIDENTIFIER 		{ $$ = new fir::var_decl_node(LINE, 2, $1, *$3, nullptr); }
	| data_type '*' tIDENTIFIER '=' expr 	{ $$ = new fir::var_decl_node(LINE, 1, $1, *$3, $5); }
	| data_type '?' tIDENTIFIER '=' expr 	{ $$ = new fir::var_decl_node(LINE, 2, $1, *$3, $5); }
	;


function_def: data_type tIDENTIFIER '(' variables ')' body	{ $$ = new fir::func_def_node(LINE, 0, *$2, $4, $1, $6, 0); }
	| data_type '*' tIDENTIFIER '(' variables ')' body 	{ $$ = new fir::func_def_node(LINE, 1, *$3, $5, $1, $7, 0); }
	| data_type tIDENTIFIER '(' variables ')' default_ret_val body	{ $$ = new fir::func_def_node(LINE, 0, *$2, $4, $1, $7, $6); }
	| data_type '*' tIDENTIFIER '(' variables ')' default_ret_val body 	{ $$ = new fir::func_def_node(LINE, 1, *$3, $5, $1, $8, $7); }
	| data_type tIDENTIFIER '('')' body	{ $$ = new fir::func_def_node(LINE, 0, *$2, nullptr, $1, $5, 0); }
	| data_type '*' tIDENTIFIER '('')' body 	{ $$ = new fir::func_def_node(LINE, 1, *$3, nullptr, $1, $6, 0); }
	| data_type tIDENTIFIER '('')' default_ret_val body	{ $$ = new fir::func_def_node(LINE, 0, *$2, nullptr, $1, $6, $5); }
	| data_type '*' tIDENTIFIER '('')' default_ret_val body 	{ $$ = new fir::func_def_node(LINE, 1, *$3, nullptr, $1, $7, $6); }
	;

function_decl: data_type tIDENTIFIER '('')'  	{ $$ = new fir::func_decl_node(LINE, 0, *$2, nullptr, $1); }
	| data_type '*' tIDENTIFIER '('')' 	{ $$ = new fir::func_decl_node(LINE, 1, *$3, nullptr, $1); }
	| data_type '?' tIDENTIFIER '('')' 	{ $$ = new fir::func_decl_node(LINE, 2, *$3, nullptr, $1); }
	| data_type tIDENTIFIER '(' variables ')'  	{ $$ = new fir::func_decl_node(LINE, 0, *$2, $4, $1); }
	| data_type '*' tIDENTIFIER '(' variables ')' 	{ $$ = new fir::func_decl_node(LINE, 1, *$3, $5, $1); }
	| data_type '?' tIDENTIFIER '(' variables ')' 	{ $$ = new fir::func_decl_node(LINE, 2, *$3, $5, $1); }
	;

default_ret_val: tARROW literal 				{ $$ = $2; }
	;

body: tPROLOGUE block block tEPILOGUE block 		{ $$ = new fir::body_node(LINE, $2, $3, $5); }
	| tPROLOGUE block block 			{ $$ = new fir::body_node(LINE, $2, $3, nullptr); }
	| block tEPILOGUE block 			{ $$ = new fir::body_node(LINE, nullptr, $1, $3); }
	| tPROLOGUE block tEPILOGUE block		{ $$ = new fir::body_node(LINE, $2, nullptr, $4); }
	| tPROLOGUE block				{ $$ = new fir::body_node(LINE, $2, nullptr, nullptr); }
	| tEPILOGUE block				{ $$ = new fir::body_node(LINE, nullptr, nullptr, $2); }
	| block 					{ $$ = new fir::body_node(LINE, nullptr, $1, nullptr); }
	;

literal: tINTEGER				{ $$ = new cdk::integer_node(LINE, $1); }
	| tFLOAT				{ $$ = new cdk::double_node(LINE, $1); }
	;

lval: tIDENTIFIER             			{ $$ = new cdk::variable_node(LINE, $1); }
	| lval '[' expr ']'			{ $$ = new fir::index_node(LINE, new cdk::rvalue_node(LINE, $1), $3); }
	| '(' expr ')' '[' expr ']'		{ $$ = new fir::index_node(LINE, $2, $5); }
	| tIDENTIFIER '('')' '[' expr ']'	{ $$ = new fir::index_node(LINE, new fir::func_call_node(LINE, *$1, nullptr), $5); }
	| tIDENTIFIER '(' exprs ')' '[' expr ']' { $$ = new fir::index_node(LINE, new fir::func_call_node(LINE, *$1, $3), $6); }
	;

block: '{' '}'					{ $$ = new fir::block_node(LINE, nullptr, nullptr); }
	| '{' instructions '}' 			{ $$ = new fir::block_node(LINE, nullptr, $2); }
	| '{' declarations '}' 			{ $$ = new fir::block_node(LINE, $2, nullptr); }
	| '{' declarations instructions '}' 	{ $$ = new fir::block_node(LINE, $2, $3); }
	;

exprs: expr					{ $$ = new cdk::sequence_node(LINE, $1); }
	| exprs ',' expr			{ $$ = new cdk::sequence_node(LINE, $3, $1); }
	;

expr: tINTEGER					{ $$ = new cdk::integer_node(LINE, $1); }
	| string				{ $$ = new cdk::string_node(LINE, $1); }
	| tNULL					{ $$ = new fir::nullptr_node(LINE); }
	| '+' expr %prec tUNARY			{ $$ = new fir::pos_node(LINE, $2); }
	| '-' expr %prec tUNARY			{ $$ = new cdk::neg_node(LINE, $2); }
	| '~' expr %prec tUNARY			{ $$ = new cdk::not_node(LINE, $2); }
	| expr '+' expr				{ $$ = new cdk::add_node(LINE, $1, $3); }
	| expr '-' expr				{ $$ = new cdk::sub_node(LINE, $1, $3); }
	| expr '*' expr				{ $$ = new cdk::mul_node(LINE, $1, $3); }
	| expr '/' expr				{ $$ = new cdk::div_node(LINE, $1, $3); }
	| expr '%' expr				{ $$ = new cdk::mod_node(LINE, $1, $3); }
	| expr '<' expr				{ $$ = new cdk::lt_node(LINE, $1, $3); }
	| expr '>' expr				{ $$ = new cdk::gt_node(LINE, $1, $3); }
	| expr tGE expr				{ $$ = new cdk::ge_node(LINE, $1, $3); }
	| expr tLE expr				{ $$ = new cdk::le_node(LINE, $1, $3); }
	| expr tNE expr				{ $$ = new cdk::ne_node(LINE, $1, $3); }
	| expr tEQ expr				{ $$ = new cdk::eq_node(LINE, $1, $3); }
	| '(' expr ')'				{ $$ = $2; }
	| lval					{ $$ = new cdk::rvalue_node(LINE, $1); } //FIXME
	| lval '=' expr				{ $$ = new cdk::assignment_node(LINE, $1, $3); }
	| lval '?'				{ $$ = new fir::addressof_node(LINE, $1); }
	| '[' expr ']'				{ $$ = new fir::alloc_node(LINE, $2); }
	| tIDENTIFIER '('')'			{ $$ = new fir::func_call_node(LINE, *$1, nullptr); delete $1; }
	| tIDENTIFIER '(' exprs ')'		{ $$ = new fir::func_call_node(LINE, *$1, $3); delete $1; }
	| tREAD					{ $$ = new fir::read_node(LINE); }
	| tSIZEOF '(' expr ')'				{ $$ = new fir::sizeof_node(LINE, $3); }
	;

variables: variable				{ $$ = new cdk::sequence_node(LINE, $1); }
	| variables ',' variable		{ $$ = new cdk::sequence_node(LINE, $3, $1); }
	;

data_type: tTYPE_VOID				{ $$ = cdk::primitive_type::create(0, cdk::TYPE_VOID); }
	| tTYPE_INTEGER				{ $$ = cdk::primitive_type::create(4, cdk::TYPE_INT); }
	| tTYPE_FLOAT				{ $$ = cdk::primitive_type::create(8, cdk::TYPE_DOUBLE); }
	| tTYPE_STRING				{ $$ = cdk::primitive_type::create(4, cdk::TYPE_STRING); }
	| '<' data_type '>'			{ $$ = cdk::reference_type::create(4, $2); }
	;

instructions: stmt				{ $$ = new cdk::sequence_node(LINE, $1); }
	| instructions stmt			{ $$ = new cdk::sequence_node(LINE, $2, $1); }
	;

stmt: expr ';'					{ $$ = new fir::evaluation_node(LINE, $1); }
	| tWRITE exprs ';'			{ $$ = new fir::print_node(LINE, $2, false); }
	| tWRITELN exprs ';'			{ $$ = new fir::print_node(LINE, $2, true); }
	| tRETURN				{ $$ = new fir::return_node(LINE); }
	| tLEAVE tINTEGER ';'			{ $$ = new fir::leave_node(LINE, $2); }
	| tLEAVE ';'				{ $$ = new fir::leave_node(LINE, 1); }
	| tRESTART tINTEGER ';'			{ $$ = new fir::restart_node(LINE, $2); }
	| tRESTART ';'				{ $$ = new fir::restart_node(LINE, 1); }
	| tWHILE expr tDO stmt			{ $$ = new fir::while_node(LINE, $2, $4); }
	| tWHILE expr tDO stmt tFINALLY stmt	{ $$ = new fir::while_node(LINE, $2, $4, $6); }
	| tIF expr tTHEN stmt %prec tIFX	{ $$ = new fir::if_node(LINE, $2, $4); }
	| tIF expr tTHEN stmt tELSE stmt	{ $$ = new fir::if_else_node(LINE, $2, $4, $6); }
	| block					{ $$ = $1; }
	;

string: tSTRING					{ $$ = $1; }
	| string tSTRING			{ $$ = $1; $$->append(*$2); delete $2; }

%%
