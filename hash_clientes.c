/**
 * @file hash_clientes.c
 * @brief Implementación de las funciones para la tabla hash de clientes.
 */

#include "hash_clientes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

/**
 * @brief Crea una nueva tabla hash para almacenar clientes.
 * @return Puntero a la tabla hash creada.
 */
ClientesHashTable *crearTablaClientes() {
    ClientesHashTable *tabla = (ClientesHashTable *)malloc(sizeof(ClientesHashTable));
    if (!tabla) {
        printf("Error: No se pudo crear la tabla hash para clientes.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < CLIENTES_TABLE_SIZE; i++) {
        tabla->buckets[i] = NULL;
    }
    return tabla;
}

/**
 * @brief Calcula el índice hash para un ID de cliente.
 * @param idCliente ID del cliente.
 * @return Índice hash en la tabla.
 */
unsigned int hashCliente(int idCliente) {
    return idCliente % CLIENTES_TABLE_SIZE;
}

/**
 * @brief Copia los clientes de la tabla hash a un arreglo.
 * @param tabla Puntero a la tabla hash de clientes.
 * @param clientes Arreglo donde se almacenarán los clientes copiados.
 */
void copiarClientesDesdeTabla(ClientesHashTable *tabla, Cliente clientes[]) {
    int index = 0;
    for (int i = 0; i < CLIENTES_TABLE_SIZE; i++) {
        Cliente *actual = tabla->buckets[i];
        while (actual) {
            clientes[index++] = *actual;
            actual = actual->next;
        }
    }
}

/**
 * @brief Cuenta el número de clientes almacenados en la tabla hash.
 * @param tabla Puntero a la tabla hash de clientes.
 * @return Número de clientes en la tabla.
 */
int obtenerNumeroDeClientes(ClientesHashTable *tabla) {
    int contador = 0;
    for (int i = 0; i < CLIENTES_TABLE_SIZE; i++) {
        Cliente *actual = tabla->buckets[i];
        while (actual) {
            contador++;
            actual = actual->next;
        }
    }
    return contador;
}

/**
 * @brief Realiza la partición para el algoritmo QuickSort.
 * @param clientes Arreglo de clientes.
 * @param low Índice inferior del rango.
 * @param high Índice superior del rango.
 * @return Índice de partición.
 */
int particionClientes(Cliente clientes[], int low, int high) {
    int pivot = clientes[high].idCliente;
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (clientes[j].idCliente < pivot) {
            i++;
            Cliente temp = clientes[i];
            clientes[i] = clientes[j];
            clientes[j] = temp;
        }
    }

    Cliente temp = clientes[i + 1];
    clientes[i + 1] = clientes[high];
    clientes[high] = temp;

    return i + 1;
}

/**
 * @brief Ordena un arreglo de clientes en paralelo utilizando QuickSort.
 * @param clientes Arreglo de clientes a ordenar.
 * @param low Índice inferior del rango.
 * @param high Índice superior del rango.
 */
void quicksortClientesParallel(Cliente clientes[], int low, int high) {
    if (low < high) {
        int pi = particionClientes(clientes, low, high);

        #pragma omp parallel sections
        {
            #pragma omp section
            quicksortClientesParallel(clientes, low, pi - 1);

            #pragma omp section
            quicksortClientesParallel(clientes, pi + 1, high);
        }
    }
}

/**
 * @brief Realiza una búsqueda binaria paralelizada para encontrar un cliente por su ID.
 * @param clientes Arreglo de clientes ordenados.
 * @param numClientes Número de clientes en el arreglo.
 * @param idBuscado ID del cliente a buscar.
 * @return Índice del cliente encontrado, o -1 si no existe.
 */
int busquedaBinariaClientesParallel(Cliente clientes[], int numClientes, int idBuscado) {
    int resultado = -1;

    #pragma omp parallel
    {
        int low = 0, high = numClientes - 1;
        while (low <= high) {
            int mid = low + (high - low) / 2;

            if (clientes[mid].idCliente == idBuscado) {
                #pragma omp critical
                {
                    resultado = mid; // Actualizamos el resultado de forma segura
                }
                break; // Salimos del bucle
            } else if (clientes[mid].idCliente < idBuscado) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
    }

    return resultado;
}

/**
 * @brief Agrega un cliente a la tabla hash.
 * @param tabla Puntero a la tabla hash.
 * @param idCliente ID único del cliente.
 * @param nombre Nombre del cliente.
 * @param telefono Teléfono del cliente.
 * @return true si el cliente fue agregado correctamente, false en caso contrario.
 */
bool agregarCliente(ClientesHashTable *tabla, int idCliente, const char *nombre, const char *telefono) {
    if (buscarCliente(tabla, idCliente)) { // Verificar si el ID ya existe
        printf("\nError: El cliente con ID=%d ya existe.\n", idCliente);
        return false;
    }

    unsigned int index = hashCliente(idCliente);

    Cliente *nuevo = (Cliente *)malloc(sizeof(Cliente));
    if (!nuevo) {
        printf("\nError: No se pudo agregar el cliente.\n");
        return false;
    }

    nuevo->idCliente = idCliente;
    strcpy(nuevo->nombre, nombre);
    strcpy(nuevo->telefono, telefono);
    nuevo->next = tabla->buckets[index];
    tabla->buckets[index] = nuevo;

    printf("\nCliente agregado: ID=%d, Nombre=%s, Teléfono=%s\n", idCliente, nombre, telefono);
    return true;
}

/**
 * @brief Busca un cliente en la tabla hash.
 * @param tabla Puntero a la tabla hash.
 * @param idCliente ID del cliente a buscar.
 * @return Puntero al cliente encontrado, o NULL si no existe.
 */
Cliente *buscarCliente(ClientesHashTable *tabla, int idCliente) {
    unsigned int index = hashCliente(idCliente);
    Cliente *actual = tabla->buckets[index];

    while (actual) {
        if (actual->idCliente == idCliente) {
            return actual;
        }
        actual = actual->next;
    }
    return NULL;
}

/**
 * @brief Elimina un cliente de la tabla hash.
 * @param tabla Puntero a la tabla hash.
 * @param idCliente ID del cliente a eliminar.
 * @return true si el cliente fue eliminado correctamente, false en caso contrario.
 */
bool eliminarCliente(ClientesHashTable *tabla, int idCliente) {
    unsigned int index = hashCliente(idCliente);
    Cliente *actual = tabla->buckets[index];
    Cliente *previo = NULL;

    while (actual) {
        if (actual->idCliente == idCliente) {
            if (previo) {
                previo->next = actual->next;
            } else {
                tabla->buckets[index] = actual->next;
            }
            free(actual);
            printf("\nCliente eliminado: ID=%d\n", idCliente);
            return true;
        }
        previo = actual;
        actual = actual->next;
    }
    printf("\nError: Cliente con ID=%d no encontrado.\n", idCliente);
    return false;
}

/**
 * @brief Actualiza la información de un cliente en la tabla hash.
 * @param tabla Puntero a la tabla hash.
 * @param idCliente ID del cliente a actualizar.
 * @param nuevoNombre Nuevo nombre del cliente.
 * @param nuevoTelefono Nuevo teléfono del cliente.
 * @return true si la actualización fue exitosa, false en caso contrario.
 */
bool actualizarCliente(ClientesHashTable *tabla, int idCliente, const char *nuevoNombre, const char *nuevoTelefono) {
    Cliente *cliente = buscarCliente(tabla, idCliente);
    if (!cliente) {
        printf("\nError: Cliente con ID=%d no encontrado.\n", idCliente);
        return false;
    }

    strcpy(cliente->nombre, nuevoNombre);
    strcpy(cliente->telefono, nuevoTelefono);
    printf("\nCliente actualizado: ID=%d, Nombre=%s, Teléfono=%s\n", idCliente, cliente->nombre, cliente->telefono);
    return true;
}

/**
 * @brief Muestra todos los clientes almacenados en la tabla hash.
 * @param tabla Puntero a la tabla hash.
 */
void mostrarTablaClientes(ClientesHashTable *tabla) {
    printf("\n--- Tabla Hash de Clientes ---\n");
    for (int i = 0; i < CLIENTES_TABLE_SIZE; i++) {
        if (tabla->buckets[i] != NULL) { // Verificar si el bucket tiene datos
            printf("Bucket %d: ", i);
            Cliente *actual = tabla->buckets[i];
            while (actual) {
                printf("[ID=%d, Nombre=%s, Teléfono=%s] -> ",
                       actual->idCliente, actual->nombre, actual->telefono);
                actual = actual->next;
            }
            printf("NULL\n");
        }
    }
}

/**
 * @brief Libera la memoria ocupada por la tabla hash de clientes.
 * @param tabla Puntero a la tabla hash a liberar.
 */
void liberarTablaClientes(ClientesHashTable *tabla) {
    for (int i = 0; i < CLIENTES_TABLE_SIZE; i++) {
        Cliente *actual = tabla->buckets[i];
        while (actual) {
            Cliente *temp = actual;
            actual = actual->next;
            free(temp);
        }
    }
    free(tabla);
}
