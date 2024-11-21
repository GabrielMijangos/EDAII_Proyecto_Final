/**
 * @file hash_menus.c
 * @brief Implementación de las funciones para manejar menús en una tabla hash.
 */

#include "hash_alimentos.h"
#include "hash_menus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

/**
 * @brief Crea una nueva tabla hash para almacenar menús.
 *
 * @return Puntero a la nueva tabla hash.
 */
MenuHashTable *crearTablaMenus() {
    MenuHashTable *tabla = (MenuHashTable *)malloc(sizeof(MenuHashTable));
    if (!tabla) {
        printf("Error: No se pudo crear la tabla hash para menús.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < MENU_TABLE_SIZE; i++) {
        tabla->buckets[i] = NULL;
    }
    return tabla;
}

/**
 * @brief Genera un índice hash para un ID de menú.
 *
 * @param idMenu ID del menú.
 * @return Índice hash correspondiente.
 */
unsigned int hashIDMenu(int idMenu) {
    return idMenu % MENU_TABLE_SIZE;
}

/**
 * @brief Copia los menús desde la tabla hash a un arreglo.
 *
 * @param tabla Puntero a la tabla hash de menús.
 * @param menus Arreglo donde se almacenarán los menús copiados.
 */
void copiarMenusDesdeTabla(MenuHashTable *tabla, Menu menus[]) {
    int index = 0;
    for (int i = 0; i < MENU_TABLE_SIZE; i++) {
        Menu *actual = tabla->buckets[i];
        while (actual) {
            menus[index++] = *actual;
            actual = actual->next;
        }
    }
}

/**
 * @brief Cuenta el número de menús almacenados en la tabla hash.
 *
 * @param tabla Puntero a la tabla hash de menús.
 * @return Número de menús en la tabla.
 */
int obtenerNumeroDeMenus(MenuHashTable *tabla) {
    int contador = 0;
    for (int i = 0; i < MENU_TABLE_SIZE; i++) {
        Menu *actual = tabla->buckets[i];
        while (actual) {
            contador++;
            actual = actual->next;
        }
    }
    return contador;
}

/**
 * @brief Realiza la partición de un arreglo de menús para QuickSort.
 *
 * @param menus Arreglo de menús.
 * @param low Índice inicial.
 * @param high Índice final.
 * @return Índice del pivote.
 */
int particionMenus(Menu menus[], int low, int high) {
    int pivot = menus[high].idMenu;
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (menus[j].idMenu < pivot) {
            i++;
            Menu temp = menus[i];
            menus[i] = menus[j];
            menus[j] = temp;
        }
    }

    Menu temp = menus[i + 1];
    menus[i + 1] = menus[high];
    menus[high] = temp;

    return i + 1;
}

/**
 * @brief Ordena un arreglo de menús usando QuickSort paralelizado.
 *
 * @param menus Arreglo de menús.
 * @param low Índice inicial.
 * @param high Índice final.
 */
void quicksortMenusParallel(Menu menus[], int low, int high) {
    if (low < high) {
        int pi = particionMenus(menus, low, high);

        #pragma omp parallel sections
        {
            #pragma omp section
            quicksortMenusParallel(menus, low, pi - 1);

            #pragma omp section
            quicksortMenusParallel(menus, pi + 1, high);
        }
    }
}

/**
 * @brief Busca un menú en un arreglo utilizando Búsqueda Binaria paralelizada.
 *
 * @param menus Arreglo de menús ordenado.
 * @param numMenus Número de menús en el arreglo.
 * @param idBuscado ID del menú a buscar.
 * @return Índice del menú encontrado, o -1 si no se encuentra.
 */
int busquedaBinariaMenusParallel(Menu menus[], int numMenus, int idBuscado) {
    int resultado = -1;

    #pragma omp parallel
    {
        int low = 0, high = numMenus - 1;
        while (low <= high) {
            int mid = low + (high - low) / 2;

            if (menus[mid].idMenu == idBuscado) {
                #pragma omp critical
                {
                    resultado = mid;
                }
                break;
            } else if (menus[mid].idMenu < idBuscado) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
    }

    return resultado;
}

/**
 * @brief Agrega un menú a la tabla hash.
 *
 * @param tabla Puntero a la tabla hash de menús.
 * @param idMenu ID del nuevo menú.
 * @param fecha Fecha del menú en formato "YYYY-MM-DD".
 * @param alimentos Arreglo de IDs de alimentos.
 * @param numAlimentos Número de alimentos.
 * @param tablaAlimentos Puntero a la tabla hash de alimentos para validación.
 * @return `true` si el menú fue agregado, `false` en caso de error.
 */
bool agregarMenu(MenuHashTable *tabla, int idMenu, const char *fecha, int *alimentos, int numAlimentos, HashTable *tablaAlimentos) {
    if (buscarMenuPorID(tabla, idMenu)) {
        printf("\nError: El menú con ID=%d ya existe.\n", idMenu);
        return false;
    }

    for (int i = 0; i < numAlimentos; i++) {
        Alimento *alimento = buscarAlimento(tablaAlimentos, alimentos[i]);
        if (!alimento || alimento->disponible == 0) {
            printf("\nError: Alimento inválido o no disponible. ID=%d\n", alimentos[i]);
            return false;
        }
    }

    unsigned int index = hashIDMenu(idMenu);

    Menu *nuevo = (Menu *)malloc(sizeof(Menu));
    if (!nuevo) {
        printf("\nError: No se pudo agregar el menú.\n");
        return false;
    }

    nuevo->idMenu = idMenu;
    strcpy(nuevo->fecha, fecha);
    memcpy(nuevo->alimentos, alimentos, numAlimentos * sizeof(int));
    nuevo->numAlimentos = numAlimentos;
    nuevo->next = tabla->buckets[index];
    tabla->buckets[index] = nuevo;

    printf("\nMenú agregado: ID=%d, Fecha=%s, Número de Alimentos=%d\n", idMenu, fecha, numAlimentos);
    return true;
}

/**
 * @brief Busca un menú en la tabla hash por su ID.
 *
 * @param tabla Puntero a la tabla hash de menús.
 * @param idMenu ID del menú a buscar.
 * @return Puntero al menú encontrado, o `NULL` si no existe.
 */
Menu *buscarMenuPorID(MenuHashTable *tabla, int idMenu) {
    unsigned int index = hashIDMenu(idMenu);
    Menu *actual = tabla->buckets[index];

    while (actual) {
        if (actual->idMenu == idMenu) {
            return actual;
        }
        actual = actual->next;
    }
    return NULL;
}

/**
 * @brief Elimina un menú de la tabla hash por su ID.
 *
 * @param tabla Puntero a la tabla hash de menús.
 * @param idMenu ID del menú a eliminar.
 * @return `true` si el menú fue eliminado, `false` si no se encuentra.
 */
bool eliminarMenu(MenuHashTable *tabla, int idMenu) {
    unsigned int index = hashIDMenu(idMenu);
    Menu *actual = tabla->buckets[index];
    Menu *previo = NULL;

    while (actual) {
        if (actual->idMenu == idMenu) {
            if (previo) {
                previo->next = actual->next;
            } else {
                tabla->buckets[index] = actual->next;
            }
            free(actual);
            printf("\nMenú eliminado: ID=%d\n", idMenu);
            return true;
        }
        previo = actual;
        actual = actual->next;
    }
    printf("\nError: Menú con ID=%d no encontrado.\n", idMenu);
    return false;
}

/**
 * @brief Actualiza la información de un menú en la tabla hash.
 *
 * @param tabla Puntero a la tabla hash de menús.
 * @param idMenu ID del menú a actualizar.
 * @param nuevaFecha Nueva fecha para el menú.
 * @param nuevosAlimentos Arreglo con los nuevos IDs de alimentos.
 * @param numAlimentos Número de alimentos en el menú actualizado.
 * @return `true` si el menú fue actualizado correctamente, `false` si no se encuentra.
 */
bool actualizarMenu(MenuHashTable *tabla, int idMenu, const char *nuevaFecha, int *nuevosAlimentos, int numAlimentos) {
    Menu *menu = buscarMenuPorID(tabla, idMenu);
    if (!menu) {
        printf("\nError: Menú con ID=%d no encontrado.\n", idMenu);
        return false;
    }

    strcpy(menu->fecha, nuevaFecha);
    memcpy(menu->alimentos, nuevosAlimentos, numAlimentos * sizeof(int));
    menu->numAlimentos = numAlimentos;
    printf("\nMenú actualizado: ID=%d, Fecha=%s, Número de Alimentos=%d\n", idMenu, menu->fecha, menu->numAlimentos);
    return true;
}

/**
 * @brief Muestra todos los menús en la tabla hash junto con sus alimentos asociados.
 *
 * @param tablaMenus Puntero a la tabla hash de menús.
 * @param tablaAlimentos Puntero a la tabla hash de alimentos para mostrar detalles.
 */
void mostrarTablaMenus(MenuHashTable *tablaMenus, HashTable *tablaAlimentos) {
    printf("\n--- Tabla Hash de Menús ---\n");
    for (int i = 0; i < MENU_TABLE_SIZE; i++) {
        if (tablaMenus->buckets[i] != NULL) { // Solo mostrar buckets no vacíos
            printf("Bucket %d:\n", i);
            Menu *actual = tablaMenus->buckets[i];
            while (actual) {
                printf("  Menú ID=%d, Fecha=%s, Número de Alimentos=%d\n",
                       actual->idMenu, actual->fecha, actual->numAlimentos);

                // Mostrar los alimentos asociados al menú
                printf("  Alimentos en el Menú:\n");
                for (int j = 0; j < actual->numAlimentos; j++) {
                    Alimento *alimento = buscarAlimento(tablaAlimentos, actual->alimentos[j]);
                    if (alimento) {
                        printf("    - ID=%d, Nombre=%s, Precio=%.2f, Disponible=%s\n",
                               alimento->id, alimento->nombre, alimento->precio,
                               alimento->disponible ? "Sí" : "No");
                    } else {
                        printf("    - Alimento con ID=%d no encontrado.\n", actual->alimentos[j]);
                    }
                }

                actual = actual->next;
            }
        }
    }
}

/**
 * @brief Libera la memoria ocupada por la tabla hash de menús.
 *
 * @param tabla Puntero a la tabla hash de menús a liberar.
 */
void liberarTablaMenus(MenuHashTable *tabla) {
    for (int i = 0; i < MENU_TABLE_SIZE; i++) {
        Menu *actual = tabla->buckets[i];
        while (actual) {
            Menu *temp = actual;
            actual = actual->next;
            free(temp);
        }
    }
    free(tabla);
}
