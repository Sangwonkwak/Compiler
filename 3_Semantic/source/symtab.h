/****************************************************/
/* File: symtab.h                                   */
/* Symbol table interface for the TINY compiler     */
/* (allows only one symbol table)                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _SYMTAB_H_
#define _SYMTAB_H_

/* SIZE is the size of the hash table */
#define SIZE 211
/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
typedef struct ParameterRec{
	int arg_size;
	char* name[SIZE];
	ExpType type[SIZE];
}ParmList;

typedef struct LineListRec{
	int lineno;
	struct LineListRec* next;
}* LineList;

typedef struct BucketListRec{
	char* name;
	ExpType type;
	LineList lines;
	int memloc;
	struct BucketListRec* next;
}* BucketList;

typedef struct ScopeListRec{
	char* name;
	BucketList bucket[SIZE];
	ParmList parm;
	struct ScopeListRec* next;
	struct ScopeListRec* parent;
}* ScopeList;

extern ScopeList scopeTable[SIZE];
void st_init();
void st_insert( char * scope, char * name, ExpType type, int lineno, int loc );
void scope_make(char *child,char *parent);
void Parm_add(char* scope,TreeNode* t,int index);
int IsFunc(char* temp);
ScopeList scope_return(char* scope);
/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
BucketList st_lookup(char *scope,char *name);
BucketList st_lookup_excluding_parent(char *scope,char *name);
/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing);

#endif
