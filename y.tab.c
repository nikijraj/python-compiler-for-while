/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "grammar.y"

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <stdarg.h>

	#define YYERROR_VERBOSE 1
	#define MAXRECST 200
	#define MAXST 100
	#define MAXCHILDREN 100
	#define MAXLEVELS 20
	#define MAXQUADS 1000
	
	extern int yylineno;
	extern int depth;
	extern int top();
	extern int pop();
//Binu	int currentScope = 1, previousScope = 1;
	int currentScope = 0 , previousScope = 1;
	
// Binu	int *arrayScope = NULL;

	int yylex();
	void yyerror(const char*);
	
	typedef struct Record
	{
		char *type;
		char *name;
		int decLine;
		int lastLine;
	} Record;

	typedef struct SymTable
	{
		int no;
		int ele_count;
		int scope;
		Record *Elements;
		int parent;
		
	} SymTable;
	
	typedef struct ASTNode
	{
		int nodeNo;
		//if the Node is an operator
    	char *NType;
	    int opCount;
    	struct ASTNode** NextLevel;
		//if the Node is an identifier or a constant
    	Record *id;
	} Node;
  
	typedef struct Quad
	{
		char *R;
		char *A1;
		char *A2;
		char *Op;
		int I;
	} Quad;
	
	SymTable *st = NULL;
	int sIndex = 0, aIndex = -1, tabCount = 0, tIndex = 0 , lIndex = 0, qIndex = 0, NodeCount = 0;
	// BINU int sIndex = -1, aIndex = -1, tabCount = 0, tIndex = 0 , lIndex = 0, qIndex = 0, NodeCount = 0;

	Node *rootNode;
	char *argsList = NULL;
	char *tString = NULL, *lString = NULL;
	Quad *quad_array = NULL;
	Node ***Tree = NULL;
	int *levelIndices = NULL;
	Node * e1, * e2, * e3 = NULL;
	
	//function prototypes 	
	Record* findRecord(const char *name, const char *type, int scope);
	Node *createID_Const(char *value, char *type, int scope);
	int power(int base, int exp);
	void updateScope(int scope);
	void resetDepth();
	int scopeBasedTableSearch(int scope);
	void initNewTable(int scope);
	void init();
	int searchRecordInScope(const char* type, const char *name, int index);
	void insertRecord(const char* type, const char *name, int lineNo, int scope);
	int searchRecordInScope(const char* type, const char *name, int index);
	void checkList(const char *name, int lineNo, int scope);
	void dispSymtable();
	void freeAll();
	void addToList(char *newVal, int flag);
	void clearArgsList();
	int isBinOp(char *Op);
	
	void Xitoa(int num, char *str)
	{
		if(str == NULL)
		{
			 printf("Allocate Memory\n");
		   return;
		}
		sprintf(str, "%d", num);
	}

	//for intermediate code in sym table	
	char *makeStr(int no, int flag)
	{
		char A[10];
		Xitoa(no, A);
		
		if(flag==1)
		{
				strcpy(tString, "T");
				strcat(tString, A);
				insertRecord("ICGTempVar", tString, -1, 0);
				return tString;
		}
		else
		{
				strcpy(lString, "L");
				strcat(lString, A);
				insertRecord("ICGTempLabel", lString, -1, 0);
				return lString;
		}

	}
	
	//fill quadrant
	void makeQuad(char *R, char *A1, char *A2, char *Op)
	{
		
		quad_array[qIndex].R = (char*)malloc(strlen(R)+1);
		quad_array[qIndex].Op = (char*)malloc(strlen(Op)+1);
		quad_array[qIndex].A1 = (char*)malloc(strlen(A1)+1);
		quad_array[qIndex].A2 = (char*)malloc(strlen(A2)+1);
		
		strcpy(quad_array[qIndex].R, R);
		strcpy(quad_array[qIndex].A1, A1);
		strcpy(quad_array[qIndex].A2, A2);
		strcpy(quad_array[qIndex].Op, Op);
		quad_array[qIndex].I = qIndex;
 
		qIndex++;
		
		return;
	}
	
	int isBinOp(char *Op)
	{
			if((strcmp(Op, "+")==0) || (strcmp(Op, "*")==0) || (strcmp(Op, "/")==0) || (strcmp(Op, ">=")==0) || (strcmp(Op, "<=")==0) || (strcmp(Op, "<")==0) || (strcmp(Op, ">")==0) || 
				 (strcmp(Op, "in")==0) || (strcmp(Op, "==")==0) || (strcmp(Op, "and")==0) || (strcmp(Op, "or")==0))
				{
					return 1;
				}
				
				else 
				{
					return 0;
				}
		}
		
		void codeGenOp(Node *opNode)
		{
			int i=0;
			if(opNode == NULL)
			{
				printf("opNode is null");
				return;
			}
			
			if(opNode->NType == NULL)
			{
				if((strcmp(opNode->id->type, "Identifier")==0) || (strcmp(opNode->id->type, "Constant")==0))
				{
					//three address code
					printf("T%d = %s\n", opNode->nodeNo, opNode->id->name);
					makeQuad(makeStr(opNode->nodeNo, 1), opNode->id->name, "-", "=");
				}
				return;
			}
			
			
			if(strcmp(opNode->NType, "For")==0)
			{
				int temp = lIndex;
				//next level of AST
				codeGenOp(opNode->NextLevel[0]);
				//three address code
				printf("L%d: If False T%d goto L%d\n", lIndex, opNode->NextLevel[0]->nodeNo, temp+1);
				makeQuad(makeStr(temp, 0), "-", "-", "Label");		
				makeQuad(makeStr(temp+1, 0), makeStr(opNode->NextLevel[0]->nodeNo, 1), "-", "If False");								
				//next level of AST
				codeGenOp(opNode->NextLevel[1]);
				//three address code
				printf("L%d: If False T%d goto L%d\n", lIndex, opNode->NextLevel[1]->nodeNo, temp+1);
				makeQuad(makeStr(temp, 0), "-", "-", "goto");
				//next level of AST
				codeGenOp(opNode->NextLevel[2]);
				//three address code
				printf("goto L%d\n", temp);
				printf("L%d: ", temp+1);
				makeQuad(makeStr(temp+1, 0), "-", "-", "Label"); 
				lIndex = lIndex+2;
				return;
			}

		if(strcmp(opNode->NType, "While")==0)
		{
			int temp = lIndex;
			//next level of AST
			codeGenOp(opNode->NextLevel[0]);
			//three address code
			printf("L%d: If False T%d goto L%d\n", lIndex, opNode->NextLevel[0]->nodeNo, lIndex+1);
			makeQuad(makeStr(temp, 0), "-", "-", "Label");		
			makeQuad(makeStr(temp+1, 0), makeStr(opNode->NextLevel[0]->nodeNo, 1), "-", "If False");								
			lIndex+=2;			
			//next level of AST
			codeGenOp(opNode->NextLevel[1]);
			//three address code
			printf("goto L%d\n", temp);
			makeQuad(makeStr(temp, 0), "-", "-", "goto");
			//three address code
			printf("L%d: ", temp+1);
			fflush(stdout);
			makeQuad(makeStr(temp+1, 0), "-", "-", "Label"); 
			lIndex = lIndex+2;
			return;
		}
		
		if(strcmp(opNode->NType, "Next")==0)
		{
			codeGenOp(opNode->NextLevel[0]);
			codeGenOp(opNode->NextLevel[1]);
			return;
		}
		
		if(strcmp(opNode->NType, "BeginBlock")==0)
		{
			codeGenOp(opNode->NextLevel[0]);
			codeGenOp(opNode->NextLevel[1]);		
			return;	
		}
		
		if(strcmp(opNode->NType, "EndBlock")==0)
		{
			switch(opNode->opCount)
			{
				case 0 : 
				{
					break;
				}
				case 1 : 
				{
					codeGenOp(opNode->NextLevel[0]);
					break;
				}
			}
			return;
		}
		
		if(strcmp(opNode->NType, "ListIndex")==0)
		{
			printf("T%d = %s[%s]\n", opNode->nodeNo, opNode->NextLevel[0]->id->name, opNode->NextLevel[1]->id->name);
			makeQuad(makeStr(opNode->nodeNo, 1), opNode->NextLevel[0]->id->name, opNode->NextLevel[1]->id->name, "=[]");
			return;
		}
		
		if(isBinOp(opNode->NType)==1)
		{
			//node has two children
			codeGenOp(opNode->NextLevel[0]);
			codeGenOp(opNode->NextLevel[1]);
			char *X1 = (char*)malloc(10);
			char *X2 = (char*)malloc(10);
			char *X3 = (char*)malloc(10);
			
			strcpy(X1, makeStr(opNode->nodeNo, 1));
			strcpy(X2, makeStr(opNode->NextLevel[0]->nodeNo, 1));
			strcpy(X3, makeStr(opNode->NextLevel[1]->nodeNo, 1));

			printf("T%d = T%d %s T%d\n", opNode->nodeNo, opNode->NextLevel[0]->nodeNo, opNode->NType, opNode->NextLevel[1]->nodeNo);
			makeQuad(X1, X2, X3, opNode->NType);
			free(X1);
			free(X2);
			free(X3);
			return;
		}
		
		if(strcmp(opNode->NType, "-")==0)
		{
			if(opNode->opCount == 1)
			{
				codeGenOp(opNode->NextLevel[0]);
				char *X1 = (char*)malloc(10);
				char *X2 = (char*)malloc(10);
				strcpy(X1, makeStr(opNode->nodeNo, 1));
				strcpy(X2, makeStr(opNode->NextLevel[0]->nodeNo, 1));
				printf("T%d = %s T%d\n", opNode->nodeNo, opNode->NType, opNode->NextLevel[0]->nodeNo);
				makeQuad(X1, X2, "-", opNode->NType);	
			}
			
			else
			{
				codeGenOp(opNode->NextLevel[0]);
				codeGenOp(opNode->NextLevel[1]);
				char *X1 = (char*)malloc(10);
				char *X2 = (char*)malloc(10);
				char *X3 = (char*)malloc(10);
			
				strcpy(X1, makeStr(opNode->nodeNo, 1));
				strcpy(X2, makeStr(opNode->NextLevel[0]->nodeNo, 1));
				strcpy(X3, makeStr(opNode->NextLevel[1]->nodeNo, 1));

				printf("T%d = T%d %s T%d\n", opNode->nodeNo, opNode->NextLevel[0]->nodeNo, opNode->NType, opNode->NextLevel[1]->nodeNo);
				makeQuad(X1, X2, X3, opNode->NType);
				free(X1);
				free(X2);
				free(X3);
				return;
			
			}
		}
		
		if(strcmp(opNode->NType, "import")==0)
		{
			printf("import %s\n", opNode->NextLevel[0]->id->name);
			makeQuad("-", opNode->NextLevel[0]->id->name, "-", "import");
			return;
		}
		
		if(strcmp(opNode->NType, "NewLine")==0)
		{
			codeGenOp(opNode->NextLevel[0]);
			codeGenOp(opNode->NextLevel[1]);
			return;
		}
		
		if(strcmp(opNode->NType, "=")==0)
		{
			codeGenOp(opNode->NextLevel[1]);
			printf("%s = T%d\n", opNode->NextLevel[0]->id->name, opNode->NextLevel[1]->nodeNo);
			makeQuad(opNode->NextLevel[0]->id->name, makeStr(opNode->NextLevel[1]->nodeNo, 1), "-", opNode->NType);
			return;
		}
		
		if(strcmp(opNode->NType, "Func_Name")==0)
		{
			printf("Begin Function %s\n", opNode->NextLevel[0]->id->name);
			makeQuad("-", opNode->NextLevel[0]->id->name, "-", "BeginF");
			codeGenOp(opNode->NextLevel[2]);
			printf("End Function %s\n", opNode->NextLevel[0]->id->name);
			makeQuad("-", opNode->NextLevel[0]->id->name, "-", "EndF");
			return;
		}
		
		if(strcmp(opNode->NType, "Func_Call")==0)
		{
			if(strcmp(opNode->NextLevel[1]->NType, "Void")==0)
			{
				printf("(T%d)Call Function %s\n", opNode->nodeNo, opNode->NextLevel[0]->id->name);
				makeQuad(makeStr(opNode->nodeNo, 1), opNode->NextLevel[0]->id->name, "-", "Call");
			}
			else
			{
				char A[10];
				char* token = strtok(opNode->NextLevel[1]->NType, ","); 
	  			int i = 0;
				while (token != NULL) 
				{
						i++; 
				    printf("Push Param %s\n", token);
				    makeQuad("-", token, "-", "Param"); 
				    token = strtok(NULL, ","); 
				}
				
				printf("(T%d)Call Function %s, %d\n", opNode->nodeNo, opNode->NextLevel[0]->id->name, i);
				sprintf(A, "%d", i);
				makeQuad(makeStr(opNode->nodeNo, 1), opNode->NextLevel[0]->id->name, A, "Call");
				printf("Pop Params for Function %s, %d\n", opNode->NextLevel[0]->id->name, i);
								
				return;
			}
		}		
		
	}
	
	Node *createID_Const(char *type, char *value, int scope)
	{
		char *val = value;
		Node *newNode;
		newNode = (Node*)calloc(1, sizeof(Node));
		newNode->NType = NULL;
		newNode->opCount = -1;
		newNode->id = findRecord(value, type, scope);
		newNode->nodeNo = NodeCount++;
		return newNode;
	}

	//AST NType
	Node *createOp(char *oper, int opCount, ...)
	{
		va_list params;
		Node *newNode;
	    int i;
    	newNode = (Node*)calloc(1, sizeof(Node));
    
	    newNode->NextLevel = (Node**)calloc(opCount, sizeof(Node*));
    
	    newNode->NType = (char*)malloc(strlen(oper)+1);
   		strcpy(newNode->NType, oper);
	    newNode->opCount = opCount;
	    va_start(params, opCount);
    
    	for (i = 0; i < opCount; i++)
		    newNode->NextLevel[i] = va_arg(params, Node*);
    
	    va_end(params);
    	newNode->nodeNo = NodeCount++;
    	return newNode;
	}
  
	void addToList(char *newVal, int flag)
	{
  		if(flag==0)
	  	{
			strcat(argsList, ", ");
			strcat(argsList, newVal);
		}
		else
		{
			strcat(argsList, newVal);
		}
  	}
  
	void clearArgsList()
	{
	    strcpy(argsList, "");
	}
  
	int power(int base, int exp)
	{
		int i =0, res = 1;
		for(i; i<exp; i++)
		{
			res *= base;
		}
		return res;
	}
	
	void updateScope(int scope)
	{
	 //BINU	currentScope = scope;
	}
	
	void resetDepth()
	{
		while(top()) pop();
		depth = 10;
	}
	
	int scopeBasedTableSearch(int scope)
	{
		int i = sIndex;
		for(i; i > -1; i--)
		{
			if(st[i].scope == scope)
			{
				return i;
			}
		}
		return -1;
	}
	
	void initNewTable(int scope)
	{
		/* Binu
		arrayScope[scope]++;
		sIndex++;
		st[sIndex].scope = power(scope, arrayScope[scope]);
		*/
		st[sIndex].no = sIndex;
		st[sIndex].scope = scope;

		st[sIndex].ele_count = 0;		
		st[sIndex].Elements = (Record*)calloc(MAXRECST, sizeof(Record));
		
		st[sIndex].parent = scopeBasedTableSearch(currentScope); 
	}
	
	void init()
	{
		int i = 0;
		st = (SymTable*)calloc(MAXST, sizeof(SymTable));
		// Binu arrayScope = (int*)calloc(10, sizeof(int));
//Binu		initNewTable(1);
		initNewTable(0);
		argsList = (char *)malloc(100);
		strcpy(argsList, "");
		tString = (char*)calloc(10, sizeof(char));
		lString = (char*)calloc(10, sizeof(char));
		quad_array = (Quad*)calloc(MAXQUADS, sizeof(Quad));
		
		levelIndices = (int*)calloc(MAXLEVELS, sizeof(int));
		Tree = (Node***)calloc(MAXLEVELS, sizeof(Node**));
		for(i = 0; i<MAXLEVELS; i++)
		{
			Tree[i] = (Node**)calloc(MAXCHILDREN, sizeof(Node*));
		}
	}

	int searchRecordInScope(const char* type, const char *name, int index)
	{
		int i =0;
		for(i=0; i<st[index].ele_count; i++)
		{
			if((strcmp(st[index].Elements[i].type, type)==0) && (strcmp(st[index].Elements[i].name, name)==0))
			{
				return i;
			}	
		}
		return -1;
	}
		
	void modifyRecordID(const char *type, const char *name, int lineNo, int scope)
	{
		int i =0;
		int index = scopeBasedTableSearch(scope);
		if(index==0)
		{
			for(i=0; i<st[index].ele_count; i++)
			{
				
				if(strcmp(st[index].Elements[i].type, type)==0 && (strcmp(st[index].Elements[i].name, name)==0))
				{
					st[index].Elements[i].lastLine = lineNo;
					return;
				}	
			}
			printf("%s '%s' at line %d Not Declared\n", type, name, yylineno);
			exit(1);
		}
		
		for(i=0; i<st[index].ele_count; i++)
		{
			if(strcmp(st[index].Elements[i].type, type)==0 && (strcmp(st[index].Elements[i].name, name)==0))
			{
				st[index].Elements[i].lastLine = lineNo;
				return;
			}	
		}
		return modifyRecordID(type, name, lineNo, st[st[index].parent].scope);
	}
	
	void insertRecord(const char* type, const char *name, int lineNo, int scope)
	{ 
		/*
		int FScope = power(scope, arrayScope[scope]);
		int index = scopeBasedTableSearch(FScope);
		*/
		int index = scopeBasedTableSearch(scope);
		int RecordIndex = searchRecordInScope(type, name, index);
		if(RecordIndex==-1)
		{
			st[index].Elements[st[index].ele_count].type = (char*)calloc(30, sizeof(char));
			st[index].Elements[st[index].ele_count].name = (char*)calloc(20, sizeof(char));
		
			strcpy(st[index].Elements[st[index].ele_count].type, type);	
			strcpy(st[index].Elements[st[index].ele_count].name, name);
			st[index].Elements[st[index].ele_count].decLine = lineNo;
			st[index].Elements[st[index].ele_count].lastLine = lineNo;
			st[index].ele_count++;

		}
		else
		{
			st[index].Elements[RecordIndex].lastLine = lineNo;
		}
	}
	
	void checkList(const char *name, int lineNo, int scope)
	{
		int index = scopeBasedTableSearch(scope);
		int i;
		if(index==0)
		{
			
			for(i=0; i<st[index].ele_count; i++)
			{
				
				if(strcmp(st[index].Elements[i].type, "ListTypeID")==0 && (strcmp(st[index].Elements[i].name, name)==0))
				{
					st[index].Elements[i].lastLine = lineNo;
					return;
				}	

				else if(strcmp(st[index].Elements[i].name, name)==0)
				{
					printf("Identifier '%s' at line %d Not Indexable\n", name, yylineno);
					exit(1);

				}

			}
			printf("Identifier '%s' at line %d Not Indexable\n", name, yylineno);
			exit(1);
		}
		
		for(i=0; i<st[index].ele_count; i++)
		{
			if(strcmp(st[index].Elements[i].type, "ListTypeID")==0 && (strcmp(st[index].Elements[i].name, name)==0))
			{
				st[index].Elements[i].lastLine = lineNo;
				return;
			}
			
			else if(strcmp(st[index].Elements[i].name, name)==0)
			{
				printf("Identifier '%s' at line %d Not Indexable\n", name, yylineno);
				exit(1);

			}
		}
		
		return checkList(name, lineNo, st[st[index].parent].scope);

	}
	
	Record* findRecord(const char *name, const char *type, int scope)
	{
		int i =0;
		int index = scopeBasedTableSearch(scope);
		if(index==0)
		{
			for(i=0; i<st[index].ele_count; i++)
			{
				
				if(strcmp(st[index].Elements[i].type, type)==0 && (strcmp(st[index].Elements[i].name, name)==0))
				{
					return &(st[index].Elements[i]);
				}	
			}
			printf("\n%s '%s' at line %d Not Found in Symbol Table at scope %d \n", type, name, yylineno, scope);
			exit(1);
		}
		
		for(i=0; i<st[index].ele_count; i++)
		{
			if(strcmp(st[index].Elements[i].type, type)==0 && (strcmp(st[index].Elements[i].name, name)==0))
			{
				return &(st[index].Elements[i]);
			}	
		}
		return findRecord(name, type, st[st[index].parent].scope);
	}

	void dispSymtable()
	{
		int i = 0, j = 0;
		
		printf("\n----------------------------Symbol Table----------------------------");
		printf("\nScope\tName\tType\t\tDeclaration\tLast Used Line\n");
		for(i=0; i<=sIndex; i++)
		{
			for(j=0; j<st[i].ele_count; j++)
			{
				printf("(%d, %d)\t%s\t%s\t%d\t\t%d\n", st[i].parent, st[i].scope, st[i].Elements[j].name, st[i].Elements[j].type, st[i].Elements[j].decLine,  st[i].Elements[j].lastLine);
			}
		}
		
		printf("-------------------------------------------------------------------------\n");
		
	}
	
	void ASTToArray(Node *root, int level)
	{
	  if(root == NULL )
	  {
	    return;
	  }
	  
	  if(root->opCount <= 0)
	  {
	  	Tree[level][levelIndices[level]] = root;
	  	levelIndices[level]++;
	  }
	  
	  if(root->opCount > 0)
	  {
	 		int j;
	 		Tree[level][levelIndices[level]] = root;
	 		levelIndices[level]++; 
	    for(j=0; j<root->opCount; j++)
	    {
	    	ASTToArray(root->NextLevel[j], level+1);
	    }
	  }
	}
	
	void printAST(Node *root)
	{
		printf("\n-------------------------Abstract Syntax Tree--------------------------\n");
		ASTToArray(root, 0);
		int j = 0, p, q, maxLevel = 0, lCount = 0;
		
		while(levelIndices[maxLevel] > 0) maxLevel++;
		
		while(levelIndices[j] > 0)
		{
			for(q=0; q<lCount; q++)
			{
				printf(" ");
			
			}
			for(p=0; p<levelIndices[j] ; p++)
			{
				if(Tree[j][p]->opCount == -1)
				{
					printf("%s  ", Tree[j][p]->id->name);
					lCount+=strlen(Tree[j][p]->id->name);
				}
				else if(Tree[j][p]->opCount == 0)
				{
					printf("%s  ", Tree[j][p]->NType);
					lCount+=strlen(Tree[j][p]->NType);
				}
				else
				{
					printf("%s(%d) ", Tree[j][p]->NType, Tree[j][p]->opCount);
				}
			}
			j++;
			printf("\n");
		}
	}
	
	int IsValidNumber(char * string)
	{
   for(int i = 0; i < strlen( string ); i ++)
   {
      //ASCII value of 0 = 48, 9 = 57. So if value is outside of numeric range then fail
      //Checking for negative sign "-" could be added: ASCII value 45.
      if (string[i] < 48 || string[i] > 57)
         return 0;
   }
 
   return 1;
	}
	
	int deadCodeElimination()
	{
		int i = 0, j = 0, flag = 1, XF=0;
		while(flag==1)
		{
			
			flag=0;
			for(i=0; i<qIndex; i++)
			{
				XF=0;
				if(!((strcmp(quad_array[i].R, "-")==0) | (strcmp(quad_array[i].Op, "Call")==0) | (strcmp(quad_array[i].Op, "Label")==0) | (strcmp(quad_array[i].Op, "goto")==0) | (strcmp(quad_array[i].Op, "If False")==0)))
				{
					for(j=0; j<qIndex; j++)
					{
							if(((strcmp(quad_array[i].R, quad_array[j].A1)==0) && (quad_array[j].I!=-1)) | ((strcmp(quad_array[i].R, quad_array[j].A2)==0) && (quad_array[j].I!=-1)))
							{
								XF=1;
							}
					}
				
					if((XF==0) & (quad_array[i].I != -1))
					{
						quad_array[i].I = -1;
						flag=1;
					}
				}
			}
		}
		return flag;
	}
		
	void copyProp()
    {
        for(int i=0; i<qIndex; i++)
            {
                if((strcmp(quad_array[i].Op,"=")==0) && (!quad_array[i].A2))
                {
                    quad_array[i].R=quad_array[i].A1;
                    quad_array[i].A1='\0';
                }
            }
    }

	void printQuads()
	{
		printf("\n--------------------------------Quadruples---------------------------------\n");
		int i = 0;
		for(i=0; i<qIndex; i++)
		{
			if(quad_array[i].I > -1)
				printf("%d\t%s\t%s\t%s\t%s\n", quad_array[i].I, quad_array[i].Op, quad_array[i].A1, quad_array[i].A2, quad_array[i].R);
		}
		printf("--------------------------------------------------------------------------\n");
	}
	
	void freeAll()
	{
		deadCodeElimination();
		printQuads();
		printf("\n");
		int i = 0, j = 0;
		for(i=0; i<=sIndex; i++)
		{
			for(j=0; j<st[i].ele_count; j++)
			{
				free(st[i].Elements[j].name);
				free(st[i].Elements[j].type);	
			}
			free(st[i].Elements);
		}
		free(st);
		free(quad_array);
	}

#line 893 "y.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
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
    T_EndOfFile = 258,
    T_Return = 259,
    T_Number = 260,
    T_True = 261,
    T_False = 262,
    T_ID = 263,
    T_Print = 264,
    T_Cln = 265,
    T_NL = 266,
    T_EQL = 267,
    T_NEQ = 268,
    T_EQ = 269,
    T_GT = 270,
    T_LT = 271,
    T_EGT = 272,
    T_ELT = 273,
    T_Or = 274,
    T_And = 275,
    T_Not = 276,
    T_In = 277,
    T_Range = 278,
    ID = 279,
    ND = 280,
    DD = 281,
    T_String = 282,
    T_If = 283,
    T_Elif = 284,
    T_For = 285,
    T_While = 286,
    T_Else = 287,
    T_Import = 288,
    T_Break = 289,
    T_Pass = 290,
    T_MN = 291,
    T_PL = 292,
    T_DV = 293,
    T_ML = 294,
    T_OP = 295,
    T_CP = 296,
    T_OB = 297,
    T_CB = 298,
    T_Def = 299,
    T_Comma = 300,
    T_List = 301
  };
#endif
/* Tokens.  */
#define T_EndOfFile 258
#define T_Return 259
#define T_Number 260
#define T_True 261
#define T_False 262
#define T_ID 263
#define T_Print 264
#define T_Cln 265
#define T_NL 266
#define T_EQL 267
#define T_NEQ 268
#define T_EQ 269
#define T_GT 270
#define T_LT 271
#define T_EGT 272
#define T_ELT 273
#define T_Or 274
#define T_And 275
#define T_Not 276
#define T_In 277
#define T_Range 278
#define ID 279
#define ND 280
#define DD 281
#define T_String 282
#define T_If 283
#define T_Elif 284
#define T_For 285
#define T_While 286
#define T_Else 287
#define T_Import 288
#define T_Break 289
#define T_Pass 290
#define T_MN 291
#define T_PL 292
#define T_DV 293
#define T_ML 294
#define T_OP 295
#define T_CP 296
#define T_OB 297
#define T_CB 298
#define T_Def 299
#define T_Comma 300
#define T_List 301

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 824 "grammar.y"
 char *text; int depth; struct ASTNode* Node;

#line 1040 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;
int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   261

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  47
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  45
/* YYNRULES -- Number of rules.  */
#define YYNRULES  95
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  173

#define YYUNDEFTOK  2
#define YYMAXUTOK   301


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   840,   840,   840,   842,   843,   845,   846,   847,   850,
     852,   852,   852,   852,   854,   855,   856,   857,   858,   859,
     860,   861,   863,   864,   865,   866,   867,   868,   869,   872,
     873,   874,   875,   876,   877,   878,   879,   881,   882,   883,
     884,   886,   887,   889,   890,   891,   892,   894,   895,   896,
     897,   899,   901,   902,   903,   904,   905,   907,   908,   909,
     912,   913,   915,   916,   918,   920,   922,   925,   926,   926,
     928,   929,   931,   931,   932,   932,   933,   935,   935,   936,
     938,   938,   938,   941,   941,   941,   943,   943,   944,   944,
     945,   945,   946,   948,   948,   950
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_EndOfFile", "T_Return", "T_Number",
  "T_True", "T_False", "T_ID", "T_Print", "T_Cln", "T_NL", "T_EQL",
  "T_NEQ", "T_EQ", "T_GT", "T_LT", "T_EGT", "T_ELT", "T_Or", "T_And",
  "T_Not", "T_In", "T_Range", "ID", "ND", "DD", "T_String", "T_If",
  "T_Elif", "T_For", "T_While", "T_Else", "T_Import", "T_Break", "T_Pass",
  "T_MN", "T_PL", "T_DV", "T_ML", "T_OP", "T_CP", "T_OB", "T_CB", "T_Def",
  "T_Comma", "T_List", "$accept", "StartDebugger", "$@1", "constant",
  "term", "list_index", "StartParse", "$@2", "basic_stmt", "arith_exp",
  "bool_exp", "bool_term", "bool_factor", "import_stmt", "pass_stmt",
  "break_stmt", "return_stmt", "assign_stmt", "print_stmt",
  "finalStatements", "cmpd_stmt", "if_stmt", "elif_stmts", "else_stmt",
  "while_stmt", "for_stmt", "start_suite", "$@3", "suite", "end_suite",
  "$@4", "$@5", "args", "$@6", "args_list", "$@7", "call_list", "$@8",
  "call_args", "$@9", "$@10", "$@11", "func_def", "$@12", "func_call", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301
};
# endif

#define YYPACT_NINF (-121)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-75)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -121,    22,   101,  -121,    72,  -121,  -121,  -121,  -121,     7,
      44,   101,    -4,  -121,    60,    26,    60,    78,  -121,  -121,
       8,    60,    90,  -121,  -121,  -121,    92,  -121,   222,  -121,
      10,  -121,  -121,  -121,  -121,  -121,  -121,  -121,    88,  -121,
    -121,  -121,  -121,  -121,  -121,  -121,   147,    16,    66,    23,
    -121,    60,  -121,    69,   222,   103,    93,   104,  -121,     8,
       2,   194,    86,  -121,  -121,     8,     8,     8,     8,     8,
     122,     8,     8,     8,     8,    60,    60,   130,   -15,    95,
     222,  -121,  -121,  -121,  -121,  -121,    98,    97,   115,   186,
     119,   186,    41,  -121,  -121,   117,    82,    82,    82,    82,
      82,  -121,     2,     2,  -121,  -121,    87,  -121,  -121,   101,
    -121,    99,    99,    99,  -121,  -121,  -121,    14,   134,  -121,
      -9,   120,  -121,   151,  -121,    23,  -121,  -121,  -121,  -121,
      60,   152,  -121,  -121,    66,  -121,   123,  -121,   142,   155,
     186,   116,   121,   157,    99,   160,   186,  -121,    66,   171,
    -121,   186,  -121,    47,  -121,    -9,   139,  -121,  -121,   142,
      65,  -121,  -121,   174,   121,   160,   142,  -121,   186,  -121,
    -121,  -121,  -121
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       2,     0,     0,     1,     0,    46,     4,    39,    40,     6,
       0,     0,     0,     5,     0,     0,     0,     0,    45,    44,
       0,     0,     0,     7,    22,     8,     0,    52,    18,    19,
      36,    37,    16,    14,    15,    21,    17,    20,     0,    53,
      57,    58,    59,    54,    55,    56,     0,    92,     0,     0,
      10,     0,    41,     6,     0,     0,     0,     0,    43,     0,
      27,     0,     0,    93,     3,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    11,     6,     0,
      47,    48,    49,    88,    86,    90,     0,     0,     0,     0,
       0,     0,     0,    28,    42,     0,    38,    32,    30,    34,
      33,    35,    24,    23,    26,    25,     0,    29,    31,     0,
      50,    85,    85,    85,    95,     9,    51,     6,     0,    67,
      60,     0,    65,    79,    12,     0,    89,    87,    91,    68,
       0,     0,    61,    62,     0,    77,     0,    83,     0,     0,
       0,     0,    82,     0,    85,     0,     0,    64,     0,     0,
      78,     0,    84,    76,    69,     0,     0,    80,    94,     0,
      72,    71,    63,     0,    82,     0,     0,    75,     0,    81,
      70,    73,    66
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -121,  -121,  -121,   -44,   -40,  -121,    -6,  -121,   -77,   -14,
     -13,    13,   169,  -121,  -121,  -121,  -121,  -121,  -121,  -120,
    -121,  -121,    30,  -121,  -121,  -121,   -76,  -121,    31,  -121,
    -121,  -121,  -121,  -121,    24,  -121,  -102,  -121,  -121,  -121,
    -121,  -121,  -121,  -121,   153
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    23,    24,    25,    26,   109,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,   132,   133,    41,    42,   120,   138,   154,   161,
     166,   167,   136,   142,   150,   164,   126,   144,    86,   112,
     111,   113,    43,    95,    44
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      54,    55,    54,    57,    87,    50,    60,    61,    62,    88,
     127,   128,   119,     6,   119,   122,    53,    12,   145,    46,
     130,    83,     3,   131,    84,    47,    46,    48,     6,    75,
      76,    53,    80,    81,    56,    13,    51,    54,    62,   165,
      73,    74,   152,    85,    20,    92,   171,    47,    59,    48,
      13,    96,    97,    98,    99,   100,    48,   102,   103,   104,
     105,   106,   106,   119,   147,     6,     7,     8,    53,   119,
     155,     6,   159,   160,   119,   158,   -74,    71,    72,    73,
      74,    12,    93,    45,    49,   137,    58,    13,   107,   108,
     141,   119,   172,    13,   -74,    64,    20,   -74,    63,    77,
      21,    65,     4,   124,   156,     5,     6,     7,     8,     9,
      10,    48,    11,    89,    91,    90,    54,   139,    71,    72,
      73,    74,    12,    71,    72,    73,    74,    94,    13,    14,
     101,    15,    16,   -13,    17,    18,    19,    20,   110,   114,
     115,    21,   121,     4,   125,    22,     5,     6,     7,     8,
       9,    10,     6,     7,     8,    78,   116,   123,   129,   135,
     134,   148,   140,    12,   143,   146,   149,   151,    12,    13,
      14,   153,    15,    16,    13,    17,    18,    19,    20,   157,
     163,    52,    21,    20,   168,   162,    22,    21,   169,    79,
       5,     6,     7,     8,   117,    10,   170,   118,     0,    82,
       0,     0,     0,     0,     0,     0,     0,    12,    65,    66,
      67,    68,    69,    13,     0,     0,    70,     0,     0,    17,
      18,    19,    20,     0,     0,     0,    21,     0,     0,     0,
      71,    72,    73,    74,     0,    93,    65,    66,    67,    68,
      69,     0,     0,     0,    70,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    71,    72,
      73,    74
};

static const yytype_int16 yycheck[] =
{
      14,    14,    16,    16,    48,    11,    20,    21,    21,    49,
     112,   113,    89,     5,    91,    91,     8,    21,   138,    12,
      29,     5,     0,    32,     8,    40,    12,    42,     5,    19,
      20,     8,    46,    46,     8,    27,    40,    51,    51,   159,
      38,    39,   144,    27,    36,    59,   166,    40,    40,    42,
      27,    65,    66,    67,    68,    69,    42,    71,    72,    73,
      74,    75,    76,   140,   140,     5,     6,     7,     8,   146,
     146,     5,    25,    26,   151,   151,    11,    36,    37,    38,
      39,    21,    41,    11,    40,   125,     8,    27,    75,    76,
     134,   168,   168,    27,    29,     3,    36,    32,     8,    11,
      40,    14,     1,   109,   148,     4,     5,     6,     7,     8,
       9,    42,    11,    10,    10,    22,   130,   130,    36,    37,
      38,    39,    21,    36,    37,    38,    39,    41,    27,    28,
       8,    30,    31,     3,    33,    34,    35,    36,    43,    41,
      43,    40,    23,     1,    45,    44,     4,     5,     6,     7,
       8,     9,     5,     6,     7,     8,    41,    40,    24,     8,
      40,    45,    10,    21,    41,    10,    45,    10,    21,    27,
      28,    11,    30,    31,    27,    33,    34,    35,    36,     8,
      41,    12,    40,    36,    10,   155,    44,    40,   164,    42,
       4,     5,     6,     7,     8,     9,   165,    11,    -1,    46,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    21,    14,    15,
      16,    17,    18,    27,    -1,    -1,    22,    -1,    -1,    33,
      34,    35,    36,    -1,    -1,    -1,    40,    -1,    -1,    -1,
      36,    37,    38,    39,    -1,    41,    14,    15,    16,    17,
      18,    -1,    -1,    -1,    22,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,    37,
      38,    39
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    48,    49,     0,     1,     4,     5,     6,     7,     8,
       9,    11,    21,    27,    28,    30,    31,    33,    34,    35,
      36,    40,    44,    50,    51,    52,    53,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    71,    72,    89,    91,    11,    12,    40,    42,    40,
      53,    40,    59,     8,    56,    57,     8,    57,     8,    40,
      56,    56,    57,     8,     3,    14,    15,    16,    17,    18,
      22,    36,    37,    38,    39,    19,    20,    11,     8,    42,
      56,    57,    91,     5,     8,    27,    85,    50,    51,    10,
      22,    10,    56,    41,    41,    90,    56,    56,    56,    56,
      56,     8,    56,    56,    56,    56,    56,    58,    58,    54,
      43,    87,    86,    88,    41,    43,    41,     8,    11,    55,
      73,    23,    73,    40,    53,    45,    83,    83,    83,    24,
      29,    32,    69,    70,    40,     8,    79,    51,    74,    57,
      10,    50,    80,    41,    84,    66,    10,    73,    45,    45,
      81,    10,    83,    11,    75,    73,    50,     8,    73,    25,
      26,    76,    69,    41,    82,    66,    77,    78,    10,    81,
      75,    66,    73
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    47,    49,    48,    50,    50,    51,    51,    51,    52,
      53,    54,    53,    53,    55,    55,    55,    55,    55,    55,
      55,    55,    56,    56,    56,    56,    56,    56,    56,    57,
      57,    57,    57,    57,    57,    57,    57,    58,    58,    58,
      58,    59,    59,    60,    61,    62,    63,    64,    64,    64,
      64,    65,    66,    66,    66,    66,    66,    67,    67,    67,
      68,    68,    69,    69,    70,    71,    72,    73,    74,    73,
      75,    75,    77,    76,    78,    76,    76,    80,    79,    79,
      82,    81,    81,    84,    83,    83,    86,    85,    87,    85,
      88,    85,    85,    90,    89,    91
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     3,     1,     1,     1,     1,     1,     4,
       2,     0,     4,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     3,     3,     3,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     1,     1,     3,     1,
       1,     2,     3,     2,     1,     1,     1,     3,     3,     3,
       4,     4,     1,     1,     1,     1,     2,     1,     1,     1,
       4,     5,     1,     5,     3,     4,    11,     1,     0,     5,
       4,     2,     0,     3,     0,     2,     0,     0,     3,     0,
       0,     4,     0,     0,     4,     0,     0,     3,     0,     3,
       0,     3,     0,     0,     8,     4
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (yylocationp);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyo, *yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2:
#line 840 "grammar.y"
                {init();}
#line 2473 "y.tab.c"
    break;

  case 3:
#line 840 "grammar.y"
                                                 {printf("\nValid Python Syntax\n");  /*printAST($2);*/ codeGenOp((yyvsp[-1].Node)); printQuads(); dispSymtable(); freeAll(); exit(0);}
#line 2479 "y.tab.c"
    break;

  case 4:
#line 842 "grammar.y"
                    {insertRecord("Constant", (yyvsp[0].text), (yylsp[0]).first_line, currentScope); (yyval.Node) = createID_Const("Constant", (yyvsp[0].text), currentScope);}
#line 2485 "y.tab.c"
    break;

  case 5:
#line 843 "grammar.y"
                    {insertRecord("Constant", (yyvsp[0].text), (yylsp[0]).first_line, currentScope); (yyval.Node) = createID_Const("Constant", (yyvsp[0].text), currentScope);}
#line 2491 "y.tab.c"
    break;

  case 6:
#line 845 "grammar.y"
            {modifyRecordID("Identifier", (yyvsp[0].text), (yylsp[0]).first_line, currentScope); (yyval.Node) = createID_Const("Identifier", (yyvsp[0].text), currentScope);}
#line 2497 "y.tab.c"
    break;

  case 7:
#line 846 "grammar.y"
                {(yyval.Node) = (yyvsp[0].Node);}
#line 2503 "y.tab.c"
    break;

  case 8:
#line 847 "grammar.y"
                  {(yyval.Node) = (yyvsp[0].Node);}
#line 2509 "y.tab.c"
    break;

  case 9:
#line 850 "grammar.y"
                                     {checkList((yyvsp[-3].text), (yylsp[-3]).first_line, currentScope); (yyval.Node) = createOp("ListIndex", 2, createID_Const("ListTypeID", (yyvsp[-3].text), currentScope), (yyvsp[-1].Node));}
#line 2515 "y.tab.c"
    break;

  case 10:
#line 852 "grammar.y"
                             {(yyval.Node)=(yyvsp[0].Node);}
#line 2521 "y.tab.c"
    break;

  case 11:
#line 852 "grammar.y"
                                                            {resetDepth();}
#line 2527 "y.tab.c"
    break;

  case 12:
#line 852 "grammar.y"
                                                                                       {(yyval.Node) = createOp("NewLine", 2, (yyvsp[-3].Node), (yyvsp[0].Node));}
#line 2533 "y.tab.c"
    break;

  case 13:
#line 852 "grammar.y"
                                                                                                                                                    {(yyval.Node)=(yyvsp[-1].Node);}
#line 2539 "y.tab.c"
    break;

  case 14:
#line 854 "grammar.y"
                       {(yyval.Node)=(yyvsp[0].Node);}
#line 2545 "y.tab.c"
    break;

  case 15:
#line 855 "grammar.y"
                        {(yyval.Node)=(yyvsp[0].Node);}
#line 2551 "y.tab.c"
    break;

  case 16:
#line 856 "grammar.y"
                         {(yyval.Node)=(yyvsp[0].Node);}
#line 2557 "y.tab.c"
    break;

  case 17:
#line 857 "grammar.y"
                         {(yyval.Node)=(yyvsp[0].Node);}
#line 2563 "y.tab.c"
    break;

  case 18:
#line 858 "grammar.y"
                       {(yyval.Node)=(yyvsp[0].Node);}
#line 2569 "y.tab.c"
    break;

  case 19:
#line 859 "grammar.y"
                      {(yyval.Node)=(yyvsp[0].Node);}
#line 2575 "y.tab.c"
    break;

  case 20:
#line 860 "grammar.y"
                        {(yyval.Node)=(yyvsp[0].Node);}
#line 2581 "y.tab.c"
    break;

  case 21:
#line 861 "grammar.y"
                         {(yyval.Node)=(yyvsp[0].Node);}
#line 2587 "y.tab.c"
    break;

  case 22:
#line 863 "grammar.y"
                 {(yyval.Node)=(yyvsp[0].Node);}
#line 2593 "y.tab.c"
    break;

  case 23:
#line 864 "grammar.y"
                                       {(yyval.Node) = createOp("+", 2, (yyvsp[-2].Node), (yyvsp[0].Node));}
#line 2599 "y.tab.c"
    break;

  case 24:
#line 865 "grammar.y"
                                       {(yyval.Node) = createOp("-", 2, (yyvsp[-2].Node), (yyvsp[0].Node));}
#line 2605 "y.tab.c"
    break;

  case 25:
#line 866 "grammar.y"
                                       {(yyval.Node) = createOp("*", 2, (yyvsp[-2].Node), (yyvsp[0].Node));}
#line 2611 "y.tab.c"
    break;

  case 26:
#line 867 "grammar.y"
                                       {(yyval.Node) = createOp("/", 2, (yyvsp[-2].Node), (yyvsp[0].Node));}
#line 2617 "y.tab.c"
    break;

  case 27:
#line 868 "grammar.y"
                           {(yyval.Node) = createOp("-", 1, (yyvsp[0].Node));}
#line 2623 "y.tab.c"
    break;

  case 28:
#line 869 "grammar.y"
                                {(yyval.Node) = (yyvsp[-1].Node);}
#line 2629 "y.tab.c"
    break;

  case 29:
#line 872 "grammar.y"
                                    {(yyval.Node) = createOp("or", 2, (yyvsp[-2].Node), (yyvsp[0].Node));}
#line 2635 "y.tab.c"
    break;

  case 30:
#line 873 "grammar.y"
                                    {(yyval.Node) = createOp("<", 2, (yyvsp[-2].Node), (yyvsp[0].Node));}
#line 2641 "y.tab.c"
    break;

  case 31:
#line 874 "grammar.y"
                                     {(yyval.Node) = createOp("and", 2, (yyvsp[-2].Node), (yyvsp[0].Node));}
#line 2647 "y.tab.c"
    break;

  case 32:
#line 875 "grammar.y"
                                    {(yyval.Node) = createOp(">", 2, (yyvsp[-2].Node), (yyvsp[0].Node));}
#line 2653 "y.tab.c"
    break;

  case 33:
#line 876 "grammar.y"
                                     {(yyval.Node) = createOp("<=", 2, (yyvsp[-2].Node), (yyvsp[0].Node));}
#line 2659 "y.tab.c"
    break;

  case 34:
#line 877 "grammar.y"
                                     {(yyval.Node) = createOp(">=", 2, (yyvsp[-2].Node), (yyvsp[0].Node));}
#line 2665 "y.tab.c"
    break;

  case 35:
#line 878 "grammar.y"
                               {checkList((yyvsp[0].text), (yylsp[0]).first_line, currentScope); (yyval.Node) = createOp("in", 2, (yyvsp[-2].Node), createID_Const("Constant", (yyvsp[0].text), currentScope));}
#line 2671 "y.tab.c"
    break;

  case 36:
#line 879 "grammar.y"
                     {(yyval.Node)=(yyvsp[0].Node);}
#line 2677 "y.tab.c"
    break;

  case 37:
#line 881 "grammar.y"
                        {(yyval.Node) = (yyvsp[0].Node);}
#line 2683 "y.tab.c"
    break;

  case 38:
#line 882 "grammar.y"
                                     {(yyval.Node) = createOp("==", 2, (yyvsp[-2].Node), (yyvsp[0].Node));}
#line 2689 "y.tab.c"
    break;

  case 39:
#line 883 "grammar.y"
                   {insertRecord("Constant", "True", (yylsp[0]).first_line, currentScope); (yyval.Node) = createID_Const("Constant", "True", currentScope);}
#line 2695 "y.tab.c"
    break;

  case 40:
#line 884 "grammar.y"
                    {insertRecord("Constant", "False", (yylsp[0]).first_line, currentScope); (yyval.Node) = createID_Const("Constant", "False", currentScope);}
#line 2701 "y.tab.c"
    break;

  case 41:
#line 886 "grammar.y"
                                {(yyval.Node) = createOp("!", 1, (yyvsp[0].Node));}
#line 2707 "y.tab.c"
    break;

  case 42:
#line 887 "grammar.y"
                                 {(yyval.Node) = (yyvsp[-1].Node);}
#line 2713 "y.tab.c"
    break;

  case 43:
#line 889 "grammar.y"
                            {insertRecord("PackageName", (yyvsp[0].text), (yylsp[0]).first_line, currentScope); (yyval.Node) = createOp("import", 1, createID_Const("PackageName", (yyvsp[0].text), currentScope));}
#line 2719 "y.tab.c"
    break;

  case 44:
#line 890 "grammar.y"
                   {(yyval.Node) = createOp("pass", 0);}
#line 2725 "y.tab.c"
    break;

  case 45:
#line 891 "grammar.y"
                     {(yyval.Node) = createOp("break", 0);}
#line 2731 "y.tab.c"
    break;

  case 46:
#line 892 "grammar.y"
                       {(yyval.Node) = createOp("return", 0);}
#line 2737 "y.tab.c"
    break;

  case 47:
#line 894 "grammar.y"
                                   {insertRecord("Identifier", (yyvsp[-2].text), (yylsp[-2]).first_line, currentScope); (yyval.Node) = createOp("=", 2, createID_Const("Identifier", (yyvsp[-2].text), currentScope), (yyvsp[0].Node));}
#line 2743 "y.tab.c"
    break;

  case 48:
#line 895 "grammar.y"
                                  {insertRecord("Identifier", (yyvsp[-2].text), (yylsp[-2]).first_line, currentScope);(yyval.Node) = createOp("=", 2, createID_Const("Identifier", (yyvsp[-2].text), currentScope), (yyvsp[0].Node));}
#line 2749 "y.tab.c"
    break;

  case 49:
#line 896 "grammar.y"
                                    {insertRecord("Identifier", (yyvsp[-2].text), (yylsp[-2]).first_line, currentScope); (yyval.Node) = createOp("=", 2, createID_Const("Identifier", (yyvsp[-2].text), currentScope), (yyvsp[0].Node));}
#line 2755 "y.tab.c"
    break;

  case 50:
#line 897 "grammar.y"
                                   {insertRecord("ListTypeID", (yyvsp[-3].text), (yylsp[-3]).first_line, currentScope); (yyval.Node) = createID_Const("ListTypeID", (yyvsp[-3].text), currentScope);}
#line 2761 "y.tab.c"
    break;

  case 51:
#line 899 "grammar.y"
                                    {(yyval.Node) = createOp("Print", 1, (yyvsp[-1].Node));}
#line 2767 "y.tab.c"
    break;

  case 52:
#line 901 "grammar.y"
                             {(yyval.Node) = (yyvsp[0].Node);}
#line 2773 "y.tab.c"
    break;

  case 53:
#line 902 "grammar.y"
                            {(yyval.Node) = (yyvsp[0].Node);}
#line 2779 "y.tab.c"
    break;

  case 54:
#line 903 "grammar.y"
                           {(yyval.Node) = (yyvsp[0].Node);}
#line 2785 "y.tab.c"
    break;

  case 55:
#line 904 "grammar.y"
                            {(yyval.Node) = (yyvsp[0].Node);}
#line 2791 "y.tab.c"
    break;

  case 56:
#line 905 "grammar.y"
                             {yyerrok; yyclearin; (yyval.Node)=createOp("SyntaxError", 0);}
#line 2797 "y.tab.c"
    break;

  case 57:
#line 907 "grammar.y"
                    {(yyval.Node) = (yyvsp[0].Node);}
#line 2803 "y.tab.c"
    break;

  case 58:
#line 908 "grammar.y"
                       {(yyval.Node) = (yyvsp[0].Node);}
#line 2809 "y.tab.c"
    break;

  case 59:
#line 909 "grammar.y"
                             {(yyval.Node) = (yyvsp[0].Node);}
#line 2815 "y.tab.c"
    break;

  case 60:
#line 912 "grammar.y"
                                          {(yyval.Node) = createOp("If", 2, (yyvsp[-2].Node), (yyvsp[0].Node));}
#line 2821 "y.tab.c"
    break;

  case 61:
#line 913 "grammar.y"
                                                     {(yyval.Node) = createOp("If", 3, (yyvsp[-3].Node), (yyvsp[-1].Node), (yyvsp[0].Node));}
#line 2827 "y.tab.c"
    break;

  case 62:
#line 915 "grammar.y"
                       {(yyval.Node)= (yyvsp[0].Node);}
#line 2833 "y.tab.c"
    break;

  case 63:
#line 916 "grammar.y"
                                                          {(yyval.Node)= createOp("Elif", 3, (yyvsp[-3].Node), (yyvsp[-1].Node), (yyvsp[0].Node));}
#line 2839 "y.tab.c"
    break;

  case 64:
#line 918 "grammar.y"
                                     {(yyval.Node) = createOp("Else", 1, (yyvsp[0].Node));}
#line 2845 "y.tab.c"
    break;

  case 65:
#line 920 "grammar.y"
                                                {(yyval.Node) = createOp("While", 2, (yyvsp[-2].Node), (yyvsp[0].Node));}
#line 2851 "y.tab.c"
    break;

  case 66:
#line 922 "grammar.y"
                                                                                         { insertRecord("Identifier", (yyvsp[-9].text), (yylsp[-10]).first_line, currentScope); Node* idNode = createID_Const("Identifier", (yyvsp[-9].text), currentScope); e1 = createOp(">=", 2, idNode, (yyvsp[-5].text)); e2 = createOp("<", 2, idNode, (yyvsp[-3].text)); (yyval.Node) = createOp("For", 3, e1, e2, (yyvsp[0].Node));}
#line 2857 "y.tab.c"
    break;

  case 67:
#line 925 "grammar.y"
                         {(yyval.Node) = (yyvsp[0].Node);}
#line 2863 "y.tab.c"
    break;

  case 68:
#line 926 "grammar.y"
                      {/* Binu initNewTable($<depth>2); updateScope($<depth>2);*/}
#line 2869 "y.tab.c"
    break;

  case 69:
#line 926 "grammar.y"
                                                                                                         {(yyval.Node) = createOp("BeginBlock", 2, (yyvsp[-1].Node), (yyvsp[0].Node));}
#line 2875 "y.tab.c"
    break;

  case 70:
#line 928 "grammar.y"
                                      {(yyval.Node) = createOp("Next", 2, (yyvsp[-1].Node), (yyvsp[0].Node));}
#line 2881 "y.tab.c"
    break;

  case 71:
#line 929 "grammar.y"
                       {(yyval.Node) = (yyvsp[0].Node);}
#line 2887 "y.tab.c"
    break;

  case 72:
#line 931 "grammar.y"
               {updateScope((yyvsp[0].depth));}
#line 2893 "y.tab.c"
    break;

  case 73:
#line 931 "grammar.y"
                                                         {(yyval.Node) = createOp("EndBlock", 1, (yyvsp[0].Node)); printf("endsuite#1\n\n");}
#line 2899 "y.tab.c"
    break;

  case 74:
#line 932 "grammar.y"
               {updateScope((yyvsp[0].depth));}
#line 2905 "y.tab.c"
    break;

  case 75:
#line 932 "grammar.y"
                                         {(yyval.Node) = createOp("EndBlock", 0);printf("endsuite#2\n\n");}
#line 2911 "y.tab.c"
    break;

  case 76:
#line 933 "grammar.y"
            {(yyval.Node) = createOp("EndBlock", 0); resetDepth();printf("endsuite#3\n\n");}
#line 2917 "y.tab.c"
    break;

  case 77:
#line 935 "grammar.y"
            {addToList((yyvsp[0].text), 1);}
#line 2923 "y.tab.c"
    break;

  case 78:
#line 935 "grammar.y"
                                                {(yyval.Node) = createOp(argsList, 0); clearArgsList();}
#line 2929 "y.tab.c"
    break;

  case 79:
#line 936 "grammar.y"
       {(yyval.Node) = createOp("Void", 0);}
#line 2935 "y.tab.c"
    break;

  case 80:
#line 938 "grammar.y"
                         {addToList((yyvsp[0].text), 0);}
#line 2941 "y.tab.c"
    break;

  case 83:
#line 941 "grammar.y"
                         {addToList((yyvsp[-1].text), 0);}
#line 2947 "y.tab.c"
    break;

  case 86:
#line 943 "grammar.y"
                 {addToList((yyvsp[0].text), 1);}
#line 2953 "y.tab.c"
    break;

  case 87:
#line 943 "grammar.y"
                                                     {(yyval.Node) = createOp(argsList, 0); clearArgsList();}
#line 2959 "y.tab.c"
    break;

  case 88:
#line 944 "grammar.y"
                                                   {addToList((yyvsp[0].text), 1);}
#line 2965 "y.tab.c"
    break;

  case 89:
#line 944 "grammar.y"
                                                                                       {(yyval.Node) = createOp(argsList, 0); clearArgsList();}
#line 2971 "y.tab.c"
    break;

  case 90:
#line 945 "grammar.y"
                                                   {addToList((yyvsp[0].text), 1);}
#line 2977 "y.tab.c"
    break;

  case 91:
#line 945 "grammar.y"
                                                                                       {(yyval.Node) = createOp(argsList, 0); clearArgsList();}
#line 2983 "y.tab.c"
    break;

  case 92:
#line 946 "grammar.y"
                                          {(yyval.Node) = createOp("Void", 0);}
#line 2989 "y.tab.c"
    break;

  case 93:
#line 948 "grammar.y"
                      {insertRecord("Func_Name", (yyvsp[0].text), (yylsp[0]).first_line, currentScope);}
#line 2995 "y.tab.c"
    break;

  case 94:
#line 948 "grammar.y"
                                                                                                                           {(yyval.Node) = createOp("Func_Name", 3, createID_Const("Func_Name", (yyvsp[-6].text), currentScope), (yyvsp[-3].Node), (yyvsp[0].Node));}
#line 3001 "y.tab.c"
    break;

  case 95:
#line 950 "grammar.y"
                                     {(yyval.Node) = createOp("Func_Call", 2, createID_Const("Func_Name", (yyvsp[-3].text), currentScope), (yyvsp[-1].Node));}
#line 3007 "y.tab.c"
    break;


#line 3011 "y.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 953 "grammar.y"


void yyerror(const char *msg)
{
	printf("\nSyntax Error at Line %d, Column : %d\n",  yylineno, yylloc.last_column);
	printf("\nyerror msg:%s\n", msg);
	exit(0);
}

int main()
{
	//printf("Enter the Expression\n");
	yyparse();
	return 0;
}

