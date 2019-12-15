/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"
#include "util.h"
#include <string.h>

/* counter for variable memory locations */
static int exp_type_check(TreeNode* t,char *scope);
static int checkNode(TreeNode* t,char* scope);
static void typeError(TreeNode* t,char* message);
static void pre_Error(TreeNode* t,char* message);
static int location = 0;
static int number = -1;//counting for if,while statement
static int cur_loc=0;
static int cur_num=-1;
static char* error_box[30];
static int error_num = 0;
/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc 
 * in postorder to tree pointed to by t
 */
static void traverse( char* scope, TreeNode * t,
               int (* preProc) (TreeNode *, char*),
               int (* postProc) (TreeNode *, char*))
{ char origin_scope[20]; int next_location, next_number;
	strcpy(origin_scope,scope);
	if (t != NULL)
  { preProc(t,scope);
	  next_location = cur_loc;
	  next_number = cur_num;
    { int i;
      for (i=0; i < MAXCHILDREN; i++)
        traverse(scope,t->child[i],preProc,postProc);
    }
    postProc(t,scope);
	strcpy(scope,origin_scope); location = next_location; number = next_number;
    traverse(scope,t->sibling,preProc,postProc);
  }
}

/* nullProc is a do-nothing procedure to 
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static int nullProc(TreeNode * t, char* scope)
{ if (t==NULL) return -2;
  else return -2;
}

/* Procedure insertNode inserts 
 * identifiers stored in t into 
 * the symbol table 
 */
static void pre_Error(TreeNode * t, char * message)
{ 
	int i;
	char* temp = (char*)malloc(sizeof(char)*100);
	sprintf(temp,"Error at line %d: %s",t->lineno,message);
	for(i=0;i<error_num;i++)
		if(strcmp(temp,error_box[i]) == 0) return;
	error_box[error_num++] = temp;
	Error = TRUE;
}

static int scope_chase(TreeNode* t,char* scope){
	switch(t->nodekind){
		case DecK:
			switch(t->kind.dec){
				case FuncK:
					strcpy(scope,t->attr.name);//scope change
					cur_num = number;
					number = -1;
					break;
				case VarK:
					cur_num = number;
					break;
				default:
					break;
			}
			break;
		case StmtK:
		  switch(t->kind.stmt){
			  case CmstmtK:
				  if(number != -1){
				  char* temp_child =(char*)malloc(sizeof(char)*(strlen(scope)+10));
				  sprintf(temp_child,"%s.%d",scope,number++);
				  strcpy(scope,temp_child);//scope change
				  cur_num = number;
				  number = 0;
				  }
				  else {
					  number++;
					  cur_num = number;
				  }
				  break;
			  default:
				  break;
		  }
		  break;
	  default:
		  break;
	}
	return -2;
}

static int insertNode( TreeNode * t, char* scope)
{ BucketList bucket;
	ScopeList l;
	char message_box[100];
	switch (t->nodekind)
  { 
	  case DecK:
		  switch(t->kind.dec){
			  case FuncK:
				  bucket = st_lookup("global",t->attr.name);
				  if(bucket != NULL){
					  sprintf(message_box,"Redefined function(%s)",t->attr.name);
					 pre_Error(t,message_box);
				  }
				  st_insert(scope,t->attr.name,t->type,t->lineno,location++);
				  scope_make(t->attr.name,scope);
				  strcpy(scope,t->attr.name);//scope change
				  cur_loc = location;
				  cur_num = number;
				  location = 0;
				  number = -1;
				  break;
			  case VarK:
			  case VararrK:
				  bucket = st_lookup(scope,t->attr.name);
				  if(bucket != NULL){
					  sprintf(message_box,"Redefined variable(%s)",t->attr.name);
					  pre_Error(t,message_box);
				  }
				  st_insert(scope,t->attr.name,t->type,t->lineno,location++);
				  cur_loc = location;
				  cur_num = number;
				  break;
			  default:
				  break;
		  }
		  break;
	  case ParK:
		  switch(t->kind.par){
			  case SinpK:
				  if(t->type == Void){
					  cur_loc = location;
					  break;
				  }
				  st_insert(scope,t->attr.name,t->type,t->lineno,location++);
				  Parm_add(scope,t,location-1);
				  cur_loc = location;
				  break;
			  case ArrpK:
				  if(t->type == Void){
					  cur_loc = location;
					  break;
				  }
				  st_insert(scope,t->attr.name,t->type,t->lineno,location++);
				  Parm_add(scope,t,location-1);
				  cur_loc = location;
				  break;
			  default:
				  break;
		  }
		  break;
	  case StmtK:
		  switch(t->kind.stmt){
			  case CmstmtK:
				  if(number != -1){
				  char* temp_child =(char*)malloc(sizeof(char)*(strlen(scope)+10));
				  sprintf(temp_child,"%s.%d",scope,number++);
				  scope_make(temp_child,scope);
				  strcpy(scope,temp_child);//scope change
				  cur_num = number;
				  cur_loc = location;
				  location = 0;
				  number = 0;
				  }
				  else {
					  number++;
					  cur_loc = location;
					  cur_num = number;
				  }
				  break;
			  default:
				  break;
		  }
		  break;
	  case ExpK:
		  switch(t->kind.exp){
			  case CallK:
				  if(st_lookup("global",t->attr.name) == NULL){
					//  printf("%dline: %s undefined function\n",t->lineno,t->attr.name);
				  }
				  else
					  st_insert("global",t->attr.name,t->type,t->lineno,0);
				  break;
			  case IdK:
				  if(st_lookup(scope,t->attr.name) == NULL){
					 // printf("%dline: %s undefined variable in %s\n",t->lineno,t->attr.name,scope);
				  }
				  else
					  st_insert(scope,t->attr.name,t->type,t->lineno,0);
				  break;
			  default:
				  break;
		  }
		  break;
	  default:
		  break;
  }
  return -2;
}

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode * syntaxTree)
{ char* scope = (char*)malloc(sizeof(char)*20);
	strcpy(scope,"global");
	st_init(); location += 2;
	traverse(scope,syntaxTree,insertNode,nullProc);
  if (TraceAnalyze)
  { fprintf(listing,"\nSymbol table:\n\n");
    printSymTab(listing);
  }
}

static void typeError(TreeNode * t, char * message)
{ 
	int i;
	char* temp = (char*)malloc(sizeof(char)*100);
	sprintf(temp,"Type error at line %d: %s",t->lineno,message);
	for(i=0;i<error_num;i++)
		if(strcmp(temp,error_box[i]) == 0) return;
	error_box[error_num++] = temp;
	Error = TRUE;
}

//not meaning -2, error occur in low level -1, void 0, int 1, int[] 2
static int exp_type_check(TreeNode* t,char *scope){
	int k;
	switch(t->kind.exp){
		case IdK:
			k = checkNode(t,scope);
			return k;
		case ConstK:
			return 1;
		case CallK:
			k = checkNode(t,scope);
			return k;
		case AssignK:
			k = checkNode(t,scope);
			return k;
		case OpK:
			k = checkNode(t,scope);
			return k;
		default:
			break;
	}
	return -2;
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
static int checkNode(TreeNode * t,char *scope){
	int a,b,arg_num,i,k;
	ScopeList l;
	BucketList bucket;
	TreeNode* tt;
	char message_box[100];
switch (t->nodekind){
	case DecK:
		switch(t->kind.exp){
			case VarK:
			case VararrK:
				if(t->type == 0){
					sprintf(message_box,"Void is not invalid type in variable declaration(%s)",t->attr.name);
					typeError(t,message_box);
				}
				break;
			default:
				break;
		}
		break;
	case ParK:
		switch(t->kind.par){
			case SinpK:
				if( (t->type == 0) && (t->attr.name != NULL) ){
					sprintf(message_box,"Void is not invalid type in parameter(%s)",t->attr.name);
					typeError(t,message_box);
				}
				break;	
			case ArrpK:
				if(t->type == 0){
					sprintf(message_box,"Void is not invalid type in parameter(%s)",t->attr.name);
					typeError(t,message_box);
				}
				break;
			default:
				break;
		}
		break;
	case StmtK:
		switch(t->kind.stmt){
			case ReturnK:
				tt = t->child[0];
				bucket = st_lookup("global",scope);
				if(tt == NULL){
					if(bucket->type != Void){
						typeError(t,"Return type Error");
						return -1;
					}
				}
				else{
					a = exp_type_check(tt,scope);
					if(bucket->type == 0){
						typeError(t,"'return' with a value, in function returning void");
						return -1;
					}
					if(a == -1) return -1;
					if(a == bucket->type) return -2;
					else{
						typeError(t,"Return type Error");
						return -1;
					}
				}
				break;
			default:
				break;
		}
		break;
	case ExpK:
		switch(t->kind.exp){
			case IdK:
				tt = t->child[0];
				l = scope_return(scope);
				if(tt != NULL){
					a = exp_type_check(tt,scope);
					if(a != 1){
						typeError(t,"Inside [] type error");
						return -1;
					}
					while(l != NULL){
						bucket = st_lookup(l->name,t->attr.name);
						if(bucket != NULL){ 
							if(bucket->type == Intarr)
								return 1;//Int return
							else{
								sprintf(message_box,"%s should be IntegerArray",t->attr.name);
								typeError(t,message_box);
								return -1;
							}
						}
						l = l->parent;
					}
				}
				else{	
					l = scope_return(scope);
					while(l != NULL){
						bucket = st_lookup(l->name,t->attr.name);
						if(bucket != NULL)
							return bucket->type;
						l = l->parent;
					}
				}
				sprintf(message_box,"Undefined variable(%s)",t->attr.name);
				pre_Error(t,message_box);
				return -1;
				break;
			case OpK:
				a = exp_type_check(t->child[0],scope);
				b = exp_type_check(t->child[1],scope);
				k = t->attr.op;
				if(k==PLUS || k==MINUS || k==TIMES || k==OVER)
				if(a==-1 || b==-1) return -1;
				if(a != b){
					typeError(t,"Types of operands are not equal");
					return -1;
				}
				if(k==PLUS || k==MINUS || k==TIMES || k==OVER){
					if(a == 2){
						typeError(t,"Arithmetic operations on IntegerArray type!");
						return -1;
					}
				}
				return a;
				break;
			case CallK:
				l = scope_return(t->attr.name);
				//Whether defined function check 
				if((l == NULL) || !IsFunc(t->attr.name)){
					sprintf(message_box,"Undefined function(%s)",t->attr.name);
					pre_Error(t,message_box);
					return -1;
				}
				if(t->child[0] == NULL) arg_num=0;
				else{
					arg_num=1;
					TreeNode* temp = t->child[0];
					while(temp->sibling != NULL) {
						arg_num++;
						temp = temp->sibling;
					}
				}
				//argument num check
				if(arg_num != l->parm.arg_size){
					typeError(t,"Argument number is not equal");
					return -1;
				}
				TreeNode* temp2 = t->child[0];
				//argument type check
				for(i=0;i<arg_num;i++){
					k = exp_type_check(temp2,scope);
					if(k == -1) return -1;
					if( l->parm.type[i] != k){
						typeError(t,"Argument type error");
						return -1;
					}
					temp2 = temp2->sibling;
				}
				return t->type;
			case AssignK:
				a = exp_type_check(t->child[0],scope);
				b = exp_type_check(t->child[1],scope);
				if(a==-1 || b==-1) return -1;
				if(a != b){
					typeError(t,"Assign type is not equal");
					return -1;
				}
				return a;
			default:
				break;
		}
		break;
	default:
		break;
}
return -2;
}

/* Procedure typeCheck performs type checking 
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode * syntaxTree){
	char *scope = (char*)malloc(sizeof(char)*20);
	strcpy(scope,"global");
	number = -1;
	cur_num = -1;
	traverse(scope,syntaxTree,scope_chase,checkNode);
	int i;
	for(i=0;i<error_num;i++)
		fprintf(listing,"%s\n",error_box[i]);
}

