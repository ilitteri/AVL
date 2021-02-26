#include <stdlib.h>

#include "stack.h"

#define SIZE 8
#define ENLARGE_FACTOR 2
#define REDUCTION_FACTOR 0.5
#define REDUCTION_LIMIT 0.25

struct Stack
{
    void **data;
    unsigned count;
    unsigned size;
};

Stack *stack_create()
{
    Stack *stack;

    if ((stack = malloc(sizeof(Stack))) == NULL)
    {
        return NULL;
    }

    stack->count = 0;
    stack->size = SIZE;

    if ((stack->data = malloc(sizeof(void *) * stack->size)) == NULL)
    {
        free(stack);
        return NULL;
    }

    return stack;
}

void stack_destroy(Stack *stack)
{
    free(stack->data);
    free(stack);
}

bool stack_resize(Stack *stack, unsigned size)
{
    void *temp = realloc(stack->data, sizeof(void *) * (SIZE));

    if (temp == NULL)
    {
        return false;
    }

    stack->size = SIZE;
    stack->data = temp;

    return true;
}

bool stack_is_empty(const Stack *stack)
{
    return !(stack->count);
}

bool push(Stack *stack, void *valor)
{
    if (stack->count == stack->size)
    {
        if (!stack_resize(stack, (unsigned)(stack->size * ENLARGE_FACTOR)))
        {
            return false;
        }
    }

    stack->data[stack->count++] = valor;

    return true;
}

void *stack_first(const Stack *stack)
{
    return stack->count ? stack->data[stack->count - 1] : NULL;
}

void *pop(Stack *stack)
{
    if (stack->count == (unsigned)(stack->size * REDUCTION_LIMIT))
    {
        if (!stack_resize(stack, (unsigned)(stack->size * REDUCTION_FACTOR)))
        {
            return NULL;
        }
    }

    return stack->count ? stack->data[--stack->count] : NULL;
}
