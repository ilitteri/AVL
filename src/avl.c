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

typedef enum Descendancy
{
    ROOT,
    LEFT_NODE,
    RIGHT_NODE
} Descendancy;

/* Creation auxiliar functions */
static Node *node_create(const char *key, void *data);
static Node *search_node(Node *current, Node *previous, const char *key, avl_cmp_key cmp, Stack *insertion_walk);
/* Destroy auxiliar functions */
static void destroy_node(Node *current, avl_destroy_data destroy_data);
static void _avl_destroy(Node *current, avl_destroy_data destroy_data);
/* Balance auxiliar functions */
static void left_left_rot(Node *a, Node *z, Node *y, Node *x, Descendancy z_des);
static void right_right_rot(Node *a, Node *z, Node *y, Node *x, Descendancy z_des);
static void left_right_rot(Node *z, Node *y, Node *x);
static void right_left_rot(Node *z, Node *y, Node *x);
static void update_root(AVL *avl, Node *node);
static void rotate_tree(AVL *avl, Node *a, Node *z, Node *y, Node *x, avl_cmp_key cmp);
static size_t node_height(const Node *current);
static bool avl_condition(Node *z);
static void balance_tree(AVL *avl, Stack *insertion_walk);
/* Remove auxiliar functions */
static Node *replace_node(Node *current);
static void remove_no_children(AVL *avl, Node *previous, Node *current, Descendancy child, bool is_root);
static void remove_one_child(AVL *avl, Node *previous, Node *current, Descendancy relacion_act_ant, Descendancy current_child, bool is_root);
static bool remove_two_children(AVL *avl, Node *previous, Node *current);
static void analyze_paternity(AVL *avl, Node *previous, Node *current, const char *key, void **data, Descendancy relacion_act_ant);
static bool _avl_remove(AVL *avl, Node *previous, Node *current, const char *key, void **data, Descendancy relacion_act_ant);

    static Node *node_create(const char *key, void *data)
{
    Node *node = malloc(sizeof(Node));

    if (node == NULL)
    {
        return NULL;
    }

    char *copy = malloc(strlen(key) + 1);

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

static void left_left_rot(Node *a, Node *z, Node *y, Node *x, Descendancy z_des)
{
    if (z_des == LEFT_NODE)
    {
        a->left = y;
    }

    else if (z_des == RIGHT_NODE)
    {
        a->right = y;
    }

    z->left = y->right;
    y->right = z;
}

static void right_right_rot(Node *a, Node *z, Node *y, Node *x, Descendancy z_des)
{
    if (z_des == LEFT_NODE)
    {
        a->left = y;
    }

    else if (z_des == RIGHT_NODE)
    {
        a->right = y;
    }

    z->right = y->left;
    y->left = z;
}

static void left_right_rot(Node *z, Node *y, Node *x)
{
    z->left = x;
    y->right = x->left;
    x->left = y;
}

static void right_left_rot(Node *z, Node *y, Node *x)
{
    z->right = x;
    y->left = x->right;
    x->right = y;
}

static void update_root(AVL *avl, Node *node)
{
    avl->root = node;
}

static void rotate_tree(AVL *avl, Node *a, Node *z, Node *y, Node *x, avl_cmp_key cmp)
{
    int z_des = a != NULL ? cmp(z->key, a->key) : ROOT;
    int y_des = cmp(y->key, z->key);
    int x_des = cmp(x->key, y->key);

    if (y_des < 0 && x_des < 0)
    {
        left_left_rot(a, z, y, x, z_des < 0 ? LEFT_NODE : z_des == 0 ? ROOT
                                                                     : RIGHT_NODE);
        if (a == NULL)
        {
            update_root(avl, y);
        }
    }
    if (y_des > 0 && x_des > 0)
    {
        right_right_rot(a, z, y, x, z_des < 0 ? LEFT_NODE : z_des == 0 ? ROOT: RIGHT_NODE);
        if (a == NULL)
        {
            update_root(avl, y);
        }
    }
    if (y_des < 0 && x_des > 0)
    {
        left_right_rot(z, y, x);
        left_left_rot(a, z, x, y); // "x" in "y" param pos 'cause of the previous rotation.
        if (a == NULL)
        {
            update_root(avl, x);
        }
    }
    if (y_des > 0 && x_des < 0)
    {
        right_left_rot(z, y, x);
        right_right_rot(a, z, x, y); // "x" in "y" param pos 'cause of the previous rotation.
        if (a == NULL)
        {
            update_root(avl, x);
        }
    }
}

static size_t node_height(const Node *current)
{
    if (current == NULL)
    {
        return 0;
    }

    size_t left_height = node_height(current->left) + 1;
    size_t right_height = node_height(current->right) + 1;

    return left_height >= right_height ? left_height : right_height;
}

static bool avl_condition(Node *z)
{
    if (z == NULL)
    {
        return false;
    }
    return abs(node_height(z->left) - node_height(z->right)) <= 1
}

static void balance_tree(AVL *avl, Stack *walk)
{
    Node *x, *y, *z, *a;

    x = pop(walk);
    y = pop(walk);
    z = pop(walk);
    a = pop(walk);

    bool avl = true;

    while (avl &= avl_condition(z))
    {
        x = y;
        y = z;
        z = a;
        a = pop(walk);
    }

    if (!avl)
    {
        rotate_tree(avl, a, z, y, x);
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
        Stack *insertion_walk;
        if ((insertion_walk = stack_create()) == NULL)
        {
            return false;
        }

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

        balance_tree(insertion_walk);
    }

    avl->count++;

    return true;
}

static Node *replace_node(Node *current)
{
    Node *ant = current;
    current = current->der;

    if (current->izq == NULL)
    {
        ant->der = current->der != NULL ? current->der : NULL;
        return current;
    }

    while (current->izq != NULL)
    {
        ant = current;
        current = current->izq;
    }

    ant->izq = current->der != NULL ? current->der : NULL;

    return current;
}

static void remove_no_children(AVL *avl, Node *previous, Node *current, Descendancy child, bool is_root)
{
    if (is_root)
    {
        avl->raiz = NULL;
        return;
    }

    if (child == RIGHT_NODE)
    {
        previous->der = NULL;
    }

    else
    {
        previous->izq = NULL;
    }
}

static void remove_one_child(AVL *avl, Node *previous, Node *current, Descendancy relacion_act_ant, Descendancy current_child, bool is_root)
{
    if (is_root)
    {
        avl->raiz = current_child == RIGHT_NODE ? current->der : current->izq;
        return;
    }

    if (relacion_act_ant == LEFT_NODE)
    {
        previous->izq = current_child == RIGHT_NODE ? current->der : current->izq;
    }

    else
    {
        previous->der = current_child == RIGHT_NODE ? current->der : current->izq;
    }
}

static bool remove_two_children(AVL *avl, Node *previous, Node *current)
{
    Node *replacement = replace_node(current);

    free(current->key);
    char *copy = malloc(strlen(replacement->key) + 1);
    if (copy == NULL)
    {
        return false;
    }
    strcpy(copy, replacement->key);

    current->key = copy;
    current->data = replacement->data;

    destroy_node(replacement, NULL);

    return true;
}

static void analyze_paternity(AVL *avl, Node *previous, Node *current, const char *key, void **data, Descendancy relacion_act_ant)
{
    bool is_root = false;
    *data = current->data;

    if (previous == NULL)
    {
        is_root = true;
    }

    if (current->izq == NULL && current->der == NULL)
    {
        remove_no_children(avl, previous, current, relacion_act_ant, is_root);
        destroy_node(current, NULL);
    }

    else if (current->izq == NULL || current->der == NULL)
    {
        remove_one_child(avl, previous, current, relacion_act_ant, current->izq != NULL ? LEFT_NODE : RIGHT_NODE, is_root);
        destroy_node(current, NULL);
    }

    else
        remove_two_children(avl, previous, current);
}

static bool _avl_remove(AVL *avl, Node *previous, Node *current, const char *key, void **data, Descendancy relacion_act_ant, Stack *remove_walk)
{
    if (current == NULL)
    {
        return false;
    }

    if (remove_walk != NULL)
    {
        push(remove_walk, current);
    }

    int cmp = avl->cmp(key, current->key);

    if (cmp == 0)
    {
        analyze_paternity(avl, previous, current, key, data, relacion_act_ant);
    }

    else if (cmp > 0)
    {
        return _avl_remove(avl, current, current->der, key, data, RIGHT_NODE, remove_walk);
    }

    else
    {
        return _avl_remove(avl, current, current->izq, key, data, LEFT_NODE, remove_walk);
    }

    return true;
}

void *avl_remove(AVL *avl, const char *key)
{
    if (avl == NULL || avl->count == 0)
    {
        return NULL;
    }

    void *data = NULL;
    
    Stack *remove_walk = stack_create();
    if ((remove_walk = stack_create()) == NULL)
    {
        return NULL;
    }

    if (!_avl_remove(avl, NULL, avl->raiz, key, &data, 0, remove_walk))
    {
        return NULL;
    }

    balance_tree(avl, remove_walk);

    avl->count--;

    return data;
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

const char *avl_iter_in_get_current(const AVL_Iter *iter)
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
