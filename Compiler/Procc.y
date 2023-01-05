%{
#include "Node.h"
#include <iostream>
#include <string>
extern int yylex();
extern void yyerror(const char* s);

// Global for the program
NProgram* gProgram = nullptr;

// Disable the warnings that Bison creates
#pragma warning(disable: 4065)
%}

/* You should not need to change the union */
%union {
    Node* node;
	NProgram* program;
	NData* data;
	NDecl* decl;
	NBlock* block;
	NStatement* statement;
	NNumeric* numeric;
	NExpr* expr;
	NComparison* comparison;
	std::string* string;
	int token;
}

%error-verbose

/* Tokens/Terminal symbols */
%token <token> TDATA TMAIN TLBRACE TRBRACE TSEMI TLPAREN TRPAREN
%token <token> TLBRACKET TRBRACKET TINC TDEC TEQUALS
%token <token> TADD TSUB TMUL TDIV
%token <token> TLESS TISEQUAL
%token <token> TVAR TARRAY
%token <token> TIF TELSE TWHILE
%token <token> TCOMMA TPENUP TPENDOWN TSETPOS TSETCOLOR TFWD TBACK TROT
%token <string> TINTEGER TIDENTIFIER

/* Types/non-terminal symbols */
%type <program> program
%type <data> data decls
%type <decl> decl;
%type <numeric> numeric
%type <expr> expr;
%type <block> main blocks
%type <statement> statement
%type <comparison> comparison

/* Operator precedence */
%left TADD TSUB
%left TMUL TDIV

%%

program		: data main 
				{ 
					std::cout << "Program\n";
                    $$ = new NProgram($1, $2);
                    gProgram = $$;
				}
;

data		: TDATA TLBRACE TRBRACE
				{
					std::cout << "Data (no decls)\n";
                    $$ = new NData;
				}
			| TDATA TLBRACE decls TRBRACE
				{
					std::cout << "Data\n";
                    $$ = $3;
				}

;

blocks      : statement
                {
                    std::cout << "single statement block" << std::endl;
                    $$ = new NBlock;
                    $$->AddStatement($1);
                }
            | blocks statement
                {
                    std::cout << "multi statement block" << std::endl;
                    $$->AddStatement($2);
                }
;

decls		: decl 
				{
					std::cout << "Single decl\n";
                    $$ = new NData;
                    $$->AddDecl($1);
				}
			| decls decl
				{
					std::cout << "Multiple decls\n";
                    $$->AddDecl($2);
				}
;

decl		: TVAR TIDENTIFIER TSEMI
				{
					std::cout << "Var declaration " << *($2) << '\n';
                    $$ = new NVarDecl(*($2));
				}
			| TARRAY TIDENTIFIER TLBRACKET numeric TRBRACKET TSEMI
				{
					std::cout << "Array declaration " << *($2) << '\n';
                    $$ = new NArrayDecl(*($2), $4);
				}
;

statement   : TIDENTIFIER TEQUALS expr TSEMI
                {
                    std::cout << "Assignment " << *($1) << '\n';
                    $$ = new NAssignVarStmt(*($1), $3);
                }
            | TIDENTIFIER TLBRACKET expr TRBRACKET TEQUALS expr TSEMI
                {
                    std::cout << "Array assignment " << *($1) << '\n';
                    $$ = new NAssignArrayStmt(*($1), $3, $6);
                }
            | TINC TIDENTIFIER TSEMI
                {
                    std::cout << "Increment" << std::endl;
                    $$ = new NIncStmt(*($2));
                }
            | TDEC TIDENTIFIER TSEMI
                {
                    std::cout << "Decrement" << std::endl;
                    $$ = new NDecStmt(*($2));
                }
            | TIF comparison TLBRACE blocks TRBRACE
                {
                    std::cout<< "If statement (no else)" << std::endl;
                    $$ = new NIfStmt($2, $4, nullptr);
                }
            | TIF comparison TLBRACE blocks TRBRACE TELSE TLBRACE blocks TRBRACE
                {
                    std::cout << "If else statement" << std::endl;
                    $$ = new NIfStmt($2, $4, $8);
                }
            | TWHILE comparison TLBRACE blocks TRBRACE
                {
                    std::cout << "while loop" << std::endl;
                    $$ = new NWhileStmt($2, $4);
                }
            | TPENUP TLPAREN TRPAREN TSEMI
                {
                    std::cout << "pen up" << std::endl;
                    $$ = new NPenUpStmt;
                }
            | TPENDOWN TLPAREN TRPAREN TSEMI
                {
                    std::cout << "pen down" << std::endl;
                    $$ = new NPenDownStmt;
                }
            | TSETPOS TLPAREN expr TCOMMA expr TRPAREN TSEMI
                {
                    std::cout << "set position" << std::endl;
                    $$ = new NSetPosStmt($3, $5);
                }
            | TSETCOLOR TLPAREN expr TRPAREN TSEMI
                {
                    std::cout << "set color" << std::endl;
                    $$ = new NSetColorStmt($3);
                }
            | TFWD TLPAREN expr TRPAREN TSEMI
                {
                    std::cout << "pen forward" << std::endl;
                    $$ = new NFwdStmt($3);
                }
            | TBACK TLPAREN expr TRPAREN TSEMI
                {
                    std::cout << "pen back" << std::endl;
                    $$ = new NBackStmt($3);
                }
            | TROT TLPAREN expr TRPAREN TSEMI
                {
                    std::cout << "rotate pen" << std::endl;
                    $$ = new NRotStmt($3);
                }
;

main		: TMAIN TLBRACE TRBRACE
				{
					std::cout << "Main (no stmts)\n";
                    $$ = new NBlock;
				}
            | TMAIN TLBRACE blocks TRBRACE
                {
                    std::cout << "Main\n";
                    $$ = $3;
//                    $$->
                }
;

expr		: numeric
				{
					std::cout << "Numeric expression\n";
                    $$ = new NNumericExpr($1);
				}
            | TIDENTIFIER
                {
                    std::cout << "Identifier expression\n";
                    $$ = new NVarExpr(*($1));
                }
            | expr TADD expr
                {
                    std::cout << "+" << std::endl;
                    $$ = new NBinaryExpr($1, $2, $3);
                }
            | expr TSUB expr
                {
                    std::cout << "-" << std::endl;
                    $$ = new NBinaryExpr($1, $2, $3);
                }
            | expr TMUL expr
                {
                    std::cout << "*" << std::endl;
                    $$ = new NBinaryExpr($1, $2, $3);
                }
            | expr TDIV expr
                {
                    std::cout << "/" << std::endl;
                    $$ = new NBinaryExpr($1, $2, $3);
                }
            | TLPAREN expr TRPAREN
                {
                    $$ = $2;
                }
            | TIDENTIFIER TLBRACKET expr TRBRACKET
                {
                    std::cout << "Array accesssor" << std::endl;
                    $$ = new NArrayExpr(*($1), $3);
                }
;

comparison  : expr TISEQUAL expr
                {
                    std::cout << "Is equal comparison" << std::endl;
                    $$ = new NComparison($1, $2, $3);
                }
            | expr TLESS expr
                {
                    std::cout << "Less than comparison" << std::endl;
                    $$ = new NComparison($1, $2, $3);
                }
;

numeric		: TINTEGER
				{
					std::cout << "Numeric value of " << *($1) << '\n';
                    $$ = new NNumeric(*($1));
				}
;

%%
