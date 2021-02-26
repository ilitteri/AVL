#include "testing.h"

#include <stdio.h>
#include <stdlib.h>

/* ******************************************************************
 *                        PROGRAMA PRINCIPAL
 * *****************************************************************/

void pruebas_avl_estudiante(void);
void pruebas_volumen_estudiante(size_t);

int main(int argc, char *argv[]) {
    if (argc > 1) {
        // Asumimos que nos están pidiendo pruebas de volumen.
        long largo = strtol(argv[1], NULL, 10);
        pruebas_volumen_estudiante((size_t) largo);

        return failure_count() > 0;
    }
    printf("\n~~~ PRUEBAS ~~~\n");
    pruebas_avl_estudiante();
    return failure_count() > 0;
}