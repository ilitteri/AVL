#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // For ssize_t in Linux.

#include "../avl.h"
#include "testing.h"

#define VOLUMEN 100
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

    print_test("El avl fue creado", avl != NULL);
    print_test("La count es 0", avl_count(avl) == 0);
    print_test("Borrar en un arbol vacio es NULL", avl_remove(avl, "hola") == NULL);
    print_test("Obtener en un arbol vacio es NULL", avl_get(avl, "hola") == NULL);
    avl_destroy(avl);
    print_test("Destruyo el avl", true);
}

static void prueba_iterar_avl_vacio() {
    AVL* avl = avl_create(strcmp, NULL);
    AVL_Iter* iter = avl_iter_in_create(avl);
    print_test("Prueba avl iter crear iterador avl vacio", iter);
    print_test("Prueba avl iter esta al final", avl_iter_in_finished(iter));
    print_test("Prueba avl iter avanzar es false", !avl_iter_in_forward(iter));
    print_test("Prueba avl iter ver actual es NULL", !avl_iter_in_get_current(iter));

    avl_iter_in_destroy(iter);
    avl_destroy(avl);
}

static void prueba_avl_save() {
    AVL *avl = avl_create(strcmp, NULL);

    char* keys[STRING_LENGTH] = {"M", "D", "A", "E", "O", "T", "U"};

    print_test("La count es 0", avl_count(avl) == 0);
    bool ok = true;
    for (size_t i = 0; i < STRING_LENGTH; i++) {
        ok &= avl_save(avl, keys[i], NULL) && avl_count(avl) == i+1;
        ok &= avl_get(avl, keys[i]) == NULL;
    }
    print_test("Agregar arreglo de keys", ok);
    avl_destroy(avl);
    print_test("Destruyo el avl", true);
}

static void prueba_avl_reemplazar() {
    AVL *avl = avl_create(strcmp, NULL);

    int a = 13;
    int b = 16;
    char* keys[STRING_LENGTH] = {"M", "D", "A", "E", "O", "T", "U"};

    print_test("La count es 0", avl_count(avl) == 0);
    bool ok = true;
    for (size_t i = 0; i < STRING_LENGTH; i++) {
        ok &= avl_save(avl, keys[i], &a) && avl_count(avl) == i+1;
    }
    for (size_t i = 0; i < STRING_LENGTH; i++) {
        ok &= avl_get(avl, keys[i]) == &a;
    }
    print_test("Agrego arreglo de keys correctamente", ok);
    for (size_t i = 0; i < STRING_LENGTH; i++) {
        ok &= avl_save(avl, keys[i], &b) && avl_count(avl) == STRING_LENGTH;
    }
    for (size_t i = 0; i < STRING_LENGTH; i++){
        ok &= avl_get(avl, keys[i]) == &b;
    }
    print_test("Reemplazo arreglo de keys correctamente", ok);
    avl_destroy(avl);
    print_test("Destruyo el avl", true);
}

static void prueba_avl_reemplazar_con_destruir() {
    AVL *avl = avl_create(strcmp, free);
    
    int *values0[INT_ARRAY_LENGTH_7];
    int *values1[INT_ARRAY_LENGTH_7];
    for (size_t i = 0; i < STRING_LENGTH; i++) {
        values0[i] = malloc(sizeof(int));
        values1[i] = malloc(sizeof(int));
    }
    char* keys[STRING_LENGTH] = {"M", "D", "A", "E", "O", "T", "U"};

    print_test("La count es 0", avl_count(avl) == 0);
    bool ok = true;
    for (size_t i = 0; i < STRING_LENGTH; i++) {
        ok &= avl_save(avl, keys[i], values0[i]) && avl_count(avl) == i+1;
        ok &= avl_get(avl, keys[i]) == values0[i];
    }
    print_test("Agrego 7 keys con destruir correctamente", ok);
    for (size_t i = 0; i < STRING_LENGTH; i++) {
        ok &= avl_save(avl, keys[i], values1[i]) && avl_count(avl) == 7;
        ok &= avl_get(avl, keys[i]) == values1[i];
    }
    print_test("Reemplazo 7 keys con destruir correctamente", ok);
    avl_destroy(avl);
    print_test("Destruyo el avl", true);
}

static void prueba_avl_remove() {
    AVL* avl = avl_create(strcmp, NULL);

    char *key1 = "perro", *value1 = "guau";
    char *key2 = "gato", *value2 = "miau";
    char *key3 = "vaca", *value3 = "mu";

    print_test("Prueba avl insertar key1", avl_save(avl, key1, value1));
    print_test("Prueba avl insertar key2", avl_save(avl, key2, value2));
    print_test("Prueba avl insertar key3", avl_save(avl, key3, value3));

    print_test("Prueba avl pertenece key3, es verdadero", avl_belongs(avl, key3));
    print_test("Prueba avl borrar key3, es value3", avl_remove(avl, key3) == value3);
    print_test("Prueba avl borrar key3, es NULL", !avl_remove(avl, key3));
    print_test("Prueba avl pertenece key3, es falso", !avl_belongs(avl, key3));
    print_test("Prueba avl obtener key3, es NULL", !avl_get(avl, key3));
    print_test("Prueba avl la count de elementos es 2", avl_count(avl) == 2);

    print_test("Prueba avl pertenece key1, es verdadero", avl_belongs(avl, key1));
    print_test("Prueba avl borrar key1, es value1", avl_remove(avl, key1) == value1);
    print_test("Prueba avl borrar key1, es NULL", !avl_remove(avl, key1));
    print_test("Prueba avl pertenece key1, es falso", !avl_belongs(avl, key1));
    print_test("Prueba avl obtener key1, es NULL", !avl_get(avl, key1));
    print_test("Prueba avl la count de elementos es 1", avl_count(avl) == 1);

    print_test("Prueba avl pertenece key2, es verdadero", avl_belongs(avl, key2));
    print_test("Prueba avl borrar key2, es value2", avl_remove(avl, key2) == value2);
    print_test("Prueba avl borrar key2, es NULL", !avl_remove(avl, key2));
    print_test("Prueba avl pertenece key2, es falso", !avl_belongs(avl, key2));
    print_test("Prueba avl obtener key2, es NULL", !avl_get(avl, key2));
    print_test("Prueba avl la count de elementos es 0", avl_count(avl) == 0);

    avl_destroy(avl);

}

static void prueba_avl_key_vacia()
{
    AVL* avl = avl_create(strcmp, NULL);

    char *key = "", *value = "";

    print_test("Prueba avl insertar key vacia", avl_save(avl, key, value));
    print_test("Prueba avl la count de elementos es 1", avl_count(avl) == 1);
    print_test("Prueba avl obtener key vacia es value", avl_get(avl, key) == value);
    print_test("Prueba avl pertenece key vacia, es true", avl_belongs(avl, key));
    print_test("Prueba avl borrar key vacia, es value", avl_remove(avl, key) == value);
    print_test("Prueba avl la count de elementos es 0", avl_count(avl) == 0);

    avl_destroy(avl);
}

static void prueba_avl_value_null() {
    AVL* avl = avl_create(strcmp, NULL);

    char *key = "", *value = NULL;

    /* Inserta 1 value y luego lo borra */
    print_test("Prueba avl insertar key vacia value NULL", avl_save(avl, key, value));
    print_test("Prueba avl la count de elementos es 1", avl_count(avl) == 1);
    print_test("Prueba avl obtener key vacia es value NULL", avl_get(avl, key) == value);
    print_test("Prueba avl pertenece key vacia, es true", avl_belongs(avl, key));
    print_test("Prueba avl borrar key vacia, es value NULL", avl_remove(avl, key) == value);
    print_test("Prueba avl la count de elementos es 0", avl_count(avl) == 0);

    avl_destroy(avl);
}

static void prueba_avl_volumen(size_t volume, bool debug)
{
    AVL* avl = avl_create(strcmp, NULL);

    const size_t key_length = 10;
    char (*keys)[key_length] = malloc(volume * key_length);

    unsigned* values[volume];

    size_t count = avl_count(avl);

    /* Inserta 'volume' parejas en el avl */
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

    if (debug) print_test("count correcta", avl_count(avl) == count);
    if (debug) print_test("Prueba avl almacenar muchos elementos", ok);

    /* Verifica que devuelva los values correctos */
    for (size_t i = 0; i < volume; i++) {
        ok = avl_belongs(avl, keys[i]);
        if (!ok) break;
        ok = avl_get(avl, keys[i]) == values[i];
        if (!ok) break;
    }

    if (debug) print_test("Prueba avl pertenece y obtener muchos elementos", ok);
    if (debug) print_test("Prueba avl la count de elementos es correcta", avl_count(avl) == volume);

    /* Verifica que borre y devuelva los values correctos */
    for (size_t i = 0; i < volume; i++) {
        ok = avl_remove(avl, keys[i]) == values[i];
        if (!ok) break;
    }

    if (debug) print_test("Prueba avl borrar muchos elementos", ok);
    if (debug) print_test("Prueba avl la count de elementos es 0", avl_count(avl) == 0);

    /* Destruye el avl y crea uno nuevo que sí libera */
    avl_destroy(avl);
    avl = avl_create(strcmp, free);

    /* Inserta 'volume' parejas en el avl */
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

static void prueba_avl_in_order()
{
    AVL* avl = avl_create(strcmp, NULL);

    char *keys[] = {"perro", "gato", "vaca"};
    char *values[] = {"guau", "miau", "mu"};

    /* Inserta 3 values */
    print_test("Prueba avl insertar key1", avl_save(avl, keys[0], values[0]));
    print_test("Prueba avl insertar key2", avl_save(avl, keys[1], values[1]));
    print_test("Prueba avl insertar key3", avl_save(avl, keys[2], values[2]));

    // Prueba de iteración sobre las keys almacenadas.
    AVL_Iter* iter = avl_iter_in_create(avl);
    const char *key;
    ssize_t indice;

    print_test("Prueba avl iterador esta al final, es false", !avl_iter_in_finished(iter));

    /* Primer value */
    key = avl_iter_in_get_current(iter);
    indice = buscar(key, keys, sizeof(keys) / sizeof(char *));
    print_test("Prueba avl iterador ver actual, es una key valida", indice == 1);
    print_test("Prueba avl iterador ver actual, no es el mismo puntero", key != keys[indice]);
    print_test("Prueba avl iterador avanzar es true", avl_iter_in_forward(iter));
    print_test("Prueba avl iterador esta al final, es false", !avl_iter_in_finished(iter));

    /* Segundo value */
    key = avl_iter_in_get_current(iter);
    indice = buscar(key, keys, sizeof(keys) / sizeof(char *));
    print_test("Prueba avl iterador ver actual, es una key valida", indice == 0);
    print_test("Prueba avl iterador ver actual, no es el mismo puntero", key != keys[indice]);
    print_test("Prueba avl iterador avanzar es true", avl_iter_in_forward(iter));
    print_test("Prueba avl iterador esta al final, es false", !avl_iter_in_finished(iter));

    /* Tercer value */
    key = avl_iter_in_get_current(iter);
    indice = buscar(key, keys, sizeof(keys) / sizeof(char *));
    print_test("Prueba avl iterador ver actual, es una key valida", indice == 2);
    print_test("Prueba avl iterador ver actual, no es el mismo puntero", key != keys[indice]);
    avl_iter_in_forward(iter);
    print_test("Prueba avl iterador esta al final, es true", avl_iter_in_finished(iter));

    /* Vuelve a tratar de avanzar, por las dudas */
    print_test("Prueba avl iterador ver actual, es NULL", !avl_iter_in_get_current(iter));
    print_test("Prueba avl iterador avanzar es false", !avl_iter_in_forward(iter));
    print_test("Prueba avl iterador esta al final, es true", avl_iter_in_finished(iter));

    avl_iter_in_destroy(iter);
    avl_destroy(avl);
}

static void prueba_avl_in_order_volumen(size_t volume)
{
    AVL *avl = avl_create(strcmp, NULL);

    const size_t key_length = 10;
    char (*keys)[key_length] = malloc(volume * key_length);

    size_t values[volume];

    size_t count = avl_count(avl);

    /* Inserta 'volume' parejas en el avl */
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
    
    print_test("count correcta", avl_count(avl) == count);
    print_test("Prueba avl almacenar muchos elementos", ok);

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
    print_test("Prueba avl iteración en volumen", ok);
    print_test("Prueba avl iteración en volumen, recorrio todo el volume", i == volume);
    print_test("Prueba avl iterador esta al final, es true", avl_iter_in_finished(iter));

    ok = true;
    for (i = 0; i < volume; i++) {
        if ( values[i] != volume ) {
            ok = false;
            break;
        }
    }
    print_test("Prueba avl iteración en volumen, se cambiaron todo los elementos", ok);

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

static void prueba_avl_iterador_interno(void) {
    /* Creo un avl */
    AVL *avl = avl_create(strcmp, NULL);

    /* Creo los data a usar */
    int vec[INT_ARRAY_LENGTH_7] = {43, 2, 644, 876, 0, 8, 3};
    char *keys[STRING_LENGTH] = {"E", "B", "A", "C", "G", "F", "H"};
    // char *vec_ordenado[INT_ARRAY_LENGTH_7] = {"A", "B", "C", "D", "E", "F", "G", "H"};

    bool insertado = true;
    for (size_t i = 0; i < STRING_LENGTH; i++) 
        insertado &= avl_save(avl, keys[i], &vec[i]);
    
    print_test("Creo un avl", true);
    print_test("Inserto varios elementos", insertado == true);

    int suma = 0;
    avl_in_order(avl, add_all, &suma);
    print_test("Itero con la funcion visitar que suma los numeros", true);
    print_test("Sumo todos los elementos de el avl", suma == (vec[0] + vec[1] + vec[2] + vec[3] + vec[4] + vec[5] + vec[6]));
    
    int suma_pares = 0;
    avl_in_order(avl, add_even, &suma_pares);
    print_test("Itero con la funcion visitar que suma los numeros pares", true);
    print_test("Sumo todos los elementos de el avl", suma_pares == (vec[1] + vec[2] + vec[3] + vec[4] + vec[5]));

    int extra = 0;
    avl_in_order(avl, NULL, &extra);
    print_test("Itero con la funcion visitar en NULL (no deberia pasar nada)", true);
    print_test("No pasa nada", extra == 0);

    int suma_corte_simple = 0; // Corte simple
    avl_in_order(avl, add_simple_cut, &suma_corte_simple);
    print_test("Itero con la funcion visitar que suma hasta una G", true);
    print_test("Sumo correctamente", suma_corte_simple == vec[0] + vec[1] + vec[2] + vec[3] + vec[5]);

    int suma_corte_izquierda = 0; // Corte rama izq
    avl_in_order(avl, add_left_branch_cut, &suma_corte_izquierda);
    print_test("Itero con la funcion visitar que suma hasta una G", true);
    print_test("Sumo correctamente", suma_corte_izquierda == 0);

    int suma_corte_derecha_izquierda = 0; // Corte rama der de rama izq
    avl_in_order(avl, add_right_left_cut, &suma_corte_derecha_izquierda);
    print_test("Itero con la funcion visitar que suma hasta una G", true);
    print_test("Sumo correctamente", suma_corte_derecha_izquierda == vec[1] + vec[2]);

    /* Destruyo el avl */
    avl_destroy(avl);
    print_test("Destruyo el avl", true);
}

static int *create_data(int a) {
    int *data = malloc(sizeof(int));
    *data = a;
    return data;
}

static void pruebas_varias() {

    int *data[INT_ARRAY_LENGHT_6];

    for (int i = 0; i < 6; i++)
        data[i] = create_data(i);

    AVL* avl = avl_create(strcmp, free);

    avl_save(avl, "hola", data[0]);
    avl_save(avl, "chau", data[1]);
    avl_save(avl, "dormir", data[2]);

    print_test("dormir pertenece", avl_belongs(avl, "dormir"));
    print_test("borrar dormir", avl_remove(avl, "dormir") == data[2]);
    print_test("borar dormir (NULL)", avl_remove(avl, "dormir") == NULL);
    print_test("dormir no pertenece", !avl_belongs(avl, "dormir"));
    print_test("no se puede obtener dormir", avl_get(avl, "dormir") == NULL);
    print_test("count es 2", avl_count(avl) == 2);

    print_test("hola pertenece", avl_belongs(avl, "hola"));
    print_test("borrar hola", avl_remove(avl, "hola") == data[0]);
    print_test("borrar hola (NULL)", avl_remove(avl, "hola") == NULL);
    print_test("hola no pertenece", !avl_belongs(avl, "hola"));
    print_test("no se puede obtener hola", avl_get(avl, "hola") == NULL);
    print_test("count es 1", avl_count(avl) == 1);

    print_test("chau pertenece", avl_belongs(avl, "chau"));
    print_test("borrar chau", avl_remove(avl, "chau") == data[1]);
    print_test("borrar chau (NULL)", avl_remove(avl, "chau") == NULL);
    print_test("chau no pertenece", !avl_belongs(avl, "chau"));
    print_test("no se puede obtener chau", avl_get(avl, "chau") == NULL);
    print_test("count es 0", avl_count(avl) == 0);
    
    avl_save(avl, "hola", data[3]);
    avl_save(avl, "chau", data[4]);
    avl_save(avl, "dormir", data[5]);

    print_test("", avl_remove(avl, "hola") == data[3]);
    print_test("", avl_remove(avl, "chau") == data[4]);
    print_test("", avl_remove(avl, "dormir") == data[5]);

    avl_destroy(avl);

    for (int i = 0; i < 6; i++)
        free(data[i]);

}

static void pruebas_borrar_punteros() {

    AVL *avl = avl_create(strcmp, free);

    int *data[INT_ARRAY_LENGHT_6];

    for (int i = 0; i < INT_ARRAY_LENGHT_6; i++)
        data[i] = create_data(i + 2);

    print_test("count es 0", avl_count(avl) == 0);
    avl_save(avl, "hola", data[0]);
    print_test("count es 1", avl_count(avl) == 1);
    avl_save(avl, "chau", data[1]);
    print_test("count es 2", avl_count(avl) == 2);
    avl_save(avl, "dormir", data[2]);
    print_test("count es 3", avl_count(avl) == 3);
    print_test("Borrar 'hola' es !NULL", avl_remove(avl, "hola") != NULL);
    print_test("count es 2", avl_count(avl) == 2);
    print_test("Borrar 'hola' es NULL", avl_remove(avl, "hola") == NULL);
    print_test("count es 2", avl_count(avl) == 2);
    print_test("Borrar es 'hola' NULL", avl_remove(avl, "hola") == NULL);
    print_test("count es 2", avl_count(avl) == 2);
    print_test("Borrar 'chau' es !NULL", avl_remove(avl, "chau") != NULL);
    print_test("count es 1", avl_count(avl) == 1);
    print_test("Borrar 'dormir' es !NULL", avl_remove(avl, "dormir") != NULL);
    print_test("count es 0", avl_count(avl) == 0);

    avl_destroy(avl);

    for (int i = 0; i < INT_ARRAY_LENGHT_6; i++)
        free(data[i]);

}
static void pruebas_avl_iguales_keys_distintas() {

    AVL *avl = avl_create(strcmp, free);

    int *data[INT_ARRAY_LENGHT_6];

    for (int i = 0; i < INT_ARRAY_LENGHT_6; i++)
        data[i] = create_data(i + 10);

    // ce y x estan tienen el mismo numero de avl.

    avl_save(avl, "ce", data[5]);
    print_test("count es 1", avl_count(avl) == 1);
    avl_save(avl, "x", data[4]);
    print_test("count es 2", avl_count(avl) == 2);
    avl_save(avl, "x", data[3]);
    print_test("count es 2", avl_count(avl) == 2);
    avl_remove(avl, "x");
    print_test("count es 1", avl_count(avl) == 1);
    print_test("No puedo obtener la key", avl_get(avl, "x") == NULL);

    avl_destroy(avl);

    for (int i = 0; i < INT_ARRAY_LENGHT_6 - 2; i++)
        free(data[i]);
}

static void pruebas_guardar_borrar() {
    AVL *avl = avl_create(strcmp, free);

    int *data[INT_ARRAY_LENGHT_6];

    for (int i = 0; i < INT_ARRAY_LENGHT_6; i++)
        data[i] = create_data(i * 100);

    print_test("count inicial es 0", avl_count(avl) == 0);
    print_test("Guardo data[0]", avl_save(avl, "ce", data[0]));
    print_test("count es 1", avl_count(avl) == 1);
    print_test("Guardo data[1]", avl_save(avl, "ce", data[1]));
    print_test("count es 1", avl_count(avl) == 1);
    print_test("Guardo data[2] en 'x'", avl_save(avl, "x", data[2]) == true);
    print_test("count es 2", avl_count(avl) == 2);
    print_test("Borro 'ce'", avl_remove(avl, "ce") == data[1]);
    print_test("count es 1", avl_count(avl) == 1);
    print_test("Guardo data[1] en 'ce'", avl_save(avl, "ce", data[1]) == true);
    print_test("Guardo data[3] en 'ce'", avl_save(avl, "ce", data[3]) == true);
    print_test("Guardo NULL como value", avl_save(avl, "messi", NULL) == true);
    print_test("Guardo \"\"", avl_save(avl, "", NULL) == true);

    avl_destroy(avl);

    free(data[4]);
    free(data[5]);

}

static int free_count = 0;
static void my_free(int *data) {
    free_count++;
    free(data);
}

static void prueba_destruir_null() {

    AVL *avl = avl_create(strcmp, (void (*)(void *))my_free);

    int *data = create_data(9);

    print_test("Guardo NULL", avl_save(avl, "hola", NULL));
    print_test("Reemplazo data", avl_save(avl, "hola", data));
    avl_destroy(avl);

    print_test("Se invocó el destructor el número esperado de veces", free_count == 2);

}

void pruebas_avl_estudiante() {
    /* Ejecuta todas las pruebas unitarias. */
    prueba_crear_avl_vacio();
    prueba_iterar_avl_vacio();
    prueba_avl_save();
    prueba_avl_reemplazar();
    prueba_avl_reemplazar_con_destruir();
    prueba_avl_remove();
    prueba_avl_key_vacia();
    prueba_avl_value_null();
    prueba_avl_volumen(VOLUMEN, true);
    prueba_avl_in_order();
    prueba_avl_in_order_volumen(VOLUMEN);
    prueba_avl_iterador_interno();

    pruebas_varias();
    pruebas_borrar_punteros();
    pruebas_avl_iguales_keys_distintas();
    pruebas_guardar_borrar();

    prueba_destruir_null();
}

void pruebas_volumen_estudiante(size_t volume) {
    prueba_avl_volumen(volume, false);
}