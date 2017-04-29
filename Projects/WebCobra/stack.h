#ifndef _stack_h_
#define _stack_h_

typedef struct
{
	int stack_size;
	void** sp;
	int nb;
} stackT;

stackT* stack_init(int n);

void stack_push(stackT* s, void* data);

void* stack_pop(stackT* s);

int stack_empty(stackT* s);

#endif