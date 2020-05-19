 /*HEADER*/

%option noyywrap

%{

/**
 * set the type of the global variable \c lexval to be a "union lext".
 *
 * This needs to be positioned before "tokensDefinitions"
 */
#define YY_DECL int yylex()
//definition of lexval
union lext lexval;


//https://stackoverflow.com/a/1811158/1887602
#include "tokensDefinitions.h"
#include "lext.h"
#include <stdlib.h>
#include <string.h>
%}

%%

 /*GRAMMAR SYMBOLS*/

 /*space*/				;
[ \t\n]+        			{return TK_SPACE;};
 /*sign*/				;
\+					{return TK_PLUS;};
\-					{return TK_MINUS;};
 /*equal*/				;
=					{return TK_EQUAL;};
 /*boolean*/
[Tt][Rr][Uu][Ee]			{lexval.bval=true; return TK_TRUE;};
[Ff][Aa][Ll][Ss][Ee]			{lexval.bval=false; return TK_FALSE;}
 /*character*/
\'.\'					{lexval.cval=yytext[1]; return TK_CHARACTER;};
 /*integer number*/			;
[0-9]+					{lexval.ival=atoi(yytext); return TK_INTEGER;}
 /*float number*/			;
[0-9]*\.[0-9]+				{lexval.dval=atof(yytext); return TK_DECIMAL;};
 /*string (possibly empty)*/		;
\"[^\"]*\"				{
						int yytextLength = strlen(yytext);
						lexval.sval=(char*)malloc(sizeof(char)*yytextLength+1-2); //1 for the null terminator, -2 for the double quotes
						if (lexval.sval == NULL) {
							exit(1);
						}
						//we copy from the 2nd character because the first is \" and we don't want it. Likewise for the last character.
						strcpy(lexval.sval, &yytext[1]);
						return TK_STRING;
					}
 /*long option value*/			;
[a-zA-Z_][a-zA-Z0-9_\-\.]*		{
						int yytextLength = strlen(yytext);
						lexval.sval=(char*)malloc(sizeof(char)*yytextLength+1); //1 for the null terminator, -2 for the double quotes
						if (lexval.sval == NULL) {
							exit(1);
						}
						//we copy from the 2nd character because the first is \" and we don't want it. Likewise for the last character.
						strcpy(lexval.sval, &yytext[0]);
						return TK_EXTENDED_IDENTIFIER;
					}

%%

 //FOOTER



