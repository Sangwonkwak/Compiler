/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "symtab.h"

/* SIZE is the size of the hash table */

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4

/* the hash function */
static int hash ( char * key )
{ int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  { temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

/* the list of line numbers of the source 
 * code in which a variable is referenced
 */
/* The record in the bucket lists for
 * each variable, including name, 
 * assigned memory location, and
 * the list of line numbers in which
 * it appears in the source code
 */

/* the hash table */
//static BucketList hashTable[SIZE];
ScopeList scopeTable[SIZE];
/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise break;
 gnored
 */
/* "global"scope make and input,output function set */
void st_init(){
	int h,h1,h2;
	h = hash("global");
	ScopeList l = (ScopeList)malloc(sizeof(struct ScopeListRec));
	ScopeList l2;
	l->name = copyString("global");
	l->parent = NULL;
	l->next = scopeTable[h];
	scopeTable[h] = l;

	st_insert(l->name,copyString("input"),Int,0,0);
	scope_make("input","global");
	l2 = scope_return("input");
	l2->parm.arg_size = 0;
	st_insert(l->name,copyString("output"),Void,0,1);
	scope_make("output","global");
	l2 = scope_return("output");
	l2->parm.type[0] = Int; l2->parm.arg_size = 1;
}

/* scope make and parent set */	
void scope_make(char *child, char *parent){
	int h = hash(child);
	int h1 = hash(parent);
	ScopeList l1 = scopeTable[h1];
	while( strcmp(parent,l1->name)!=0 )
		l1 = l1->next;
	ScopeList l = (ScopeList)malloc(sizeof(struct ScopeListRec));
	l->name = child;
	l->parent = l1;
	l->next = scopeTable[h];
	scopeTable[h] = l;
	//printf("new scope: %s, parent scope: %s\n",l->name,l1->name);//	
}

void Parm_add(char* scope,TreeNode* t,int index){
	int h = hash(scope);
	ScopeList l = scopeTable[h];
	while( strcmp(l->name,scope) != 0 )
		l = l->next;
	if(t->type == Void){
		l->parm.arg_size = 0;
		return ;
	}
	l->parm.arg_size = index+1;
	l->parm.name[index] = t->attr.name;
	l->parm.type[index] = t->type;
}

ScopeList scope_return(char *scope){
	int h = hash(scope);
	ScopeList l = scopeTable[h];
	while((l!= NULL) && (strcmp(l->name,scope) !=0))
		l = l->next;
	if(l == NULL) return NULL;
	else return l;
}

void st_insert( char* scope, char * name, ExpType type, int lineno, int loc )
{
	int h1 = hash(scope), h2 = hash(name);
	ScopeList l1 = scopeTable[h1];
	while((l1 != NULL) && (strcmp(scope,l1->name) != 0))
		l1 = l1->next;
		BucketList l2 = l1->bucket[h2];
		while((l2 != NULL)&&(strcmp(name,l2->name) !=0))
			l2=l2->next;
		if(l2 == NULL){
			l2 = (BucketList)malloc(sizeof(struct BucketListRec));
			l2->name = name;
			l2->type = type;
			l2->lines = (LineList)malloc(sizeof(struct LineListRec));
			l2->lines->lineno = lineno;
			l2->lines->next = NULL;
			l2->memloc = loc;
			l2->next = l1->bucket[h2];
			l1->bucket[h2] = l2;
			//printf("%s: type:%d\n",name,type);
		}
		else{
			LineList t = l2->lines;
			while(t->next != NULL) t= t->next;
			t->next = (LineList)malloc(sizeof(struct LineListRec));
			t->next->lineno = lineno;
			t->next->next = NULL;
		}
}
/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
BucketList st_lookup ( char *scope, char * name )
{ 
	int h1 = hash(scope),h2 = hash(name);
	ScopeList l1 = scopeTable[h1];
	BucketList l2;
	while( (l1 != NULL) && (strcmp(scope,l1->name) != 0))
		l1 = l1->next;
	if(l1 == NULL) return NULL;
	else{
		l2 = l1->bucket[h2];
		while((l2 != NULL) && (strcmp(name,l2->name) != 0))
			l2 = l2->next;
		if(l2 == NULL) return NULL;
		else return l2;
	}
}

BucketList st_lookup_excluding_parent(char *scope, char *name){
	return NULL;
}

int IsFunc(char *temp){
	int h = hash(temp);
	ScopeList l = scopeTable[h];
	while( (l != NULL) && (strcmp(l->name,temp) != 0))
		l = l->next;
	if(l == NULL) return 0;
	else return 1;
}

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing)
{	int i,j,k;
	char* temp[3];
	temp[0] = "Void";temp[1] = "Integer";temp[2] = "IntegerArray";
	fprintf(listing,"< Symbol Table >\n");
	fprintf(listing,"Variable Name  Variable Type  Scope Name  Location  Line Numbers\n");
	fprintf(listing,"-------------  -------------  ----------  --------  ------------\n");
	for(i=0;i<SIZE;i++){
		ScopeList l = scopeTable[i];
		while(l != NULL){
			for(j=0;j<SIZE;j++){
				BucketList b = l->bucket[j];
				while(b != NULL){
					LineList ll = b->lines;
					fprintf(listing,"%-13s  ",b->name);
					if( !strcmp(l->name,"global") && IsFunc(b->name))
						fprintf(listing,"%-13s  ","Function");
					else fprintf(listing,"%-13s  ",temp[b->type]);
					fprintf(listing,"%-10s  ",l->name);
					fprintf(listing,"%-8d  ",b->memloc);
					while(ll != NULL){
						fprintf(listing,"%d  ",ll->lineno);
						ll = ll->next;
					}
					fprintf(listing,"\n");
					b = b->next;
				}
			}
			l = l->next;
		}
	}
}

