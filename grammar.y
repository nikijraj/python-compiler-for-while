%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#define YYERROR_VERBOSE 1
#define MAXARRAYSCOPE 256
#define MAXRECST 200
#define MAXST 100
#define MAXCHILDREN 100
#define MAXLEVELS 20
#define MAXQUADS 1000
	
extern int yylineno;
extern int depth;
extern int top();
extern int pop();
	
	

int currentScope = 0;
// Bob int currentScope = -1;
int scopeChange = 0; //flag
	
//Bob
int *arrayScope = NULL;

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
	//int no;
	int ele_count;
	int symTableScope;
	Record *Elements;
	int parentScopeIndex;

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
// Bob int sIndex = 0, aIndex = -1, tabCount = 0, tIndex = 0 , label_index = 0, qIndex = 0, NodeCount = 0;
int sIndex = -1, aIndex = -1, tabCount = 0, tIndex = 0 , label_index = 0, qIndex = 0, NodeCount = 0;
Node *rootNode;
char *argsList = NULL;
char *tString = NULL, *lString = NULL;
Quad *quad_array = NULL;
Node ***Tree = NULL;
int *levelIndices = NULL;
Node * e1, * e2, * e3 = NULL;

//function prototypes 	
Record* findRecord(const char *name, const char *type, int codeScope);
Node *createID_Const(char *value, char *type, int codeScope);
int hashScope(int codeScope);
void updateScope(int codeScope);
void resetDepth();
int scopeBasedTableSearch(int symTableScope);
void initNewTable(int codeScope);
void init();
int searchRecordInScope(const char* type, const char *name, int index);
void insertRecord(const char* type, const char *name, int lineNo, int codeScope);
int searchRecordInScope(const char* type, const char *name, int index);
void checkList(const char *name, int lineNo, int codeScope);
void dispSymtable();
void freeAll();
void addToList(char *newVal, int flag);
void clearArgsList();
int isBinOp(char *Op);
int tempNum(char* arr);
int digi(char* arr);
void optimization();

int digi(char* arr)
{
	int i =0, l=sizeof(arr);
	char ret[l];
	for(i=0;i<l;i++)
	{
		ret[i]=arr[i];
	}
	return atoi(ret);
}

int tempNum(char* arr)
{
	int i =0, l=sizeof(arr);
	int ll=l-1;
	char ret[ll];
	for(i=1;i<l;i++)
	{
		ret[i-1]=arr[i];
	}
	return atoi(ret);
}

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
			insertRecord("TempVar", tString, -1, 1);
//Bob			insertRecord("TempVar", tString, -1, 0);
			return tString;
	}
	else
	{
			strcpy(lString, "L");
			strcat(lString, A);
			insertRecord("TempLabel", lString, -1, 1);
//Bob			insertRecord("TempLabel", lString, -1, 0);
			return lString;
	}

}
	
//add a quadrant record 
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
			
			int flag=0,i=0;
			for(i=0; i<qIndex; i++)
			{
				if((strcmp(opNode->id->name,quad_array[i].R)==0) && (strcmp(quad_array[i].A2,"-")==0) && (strcmp(quad_array[i].Op,"=")==0))			
					{
						flag=1;
						break;
					}
			
			}
			
			if(flag==1)
			{
				printf("%s = %s\n",quad_array[i].A1, opNode->id->name);
				
				int n=tempNum(quad_array[i].A1);
				makeQuad(makeStr(n,1), opNode->id->name, "-", "=");
					
			}
			
			else
			{	
				printf("T%d = %s\n", opNode->nodeNo, opNode->id->name);
				makeQuad(makeStr(opNode->nodeNo, 1), opNode->id->name, "-", "=");
			}
		}
		return;
	}
	
	if(strcmp(opNode->NType, "=")==0)
	{
		codeGenOp(opNode->NextLevel[1]); 
	
		printf("%s = T%d\n", opNode->NextLevel[0]->id->name, opNode->NextLevel[1]->nodeNo);
		makeQuad(opNode->NextLevel[0]->id->name, makeStr(opNode->NextLevel[1]->nodeNo, 1), "-", opNode->NType);
		
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
		
		/*int flag=0,i=0,j=0;
		for(i=0; i<qIndex; i++)
		{
			for(j=0; j<qIndex; j++) 
			{
				if((strcmp(opNode->NextLevel[0]->id->name,quad_array[i].R)==0) && (strcmp(opNode->NextLevel[1]->id->name,quad_array[j].R)==0) && (strcmp(quad_array[i].A2,"-")==0) && (strcmp(quad_array[i].Op,"=")==0) && (strcmp(quad_array[j].A2,"-")==0) && (strcmp(quad_array[j].Op,"=")==0))			
				{
					flag=3;
					break;
				}
				
				else if((strcmp(opNode->NextLevel[0]->id->name,quad_array[j].R)==0) && (strcmp(opNode->NextLevel[1]->id->name,quad_array[i].R)==0) && (strcmp(quad_array[i].A2,"-")==0) && (strcmp(quad_array[i].Op,"=")==0) && (strcmp(quad_array[j].A2,"-")==0) && (strcmp(quad_array[j].Op,"=")==0))			
				{
					flag=4;
					break;
				}
				
			
				//if(flag==0)
				//{				
				else if((strcmp(opNode->NextLevel[0]->id->name,quad_array[i].R)==0) && (strcmp(quad_array[i].A2,"-")==0) && (strcmp(quad_array[i].Op,"=")==0))			
				{
					flag=1;
					break;
				}
				
				else if((strcmp(opNode->NextLevel[1]->id->name,quad_array[i].R)==0) && (strcmp(quad_array[i].A2,"-")==0) && (strcmp(quad_array[i].Op,"=")==0))			
				{
					flag=2;
					break;
				}
				//}
			}
			if(flag!=0)
				break;
			else
				continue;	
		}*/
		
		int l=0,r=0,i=0,j=0,flag=0;
		for(i=0; i<qIndex; i++)
		{
			if((l!=1) && (strcmp(opNode->NextLevel[0]->id->name,quad_array[i].R)==0) && (strcmp(quad_array[i].A2,"-")==0) && (strcmp(quad_array[i].Op,"=")==0))			
			{
				l=1;
				j=i;
			}
				
			if((r!=1) && (strcmp(opNode->NextLevel[1]->id->name,quad_array[i].R)==0) && (strcmp(quad_array[i].A2,"-")==0) && (strcmp(quad_array[i].Op,"=")==0))			
			{
				r=1;
			}
			
			if(l!=0 && r!=0)
				break;
			
		}	
		if(l==1 && r==1)
			flag=1;
		else if(r==0 && l!=0)
			flag=2;
		else if(r!=0 && l==0)
			flag=3;	
		
		/*if(flag==1)
		{
			printf("T%d = %s %s T%d\n", opNode->nodeNo, quad_array[i].A1, opNode->NType, opNode->NextLevel[1]->nodeNo);
			
			int n=tempNum(quad_array[i].A1);
			strcpy(X2, makeStr(n, 1));
			strcpy(X3, makeStr(opNode->NextLevel[1]->nodeNo, 1));
			
		}
		
		else if(flag==2)
		{
			printf("T%d = T%d %s %s\n", opNode->nodeNo, opNode->NextLevel[0]->nodeNo, opNode->NType, quad_array[i].R);
			
			int n=tempNum(quad_array[i].A1);
			strcpy(X2, makeStr(opNode->NextLevel[0]->nodeNo, 1));
			strcpy(X3, makeStr(n, 1));
			
		}
		
		else if(flag==3)
		{
			printf("T%d = %s %s %s\n", opNode->nodeNo, quad_array[i].R, opNode->NType, quad_array[j].R);
			
			int m=tempNum(quad_array[i].A1);
			int n=tempNum(quad_array[j].A1);
			
			strcpy(X2, makeStr(m, 1));
			strcpy(X3, makeStr(n, 1));
		}
		
		else if(flag==4)
		{
			printf("T%d = %s %s %s\n", opNode->nodeNo, quad_array[j].R, opNode->NType, quad_array[i].R);
			
			int m=tempNum(quad_array[i].A1);
			int n=tempNum(quad_array[j].A1);
			
			strcpy(X2, makeStr(n, 1));
			strcpy(X3, makeStr(m, 1));
		}*/
		

		if(flag==1)
		{
			printf("T%d = %s %s %s\n", opNode->nodeNo, quad_array[j].A1, opNode->NType, quad_array[i].A1);
			
			int m=tempNum(quad_array[i].A1);
			int n=tempNum(quad_array[j].A1);
			
			strcpy(X2, makeStr(n, 1));
			strcpy(X3, makeStr(m, 1));	
		}
		
		else if(flag==2)
		{
			printf("T%d = %s %s T%d\n", opNode->nodeNo, quad_array[j].A1, opNode->NType, opNode->NextLevel[1]->nodeNo);
			
			int n=tempNum(quad_array[j].A1);
			strcpy(X2, makeStr(n, 1));
			strcpy(X3, makeStr(opNode->NextLevel[1]->nodeNo, 1));	
		}
		
		else if(flag==3)
		{
			printf("T%d = T%d %s %s\n", opNode->nodeNo, opNode->NextLevel[0]->nodeNo, opNode->NType, quad_array[i].A1);
			
			int n=tempNum(quad_array[j].A1);
			strcpy(X2, makeStr(opNode->NextLevel[0]->nodeNo, 1));
			strcpy(X3, makeStr(n, 1));	
		}	
		
		else
		{		
			strcpy(X2, makeStr(opNode->NextLevel[0]->nodeNo, 1));
			strcpy(X3, makeStr(opNode->NextLevel[1]->nodeNo, 1));
			printf("T%d = T%d %s T%d\n", opNode->nodeNo, opNode->NextLevel[0]->nodeNo, opNode->NType, opNode->NextLevel[1]->nodeNo);
		}
			
		makeQuad(X1, X2, X3, opNode->NType);
		free(X1);
		free(X2);
		free(X3);
		
		return;
	}
	
	if(strcmp(opNode->NType, "For")==0)
	{
		int temp = label_index;
		//next level of AST
		codeGenOp(opNode->NextLevel[0]);
		printf("\nL%d: ", label_index);
		codeGenOp(opNode->NextLevel[1]);
		//three address code
		printf("If False T%d goto L%d\n", opNode->NextLevel[1]->nodeNo, temp+1);
		makeQuad(makeStr(temp, 0), "-", "-", "Label");		
		makeQuad(makeStr(temp+1, 0), makeStr(opNode->NextLevel[0]->nodeNo, 1), "-", "If False");								
		//next level of AST
		codeGenOp(opNode->NextLevel[2]);
		//three address code
		printf("If False T%d goto L%d\n", opNode->NextLevel[2]->nodeNo, temp+1);
		makeQuad(makeStr(temp, 0), "-", "-", "goto");
		//next level of AST
		codeGenOp(opNode->NextLevel[3]);
		//three address code
		printf("goto L%d\n", temp);
		printf("L%d: ", temp+1);
		makeQuad(makeStr(temp+1, 0), "-", "-", "Label"); 
		label_index = label_index+2;
		return;
	}

	if(strcmp(opNode->NType, "While")==0)
	{
		int temp = label_index;
		//next level of AST
		printf("\nL%d: ", label_index);
		codeGenOp(opNode->NextLevel[0]);
		//three address code
		printf("If False T%d goto L%d\n", opNode->NextLevel[0]->nodeNo, label_index+1);
		makeQuad(makeStr(temp, 0), "-", "-", "Label");		
		makeQuad(makeStr(temp+1, 0), makeStr(opNode->NextLevel[0]->nodeNo, 1), "-", "If False");								
		//next level of AST
		codeGenOp(opNode->NextLevel[1]);
		//three address code
		printf("goto L%d\n", temp);
		makeQuad(makeStr(temp, 0), "-", "-", "goto");
		//three address code
		printf("L%d: ", temp+1);
		fflush(stdout);
		makeQuad(makeStr(temp+1, 0), "-", "-", "Label"); 
		label_index = label_index+2;
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

	
Node *createID_Const(char *type, char *value, int codeScope)
{
	char *val = value;
	Node *newNode;
	newNode = (Node*)calloc(1, sizeof(Node));
	newNode->NType = NULL;
	newNode->opCount = -1;
	newNode->id = findRecord(value, type, codeScope);
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
 
int hashScope(int codeScope)
{
	return pow(codeScope, arrayScope[codeScope] + 1);
	/* Bob
	int i =0, res = 1;
	for(i; i<exp; i++)
	{
		res *= base;
	}
	return res;
	*/
}
	
void updateScope(int codeScope)
{
 //Bob	currentScope = scope;
 	currentScope = codeScope;
}
	
void resetDepth()
{
	while(top()) pop();
	depth = 10;
}
	
int scopeBasedTableSearch(int symTableScope)
{
//Bob	return scope; //scope and index are same
	int i = sIndex;
	for(i; i > -1; i--)
	{
		if(st[i].symTableScope == symTableScope)
		{
			return i;
		}
	}
	return -1;
}
	
void initNewTable(int codeScope)
{
	/* Bob */
	arrayScope[codeScope]++;
	sIndex++;
//	st[sIndex].scope = power(scope, arrayScope[scope]);
	st[sIndex].symTableScope = hashScope(codeScope);
/* Bob */
//	st[sIndex].no = sIndex;
//Bob	sIndex++;
//Bob	sIndex=scope;
//Bob
// Bob	if(st[sIndex].ele_count ==0) // create new only if this is empty
//Bob	{
//Bob		st[sIndex].scope = scope;
		st[sIndex].ele_count = 0;		
		st[sIndex].Elements = (Record*)calloc(MAXRECST, sizeof(Record));
//Bob	}
	
	st[sIndex].parentScopeIndex = scopeBasedTableSearch(codeScope-1); 
//	st[sIndex].parentScopeIndex = scopeBasedTableSearch(currentScope); 
}
	
void init()
{
		int i = 0;
		st = (SymTable*)calloc(MAXST, sizeof(SymTable));
		//Bob
		arrayScope = (int*)calloc(MAXARRAYSCOPE, sizeof(int));
//Bob		initNewTable(1);
		initNewTable(++currentScope);
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
		for(i=0; (index>=0) && i<st[index].ele_count; i++)
		{
			if((strcmp(st[index].Elements[i].type, type)==0) && (strcmp(st[index].Elements[i].name, name)==0))
			{
				return i;
			}	
		}
		return -1;
}
		
void modifyRecordID(const char *type, const char *name, int lineNo, int codeScope)
{
		int i =0;
//		int FScope = power(scope, arrayScope[scope]);
		int FScope = hashScope(codeScope);
		int index = scopeBasedTableSearch(FScope);
//Bob		int index = scopeBasedTableSearch(scope);
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
//Bob		return modifyRecordID(type, name, lineNo, st[st[index].parentScopeIndex].scope);
		return modifyRecordID(type, name, lineNo, codeScope-1);
}
	
void insertRecord(const char* type, const char *name, int lineNo, int codeScope)
{ 
//		int FScope = power(scope, arrayScope[scope]);
		int FScope = hashScope(codeScope);
		int index = scopeBasedTableSearch(FScope);
	//Bob	int index = scopeBasedTableSearch(scope);
		int RecordIndex = searchRecordInScope(type, name, index);
		if(RecordIndex==-1)
		{
			st[index].Elements[st[index].ele_count].type = (char*)calloc(30, sizeof(char));
			st[index].Elements[st[index].ele_count].name = (char*)calloc(80, sizeof(char));
		
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
	
void checkList(const char *name, int lineNo, int codeScope)
{
//		int FScope = power(scope, arrayScope[scope]);
		int FScope = hashScope(codeScope);
		int index = scopeBasedTableSearch(FScope);
//		int index = scopeBasedTableSearch(scope);
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
		
//Bob		return checkList(name, lineNo, st[st[index].parentScopeIndex].scope);
		return checkList(name, lineNo, codeScope-1);

}
	
Record* findRecord(const char *name, const char *type, int codeScope)
{
		int i =0;
//		int FScope = power(scope, arrayScope[scope]);
		int FScope = hashScope(codeScope);
		int index = scopeBasedTableSearch(FScope);
//Bob		int index = scopeBasedTableSearch(scope);
		if(index==0)
		{
			for(i=0; i<st[index].ele_count; i++)
			{
				
				if(strcmp(st[index].Elements[i].type, type)==0 && (strcmp(st[index].Elements[i].name, name)==0))
				{
					return &(st[index].Elements[i]);
				}	
			}
			printf("\n%s '%s' at line %d Not Found in Symbol Table at scope %d \n", type, name, yylineno, codeScope);
			exit(1);
		}
		
		for(i=0; i<st[index].ele_count; i++)
		{
			if(strcmp(st[index].Elements[i].type, type)==0 && (strcmp(st[index].Elements[i].name, name)==0))
			{
				return &(st[index].Elements[i]);
			}	
		}
//Bob	return findRecord(name, type, st[st[index].parentScopeIndex].scope);
	return findRecord(name, type, codeScope-1);
}

void dispSymtable()
{
	int i = 0, j = 0;

	printf("\n----------------------------------------------------------------Symbol Table----------------------------------------------------------------\n");
	printf("\n  Scope\t\t\tName\t\t\tType\t\t\t\tDeclaration\t\t\tLast Used Line\n\n");
	for(i=0; i<=sIndex; i++)
	{
		for(j=0; j<st[i].ele_count; j++)
		{
//Bob			printf("(%d, %d)\t\t\t%s\t\t\t%s\t\t\t%d\t\t\t\t%d\n", st[i].parentScopeIndex, st[i].symTableScope, st[i].Elements[j].name, st[i].Elements[j].type, st[i].Elements[j].decLine,  st[i].Elements[j].lastLine);
			printf("  %d\t\t\t%s\t\t\t%s\t\t\t%d\t\t\t\t%d\n", st[i].symTableScope, st[i].Elements[j].name, st[i].Elements[j].type, st[i].Elements[j].decLine,  st[i].Elements[j].lastLine);
		}
	}

	printf("-------------------------------------------------------------------------------------------------------------------------------------------------\n");

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
	
	printf("\n-------------------------------------------------------------Abstract Syntax Tree------------------------------------------------\n");
	
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

// Code Optimization

void commonSubexprElim()
{
	int i = 0, j = 0;
		for(i=0; i<qIndex; i++)
		{
			for(j=i+1; j<qIndex; j++)
			{
				if((strcmp(quad_array[i].A1, quad_array[j].A1)==0) && (strcmp(quad_array[i].A2, quad_array[j].A2)==0) && (strcmp(quad_array[i].Op, quad_array[j].Op)==0))
				{
					//quad_array[i].R='\0';
					quad_array[j].I = -1;
				}
			}
		}

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
		if((strcmp(quad_array[i].Op,"=")==0) && (strcmp(quad_array[i].A1,"-")==1) && (strcmp(quad_array[i].A2,"-")==0))
		{
			quad_array[i].R=quad_array[i].A1;
			quad_array[i].A1="-";
		}
        }
}

void strenRed()
{
	for(int i=0; i<qIndex; i++)
	{
		if((strcmp(quad_array[i].Op,"*")==0) && (atoi(quad_array[i].A2)==2) )
		{
			strcpy(quad_array[i].Op,"<<");
			strcpy(quad_array[i].A2,"1");
		}
		if((strcmp(quad_array[i].Op,"/")==0) && (digi(quad_array[i].A2)==2))
		{
			strcpy(quad_array[i].Op,"<<");
			strcpy(quad_array[i].A2,"1");
			//makeQuad(quad_array[i].R,quad_array[i].A1,"1","<<");
		}
        }
}

void printQuads()
{
	printf("\n--------------------------------------------------------------------------------------------------------------------------------------------------\n");
	printf("\n--------------------------------------------------------------------Quadruples---------------------------------------------------------------------\n");
	printf("\nLno.			Oper.			Arg1			Arg2			Res\n\n");

	int i = 0;
	for(i=0; i<qIndex; i++)
	{
		if(quad_array[i].I > -1)
			printf("%d\t\t\t%s\t\t\t%s\t\t\t%s\t\t\t%s\n", quad_array[i].I, quad_array[i].Op, quad_array[i].A1, quad_array[i].A2, quad_array[i].R);
	}
	printf("--------------------------------------------------------------------------------------------------------------------------------------------------\n");
}

void optimization()
{
	//copyProp();
	strenRed();
	//commonSubexprElim();
	//deadCodeElimination();

	printQuads();
	printf("\n");
}

void freeAll()
{
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

%}

%union { char *text; int depth; struct ASTNode* Node;};
%locations
   	  
%token T_EndOfFile T_Return T_Number T_True T_False T_ID T_Print T_Cln T_NL T_EQL T_NEQ T_EQ T_GT T_LT T_EGT T_ELT T_Or T_And T_Not T_In T_Range ID ND DD T_String T_If T_Elif T_For T_While T_Else T_Import T_Break T_Pass T_MN T_PL T_DV T_ML T_OP T_CP T_OB T_CB T_Def T_Comma T_List

%right T_EQL                                          
%left T_PL T_MN
%left T_ML T_DV
%nonassoc T_If
%nonassoc T_Elif
%nonassoc T_Else

%type<Node> StartDebugger args begin_block block end_block func_call call_args StartParse finalStatements arith_exp bool_exp term constant basic_stmt cmpd_stmt func_def list_index import_stmt pass_stmt break_stmt for_stmt while_stmt return_stmt assign_stmt bool_term bool_factor

%%

StartDebugger : {init();} StartParse T_EndOfFile {printf("\n-------------------------------------------------------------------------------------------------------------------------------------------------\nValid Python Syntax\n\n-------------------------------------------------------------Three Address Code--------------------------------------------------------------\n");  /*printAST($2);*/ codeGenOp($2); printQuads(); dispSymtable(); optimization(); freeAll(); exit(0);} ;

constant : T_Number {insertRecord("Constant", $<text>1, @1.first_line, currentScope); $$ = createID_Const("Constant", $<text>1, currentScope);}
         | T_String {insertRecord("Constant", $<text>1, @1.first_line, currentScope); $$ = createID_Const("Constant", $<text>1, currentScope);}
         | T_True {insertRecord("Constant", "True", @1.first_line, currentScope); $$ = createID_Const("Constant", "True", currentScope);}
         | T_False {insertRecord("Constant", "False", @1.first_line, currentScope); $$ = createID_Const("Constant", "False", currentScope);};

term : T_ID {modifyRecordID("Identifier", $<text>1, @1.first_line, currentScope); $$ = createID_Const("Identifier", $<text>1, currentScope);} 
     | constant {$$ = $1;} 
     | list_index {$$ = $1;};


list_index : T_ID T_OB constant T_CB {checkList($<text>1, @1.first_line, currentScope); $$ = createOp("ListIndex", 2, createID_Const("ListTypeID", $<text>1, currentScope), $3);};

StartParse : T_NL StartParse {$$=$2;}| finalStatements T_NL {resetDepth();} StartParse {$$ = createOp("NewLine", 2, $1, $4);}| finalStatements T_NL {$$=$1;};

basic_stmt : pass_stmt {$$=$1;}
           | break_stmt {$$=$1;}
           | import_stmt {$$=$1;}
           | assign_stmt {$$=$1;}
           | arith_exp {$$=$1;}
           | bool_exp {$$=$1;}
           | return_stmt {$$=$1;};

arith_exp : term {$$=$1;}
          | arith_exp  T_PL  arith_exp {$$ = createOp("+", 2, $1, $3);}
          | arith_exp  T_MN  arith_exp {$$ = createOp("-", 2, $1, $3);}
          | arith_exp  T_ML  arith_exp {$$ = createOp("*", 2, $1, $3);}
          | arith_exp  T_DV  arith_exp {$$ = createOp("/", 2, $1, $3);}
          | T_MN arith_exp {$$ = createOp("-", 1, $2);}
          | T_OP arith_exp T_CP {$$ = $2;} ;
		    

bool_exp : bool_term T_Or bool_term {$$ = createOp("or", 2, $1, $3);}
         | arith_exp T_LT arith_exp {$$ = createOp("<", 2, $1, $3);}
         | bool_term T_And bool_term {$$ = createOp("and", 2, $1, $3);}
         | arith_exp T_GT arith_exp {$$ = createOp(">", 2, $1, $3);}
         | arith_exp T_ELT arith_exp {$$ = createOp("<=", 2, $1, $3);}
         | arith_exp T_EGT arith_exp {$$ = createOp(">=", 2, $1, $3);}
         | arith_exp T_In T_ID {checkList($<text>3, @3.first_line, currentScope); $$ = createOp("in", 2, $1, createID_Const("Constant", $<text>3, currentScope));}
         | bool_term {$$=$1;}; 

bool_term : bool_factor {$$ = $1;}
          | arith_exp T_EQ arith_exp {$$ = createOp("==", 2, $1, $3);}
          | T_True {insertRecord("Constant", "True", @1.first_line, currentScope); $$ = createID_Const("Constant", "True", currentScope);}
          | T_False {insertRecord("Constant", "False", @1.first_line, currentScope); $$ = createID_Const("Constant", "False", currentScope);}; 
          
bool_factor : T_Not bool_factor {$$ = createOp("!", 1, $2);}
            | T_OP bool_exp T_CP {$$ = $2;}; 

import_stmt : T_Import T_ID {insertRecord("PackageName", $<text>2, @2.first_line, currentScope); $$ = createOp("import", 1, createID_Const("PackageName", $<text>2, currentScope));};
pass_stmt : T_Pass {$$ = createOp("pass", 0);};
break_stmt : T_Break {$$ = createOp("break", 0);};
return_stmt : T_Return {$$ = createOp("return", 0);};;

assign_stmt : T_ID T_EQL arith_exp {insertRecord("Identifier", $<text>1, @1.first_line, currentScope); $$ = createOp("=", 2, createID_Const("Identifier", $<text>1, currentScope), $3);}  
            | T_ID T_EQL bool_exp {insertRecord("Identifier", $<text>1, @1.first_line, currentScope);$$ = createOp("=", 2, createID_Const("Identifier", $<text>1, currentScope), $3);}   
            | T_ID  T_EQL func_call {insertRecord("Identifier", $<text>1, @1.first_line, currentScope); $$ = createOp("=", 2, createID_Const("Identifier", $<text>1, currentScope), $3);} 
            | T_ID T_EQL T_OB T_CB {insertRecord("ListTypeID", $<text>1, @1.first_line, currentScope); $$ = createID_Const("ListTypeID", $<text>1, currentScope);} ;
	      
finalStatements : basic_stmt {$$ = $1;}
                | cmpd_stmt {$$ = $1;}
                | func_def {$$ = $1;}
                | func_call {$$ = $1;}
                | error T_NL {yyerrok; yyclearin; $$=createOp("SyntaxError", 0);};

cmpd_stmt : while_stmt {$$ = $1;}
		  | for_stmt {$$ = $1;};


while_stmt : T_While bool_exp T_Cln begin_block {$$ = createOp("While", 2, $2, $4);}; 

for_stmt : T_For T_ID T_In T_Range T_OP term T_Comma term T_CP T_Cln begin_block { 
	insertRecord("Identifier", $<text>2, @1.first_line, currentScope); 
	Node* idNode = createID_Const("Identifier", $<text>2, currentScope); 
	e1 = createOp("=", 2, idNode, $<text>6); 
	e2 = createOp(">=", 2, idNode, $<text>6); 
	e3 = createOp("<", 2, idNode, $<text>8); 
	$$ = createOp("For", 4, e1, e2, e3, $11);
} 


begin_block : basic_stmt {$$ = $1;}
            | T_NL ID {
				if(scopeChange) {
					initNewTable(currentScope+1); currentScope++; 
				}
			} 
			finalStatements block {$$ = createOp("BeginBlock", 2, $4, $5);};

block : T_NL ND finalStatements block {$$ = createOp("Next", 2, $3, $4);}
      | T_NL end_block {$$ = $2;};

end_block : DD {if(scopeChange) { currentScope--; scopeChange-- ;}  } finalStatements {$$ = createOp("EndBlock", 1, $3);} 
          | DD { if(scopeChange) { currentScope--; scopeChange-- ;} } {$$ = createOp("EndBlock", 0);}
          | { if(scopeChange) {currentScope--; scopeChange-- ;} $$ = createOp("EndBlock", 0); resetDepth();};

args : T_ID {addToList($<text>1, 1);} args_list {$$ = createOp(argsList, 0); clearArgsList();} 
     | {$$ = createOp("Void", 0);};

args_list : T_Comma T_ID {addToList($<text>2, 0);} args_list | ;


call_list : T_Comma term {addToList($<text>1, 0);} call_list | ;

call_args : T_ID {addToList($<text>1, 1);} call_list {$$ = createOp(argsList, 0); clearArgsList();}
					| T_Number {addToList($<text>1, 1);} call_list {$$ = createOp(argsList, 0); clearArgsList();}
					| T_String {addToList($<text>1, 1);} call_list {$$ = createOp(argsList, 0); clearArgsList();}	
					| {$$ = createOp("Void", 0);};

func_def : T_Def T_ID {
	insertRecord("Func_Name", $<text>2, @2.first_line, currentScope);
	scopeChange++ ; } 
	T_OP args T_CP T_Cln begin_block {
	$$ = createOp("Func_Name", 3, createID_Const("Func_Name", $<text>2, currentScope), $5, $8);};

func_call : T_ID T_OP call_args T_CP {$$ = createOp("Func_Call", 2, createID_Const("Func_Name", $<text>1, currentScope), $3);};

 
%%

void yyerror(const char *msg)
{
	printf("\nSyntax Error at Line %d, Column : %d\n",  yylineno, yylloc.last_column);
	printf("\nyerror msg:%s\n", msg);
	exit(0);
}

int main()
{
	yyparse();
	return 0;
}

