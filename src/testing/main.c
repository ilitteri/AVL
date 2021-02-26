#include "testing.h"

#include <stdio.h>
#include <stdlib.h>

void avl_test(void);
void avl_volume_my_tests(size_t);

int main(int argc, char *argv[]) {
    if (argc > 1) {
        // Asumimos que nos están pidiendo pruebas de volumen.
        long length = strtol(argv[1], NULL, 10);
        avl_volume_my_tests((size_t) length);

        return failure_count() > 0;
    }
    printf("\n~~~ TESTS ~~~\n");
    avl_test();
    return failure_count() > 0;
}