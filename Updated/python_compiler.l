%{
	#include <string.h>
	#include "y.tab.h"
	#define stack_size 100
	#define DEBUG 1
	
	int yycolumn = 1;
	int startFlag = 1;

	FILE* ftokens=NULL;
	
	#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; 		\
    yylloc.first_column = yycolumn; yylloc.last_column = yycolumn + yyleng - 1; 	\
    yycolumn += yyleng;												
    
 	static int sp=0, stack [stack_size];
 	
 	static void disp_token(const char *X)
 	{
 		#ifdef DEBUG
		if(!ftokens)
			ftokens=fopen("Tokens.txt","w");
 		if(startFlag)
 		{
 			fprintf(ftokens,"\n---------------------------------------------------------------Token Sequence-----------------------------------------------------------------\n1 ");
 			startFlag=0;
 		}
 		
 		if(strcmp(X, "NL")==0)
 		{
 			fprintf(ftokens,"T_%s\n%d ", X, yylineno);
 		}
		else
		{
			fprintf(ftokens,"T_%s ", X);	
		}
 		#endif
 	}
 	static void push (int i) 
 	{
   		if (++sp<stack_size) stack[sp]= i;
   		else {printf ("error: stack overflow\n"); exit(1);}
 	}
 	
 	int pop () 
 	{
   		if (sp>-1) return stack[sp--];
   		else {printf ("error: stack underflow\n"); exit(1);}
 	}
 	
 	int top()
 	{
 		if(sp>-1) return stack[sp];
 		else return 1;
 	}
 	
 	static int indent_depth(const char *K)
	{
		int len = strlen(K), i, tab_count=1;
		for(i=0; i< len ; i++)
		{
			if(K[i]=='\t')
			{
				tab_count++;
			}
			else
			{
				fprintf(ftokens,"Nope");
				break;
			}	
		}
		return tab_count;
	}	
 	int depth = 1;
 		
%}
%option yylineno
whitespace [ ]
Multiline_comment \'\'\'([\r\n]|([^''']|[\r\n]))*\'\'\' 
comment [\t]*#.*
%%

{comment} {} 
[\t]*		{
			depth = indent_depth(yytext);
//			fprintf(ftokens,"Depth : %d ",  depth);
			if(depth < top())
			{
				while (depth < top()) pop();
				yylval.depth = depth;
				disp_token("Dedent");
				return T_Dedent;
			}
			
			if(depth == top())
			{
				disp_token("Nodent");
				yylval.depth = depth;
				return T_Nodent;				
			}
			if(depth > top())
			{
				push(depth);
				disp_token("Indent");
				yylval.depth = depth;
				return T_Indent;
			}
			
		}

"import" {disp_token("IMPT"); return T_Import;}
"print"  {disp_token("Print"); return T_Print;}
"pass" {disp_token("Pass"); return T_Pass;}
"if" {disp_token("If"); return T_If;}
"in" {disp_token("In"); return T_In;}
"range" {disp_token("Range"); return T_Range;}
"for" {disp_token("For"); return T_For;}
"while" {disp_token("While"); return T_While;}
"break" {disp_token("Break"); return T_Break;}
"and" {disp_token("And"); return T_And;}
"or" {disp_token("Or"); return T_Or;}
"not" {disp_token("Not"); return T_Not;}
"elif" {disp_token("Elif"); return T_Elif;}
"else" {disp_token("Else"); return T_Else;}
"def" {disp_token("Def"); return T_Def;}
"return" {disp_token("Return"); return T_Return;}
":" {disp_token("Cln"); return T_Cln;}
">" {disp_token("GT"); return T_GT;}
"<" {disp_token("LT"); return T_LT;}
">=" {disp_token("EGT"); return T_EGT;}
"<=" {disp_token("ELT"); return T_ELT;}
"==" {disp_token("EQ"); return T_EQ;}
"!=" {disp_token("NEQ"); return T_NEQ;}
"True" {disp_token("True"); return T_True;}
"False" {disp_token("False"); return T_False;}
"+" {disp_token("PL"); return T_PL;}
"-" {disp_token("MN"); return T_MN;}
"*" {disp_token("ML"); return T_ML;}
"/" {disp_token("DV"); return T_DV;}
"(" { disp_token("OP"); return T_OP;}
")" {disp_token("CP"); return T_CP;}
"[" {disp_token("OB"); return T_OB;}
"]" {disp_token("CB"); return T_CB;}
"," {disp_token("Comma"); return T_Comma;}
"=" {disp_token("EQL"); return T_EQL;}
"list" {disp_token("List"); return T_List;}
[0-9]+ {yylval.text = strdup(yytext); disp_token(yylval.text); return T_Number;}
[_a-zA-Z][_a-zA-Z0-9]* {if(strlen(yytext)>79){yytext[78]='\0';} yylval.text = strdup(yytext); disp_token(yylval.text); return T_ID;}
\"([^\"\n])*\" {yylval.text = strdup(yytext); disp_token(yylval.text); return T_String;}
\'([^\'\n])*\' {yylval.text = strdup(yytext); disp_token(yylval.text); return T_String;}
{whitespace} {} 
{Multiline_comment} {} 
"\n" {yycolumn=1; disp_token("NL"); return T_NL;}
<<EOF>> {disp_token("EOF"); return T_EndOfFile;}

%%
