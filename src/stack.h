#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

struct Stack;  // Definición completa en stack.c.
typedef struct Stack Stack;

// Crea una stack.
// Post: devuelve una nueva stack vacía.
Stack *stack_create(void);

// Destruye la stack.
// Pre: la stack fue creada.
// Post: se eliminaron todos los elementos de la stack.
void stack_destroy(Stack *stack);

// Devuelve verdadero si la stack no tiene elementos astackdos, false en caso contrario.
// Pre: la stack fue creada.
bool stack_is_empty(const Stack *stack);

// Agrega un nuevo elemento a la stack. Devuelve falso en caso de error.
// Pre: la stack fue creada.
// Post: se agregó un nuevo elemento a la stack, value es el nuevo tope.
bool push(Stack *stack, void *value);

// Obtiene el value del tope de la stack. Si la stack tiene elementos,
// se devuelve el value del tope. Si está vacía devuelve NULL.
// Pre: la stack fue creada.
// Post: se devolvió el value del tope de la stack, cuando la stack no está
// vacía, NULL en caso contrario.
void *stack_first(const Stack *stack);

// Saca el elemento tope de la stack. Si la stack tiene elementos, se quita el
// tope de la stack, y se devuelve ese value. Si la stack está vacía, devuelve
// NULL.
// Pre: la stack fue creada.
// Post: si la stack no estaba vacía, se devuelve el value del tope anterior
// y la stack contiene un elemento menos.
void *pop(Stack *stack);

#endif  // STACK_H
