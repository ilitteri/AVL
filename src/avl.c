#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "avl.h"
#include "stack.h"

struct Node
{
    struct Node *left;
    struct Node *right;
    char *key;
    void *data;
};

struct AVL
{
    Node *root;
    avl_cmp_key cmp;
    avl_destroy_data destroy_data;
    size_t count;
};

struct AVL_Iter
{
    Stack *states;
};

static Node *node_create(const char *key, void *data);
static Node *search_node(Node *current, Node *previous, const char *key, avl_cmp_key cmp, Stack *insertion_walk);
static void destroy_node(Node *current, avl_destroy_data destroy_data)
static void _avl_destroy(Node *current, avl_destroy_data destroy_data)

static Node *node_create(const char *key, void *data)
{
    Node *node = malloc(sizeof(Node));

    if (node == NULL)
    {
        return NULL;
    }

    char *copy = malloc(strlen(key)+1);

    if (copy == NULL)
    {
        free(node);
        return NULL;
    }

    strcpy(copy, key);

    node->key = copy;
    node->data = data;
    node->left = NULL;
    node->right = NULL;

    return node;
}

AVL *avl_crear(avl_cmp_key cmp, avl_destroy_data destroy_data)
{
    AVL *avl;

    if ((avl = malloc(sizeof(AVL))) == NULL)
    {
        return NULL;
    }

    avl->root = NULL;
    avl->cmp = cmp;
    avl->destroy_data = destroy_data;
    avl->count = 0;

    return avl;
}

static Node *search_node(Node *current, Node *previous, const char *key, avl_cmp_key cmp, Stack *insertion_walk)
{
    if (current == NULL)
    {
        return previous;
    }

    if (insertion_walk != NULL)
    {
        push(insertion_walk, current);
    }

    int comparison = cmp(key, current->key);

    return comparison == 0 ? current : comparison < 0 ? search_node(current->left, current, key, cmp)
                                                       : search_node(current->right, current, key, cmp);
}

static bool avl_condition(Node *z)
{
    if (z == NULL)
    {
        return false;
    }
    return abs(node_height(z->left) - node_height(z->right)) <= 1
}

static void is_avl(Stack *insertion_walk)
{
    Node *x, *y, *z, *a;

    x = pop(insertion_walk);
    y = pop(insertion_walk);
    z = pop(insertion_walk);
    a = pop(insertion_walk);

    bool avl = true;

    while (avl &= avl_condition(z))
    {
        x = y;
        y = z;
        z = a;
        a = pop(insertion_walk);
    }

    if (!avl)
    {
       balance_tree(a, z, y, x);
    }
}

bool avl_save(AVL *avl, const char *key, void *data)
{
    if (avl == NULL)
    {
        return false;
    }

    if (avl->count == 0)
    {
        if ((avl->root = node_create(key, data)) == NULL)
        {
            return false;
        }
    }

    else
    {
        Stack *insertion_walk = stack_create();
        Node *aux = search_node(avl->root, NULL, key, avl->cmp, insertion_walk);
        int comparison = avl->cmp(key, aux->key);

        if (comparison == 0)
        {
            if (avl->destroy_data != NULL)
            {
                avl->destroy_data(aux->data);
            }
            aux->data = data;
            return true;
        }

        else if (comparison < 0)
        {
            if ((aux->izq = node_create(key, data)) == NULL)
            {
                return false;
            }
        }

        else if (comparison > 0)
        {
            if ((aux->der = node_create(key, data)) == NULL)
            {
                return false;
            }
        }

        is_avl(insertion_walk);
    }

    avl->count++;

    return true;
}

void *avl_get(const AVL *avl, const char *key)
{
    if (avl == NULL)
    {
        return NULL;
    }

    Node *node;

    if ((node = search_node(avl->root, NULL, key, avl->cmp)) == NULL)
    {
        return NULL;
    }

    return avl->cmp(node->key, key) == 0 ? node->data : NULL;
}

bool avl_belongs(const AVL *avl, const char *key)
{
    if (avl == NULL)
    {
        return false;
    }

    Node *node;

    if ((node = buscar_nodo(avl->root, NULL, key, avl->cmp)) == NULL)
    {
        return false;
    }

    return avl->cmp(node->key, key) == 0;
}

size_t avl_count(AVL *avl)
{
    return avl == NULL ? 0 : avl->count;
}

static void destroy_node(Node *current, avl_destroy_data destroy_data)
{
    if (destroy_data != NULL)
    {
        destroy_data(current->data);
    }
    free(current->key);
    free(current);
}

static void _avl_destroy(Node *current, avl_destroy_data destroy_data)
{
    if (current == NULL)
    {
        return;
    }

    if (current->izq != NULL)
    {
        _avl_destroy(current->izq, destroy_data);
    }

    if (current->der != NULL)
    {
        _avl_destroy(current->der, destroy_data);
    }

    destroy_node(current, destroy_data);
}

void avl_destroy(AVL *avl)
{
    _avl_destroy(avl->root, avl->destroy_data);
    free(avl);
}

void enqueue_nodes(Node *current, Stack *states)
{
    if (current == NULL)
    {
        return;
    }
    stack_enqueue(states, current);
    enqueue_nodes(current->left, states); //apilar states iniciales
}

AVL_Iter *avl_iter_in_create(const AVL *avl)
{
    if (avl == NULL)
    {
        return NULL;
    }

    AVL_Iter *iter;

    if ((iter = malloc(sizeof(AVL_Iter))) == NULL)
    {
        return NULL;
    }

    if ((iter->states = stack_create()) == NULL)
    {
        free(iter);
        return NULL;
    }

    enqueue_nodes(avl->root, iter->states);

    return iter;
}

bool avl_iter_in_forward(AVL_Iter *iter)
{
    if (stack_is_empty(iter->states))
    {
        return false;
    }

    Node *current;

    if ((current = stack_dequeue(iter->states)) == NULL)
    {
        return false;
    }

    establecer_orden_iteracion(current->der, iter->states);

    return true;
}

const char *avl_iter_in_get_actual(const AVL_Iter *iter)
{
    return stack_is_empty(iter->states) ? NULL : ((Node *)stack_first(iter->states))->key;
}

bool avl_iter_in_finished(const AVL_Iter *iter)
{
    return stack_is_empty(iter->states);
}

void avl_iter_in_destroy(AVL_Iter *iter)
{
    stack_destroy(iter->states);
    free(iter);
}
