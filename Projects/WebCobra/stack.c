#include "stack.h"
#include <stdlib.h>
#include <string.h>

stackT* stack_init(int n)
{
	stackT* s = malloc(sizeof(stackT));
	s->stack_size = n;
	s->sp = malloc(s->stack_size*sizeof(char*));
	s->nb=0;
	return s;
}

void stack_doublesize(stackT* s)
{
	void** new_sp;

	new_sp = malloc(2 * s->stack_size*sizeof(char*));
	memcpy(new_sp, s->sp, s->stack_size*sizeof(char*) );
	s->stack_size *= 2;
	free(s->sp);
	s->sp = new_sp;
}

void stack_push(stackT* s, void* data)
{
	if (s->nb==s->stack_size-1)
		stack_doublesize(s);
	s->sp[s->nb] = data;
	s->nb++;
}

void* stack_pop(stackT* s)
{
	return s->sp[--s->nb];
}

int stack_empty(stackT* s)
{
	return s->nb==0;
}



