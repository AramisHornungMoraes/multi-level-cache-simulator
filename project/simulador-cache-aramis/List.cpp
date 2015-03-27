/*
AHM Engine
Copyright © 2013 Aramis Hornung Moraes.

This file is part of the AHM Engine project. See List.h for more information about the license.
*/

#include <stdio.h>
#include <stdlib.h>
#include "List.h"

struct node *AllocateNode()
{
	struct node *n;
	n = (struct node*) malloc(sizeof(struct node));
	n->data = NULL;

	n->prev = NULL;
	n->next = NULL;

	return n;
}

struct list *AllocateList()
{
	struct list *l;
	l = (struct list*) malloc(sizeof(struct list));
	l->first = NULL;
	l->last = NULL;
	return l;
}

int IsListEmpty(struct list *l)
{
	if(l->first == NULL && l->first == NULL)
		return 1;
	else return 0;
}

void InsertFirst(struct list *l, int data)
{
	struct node *n = AllocateNode();

	n->data = data;
	if(!IsListEmpty(l))
    {
		n->next = l->first;
        l->first = n;
    }
    else
    {
        n->next = NULL;
        l->first=l->last = n;
    }
}

void InsertAfter(struct list *l, struct node *rn, int data)
{
	struct node *n = AllocateNode();
	n->data = data;
	n->next = rn->next;
	rn->next = n;
    if(n->next == NULL)
        l->last = n;
}

void InsertLast(struct list *l, int data)
{
	if(!IsListEmpty(l))
        InsertAfter(l, l->last, data);
    else
        InsertFirst(l, data);
}


int RemoveFirst(struct list *l)
{
    int aux = NULL;
    struct node *n;
	if(!IsListEmpty(l))
    {
        n = l->first;
        aux = l->first->data;
        l->first = l->first->next;
    }
    return aux;
}

int RemoveLast(struct list *l)
{
    int aux = NULL;
	if(!IsListEmpty(l))
    {
        struct node *n;
        aux = l->last->data;
        n = l->first;
		if(l->first == l->last)
		{
			free(l->first);
			l->first = l->last = NULL;
		}
		else
		{
			while(n->next->next != NULL)
				n = n->next;
			RemoveAfter(l, n);
		}
    }
    return aux;
}

int RemoveAfter(struct list *l, struct node *n)
{
    int aux = NULL;
    if(n->next != NULL)
    {
		aux = n->next->data;
		if(n->next->next != NULL)
		{
			n->next = n->next->next;
			free(n->next);
		}
		else
		{
			n->next = NULL;
			l->last = n;
		}
    }
    return aux;
}

void PrintList(struct list *l)
{
	node *fn = l->first;
	while(fn != NULL)
	{
		printf("%s\n", fn->data);
		fn = fn->next;
	}
}

void CleanList(list *l)
{
	node *fn = l->first;
	while(fn != NULL)
	{
		node *aux = fn->next;
		free(fn);
		fn = aux;
	}
	l->first = l->last = NULL;
}