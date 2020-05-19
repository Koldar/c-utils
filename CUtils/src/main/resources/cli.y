//HEADER

%{

#include <stdio.h>
#include <stdlib.h>

#include "parseNode.h"
#include "lext.h"
//the type of $1, $2, $$ and so on
#define YYSTYPE struct parse_node*

//function declarations
int yylex();
int yyparse();
void yyerror (char const *msg);
//this is defined in flex
extern union lext lexval;

struct parse_node* root = NULL;
%}

%locations

//a set of token. Bison will automatically generate a file (via option --defines=FILENAME) an heander file containing a define per %token you've inserted). See https://www.gnu.org/software/bison/manual/html_node/Token-Decl.html

//single symbols
%token TK_PLUS
%token TK_MINUS
%token TK_EQUAL

//complex values
%token TK_TRUE
%token TK_FALSE
/**
 * A character is a single ASCII character wrapped with \' characters.
 */
%token TK_CHARACTER
/**
 * A string is a sequence of character starting and ending with \"
 */
%token TK_STRING

//complex terminals
%token TK_SPACE
%token TK_INTEGER
%token TK_DECIMAL
/*
 * An extended identifier is a sequence of character which the first is either a letter or an underscore.
 * Any other character is either a letter, a number, an underscore, a point or an hyphen.
 */
%token TK_EXTENDED_IDENTIFIER


%%

cli:
	statements { 
		cuInitVarArgsOnStack(va); 
		root = newNonTerminalParseNode(PNT_CLI, NULL, va);  
		root->firstChild = newNonTerminalParseNode(PNT_STATEMENTS, root, va); 
		root->firstChild->firstChild=$1;
		$$ = root;
	} 
	;

statements:
	statements TK_SPACE statement { 
		struct parse_node* tmp = $1; 
		while(tmp->nextSibling != NULL) {
			tmp=tmp->nextSibling;
		}
		tmp->nextSibling = $3;
		
		$$ = $1;
	}
	| statement {
		$$ = $1;
	} 
	;

statement:
	short_option { $$ = $1; } 
	| long_option { $$ = $1; } 
	;

short_option:
	short_option_definition {
		cuInitVarArgsOnStack(va, $1); $$ = newShortOptionMultiValueParseNode(NULL, va);
	}
	| short_option_definition TK_SPACE value {cuInitVarArgsOnStack(va, $1, $3); newShortOptionMultiValueParseNode(NULL, va); }
	;
	
short_option_definition:
	TK_MINUS TK_EXTENDED_IDENTIFIER { $$ = newValueParseNode(VNT_STRING_VALUE, NULL, lexval); }

long_option:
	long_option_definition {cuInitVarArgsOnStack(va, $1); $$ = newNonTerminalParseNode(PNT_LONG_OPTION, NULL, va);}
	| long_option_definition TK_EQUAL value {cuInitVarArgsOnStack(va, $1, $3); $$ = newNonTerminalParseNode(PNT_LONG_OPTION, NULL, va);}
	;
	
long_option_definition:
	TK_MINUS TK_MINUS TK_EXTENDED_IDENTIFIER {$$ = newValueParseNode(VNT_STRING_VALUE, NULL, lexval);}

value:
	number { $$ = $1; }
	| boolean_value { $$ = $1; }
	| TK_STRING {$$ = newValueParseNode(VNT_STRING_VALUE, NULL, lexval);}
	| TK_CHARACTER {$$ = newValueParseNode(VNT_CHARACTER_VALUE, NULL, lexval);}
	;

boolean_value:
	TK_TRUE {$$ = newValueParseNode(VNT_BOOLEAN_VALUE, NULL, lexval);}
	| TK_FALSE {$$ = newValueParseNode(VNT_BOOLEAN_VALUE, NULL, lexval);}
	;
	

integer_number:
	TK_INTEGER {$$ = newValueParseNode(VNT_INTEGER_VALUE, NULL, lexval);}
	| TK_PLUS TK_INTEGER {$$ = newValueParseNode(VNT_INTEGER_VALUE, NULL, lexval);}
	| TK_MINUS TK_INTEGER {lexval.ival = -lexval.ival; $$ = newValueParseNode(VNT_INTEGER_VALUE, NULL, lexval);}
	;

decimal_number:
	TK_DECIMAL {$$ = newValueParseNode(VNT_DECIMAL_VALUE, NULL, lexval);}
	| TK_PLUS TK_DECIMAL {$$ = newValueParseNode(VNT_DECIMAL_VALUE, NULL, lexval);}
	| TK_MINUS TK_DECIMAL {lexval.fval = -lexval.fval; $$ = newValueParseNode(VNT_DECIMAL_VALUE, NULL, lexval);}
	;

number:
	integer_number { $$ = $1; }
	| decimal_number { $$ = $1; }
	;

%%
 //FOOTER

int main(int argc, const char** args) {
	do {
		yyparse();
	} while (!feof(stdin));
	
}

void yyerror (char const *msg){
	//fprintf(stderr, "parse error from row,col=%d,%d till row,col=%d,%d! %s\n", locp->first_line, locp->first_column, locp->last_line, locp->last_column, s);
	fprintf(stderr, "Parsing error: %s\n", msg);
	exit(-1);
}


