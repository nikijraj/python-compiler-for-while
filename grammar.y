%{
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdarg.h>
#include<math.h>
#include<ctype.h>

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

FILE* ftac=NULL;
FILE* fquads=NULL;
FILE* fsym=NULL;

int yylex();
void yyerror(const char*);
	
/* --------------------------------------------------------- Structures --------------------------------------------------------------------------------------*/
typedef struct Record
{
	char *type;
	char *name;
	char *datatype;
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
	//for optimisation
	int I;
} Quad;

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
	

/* -------------------------------------------------------- Global Declarations & Definitions -----------------------------------------------------------------------*/

SymTable *st = NULL;
Node *rootNode;
Quad *quad_array = NULL;
Node ***Tree = NULL;
Node * e1, * e2, * e3 = NULL;

char g_dataType[100] = "";
int currentScope = 0;
int scopeChange = 0; //flag
int *arrayScope = NULL;
int sIndex = -1, aIndex = -1, tabCount = 0, tIndex = 0 , label_index = 0, qIndex = 0, NodeCount = 0;
char *argsList = NULL;
char *tString = NULL, *lString = NULL;
int *levelIndices = NULL;

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* -------------------------------------------------------------- Function Prototypes --------------------------------------------------------------------------*/

Record* findRecord(const char *name, const char *type, int codeScope);
Node *pushID_Const(char *value, char *type, int codeScope);
int hashScope(int codeScope);
void updateScope(int codeScope);
void resetDepth();
int scopeBasedTableSearch(int symTableScope);
void initNewTable(int codeScope);
void init();
void insertRecord(const char* type, const char *name, const char* datatype, int lineNo, int codeScope);
int searchRecordInScope(const char* type, const char *name, int index);
void checkList(const char *name, int lineNo, int codeScope);
void dispSymtable();
void freeAll();
void addToList(char *newVal, int flag);
void clearArgsList();
int isBinOp(char *Op);
int tempNum(char* arr);
void optimization();
void printQuads(char *text);
void printAST(Node *root);
void ASTToArray(Node *root, int level);

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* ----------------------------------------------------------------- Print functions ---------------------------------------------------------------------------*/

void dispSymtable()
{
	int i = 0, j = 0;

	fprintf(fsym,"\n---------------------------------------------------------------------------------------------Symbol Table----------------------------------------------------------------------------------------------\n");
//	fprintf(fsym,"\n  Scope\t\t\tName\t\t\tData Type\t\t\tType\t\t\t\tDeclaration\t\t\tLast Used Line\n\n");
	fprintf(fsym,"\n%-35s%-35s%-35s%-35s%-35s%-35s\n\n","Scope","Name","Data Type","Type","Declaration","Last Used Line");
	for(i=0; i<=sIndex; i++)
	{
		for(j=0; j<st[i].ele_count; j++)
		{
//			fprintf(fsym,"  %d\t\t\t%s\t\t\t%s\t\t\t%s\t\t\t%d\t\t\t\t%d\n", st[i].symTableScope, st[i].Elements[j].name, st[i].Elements[j].datatype, st[i].Elements[j].type, st[i].Elements[j].decLine,  st[i].Elements[j].lastLine);
			fprintf(fsym,"%-35d%-35s%-35s%-35s%-35d%-35d\n", st[i].symTableScope, st[i].Elements[j].name, st[i].Elements[j].datatype, st[i].Elements[j].type, st[i].Elements[j].decLine,  st[i].Elements[j].lastLine);
		}
	}

	fprintf(fsym,"-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

}


void printQuads(char *text)
{
	//printf("\n--------------------------------------------------------------------------------------------------------------------------------------------------\n");
	fprintf(fquads,"-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
	fprintf(fquads,"\n-------------------------------------------------------------------------Quadruples %s--------------------------------------------------------------------------------------\n",text);
//	fprintf(fquads,"\n---------------------------------------------------------------Quadruples %s----------------------------------------------------------------\n",text);
	fprintf(fquads,"\nLno.			Oper.			Arg1			Arg2			Res\n\n");

	int i = 0;
	for(i=0; i<qIndex; i++)
	{
		if(quad_array[i].I > -1)
			fprintf(fquads,"%d\t\t\t%s\t\t\t%s\t\t\t%s\t\t\t%s\n", quad_array[i].I, quad_array[i].Op, quad_array[i].A1, quad_array[i].A2, quad_array[i].R);
	}
	fprintf(fquads,"-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}


/*
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
*/
/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* -------------------------------------------------------------- Shared Functions ----------------------------------------------------------------------*/

int pow2(int num)
{
	float x=log2(num);
	int xi= (int)x;

	if(x-xi==0.0)
		return xi;
	else
		return 0;
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
	
char *makeStr(int no, int flag, char *datatype)
{
	//for intermediate code in sym table
	char A[10];
	Xitoa(no, A);
	
	if(flag==1)
	{
			strcpy(tString, "T");
			strcat(tString, A);
			insertRecord("TempVar", tString, datatype, -1, 1);
			return tString;
	}
	else
	{
			strcpy(lString, "L");
			strcat(lString, A);
			insertRecord("TempLabel", lString, "N/A", -1, 1);
			return lString;
	}

}
	
void makeQuad(char *R, char *A1, char *A2, char *Op)
{
	//add a quadrant record 
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
    if((strcmp(Op, "+")==0) || (strcmp(Op, "*")==0) || (strcmp(Op, "/")==0)) {
      return 1;
    } else if ((strcmp(Op, ">=")==0) || (strcmp(Op, "<=")==0) || (strcmp(Op, "<")==0) || (strcmp(Op, ">")==0)) {
      return 2;
    } else if ((strcmp(Op, "==")==0) || (strcmp(Op, "and")==0) || (strcmp(Op, "or")==0)) {
			return 3;
    } else if (strcmp(Op, "in") == 0) {
      return 4;
    }else {
			return 0;
	}
}

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* -------------------------------------------------- Intermediate Code Generation -------------------------------------------------------------------------------*/

void codeGenOp(Node *opNode)
{
	int i=0;
	if(opNode == NULL)
	{
		fprintf(ftac,"opNode is null");
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
				fprintf(ftac,"%s = %s\n",quad_array[i].A1, opNode->id->name);
				
				int n=tempNum(quad_array[i].A1);
				makeQuad(makeStr(n,1, "unknown"), opNode->id->name, "-", "=");
					
			}
			
			else
			{	
				fprintf(ftac,"T%d = %s\n", opNode->nodeNo, opNode->id->name);
				makeQuad(makeStr(opNode->nodeNo, 1, "TempVarType"), opNode->id->name, "-", "=");
			}
		}
		return;
	}
	
	if(strcmp(opNode->NType, "=")==0)
	{
		codeGenOp(opNode->NextLevel[1]); 
	
		fprintf(ftac,"%s = T%d\n", opNode->NextLevel[0]->id->name, opNode->NextLevel[1]->nodeNo);
		makeQuad(opNode->NextLevel[0]->id->name, makeStr(opNode->NextLevel[1]->nodeNo, 1, "TempVarType"), "-", opNode->NType);
		
		return;
	}
	
	int bin = isBinOp(opNode->NType);
	if(bin)
	{
		//node has two children
		
		codeGenOp(opNode->NextLevel[0]);
		codeGenOp(opNode->NextLevel[1]);
				
		char *X1 = (char*)malloc(10);
		char *X2 = (char*)malloc(10);
		char *X3 = (char*)malloc(10);
		
			
    	char* r = (char*)malloc(10);
    	char* t1 = (char*)malloc(10);
	    char* t2 = (char*)malloc(10);
		
   	 	if (bin == 1) {
   	   		strcpy(r, "float");
			strcpy(t1, "float");
			strcpy(t2, "float");
		} else if (bin == 2) {
	 		strcpy(r, "bool");
			strcpy(t1, "float");
			strcpy(t2, "float");
		} else if (bin == 3) {
			strcpy(r, "bool");
			strcpy(t1, "bool");
			strcpy(t2, "bool"); 
		} else if (bin == 4) {
			strcpy(r, "bool");
			strcpy(t1, "unknown");
			strcpy(t2, "list"); 
		}
		strcpy(X1, makeStr(opNode->nodeNo, 1,r));
		
		
		int l=0,r1=0,i=0,j=0,flag=0;
		for(i=0; i<qIndex; i++)
		{
			if( (!opNode->NextLevel[0]->id) || (!opNode->NextLevel[1]->id) )
				break;
			if((l!=1) && (strcmp(opNode->NextLevel[0]->id->name,quad_array[i].R)==0) && (strcmp(quad_array[i].A2,"-")==0) && (strcmp(quad_array[i].Op,"=")==0))			
			{
				l=1;
				j=i;
			}
				
			if((r1!=1) && (strcmp(opNode->NextLevel[1]->id->name,quad_array[i].R)==0) && (strcmp(quad_array[i].A2,"-")==0) && (strcmp(quad_array[i].Op,"=")==0))			
			{
				r1=1;
			}
			
			if(l!=0 && r1!=0)
				break;
			
		}	
		if(l==1 && r1==1)
			flag=1;
		else if(r1==0 && l!=0)
			flag=2;
		else if(r1!=0 && l==0)
			flag=3;	
		

		if(flag==1)
		{
			fprintf(ftac,"T%d = %s %s %s\n", opNode->nodeNo, quad_array[j].A1, opNode->NType, quad_array[i].A1);
			
			int m=tempNum(quad_array[i].A1);
			int n=tempNum(quad_array[j].A1);
			
			strcpy(X2, makeStr(n, 1, t1));
			strcpy(X3, makeStr(m, 1, t2));	
		}
		
		else if(flag==2)
		{
			fprintf(ftac,"T%d = %s %s T%d\n", opNode->nodeNo, quad_array[j].A1, opNode->NType, opNode->NextLevel[1]->nodeNo);
			
			int n=tempNum(quad_array[j].A1);
			strcpy(X2, makeStr(n, 1, t1));
			strcpy(X3, makeStr(opNode->NextLevel[1]->nodeNo, 1, t2));	
		}
		
		else if(flag==3)
		{
			fprintf(ftac,"T%d = T%d %s %s\n", opNode->nodeNo, opNode->NextLevel[0]->nodeNo, opNode->NType, quad_array[i].A1);
			
			int n=tempNum(quad_array[j].A1);
			strcpy(X2, makeStr(opNode->NextLevel[0]->nodeNo, 1, t1));
			strcpy(X3, makeStr(n, 1, t2));	
		}	
		
		else
		{		
			strcpy(X2, makeStr(opNode->NextLevel[0]->nodeNo, 1, t1));
			strcpy(X3, makeStr(opNode->NextLevel[1]->nodeNo, 1, t2));
			fprintf(ftac,"T%d = T%d %s T%d\n", opNode->nodeNo, opNode->NextLevel[0]->nodeNo, opNode->NType, opNode->NextLevel[1]->nodeNo);
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
		fprintf(ftac,"\nL%d: ", label_index);
		makeQuad(makeStr(temp, 0, "LabelType"), "-", "-", "Label");		
		codeGenOp(opNode->NextLevel[1]);
		
		//three address code
		fprintf(ftac,"If False T%d goto L%d\n", opNode->NextLevel[1]->nodeNo, temp+1);
		makeQuad(makeStr(temp+1, 0, "LabelType"), makeStr(opNode->NextLevel[1]->nodeNo, 1, "TempVarType"), "-", "If False");	

		//next level of AST
		codeGenOp(opNode->NextLevel[2]);

		//three address code
		fprintf(ftac,"If False T%d goto L%d\n", opNode->NextLevel[2]->nodeNo, temp+1);
		makeQuad(makeStr(temp+1, 0, "LabelType"), makeStr(opNode->NextLevel[2]->nodeNo, 1, "TempVarType"), "-", "if false");	

		//next level of AST
		codeGenOp(opNode->NextLevel[3]);

		//three address code
		fprintf(ftac,"goto L%d\n", temp);
		makeQuad(makeStr(temp, 0, "LabelType"), "-", "-", "goto");
		fprintf(ftac,"L%d: ", temp+1);
		makeQuad(makeStr(temp+1, 0, "LabelType"), "-", "-", "Label"); 
		label_index = label_index+2;
		return;
	}

	if(strcmp(opNode->NType, "While")==0)
	{
		int temp = label_index;
		
		//next level of AST
		fprintf(ftac,"\nL%d: ", label_index);
		makeQuad(makeStr(temp, 0, "LabelType"), "-", "-", "Label");		
		codeGenOp(opNode->NextLevel[0]);
		
		//three address code
		fprintf(ftac,"If False T%d goto L%d\n", opNode->NextLevel[0]->nodeNo, label_index+1);
		makeQuad(makeStr(temp+1, 0, "LabelType"), makeStr(opNode->NextLevel[0]->nodeNo, 1, "TempVarType"), "-", "If False");
			
		//next level of AST
		codeGenOp(opNode->NextLevel[1]);
		
		//three address code
		fprintf(ftac,"goto L%d\n", temp);
		makeQuad(makeStr(temp, 0, "LabelType"), "-", "-", "goto");
		
		//three address code
		fprintf(ftac,"L%d: ", temp+1);
		fflush(stdout);
		makeQuad(makeStr(temp+1, 0, "LabelType"), "-", "-", "Label"); 
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
		fprintf(ftac,"T%d = %s[%s]\n", opNode->nodeNo, opNode->NextLevel[0]->id->name, opNode->NextLevel[1]->id->name);
		makeQuad(makeStr(opNode->nodeNo, 1, "unknown"), opNode->NextLevel[0]->id->name, opNode->NextLevel[1]->id->name, "=[]");
		return;
	}
	
		
	if(strcmp(opNode->NType, "-")==0)
	{
		if(opNode->opCount == 1)
		{
			codeGenOp(opNode->NextLevel[0]);
			char *X1 = (char*)malloc(10);
			char *X2 = (char*)malloc(10);
			strcpy(X1, makeStr(opNode->nodeNo, 1, "unknown"));
			strcpy(X2, makeStr(opNode->NextLevel[0]->nodeNo, 1, "unknown"));
			fprintf(ftac,"T%d = %s T%d\n", opNode->nodeNo, opNode->NType, opNode->NextLevel[0]->nodeNo);
			makeQuad(X1, X2, "-", opNode->NType);	
		}
		
		else
		{
			codeGenOp(opNode->NextLevel[0]);
			codeGenOp(opNode->NextLevel[1]);
			char *X1 = (char*)malloc(10);
			char *X2 = (char*)malloc(10);
			char *X3 = (char*)malloc(10);
	
			strcpy(X1, makeStr(opNode->nodeNo, 1, "unknown"));
			strcpy(X2, makeStr(opNode->NextLevel[0]->nodeNo, 1, "unknown"));
			strcpy(X3, makeStr(opNode->NextLevel[1]->nodeNo, 1, "unknown"));

			fprintf(ftac,"T%d = T%d %s T%d\n", opNode->nodeNo, opNode->NextLevel[0]->nodeNo, opNode->NType, opNode->NextLevel[1]->nodeNo);
			makeQuad(X1, X2, X3, opNode->NType);
			free(X1);
			free(X2);
			free(X3);
			return;
		
		}
	}
		
	if(strcmp(opNode->NType, "import")==0)
	{
		fprintf(ftac,"import %s\n", opNode->NextLevel[0]->id->name);
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
		fprintf(ftac,"Begin Function %s\n", opNode->NextLevel[0]->id->name);
		makeQuad("-", opNode->NextLevel[0]->id->name, "-", "BeginF");
		codeGenOp(opNode->NextLevel[2]);
		fprintf(ftac,"End Function %s\n", opNode->NextLevel[0]->id->name);
		makeQuad("-", opNode->NextLevel[0]->id->name, "-", "EndF");
		return;
	}
		
	if(strcmp(opNode->NType, "Func_Call")==0)
	{
		if(strcmp(opNode->NextLevel[1]->NType, "Void")==0)
		{
			fprintf(ftac,"(T%d)Call Function %s\n", opNode->nodeNo, opNode->NextLevel[0]->id->name);
			makeQuad(makeStr(opNode->nodeNo, 1, "fun"), opNode->NextLevel[0]->id->name, "-", "Call");
		}
		else
		{
			char A[10];
			char* token = strtok(opNode->NextLevel[1]->NType, ","); 
  			int i = 0;
			while (token != NULL) 
			{
      			    i++; 
			    fprintf(ftac,"Push Param %s\n", token);
			    makeQuad("-", token, "-", "Param"); 
			    token = strtok(NULL, ","); 
			}
				
			fprintf(ftac,"(T%d)Call Function %s, %d\n", opNode->nodeNo, opNode->NextLevel[0]->id->name, i);
			sprintf(A, "%d", i);
			makeQuad(makeStr(opNode->nodeNo, 1, "fun"), opNode->NextLevel[0]->id->name, A, "Call");
			fprintf(ftac,"Pop Params for Function %s, %d\n", opNode->NextLevel[0]->id->name, i);
							
			return;
		}
	}		
	
}

/* ------------------------------------------------------------- AST ------------------------------------------------------------------------------------------------*/

Node *pushID_Const(char *type, char *value, int codeScope)
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

Node *pushOp(char *oper, int opCount, ...)
{
	//AST NType
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
//paste rest of the AST code here

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* ----------------------------------------------------------- Symbol Table -------------------------------------------------------------------------------------------*/

int hashScope(int codeScope)
{
	return pow(codeScope, arrayScope[codeScope] + 1);
}
	
void updateScope(int codeScope)
{
 	currentScope = codeScope;
}
	
void resetDepth()
{
	while(top()) pop();
	depth = 10;
}
	
void initNewTable(int codeScope)
{
	arrayScope[codeScope]++;
	sIndex++;
//	st[sIndex].scope = power(scope, arrayScope[scope]);
	st[sIndex].symTableScope = hashScope(codeScope);

//	st[sIndex].no = sIndex;

	st[sIndex].ele_count = 0;		
	st[sIndex].Elements = (Record*)calloc(MAXRECST, sizeof(Record));
	
	st[sIndex].parentScopeIndex = scopeBasedTableSearch(codeScope-1); 
//	st[sIndex].parentScopeIndex = scopeBasedTableSearch(currentScope); 
}

int scopeBasedTableSearch(int symTableScope)
{
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
		
Record* modifyRecordID(const char *type, const char *name, int lineNo, int codeScope)
{
		int i =0;
		
		//int FScope = power(scope, arrayScope[scope]);
		int FScope = hashScope(codeScope);
		
		int index = scopeBasedTableSearch(FScope);

		if(index==0)
		{
			for(i=0; i<st[index].ele_count; i++)
			{
				
				if(strcmp(st[index].Elements[i].type, type)==0 && (strcmp(st[index].Elements[i].name, name)==0))
				{
					st[index].Elements[i].lastLine = lineNo;
					return &(st[index].Elements[i]);
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
				return &(st[index].Elements[i]);
			}	
		}

		return modifyRecordID(type, name, lineNo, codeScope-1);
}
	
void insertRecord(const char* type, const char *name, const char * datatype, int lineNo, int codeScope)
{ 
		//int FScope = power(scope, arrayScope[scope]);
		int FScope = hashScope(codeScope);
		int index = scopeBasedTableSearch(FScope);

		int RecordIndex = searchRecordInScope(type, name, index);
		if(RecordIndex==-1)
		{
			st[index].Elements[st[index].ele_count].type = (char*)calloc(30, sizeof(char));
			st[index].Elements[st[index].ele_count].name = (char*)calloc(80, sizeof(char));
      		st[index].Elements[st[index].ele_count].datatype = (char*)calloc(80, sizeof(char));
		
			strcpy(st[index].Elements[st[index].ele_count].type, type);	
			strcpy(st[index].Elements[st[index].ele_count].name, name);
			strcpy(st[index].Elements[st[index].ele_count].datatype, datatype);
			st[index].Elements[st[index].ele_count].decLine = lineNo;
			st[index].Elements[st[index].ele_count].lastLine = lineNo;
			st[index].ele_count++;

		}
		else
		{
			st[index].Elements[RecordIndex].lastLine = lineNo;
		}
}

Record* findRecord(const char *name, const char *type, int codeScope)
{
		int i =0;
		
		//int FScope = power(scope, arrayScope[scope]);
		int FScope = hashScope(codeScope);
		
		int index = scopeBasedTableSearch(FScope);

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

	return findRecord(name, type, codeScope-1);
}

//paste rest of the symbol table code here

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
	  
void init()
{
		int i = 0;
		st = (SymTable*)calloc(MAXST, sizeof(SymTable));
		
		arrayScope = (int*)calloc(MAXARRAYSCOPE, sizeof(int));

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

	
void checkList(const char *name, int lineNo, int codeScope)
{
		//int FScope = power(scope, arrayScope[scope]);
		int FScope = hashScope(codeScope);
		
		int index = scopeBasedTableSearch(FScope);
		//int index = scopeBasedTableSearch(scope);
		
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
		
		return checkList(name, lineNo, codeScope-1);

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

/* ----------------------------------------------------------------------------- Code Optimization ----------------------------------------------------------------------*/

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
		printQuads("Common Sub Expression ELimination");

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
		if((strcmp(quad_array[i].Op,"=")==0) && (strcmp(quad_array[i].A1,"-")!=0) && (strcmp(quad_array[i].A2,"-")==0) && ((quad_array[i].A1[0])!='T') && ((quad_array[i].R[0])!='T'))
		{
			strcpy(quad_array[i].R,quad_array[i].A1);
			quad_array[i].A1="-";
		}
        }
}


void constantFolding()
{
	for(int i=0; i<qIndex; i++)
	{				
		if(strcmp(quad_array[i].A2,"-")!=0)
		{
			
			char* l=(char*)malloc(100);
			char* r=(char*)malloc(100);
			int a=0,b=0;
			
			for(int j=0;j<qIndex; j++)
			{
				if((a!=1) && (strcmp(quad_array[i].A1,quad_array[j].R)==0) && (strcmp(quad_array[j].A2,"-")==0))
					{
						strcpy(l,quad_array[j].A1);
						a=1;
					}
				
				
				if((b!=1) && (strcmp(quad_array[i].A2,quad_array[j].R)==0) && (strcmp(quad_array[j].A2,"-")==0))
					{
						strcpy(r,quad_array[j].A1);
						b=1;
					}
					
				if(a==1 && b==1)
					break;				
			}
		
				
			float res=0;
			
			if(strcmp(quad_array[i].Op,"*")==0)
			{	
				res= atof(l) * atof(r); 
				char* res_str=(char*)malloc(100);;
				sprintf(res_str, "%f", res);
				
				quad_array[i].A1=res_str;
				quad_array[i].A2="-";
				quad_array[i].Op="=";
			}
			
			else if(strcmp(quad_array[i].Op,"+")==0)
			{	
				res= atof(l) + atof(r); 
				char* res_str=(char*)malloc(100);;
				sprintf(res_str, "%f", res);
				
				quad_array[i].A1=res_str;
				quad_array[i].A2="-";
				quad_array[i].Op="=";
			}
			
			else if(strcmp(quad_array[i].Op,"/")==0)
			{	res= atof(l) / atof(r); 
				char* res_str=(char*)malloc(100);;
				sprintf(res_str, "%f", res);
				
				quad_array[i].A1=res_str;
				quad_array[i].A2="-";
				quad_array[i].Op="=";
				
			}
			
			else if(strcmp(quad_array[i].Op,"<<")==0)
			{	int res1= (atoi(l)<<atoi(quad_array[i].A2)); 
				char* res_str=(char*)malloc(100);;
				sprintf(res_str, "%d", res1);
				
				quad_array[i].A1=res_str;
				quad_array[i].A2="-";
				quad_array[i].Op="=";
				
			}
			
			else if(strcmp(quad_array[i].Op,">>")==0)
			{	int res1= (atoi(l)>>atoi(quad_array[i].A2)); 
				char* res_str=(char*)malloc(100);;
				sprintf(res_str, "%d", res1);
				
				quad_array[i].A1=res_str;
				quad_array[i].A2="-";
				quad_array[i].Op="=";
				
			}			
		}
		
		else
			continue;
	
	}
	printQuads("Constant Folding");
}

void strengthRedn()
{
	for(int i=0; i<qIndex; i++)
	{
		if(strcmp(quad_array[i].A2,"-")!=0)
		{
			for(int j=0; j<qIndex; j++)
			{
				if((strcmp(quad_array[i].Op,"*")==0) && (strcmp(quad_array[i].A2,quad_array[j].R)==0) && (strcmp(quad_array[j].A2,"-")==0))
				{
					char* ns=(char*)malloc(20);
					int n = pow2(atoi(quad_array[j].A1));

					if(n>0)
					{
						quad_array[i].Op="<<";
						sprintf(ns, "%d", n);
						strcpy(quad_array[i].A2,ns);
					}
				}
				else if((strcmp(quad_array[i].Op,"/")==0) && (strcmp(quad_array[i].A2,quad_array[j].R)==0) && (strcmp(quad_array[j].A2,"-")==0))
				{
					char* ns=(char*)malloc(20);

					int n = pow2(atoi(quad_array[j].A1));

					if(n>0)
					{
						quad_array[i].Op=">>";
						sprintf(ns, "%d", n);
						strcpy(quad_array[i].A2,ns);
					}
					/*else
					{
						float m = (1/(atof(quad_array[j].A1)));
						quad_array[i].Op="*";
						sprintf(ns, "%f", m);
					}*/
				}
			}
		}
	}
	printQuads("Strength Reduction");
}

void constantProp()
{
	for(int i=0; i<qIndex; i++)
	{				
		if(strcmp(quad_array[i].A2,"-")==0 && (atoi(quad_array[i].A1)!=0))
		{
			for(int j=i+1; j<qIndex; j++)
			{
				if((strcmp(quad_array[i].R, quad_array[j].A1)==0) && (atoi(quad_array[j].A1)!=0) )
				{
					strcpy(quad_array[j].A1,quad_array[i].A1);
				}
				if((strcmp(quad_array[i].R, quad_array[j].A2)==0) && (atoi(quad_array[j].A2)!=0) )
				{
					strcpy(quad_array[j].A2,quad_array[i].A1);
				}
			}
		}
	}
	printQuads("Constant Propagation");
}

void optimization()
{
	//Calling optimization functions
	
	strengthRedn();
	commonSubexprElim();
	constantProp();
	constantFolding();
	deadCodeElimination();

	//printQuads("Post Optimization");
	printQuads("Dead Code Elimination");
	printf("\n");
}

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------*/



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

%type<Node> RunCode args begin_block block end_block func_call call_args BeginParse finalStatements arith_exp bool_exp term constant basic_stmt cmpd_stmt func_def list_index import_stmt pass_stmt break_stmt for_stmt while_stmt return_stmt assign_stmt bool_term bool_factor

%%

RunCode : {init();} BeginParse T_EndOfFile {ftac=fopen("TAC.txt","w"); fquads=fopen("Quads.txt","w"); fsym=fopen("Symtable.txt","w"); printf("\n-------------------------------------------------------------------------------------------------------------------------------------------------\nValid Python Syntax!\n-------------------------------------------------------------------------------------------------------------------------------------------------\n"); fprintf(ftac,"-------------------------------------------------------------Three Address Code--------------------------------------------------------------\n");  /*printAST($2);*/ codeGenOp($2); printQuads(""); dispSymtable(); optimization(); freeAll(); exit(0);} ;

constant : T_Number {insertRecord("Constant", $<text>1, "int", @1.first_line, currentScope);strcpy(g_dataType, "int");  $$ = pushID_Const("Constant", $<text>1, currentScope);}
         | T_String {insertRecord("Constant", $<text>1, "str", @1.first_line, currentScope); strcpy(g_dataType, "str"); $$ = pushID_Const("Constant", $<text>1, currentScope);}
         | T_True {insertRecord("Constant", "True", "bool", @1.first_line, currentScope); strcpy(g_dataType, "bool"); $$ = pushID_Const("Constant", "True", currentScope);}
         | T_False {insertRecord("Constant", "False", "bool", @1.first_line, currentScope); strcpy(g_dataType, "bool"); $$ = pushID_Const("Constant", "False", currentScope);};
term : T_ID {Record *element = modifyRecordID("Identifier", $<text>1, @1.first_line, currentScope); strcpy(g_dataType, element->datatype); $$ = pushID_Const("Identifier", $<text>1, currentScope);} 
     | constant {$$ = $1;} 
     | list_index {$$ = $1;};


list_index : T_ID T_OB constant T_CB {checkList($<text>1, @1.first_line, currentScope); $$ = pushOp("ListIndex", 2, pushID_Const("ListTypeID", $<text>1, currentScope), $3);};

BeginParse : T_NL BeginParse {$$=$2;}| finalStatements T_NL {resetDepth();} BeginParse {$$ = pushOp("NewLine", 2, $1, $4);}| finalStatements T_NL {$$=$1;};

basic_stmt : pass_stmt {$$=$1;}
           | break_stmt {$$=$1;}
           | import_stmt {$$=$1;}
           | assign_stmt {$$=$1;}
           | arith_exp {$$=$1;}
           | bool_exp {$$=$1;}
           | return_stmt {$$=$1;};

arith_exp : term {$$=$1;}
          | arith_exp  T_PL  arith_exp {$$ = pushOp("+", 2, $1, $3);}
          | arith_exp  T_MN  arith_exp {$$ = pushOp("-", 2, $1, $3);}
          | arith_exp  T_ML  arith_exp {$$ = pushOp("*", 2, $1, $3);}
          | arith_exp  T_DV  arith_exp {$$ = pushOp("/", 2, $1, $3);}
          | T_MN arith_exp {$$ = pushOp("-", 1, $2);}
          | T_OP arith_exp T_CP {$$ = $2;} ;
		    

bool_exp : bool_term T_Or bool_term {$$ = pushOp("or", 2, $1, $3);}
         | arith_exp T_LT arith_exp {$$ = pushOp("<", 2, $1, $3);}
         | bool_term T_And bool_term {$$ = pushOp("and", 2, $1, $3);}
         | arith_exp T_GT arith_exp {$$ = pushOp(">", 2, $1, $3);}
         | arith_exp T_ELT arith_exp {$$ = pushOp("<=", 2, $1, $3);}
         | arith_exp T_EGT arith_exp {$$ = pushOp(">=", 2, $1, $3);}
         | arith_exp T_In T_ID {checkList($<text>3, @3.first_line, currentScope); $$ = pushOp("in", 2, $1, pushID_Const("Constant", $<text>3, currentScope));}
         | bool_term {$$=$1;}; 

bool_term : bool_factor {$$ = $1;}
          | arith_exp T_EQ arith_exp {$$ = pushOp("==", 2, $1, $3);}
          | T_True {insertRecord("Constant", "True", "bool", @1.first_line, currentScope); $$ = pushID_Const("Constant", "True", currentScope);}
          | T_False {insertRecord("Constant", "False", "bool", @1.first_line, currentScope); $$ = pushID_Const("Constant", "False", currentScope);}; 
          
bool_factor : T_Not bool_factor {$$ = pushOp("!", 1, $2);}
            | T_OP bool_exp T_CP {$$ = $2;}; 

import_stmt : T_Import T_ID {insertRecord("PackageName", $<text>2, "N/A", @2.first_line, currentScope); $$ = pushOp("import", 1, pushID_Const("PackageName", $<text>2, currentScope));};
pass_stmt : T_Pass {$$ = pushOp("pass", 0);};
break_stmt : T_Break {$$ = pushOp("break", 0);};
return_stmt : T_Return {$$ = pushOp("return", 0);};;

assign_stmt : T_ID T_EQL arith_exp { if(!strlen(g_dataType)) { strcpy(g_dataType, "float"); } insertRecord("Identifier", $<text>1, g_dataType, @1.first_line, currentScope); strcpy(g_dataType,""); $$ = pushOp("=", 2, pushID_Const("Identifier", $<text>1, currentScope), $3);}  
            | T_ID T_EQL bool_exp {insertRecord("Identifier", $<text>1, "bool", @1.first_line, currentScope);$$ = pushOp("=", 2, pushID_Const("Identifier", $<text>1, currentScope), $3);}   
            | T_ID  T_EQL func_call {insertRecord("Identifier", $<text>1, "unknown return", @1.first_line, currentScope); $$ = pushOp("=", 2, pushID_Const("Identifier", $<text>1, currentScope), $3);} 
            | T_ID T_EQL T_OB T_CB {insertRecord("ListTypeID", $<text>1, "list", @1.first_line, currentScope); $$ = pushID_Const("ListTypeID", $<text>1, currentScope);} ;
	      
finalStatements : basic_stmt {$$ = $1;}
                | cmpd_stmt {$$ = $1;}
                | func_def {$$ = $1;}
                | func_call {$$ = $1;}
                | error T_NL {yyerrok; yyclearin; $$=pushOp("SyntaxError", 0);};

cmpd_stmt : while_stmt {$$ = $1;}
		  | for_stmt {$$ = $1;};


while_stmt : T_While bool_exp T_Cln begin_block {$$ = pushOp("While", 2, $2, $4);}; 

for_stmt : T_For T_ID T_In T_Range T_OP term T_Comma term T_CP T_Cln begin_block { 
	insertRecord("Identifier", $<text>2, "int", @1.first_line, currentScope); 
	Node* idNode = pushID_Const("Identifier", $<text>2, currentScope); 
	e1 = pushOp("=", 2, idNode, $<text>6); 
	e2 = pushOp(">=", 2, idNode, $<text>6); 
	e3 = pushOp("<", 2, idNode, $<text>8); 
	$$ = pushOp("For", 4, e1, e2, e3, $11);
}; 


begin_block : basic_stmt {$$ = $1;}
            | T_NL ID {
				if(scopeChange) {
					initNewTable(currentScope+1); currentScope++; 
				}
			} 
			finalStatements block {$$ = pushOp("BeginBlock", 2, $4, $5);};

block : T_NL ND finalStatements block {$$ = pushOp("Next", 2, $3, $4);}
      | T_NL end_block {$$ = $2;};

end_block : DD {if(scopeChange) { currentScope--; scopeChange-- ;}  } finalStatements {$$ = pushOp("EndBlock", 1, $3);} 
          | DD { if(scopeChange) { currentScope--; scopeChange-- ;} } {$$ = pushOp("EndBlock", 0);}
          | { if(scopeChange) {currentScope--; scopeChange-- ;} $$ = pushOp("EndBlock", 0); resetDepth();};

args : T_ID {addToList($<text>1, 1);} args_list {$$ = pushOp(argsList, 0); clearArgsList();} 
     | {$$ = pushOp("Void", 0);};

args_list : T_Comma T_ID {addToList($<text>2, 0);} args_list | ;


call_list : T_Comma term {addToList($<text>1, 0);} call_list | ;

call_args : T_ID {addToList($<text>1, 1);} call_list {$$ = pushOp(argsList, 0); clearArgsList();}
					| T_Number {addToList($<text>1, 1);} call_list {$$ = pushOp(argsList, 0); clearArgsList();}
					| T_String {addToList($<text>1, 1);} call_list {$$ = pushOp(argsList, 0); clearArgsList();}	
					| {$$ = pushOp("Void", 0);};

func_def : T_Def T_ID {
	insertRecord("Func_Name", $<text>2, "fun", @2.first_line, currentScope);
	scopeChange++ ; } 
	T_OP args T_CP T_Cln begin_block {
	$$ = pushOp("Func_Name", 3, pushID_Const("Func_Name", $<text>2, currentScope), $5, $8);};

func_call : T_ID T_OP call_args T_CP {$$ = pushOp("Func_Call", 2, pushID_Const("Func_Name", $<text>1, currentScope), $3);};

 
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

