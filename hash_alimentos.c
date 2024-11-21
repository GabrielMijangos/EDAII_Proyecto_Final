/**
 * @file hash_alimentos.c
 * @brief Implementación de las funciones para la tabla hash de alimentos.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <omp.h>
#include "hash_alimentos.h"

/**
 * @brief Crea una nueva tabla hash para almacenar alimentos.
 * @return Puntero a la tabla hash creada.
 */
HashTable *crearTablaHash() {
    HashTable *tabla = (HashTable *)malloc(sizeof(HashTable));
    if (!tabla) {
        printf("\nError: No se pudo crear la tabla hash.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        tabla->buckets[i] = NULL;
    }
    return tabla;
}

/**
 * @brief Calcula el índice hash usando el método Mid-Square.
 * @param id ID del alimento.
 * @return Índice hash calculado.
 */
unsigned int hashFunctionMidSquare(int id) {
    unsigned long long square = (unsigned long long)id * id;
    unsigned int midBits = (square >> 10) & ((1 << 6) - 1);
    return midBits % HASH_TABLE_SIZE;
}

/**
 * @brief Copia los alimentos desde la tabla hash a un arreglo.
 * @param tabla Puntero a la tabla hash.
 * @param alimentos Arreglo donde se copiarán los alimentos.
 */
void copiarAlimentosDesdeTabla(HashTable *tabla, Alimento alimentos[]) {
    int index = 0;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) { // Recorre cada bucket
        Alimento *actual = tabla->buckets[i];
        while (actual) { // Recorre la lista enlazada en cada bucket
            alimentos[index++] = *actual; // Copia el alimento al arreglo
            actual = actual->next;
        }
    }
}

/**
 * @brief Cuenta el número de alimentos en la tabla hash.
 * @param tabla Puntero a la tabla hash.
 * @return Número total de alimentos en la tabla.
 */
int obtenerNumeroDeAlimentos(HashTable *tabla) {
    int contador = 0;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        Alimento *actual = tabla->buckets[i];
        while (actual) {
            contador++;
            actual = actual->next;
        }
    }
    return contador;
}

/**
 * @brief Realiza la partición para el algoritmo QuickSort.
 * @param alimentos Arreglo de alimentos.
 * @param low Índice inferior del rango.
 * @param high Índice superior del rango.
 * @return Índice de partición.
 */
int particionAlimentos(Alimento alimentos[], int low, int high) {
    float pivot = alimentos[high].precio; // Puedes cambiar a alimentos[high].id para ordenar por ID
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (alimentos[j].precio < pivot) { // Cambia aquí el criterio si es necesario
            i++;
            Alimento temp = alimentos[i];
            alimentos[i] = alimentos[j];
            alimentos[j] = temp;
        }
    }

    Alimento temp = alimentos[i + 1];
    alimentos[i + 1] = alimentos[high];
    alimentos[high] = temp;

    return i + 1;
}

/**
 * @brief Ordena un arreglo de alimentos en paralelo utilizando QuickSort.
 * @param alimentos Arreglo de alimentos a ordenar.
 * @param low Índice inferior del rango.
 * @param high Índice superior del rango.
 */
void quicksortAlimentosParallel(Alimento alimentos[], int low, int high) {
    if (low < high) {
        int pi = particionAlimentos(alimentos, low, high);

        #pragma omp parallel sections
        {
            #pragma omp section
            quicksortAlimentosParallel(alimentos, low, pi - 1);

            #pragma omp section
            quicksortAlimentosParallel(alimentos, pi + 1, high);
        }
    }
}

/**
 * @brief Realiza una búsqueda binaria paralelizada para encontrar un alimento por su ID.
 * @param alimentos Arreglo de alimentos ordenados.
 * @param numAlimentos Número de alimentos en el arreglo.
 * @param idBuscado ID del alimento a buscar.
 * @return Índice del alimento encontrado, o -1 si no existe.
 */
int busquedaBinariaAlimentosParallel(Alimento alimentos[], int numAlimentos, int idBuscado) {
    int resultado = -1;

    #pragma omp parallel
    {
        int low = 0, high = numAlimentos - 1;
        while (low <= high) {
            int mid = low + (high - low) / 2;

            if (alimentos[mid].id == idBuscado) {
                #pragma omp critical
                {
                    resultado = mid; // Actualizamos el resultado de forma segura
                }
                break; // Salimos del bucle
            } else if (alimentos[mid].id < idBuscado) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
    }

    return resultado; // Retornamos el índice encontrado o -1 si no existe
}

/**
 * @brief Inserta un alimento en la tabla hash.
 * @param tabla Puntero a la tabla hash.
 * @param id ID del alimento.
 * @param nombre Nombre del alimento.
 * @param precio Precio del alimento.
 * @param disponibleStr Disponibilidad del alimento ("Si" o "No").
 * @return true si se insertó correctamente, false en caso contrario.
 */
bool insertarAlimento(HashTable *tabla, int id, const char *nombre, float precio, const char *disponibleStr) {
    int disponible;
    if (strcmp(disponibleStr, "Si") == 0) {
        disponible = 1;
    } else if (strcmp(disponibleStr, "No") == 0) {
        disponible = 0;
    } else {
        printf("\nError: Entrada inválida para disponibilidad. Use estrictamente 'Si' o 'No'.\n");
        return false;
    }

    if (buscarAlimento(tabla, id)) {
        printf("\nError: El alimento con ID=%d ya existe.\n", id);
        return false;
    }

    unsigned int index = hashFunctionMidSquare(id);

    Alimento *nuevo = (Alimento *)malloc(sizeof(Alimento));
    if (!nuevo) {
        printf("\nError: No se pudo insertar el alimento.\n");
        return false;
    }

    nuevo->id = id;
    strcpy(nuevo->nombre, nombre);
    nuevo->precio = precio;
    nuevo->disponible = disponible;
    nuevo->next = tabla->buckets[index];
    tabla->buckets[index] = nuevo;

    printf("\nAlimento insertado: ID=%d, Nombre=%s, Precio=%.2f, Disponible=%s\n",
           id, nombre, precio, disponible ? "Si" : "No");
    return true;
}

/**
 * @brief Busca un alimento en la tabla hash por su ID.
 * @param tabla Puntero a la tabla hash.
 * @param id ID del alimento a buscar.
 * @return Puntero al alimento encontrado, o NULL si no existe.
 */
Alimento *buscarAlimento(HashTable *tabla, int id) {
    unsigned int index = hashFunctionMidSquare(id);
    Alimento *actual = tabla->buckets[index];

    while (actual) {
        if (actual->id == id) {
            return actual;
        }
        actual = actual->next;
    }
    return NULL;
}

/**
 * @brief Elimina un alimento de la tabla hash por su ID.
 * @param tabla Puntero a la tabla hash.
 * @param id ID del alimento a eliminar.
 * @return true si se eliminó correctamente, false en caso contrario.
 */
bool eliminarAlimento(HashTable *tabla, int id) {
    unsigned int index = hashFunctionMidSquare(id);
    Alimento *actual = tabla->buckets[index];
    Alimento *previo = NULL;

    while (actual) {
        if (actual->id == id) {
            if (previo) {
                previo->next = actual->next;
            } else {
                tabla->buckets[index] = actual->next;
            }
            free(actual);
            printf("\nAlimento eliminado: ID=%d\n", id);
            return true;
        }
        previo = actual;
        actual = actual->next;
    }
    printf("\nError: Alimento con ID=%d no encontrado.\n", id);
    return false;
}

/**
 * @brief Actualiza un alimento en la tabla hash.
 * @param tabla Puntero a la tabla hash.
 * @param idAlimento ID del alimento a actualizar.
 * @param nuevoNombre Nuevo nombre del alimento.
 * @param nuevoPrecio Nuevo precio del alimento.
 * @param nuevaDisponibilidadStr Nueva disponibilidad del alimento ("Si" o "No").
 * @return true si se actualizó correctamente, false en caso contrario.
 */
bool actualizarAlimento(HashTable *tabla, int idAlimento, const char *nuevoNombre, float nuevoPrecio, const char *nuevaDisponibilidadStr) {
    Alimento *alimento = buscarAlimento(tabla, idAlimento);
    if (!alimento) {
        printf("\nError: Alimento con ID=%d no encontrado.\n", idAlimento);
        return false;
    }

    int nuevaDisponibilidad;
    if (strcmp(nuevaDisponibilidadStr, "Si") == 0) {
        nuevaDisponibilidad = 1;
    } else if (strcmp(nuevaDisponibilidadStr, "No") == 0) {
        nuevaDisponibilidad = 0;
    } else {
        printf("\nError: Entrada inválida para disponibilidad. Use estrictamente 'Si' o 'No'.\n");
        return false;
    }

    strcpy(alimento->nombre, nuevoNombre);
    alimento->precio = nuevoPrecio;
    alimento->disponible = nuevaDisponibilidad;

    printf("\nAlimento actualizado: ID=%d, Nombre=%s, Precio=%.2f, Disponible=%s\n",
           idAlimento, alimento->nombre, alimento->precio, alimento->disponible ? "Si" : "No");
    return true;
}

/**
 * @brief Muestra el contenido de la tabla hash.
 * @param tabla Puntero a la tabla hash.
 */
void mostrarTabla(HashTable *tabla) {
    printf("\n--- Tabla Hash de Alimentos ---\n");
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        if (tabla->buckets[i] != NULL) {
            printf("Bucket %d: ", i);
            Alimento *actual = tabla->buckets[i];
            while (actual) {
                printf("[ID=%d, Nombre=%s, Precio=%.2f, Disponible=%s] -> ",
                       actual->id, actual->nombre, actual->precio, actual->disponible ? "Sí" : "No");
                actual = actual->next;
            }
            printf("NULL\n");
        }
    }
}

/**
 * @brief Libera la memoria ocupada por la tabla hash.
 * @param tabla Puntero a la tabla hash a liberar.
 */
void liberarTabla(HashTable *tabla) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        Alimento *actual = tabla->buckets[i];
        while (actual) {
            Alimento *temp = actual;
            actual = actual->next;
            free(temp);
        }
    }
    free(tabla);
}
