#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

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
static Node *search_node(Node *current, Node *previous, const char *key, avl_cmp_key cmp, Stack *walk);
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
static int node_height(const Node *current);
static bool avl_condition(Node *z);
static void balance_tree(AVL *avl, Stack *insertion_walk);
/* Remove auxiliar functions */
static Node *replace_node(Node *current);
static void remove_no_children(AVL *avl, Node *previous, Node *current, Descendancy child, bool is_root);
static void remove_one_child(AVL *avl, Node *previous, Node *current, Descendancy relacion_act_ant, Descendancy current_child, bool is_root);
static bool remove_two_children(AVL *avl, Node *previous, Node *current);
static void analyze_paternity(AVL *avl, Node *previous, Node *current, const char *key, void **data, Descendancy relacion_act_ant);
static bool _avl_remove(AVL *avl, Node *previous, Node *current, const char *key, void **data, Descendancy relacion_act_ant, char **parent_of_removed_key);

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

AVL *avl_create(avl_cmp_key cmp, avl_destroy_data destroy_data)
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

static Node *search_node(Node *current, Node *previous, const char *key, avl_cmp_key cmp, Stack *walk)
{
    if (current == NULL)
    {
        return previous;
    }

    if (walk != NULL)
    {
        push(walk, current);
    }

    int comparison = cmp(key, current->key);

    return comparison == 0 ? current : comparison < 0 ? search_node(current->left, current, key, cmp, walk)
                                                      : search_node(current->right, current, key, cmp, walk);
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
        left_left_rot(a, z, y, x, z_des < 0 ? LEFT_NODE : z_des == 0 ? ROOT : RIGHT_NODE);
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
        left_left_rot(a, z, x, y, z_des < 0 ? LEFT_NODE : z_des == 0 ? ROOT: RIGHT_NODE); // "x" in "y" param pos 'cause of the previous rotation.
        if (a == NULL)
        {
            update_root(avl, x);
        }
    }
    if (y_des > 0 && x_des < 0)
    {
        right_left_rot(z, y, x);
        right_right_rot(a, z, x, y, z_des < 0 ? LEFT_NODE : z_des == 0 ? ROOT: RIGHT_NODE); // "x" in "y" param pos 'cause of the previous rotation.
        if (a == NULL)
        {
            update_root(avl, x);
        }
    }
}

static int node_height(const Node *current)
{
    if (current == NULL)
    {
        return 0;
    }

    int left_height = node_height(current->left) + 1;
    int right_height = node_height(current->right) + 1;

    return left_height >= right_height ? left_height : right_height;
}

static bool avl_condition(Node *z)
{
    if (z == NULL)
    {
        return true;
    }
    return abs(node_height(z->left) - node_height(z->right)) <= 1;
}

static void balance_tree(AVL *avl, Stack *walk)
{
    Node *x, *y, *z, *a;

    x = (Node *)pop(walk);
    y = (Node *)pop(walk);
    z = (Node *)pop(walk);
    a = (Node *)pop(walk);

    bool is_avl = true;

    while ((is_avl &= avl_condition(z)) && a != NULL)
    {
        x = y;
        y = z;
        z = a;
        a = (Node *)pop(walk);
    }

    if (!is_avl)
    {
        rotate_tree(avl, a, z, y, x, avl->cmp);
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
            avl->count--;
        }

        else if (comparison < 0)
        {
            if ((aux->left = node_create(key, data)) == NULL)
            {
                return false;
            }
        }

        else if (comparison > 0)
        {
            if ((aux->right = node_create(key, data)) == NULL)
            {
                return false;
            }
        }

        balance_tree(avl, insertion_walk);

        while (!stack_is_empty(insertion_walk))
        {
            pop(insertion_walk);
        }
        stack_destroy(insertion_walk);
    }

    avl->count++;

    return true;
}

static Node *replace_node(Node *current)
{
    Node *ant = current;
    current = current->right;

    if (current->left == NULL)
    {
        ant->right = current->right != NULL ? current->right : NULL;
        return current;
    }

    while (current->left != NULL)
    {
        ant = current;
        current = current->left;
    }

    ant->left = current->right != NULL ? current->right : NULL;

    return current;
}

static void remove_no_children(AVL *avl, Node *previous, Node *current, Descendancy child, bool is_root)
{
    if (is_root)
    {
        avl->root = NULL;
        return;
    }

    if (child == RIGHT_NODE)
    {
        previous->right = NULL;
    }

    else
    {
        previous->left = NULL;
    }
}

static void remove_one_child(AVL *avl, Node *previous, Node *current, Descendancy relacion_act_ant, Descendancy current_child, bool is_root)
{
    if (is_root)
    {
        avl->root = current_child == RIGHT_NODE ? current->right : current->left;
        return;
    }

    if (relacion_act_ant == LEFT_NODE)
    {
        previous->left = current_child == RIGHT_NODE ? current->right : current->left;
    }

    else
    {
        previous->right = current_child == RIGHT_NODE ? current->right : current->left;
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

    if (current->left == NULL && current->right == NULL)
    {
        remove_no_children(avl, previous, current, relacion_act_ant, is_root);
        destroy_node(current, NULL);
    }

    else if (current->left == NULL || current->right == NULL)
    {
        remove_one_child(avl, previous, current, relacion_act_ant, current->left != NULL ? LEFT_NODE : RIGHT_NODE, is_root);
        destroy_node(current, NULL);
    }

    else
        remove_two_children(avl, previous, current);
}

static bool _avl_remove(AVL *avl, Node *previous, Node *current, const char *key, void **data, Descendancy relacion_act_ant, char **parent_of_removed_key)
{
    if (current == NULL)
    {
        return false;
    }

    int cmp = avl->cmp(key, current->key);

    if (cmp == 0)
    {
        analyze_paternity(avl, previous, current, key, data, relacion_act_ant);
        if (previous != NULL)
        {
            *parent_of_removed_key = previous->key;
        }
    }

    else if (cmp > 0)
    {
        return _avl_remove(avl, current, current->right, key, data, RIGHT_NODE, parent_of_removed_key);
    }

    else
    {
        return _avl_remove(avl, current, current->left, key, data, LEFT_NODE, parent_of_removed_key);
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

    char *parent_of_remove_key = NULL;

    if (!_avl_remove(avl, NULL, avl->root, key, &data, 0, &parent_of_remove_key))
    {
        return NULL;
    }
    
    if (parent_of_remove_key != NULL)
    {
        Stack *remove_walk;
        if ((remove_walk = stack_create()) == NULL)
        {
            return NULL;
        }
        
        search_node(avl->root, NULL, parent_of_remove_key, avl->cmp, remove_walk);

        balance_tree(avl, remove_walk);

        while (!stack_is_empty(remove_walk))
        {
            pop(remove_walk);
        }
        stack_destroy(remove_walk);
    }

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

    if ((node = search_node(avl->root, NULL, key, avl->cmp, NULL)) == NULL)
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

    if ((node = search_node(avl->root, NULL, key, avl->cmp, NULL)) == NULL)
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

    if (current->left != NULL)
    {
        _avl_destroy(current->left, destroy_data);
    }

    if (current->right != NULL)
    {
        _avl_destroy(current->right, destroy_data);
    }

    destroy_node(current, destroy_data);
}

void avl_destroy(AVL *avl)
{
    _avl_destroy(avl->root, avl->destroy_data);
    free(avl);
}

static void _avl_in_order(Node *current, bool func(const char *, void *, void *), void *extra, bool *visit)
{
    if (current == NULL || func == NULL || !(*visit))
    {
        return;
    }

    // left - current - Der
    _avl_in_order(current->left, func, extra, visit);
    if (*visit)
    {
        *visit &= func(current->key, current->data, extra);
    }
    _avl_in_order(current->right, func, extra, visit);
}

void avl_in_order(AVL *avl, bool func(const char *, void *, void *), void *extra)
{
    if (avl == NULL)
    {
        return;
    }
    bool visit = true;
    _avl_in_order(avl->root, func, extra, &visit);
}

void enqueue_nodes(Node *current, Stack *states)
{
    if (current == NULL)
    {
        return;
    }
    push(states, current);
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

    if ((current = pop(iter->states)) == NULL)
    {
        return false;
    }

    enqueue_nodes(current->right, iter->states);

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
