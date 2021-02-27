#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // For ssize_t in Linux.

#include "../avl.h"
#include "testing.h"

#define VOLUMEN 5000
#define INT_ARRAY_LENGHT_6 6
#define INT_ARRAY_LENGTH_7 7
#define STRING_LENGTH 7

#define MIN_RAND 0
#define MAXIMO_RAND 99999999

static void swap(const char *x, const char *y) 
{ 
    const char *temp = x; 
    x = y; 
    y = temp; 
} 

static void prueba_crear_avl_vacio() {
    AVL *avl = avl_create(strcmp, NULL);

    print_test("AVL created", avl != NULL);
    print_test("AVL coun is 0", avl_count(avl) == 0);
    print_test("Remove a node in an empty tree returns NULL", avl_remove(avl, "hello") == NULL);
    print_test("Ger a node in an empty tree returns NULL", avl_get(avl, "hello") == NULL);
    avl_destroy(avl);
    print_test("Destroy AVL", true);
}

static void prueba_iterar_avl_vacio() {
    AVL* avl = avl_create(strcmp, NULL);
    AVL_Iter* iter = avl_iter_in_create(avl);
    print_test("AVL test create empty iter in", iter);
    print_test("AVL test iter is at the end", avl_iter_in_finished(iter));
    print_test("AVL test iter forward is false", !avl_iter_in_forward(iter));
    print_test("AVL test iter get current is NULL", !avl_iter_in_get_current(iter));

    avl_iter_in_destroy(iter);
    avl_destroy(avl);
}

static void avl_save_test() {
    AVL *avl = avl_create(strcmp, NULL);

    char* keys[STRING_LENGTH] = {"M", "D", "A", "E", "O", "T", "U"};

    print_test("AVL coun is 0", avl_count(avl) == 0);
    bool ok = true;
    for (size_t i = 0; i < STRING_LENGTH; i++) {
        ok &= avl_save(avl, keys[i], NULL) && avl_count(avl) == i+1;
        ok &= avl_get(avl, keys[i]) == NULL;
    }
    print_test("Save key array", ok);
    avl_destroy(avl);
    print_test("Destroy AVL", true);
}

static void avl_replace_test() {
    AVL *avl = avl_create(strcmp, NULL);

    int a = 13;
    int b = 16;
    char* keys[STRING_LENGTH] = {"M", "D", "A", "E", "O", "T", "U"};

    print_test("AVL coun is 0", avl_count(avl) == 0);
    bool ok = true;
    for (size_t i = 0; i < STRING_LENGTH; i++) {
        ok &= avl_save(avl, keys[i], &a) && avl_count(avl) == i+1;
    }
    for (size_t i = 0; i < STRING_LENGTH; i++) {
        ok &= avl_get(avl, keys[i]) == &a;
    }
    print_test("Save key array", ok);
    for (size_t i = 0; i < STRING_LENGTH; i++) {
        ok &= avl_save(avl, keys[i], &b) && avl_count(avl) == STRING_LENGTH;
    }
    for (size_t i = 0; i < STRING_LENGTH; i++){
        ok &= avl_get(avl, keys[i]) == &b;
    }
    print_test("Replace key array", ok);
    avl_destroy(avl);
    print_test("Destroy AVL", true);
}

static void avl_replace_with_destroy_test() {
    AVL *avl = avl_create(strcmp, free);
    
    int *values0[INT_ARRAY_LENGTH_7];
    int *values1[INT_ARRAY_LENGTH_7];
    for (size_t i = 0; i < STRING_LENGTH; i++) {
        values0[i] = malloc(sizeof(int));
        values1[i] = malloc(sizeof(int));
    }
    char* keys[STRING_LENGTH] = {"M", "D", "A", "E", "O", "T", "U"};

    print_test("AVL coun is 0", avl_count(avl) == 0);
    bool ok = true;
    for (size_t i = 0; i < STRING_LENGTH; i++) {
        ok &= avl_save(avl, keys[i], values0[i]) && avl_count(avl) == i+1;
        ok &= avl_get(avl, keys[i]) == values0[i];
    }
    print_test("Save 7 keys with destroy", ok);
    for (size_t i = 0; i < STRING_LENGTH; i++) {
        ok &= avl_save(avl, keys[i], values1[i]) && avl_count(avl) == 7;
        ok &= avl_get(avl, keys[i]) == values1[i];
    }
    print_test("Replace 7 keys with destroy", ok);
    avl_destroy(avl);
    print_test("Destroy AVL", true);
}

static void avl_remove_test() {
    AVL* avl = avl_create(strcmp, NULL);

    char *key1 = "perro", *value1 = "guau";
    char *key2 = "gato", *value2 = "miau";
    char *key3 = "vaca", *value3 = "mu";

    print_test("AVL test save key1", avl_save(avl, key1, value1));
    print_test("AVL test save key2", avl_save(avl, key2, value2));
    print_test("AVL test save key3", avl_save(avl, key3, value3));

    print_test("AVL test key3 belongs, is true", avl_belongs(avl, key3));
    print_test("AVL test remove key3, is value3", avl_remove(avl, key3) == value3);
    print_test("AVL test remove key3, is NULL", !avl_remove(avl, key3));
    print_test("AVL test key3 belongs, is false", !avl_belongs(avl, key3));
    print_test("AVL test get key3, is NULL", !avl_get(avl, key3));
    print_test("AVL test count is 2", avl_count(avl) == 2);

    print_test("AVL test key1 belongs, is true", avl_belongs(avl, key1));
    print_test("AVL test remove key1, is value1", avl_remove(avl, key1) == value1);
    print_test("AVL test remove key1, is NULL", !avl_remove(avl, key1));
    print_test("AVL test key1 belongs, is false", !avl_belongs(avl, key1));
    print_test("AVL test get key1, is NULL", !avl_get(avl, key1));
    print_test("AVL test count is 1", avl_count(avl) == 1);

    print_test("AVL test key2 belongs, is true", avl_belongs(avl, key2));
    print_test("AVL test remove key2, is value2", avl_remove(avl, key2) == value2);
    print_test("AVL test remove key2, is NULL", !avl_remove(avl, key2));
    print_test("AVL test key2 belongs, is false", !avl_belongs(avl, key2));
    print_test("AVL test get key2, is NULL", !avl_get(avl, key2));
    print_test("AVL test count is 0", avl_count(avl) == 0);

    avl_destroy(avl);

}

static void avl_empty_key_test()
{
    AVL* avl = avl_create(strcmp, NULL);

    char *key = "", *value = "";

    print_test("AVL test save key vacia", avl_save(avl, key, value));
    print_test("AVL test count is 1", avl_count(avl) == 1);
    print_test("AVL test get key vacia is value", avl_get(avl, key) == value);
    print_test("AVL test key  belongsvacia, is true", avl_belongs(avl, key));
    print_test("AVL test remove key vacia, is value", avl_remove(avl, key) == value);
    print_test("AVL test count is 0", avl_count(avl) == 0);

    avl_destroy(avl);
}

static void avl_value_null_test() {
    AVL* avl = avl_create(strcmp, NULL);

    char *key = "", *value = NULL;

    /* Inserta 1 value y luego lo borra */
    print_test("AVL test save key vacia value NULL", avl_save(avl, key, value));
    print_test("AVL test count is 1", avl_count(avl) == 1);
    print_test("AVL test get key vacia is value NULL", avl_get(avl, key) == value);
    print_test("AVL test key  belongsvacia, is true", avl_belongs(avl, key));
    print_test("AVL test remove key vacia, is value NULL", avl_remove(avl, key) == value);
    print_test("AVL test count is 0", avl_count(avl) == 0);

    avl_destroy(avl);
}

static void avl_volume_test(size_t volume, bool debug)
{
    AVL* avl = avl_create(strcmp, NULL);

    const size_t key_length = 10;
    char (*keys)[key_length] = malloc(volume * key_length);

    unsigned* values[volume];

    size_t count = avl_count(avl);

    /* Inserta 'volume' parejas in el avl */
    bool ok = true;
    for (unsigned i = 0; i < volume; i++) {
        values[i] = malloc(sizeof(int));
        sprintf(keys[i], "%08d", i);
        *values[i] = i;
        count++;
        if (!ok) break;
    }

    for (unsigned i = 0; i < volume; i++)
    {
        size_t random_number = ((size_t)rand() % ((volume-1) + 1 - MIN_RAND) + MIN_RAND);
        swap(keys[i], keys[random_number]);
    }

    for (unsigned i = 0; i < volume; i++)
    {
        ok &= avl_save(avl, keys[i], values[i]);
    }

    if (debug) print_test("Correct count", avl_count(avl) == count);
    if (debug) print_test("Save a lot of elements", ok);

    /* Verifica que devuelva los values correctos */
    for (size_t i = 0; i < volume; i++) {
        ok = avl_belongs(avl, keys[i]);
        if (!ok) break;
        ok = avl_get(avl, keys[i]) == values[i];
        if (!ok) break;
    }

    if (debug) print_test("AVL test & belongs with a lot of elements", ok);
    if (debug) print_test("AVL test count is correct", avl_count(avl) == volume);

    /* Verifica que borre y devuelva los values correctos */
    for (size_t i = 0; i < volume; i++) {
        ok = avl_remove(avl, keys[i]) == values[i];
        if (!ok) break;
    }

    if (debug) print_test("AVL test remove a lot of elements", ok);
    if (debug) print_test("AVL test count is 0", avl_count(avl) == 0);

    /* Destruye el avl y crea uno nuevo que sí libera */
    avl_destroy(avl);
    avl = avl_create(strcmp, free);

    /* Inserta 'volume' parejas in el avl */
    ok = true;
    for (size_t i = 0; i < volume; i++) {
        ok = avl_save(avl, keys[i], values[i]);
        if (!ok) break;
    }

    free(keys);

    /* Destruye el avl - debería liberar los enteros */
    avl_destroy(avl);

}

static ssize_t buscar(const char* key, char* keys[], size_t volume)
{
    for (size_t i = 0; i < volume; i++) {
        if (strcmp(key, keys[i]) == 0) return (ssize_t) i;
    }
    return -1;
}

static void avl_in_order_test()
{
    AVL* avl = avl_create(strcmp, NULL);

    char *keys[] = {"perro", "gato", "vaca"};
    char *values[] = {"guau", "miau", "mu"};

    /* Inserta 3 values */
    print_test("AVL test save key1", avl_save(avl, keys[0], values[0]));
    print_test("AVL test save key2", avl_save(avl, keys[1], values[1]));
    print_test("AVL test save key3", avl_save(avl, keys[2], values[2]));

    // Prueba de iteración sobre las keys almacenadas.
    AVL_Iter* iter = avl_iter_in_create(avl);
    const char *key;
    ssize_t indice;

    print_test("AVL iterator test finished, is false", !avl_iter_in_finished(iter));

    /* Primer value */
    key = avl_iter_in_get_current(iter);
    indice = buscar(key, keys, sizeof(keys) / sizeof(char *));
    print_test("AVL iterator test get current, is a valid key", indice == 1);
    print_test("AVL iterator test get current, isn't the same pointer", key != keys[indice]);
    print_test("AVL iterator test forwars is true", avl_iter_in_forward(iter));
    print_test("AVL iterator test finished, is false", !avl_iter_in_finished(iter));

    /* Segundo value */
    key = avl_iter_in_get_current(iter);
    indice = buscar(key, keys, sizeof(keys) / sizeof(char *));
    print_test("AVL iterator test get current, is a valid key", indice == 0);
    print_test("AVL iterator test get current, isn't the same pointer", key != keys[indice]);
    print_test("AVL iterator test forwars is true", avl_iter_in_forward(iter));
    print_test("AVL iterator test finished, is false", !avl_iter_in_finished(iter));

    /* Tercer value */
    key = avl_iter_in_get_current(iter);
    indice = buscar(key, keys, sizeof(keys) / sizeof(char *));
    print_test("AVL iterator test get current, is a valid key", indice == 2);
    print_test("AVL iterator test get current, isn't the same pointer", key != keys[indice]);
    avl_iter_in_forward(iter);
    print_test("AVL iterator test finished, is true", avl_iter_in_finished(iter));

    /* Vuelve a tratar de avanzar, por las dudas */
    print_test("AVL iterator test get current, is NULL", !avl_iter_in_get_current(iter));
    print_test("AVL iterator test forwars is false", !avl_iter_in_forward(iter));
    print_test("AVL iterator test finished, is true", avl_iter_in_finished(iter));

    avl_iter_in_destroy(iter);
    avl_destroy(avl);
}

static void avl_in_order_volumen_test(size_t volume)
{
    AVL *avl = avl_create(strcmp, NULL);

    const size_t key_length = 10;
    char (*keys)[key_length] = malloc(volume * key_length);

    size_t values[volume];

    size_t count = avl_count(avl);

    /* Inserta 'volume' parejas in el avl */
    bool ok = true;
    for (unsigned i = 0; i < volume; i++) {
        sprintf(keys[i], "%08d", i);
        values[i] = i;
        count++;
    }

    for (unsigned i = 0; i < volume; i++)
    {
        size_t random_number = ((size_t)rand() % ((volume-1) + 1 - MIN_RAND) + MIN_RAND);
        swap(keys[i], keys[random_number]);
    }

    for (unsigned i = 0; i < volume; i++)
    {
        ok &= avl_save(avl, keys[i], &values[i]);
    }
    
    print_test("Correct count", avl_count(avl) == count);
    print_test("Save a lot of elements", ok);

    unsigned i;
    const char *key;
    size_t *value;

    AVL_Iter *iter = avl_iter_in_create(avl);

    for (i = 0; i < volume; i++) {
        if ( avl_iter_in_finished(iter) ) {
            ok = false;
            break;
        }
        key = avl_iter_in_get_current(iter);
        if ( key == NULL ) {
            printf("%d\n", i);
            ok = false;
            break;
        }
        value = avl_get(avl, key);
        if ( value == NULL ) {
            ok = false;
            break;
        }
        *value = volume;
        avl_iter_in_forward(iter);
    }
    print_test("AVL iterator volume test", ok);
    print_test("AVL iterator volume test, iterate the hole volume", i == volume);
    print_test("AVL iterator test finished, is true", avl_iter_in_finished(iter));

    ok = true;
    for (i = 0; i < volume; i++) {
        if ( values[i] != volume ) {
            ok = false;
            break;
        }
    }
    print_test("AVL iterator volume test, replaced all the elements", ok);

    free(keys);
    avl_iter_in_destroy(iter);
    avl_destroy(avl);
}

bool add_even(const char *key, void *data, void *extra) {
    if (*(int *) data % 2 == 0) 
        *(int *) extra += *(int *) data;
    return true;
}

bool add_all(const char *key, void *data, void *extra) {
    *(int *) extra += *(int *) data;
    return true;
}

bool add_simple_cut(const char *key, void *data, void *extra) {
    if (strcmp(key, "G") == 0) return false;
    *(int *) extra += *(int *) data;
    return true;
}

bool add_left_branch_cut(const char *key, void *data, void *extra) {
    if (strcmp(key, "E") < 0) return false;
    *(int *) extra += *(int *) data;
    return true;
}

bool add_right_left_cut(const char *key, void *data, void *extra) {
    if (strcmp(key, "C") == 0) return false;
    *(int *) extra += *(int *) data;
    return true;
}

static void avl_outer_iterator_test(void) {
    /* Create an AVL */
    AVL *avl = avl_create(strcmp, NULL);

    /* Creo los data a usar */
    int vec[INT_ARRAY_LENGTH_7] = {43, 2, 644, 876, 0, 8, 3};
    char *keys[STRING_LENGTH] = {"E", "B", "A", "C", "G", "F", "H"};
    // char *vec_ordenado[INT_ARRAY_LENGTH_7] = {"A", "B", "C", "D", "E", "F", "G", "H"};

    bool insertado = true;
    for (size_t i = 0; i < STRING_LENGTH; i++) 
        insertado &= avl_save(avl, keys[i], &vec[i]);
    
    print_test("Create an AVL", true);
    print_test("Save some elements", insertado == true);

    int suma = 0;
    avl_in_order(avl, add_all, &suma);
    print_test("Iterate with a function", true);
    print_test("Adding all the elements", suma == (vec[0] + vec[1] + vec[2] + vec[3] + vec[4] + vec[5] + vec[6]));
    
    int suma_pares = 0;
    avl_in_order(avl, add_even, &suma_pares);
    print_test("Iterate with a function pares", true);
    print_test("Adding all the even elements", suma_pares == (vec[1] + vec[2] + vec[3] + vec[4] + vec[5]));

    int extra = 0;
    avl_in_order(avl, NULL, &extra);
    print_test("Iterate with a NULL function (nothing happens)", true);
    print_test("Nothing happend", extra == 0);

    int suma_corte_simple = 0; // Corte simple
    avl_in_order(avl, add_simple_cut, &suma_corte_simple);
    print_test("Iterate till G", true);
    print_test("Add correctly", suma_corte_simple == vec[0] + vec[1] + vec[2] + vec[3] + vec[5]);

    int suma_corte_izquierda = 0; // Corte rama izq
    avl_in_order(avl, add_left_branch_cut, &suma_corte_izquierda);
    print_test("Iterate till E", true);
    print_test("Add correctly", suma_corte_izquierda == 0);

    int suma_corte_derecha_izquierda = 0; // Corte rama der de rama izq
    avl_in_order(avl, add_right_left_cut, &suma_corte_derecha_izquierda);
    print_test("Iterate till C", true);
    print_test("Add correctly", suma_corte_derecha_izquierda == vec[1] + vec[2]);

    /* Destroy AVL */
    avl_destroy(avl);
    print_test("Destroy AVL", true);
}

static int *create_data(int a) {
    int *data = malloc(sizeof(int));
    *data = a;
    return data;
}

static void avl_various_tests() {

    int *data[INT_ARRAY_LENGHT_6];

    for (int i = 0; i < 6; i++)
        data[i] = create_data(i);

    AVL* avl = avl_create(strcmp, free);

    avl_save(avl, "hello", data[0]);
    avl_save(avl, "bye", data[1]);
    avl_save(avl, "sleep", data[2]);

    print_test("sleep belongs", avl_belongs(avl, "sleep"));
    print_test("remove sleep", avl_remove(avl, "sleep") == data[2]);
    print_test("borar sleep (NULL)", avl_remove(avl, "sleep") == NULL);
    print_test("sleep no belongs", !avl_belongs(avl, "sleep"));
    print_test("can't get sleep", avl_get(avl, "sleep") == NULL);
    print_test("count is 2", avl_count(avl) == 2);

    print_test("hello belongs", avl_belongs(avl, "hello"));
    print_test("remove hello", avl_remove(avl, "hello") == data[0]);
    print_test("remove hello (NULL)", avl_remove(avl, "hello") == NULL);
    print_test("hello no belongs", !avl_belongs(avl, "hello"));
    print_test("can't get hello", avl_get(avl, "hello") == NULL);
    print_test("count is 1", avl_count(avl) == 1);

    print_test("bye belongs", avl_belongs(avl, "bye"));
    print_test("remove bye", avl_remove(avl, "bye") == data[1]);
    print_test("remove bye (NULL)", avl_remove(avl, "bye") == NULL);
    print_test("bye no belongs", !avl_belongs(avl, "bye"));
    print_test("can't get bye", avl_get(avl, "bye") == NULL);
    print_test("count is 0", avl_count(avl) == 0);
    
    avl_save(avl, "hello", data[3]);
    avl_save(avl, "bye", data[4]);
    avl_save(avl, "sleep", data[5]);

    print_test("", avl_remove(avl, "hello") == data[3]);
    print_test("", avl_remove(avl, "bye") == data[4]);
    print_test("", avl_remove(avl, "sleep") == data[5]);

    avl_destroy(avl);

    for (int i = 0; i < 6; i++)
        free(data[i]);

}

static void avl_destroy_pointers_tests() {

    AVL *avl = avl_create(strcmp, free);

    int *data[INT_ARRAY_LENGHT_6];

    for (int i = 0; i < INT_ARRAY_LENGHT_6; i++)
        data[i] = create_data(i + 2);

    print_test("count is 0", avl_count(avl) == 0);
    avl_save(avl, "hello", data[0]);
    print_test("count is 1", avl_count(avl) == 1);
    avl_save(avl, "bye", data[1]);
    print_test("count is 2", avl_count(avl) == 2);
    avl_save(avl, "sleep", data[2]);
    print_test("count is 3", avl_count(avl) == 3);
    print_test("Remove 'hello' is !NULL", avl_remove(avl, "hello") != NULL);
    print_test("count is 2", avl_count(avl) == 2);
    print_test("Remove 'hello' is NULL", avl_remove(avl, "hello") == NULL);
    print_test("count is 2", avl_count(avl) == 2);
    print_test("Remove is 'hello' NULL", avl_remove(avl, "hello") == NULL);
    print_test("count is 2", avl_count(avl) == 2);
    print_test("Remove 'bye' is !NULL", avl_remove(avl, "bye") != NULL);
    print_test("count is 1", avl_count(avl) == 1);
    print_test("Remove 'sleep' is !NULL", avl_remove(avl, "sleep") != NULL);
    print_test("count is 0", avl_count(avl) == 0);

    avl_destroy(avl);

    for (int i = 0; i < INT_ARRAY_LENGHT_6; i++)
        free(data[i]);

}
static void avl_test_iguales_keys_distintas() {

    AVL *avl = avl_create(strcmp, free);

    int *data[INT_ARRAY_LENGHT_6];

    for (int i = 0; i < INT_ARRAY_LENGHT_6; i++)
        data[i] = create_data(i + 10);

    // ce y x estan tienen el mismo numero de avl.

    avl_save(avl, "ce", data[5]);
    print_test("count is 1", avl_count(avl) == 1);
    avl_save(avl, "x", data[4]);
    print_test("count is 2", avl_count(avl) == 2);
    avl_save(avl, "x", data[3]);
    print_test("count is 2", avl_count(avl) == 2);
    avl_remove(avl, "x");
    print_test("count is 1", avl_count(avl) == 1);
    print_test("Can't get the key", avl_get(avl, "x") == NULL);

    avl_destroy(avl);

    for (int i = 0; i < INT_ARRAY_LENGHT_6 - 2; i++)
        free(data[i]);
}

static void avl_save_remove_tests() {
    AVL *avl = avl_create(strcmp, free);

    int *data[INT_ARRAY_LENGHT_6];

    for (int i = 0; i < INT_ARRAY_LENGHT_6; i++)
        data[i] = create_data(i * 100);

    print_test("initial count is 0", avl_count(avl) == 0);
    print_test("Save data[0]", avl_save(avl, "ce", data[0]));
    print_test("count is 1", avl_count(avl) == 1);
    print_test("Save data[1]", avl_save(avl, "ce", data[1]));
    print_test("count is 1", avl_count(avl) == 1);
    print_test("Save data[2] in 'x'", avl_save(avl, "x", data[2]) == true);
    print_test("count is 2", avl_count(avl) == 2);
    print_test("Remove 'ce'", avl_remove(avl, "ce") == data[1]);
    print_test("count is 1", avl_count(avl) == 1);
    print_test("Save data[1] in 'ce'", avl_save(avl, "ce", data[1]) == true);
    print_test("Save data[3] in 'ce'", avl_save(avl, "ce", data[3]) == true);
    print_test("Save NULL as value", avl_save(avl, "messi", NULL) == true);
    print_test("Save \"\"", avl_save(avl, "", NULL) == true);

    avl_destroy(avl);

    free(data[4]);
    free(data[5]);

}

static int free_count = 0;
static void my_free(int *data) {
    free_count++;
    free(data);
}

static void destroy_null_test() {

    AVL *avl = avl_create(strcmp, (void (*)(void *))my_free);

    int *data = create_data(9);

    print_test("Save NULL", avl_save(avl, "hello", NULL));
    print_test("Replace data", avl_save(avl, "hello", data));
    avl_destroy(avl);

    print_test("Destructor invoqued the expected times", free_count == 2);

}

void avl_test() {
    prueba_crear_avl_vacio();
    prueba_iterar_avl_vacio();
    avl_save_test();
    avl_replace_test();
    avl_replace_with_destroy_test();
    avl_remove_test();
    avl_empty_key_test();
    avl_value_null_test();
    avl_volume_test(VOLUMEN, true);
    avl_in_order_test();
    avl_in_order_volumen_test(VOLUMEN);
    avl_outer_iterator_test();

    avl_various_tests();
    avl_destroy_pointers_tests();
    avl_test_iguales_keys_distintas();
    avl_save_remove_tests();

    destroy_null_test();
}

void avl_volume_my_tests(size_t volume) {
    avl_volume_test(volume, false);
}