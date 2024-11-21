/**
 * @file hash_pedidos.c
 * @brief Implementación de las funciones para la gestión de pedidos utilizando una tabla hash.
 */

#include "hash_pedidos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

/**
 * @brief Crea una nueva tabla hash para pedidos.
 * @return Puntero a la tabla hash creada.
 */
PedidosHashTable *crearTablaPedidos() {
    PedidosHashTable *tabla = (PedidosHashTable *)malloc(sizeof(PedidosHashTable));
    if (!tabla) {
        printf("Error: No se pudo crear la tabla hash para pedidos.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < PEDIDOS_TABLE_SIZE; i++) {
        tabla->buckets[i] = NULL;
    }
    return tabla;
}

/**
 * @brief Calcula el índice hash para un ID de pedido.
 * @param idPedido ID único del pedido.
 * @return Índice hash calculado.
 */
unsigned int hashPedido(int idPedido) {
    return idPedido % PEDIDOS_TABLE_SIZE;
}

/**
 * @brief Copia los pedidos de la tabla hash a un arreglo.
 * @param tabla Puntero a la tabla hash de pedidos.
 * @param pedidos Arreglo donde se copiarán los pedidos.
 */
void copiarPedidosDesdeTabla(PedidosHashTable *tabla, Pedido pedidos[]) {
    int index = 0;
    for (int i = 0; i < PEDIDOS_TABLE_SIZE; i++) {
        Pedido *actual = tabla->buckets[i];
        while (actual) {
            pedidos[index++] = *actual;
            actual = actual->next;
        }
    }
}

/**
 * @brief Cuenta el número de pedidos en la tabla hash.
 * @param tabla Puntero a la tabla hash de pedidos.
 * @return Número total de pedidos en la tabla.
 */
int obtenerNumeroDePedidos(PedidosHashTable *tabla) {
    int contador = 0;
    for (int i = 0; i < PEDIDOS_TABLE_SIZE; i++) {
        Pedido *actual = tabla->buckets[i];
        while (actual) {
            contador++;
            actual = actual->next;
        }
    }
    return contador;
}

/**
 * @brief Realiza la partición de un arreglo de pedidos para QuickSort.
 * @param pedidos Arreglo de pedidos.
 * @param low Índice inferior del rango.
 * @param high Índice superior del rango.
 * @return Índice del pivote.
 */
int particionPedidos(Pedido pedidos[], int low, int high) {
    int pivot = pedidos[high].idPedido;
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (pedidos[j].idPedido < pivot) {
            i++;
            Pedido temp = pedidos[i];
            pedidos[i] = pedidos[j];
            pedidos[j] = temp;
        }
    }

    Pedido temp = pedidos[i + 1];
    pedidos[i + 1] = pedidos[high];
    pedidos[high] = temp;

    return i + 1;
}

/**
 * @brief Ordena un arreglo de pedidos en paralelo utilizando QuickSort.
 * @param pedidos Arreglo de pedidos a ordenar.
 * @param low Índice inferior del rango.
 * @param high Índice superior del rango.
 */
void quicksortPedidosParallel(Pedido pedidos[], int low, int high) {
    if (low < high) {
        int pi = particionPedidos(pedidos, low, high);

        #pragma omp parallel sections
        {
            #pragma omp section
            quicksortPedidosParallel(pedidos, low, pi - 1);

            #pragma omp section
            quicksortPedidosParallel(pedidos, pi + 1, high);
        }
    }
}

/**
 * @brief Realiza una búsqueda binaria paralelizada en un arreglo de pedidos.
 * @param pedidos Arreglo de pedidos ordenados.
 * @param numPedidos Número total de pedidos en el arreglo.
 * @param idBuscado ID único del pedido a buscar.
 * @return Índice del pedido encontrado, o `-1` si no existe.
 */
int busquedaBinariaPedidosParallel(Pedido pedidos[], int numPedidos, int idBuscado) {
    int resultado = -1;

    #pragma omp parallel
    {
        int low = 0, high = numPedidos - 1;
        while (low <= high) {
            int mid = low + (high - low) / 2;

            if (pedidos[mid].idPedido == idBuscado) {
                #pragma omp critical
                {
                    resultado = mid; // Actualizamos el resultado de forma segura
                }
                break;
            } else if (pedidos[mid].idPedido < idBuscado) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
    }

    return resultado;
}

/**
 * @brief Agrega un pedido a la tabla hash con validación de duplicados y referencias.
 * @param tablaPedidos Puntero a la tabla hash de pedidos.
 * @param tablaMenus Puntero a la tabla hash de menús para validar los menús solicitados.
 * @param tablaClientes Puntero a la tabla hash de clientes para validar el cliente.
 * @param idPedido ID único del pedido.
 * @param idCliente ID del cliente que realizó el pedido.
 * @param fecha Fecha del pedido (formato YYYY-MM-DD).
 * @param menus Arreglo de IDs de los menús solicitados.
 * @param numMenus Número de menús en el pedido.
 * @param estado Estado inicial del pedido.
 * @return `true` si el pedido fue agregado exitosamente, `false` en caso de error.
 */
bool agregarPedido(PedidosHashTable *tablaPedidos, MenuHashTable *tablaMenus, ClientesHashTable *tablaClientes,
                   int idPedido, int idCliente, const char *fecha, int *menus, int numMenus, const char *estado) {
    if (buscarPedido(tablaPedidos, idPedido)) {
        printf("\nError: El pedido con ID=%d ya existe.\n", idPedido);
        return false;
    }

    if (!buscarCliente(tablaClientes, idCliente)) {
        printf("\nError: El cliente con ID=%d no existe. Pedido no agregado.\n", idCliente);
        return false;
    }

    for (int i = 0; i < numMenus; i++) {
        if (!buscarMenuPorID(tablaMenus, menus[i])) {
            printf("\nError: El menú con ID=%d no existe. Pedido no agregado.\n", menus[i]);
            return false;
        }
    }

    unsigned int index = hashPedido(idPedido);

    Pedido *nuevo = (Pedido *)malloc(sizeof(Pedido));
    if (!nuevo) {
        printf("\nError: No se pudo agregar el pedido.\n");
        return false;
    }

    nuevo->idPedido = idPedido;
    nuevo->idCliente = idCliente;
    strcpy(nuevo->fecha, fecha);
    memcpy(nuevo->menus, menus, numMenus * sizeof(int));
    nuevo->numMenus = numMenus;
    strcpy(nuevo->estado, estado);
    nuevo->next = tablaPedidos->buckets[index];
    tablaPedidos->buckets[index] = nuevo;

    printf("\nPedido agregado: ID=%d, Cliente=%d, Fecha=%s, Estado=%s\n", idPedido, idCliente, fecha, estado);
    return true;
}

/**
 * @brief Busca un pedido en la tabla hash por su ID.
 * @param tablaPedidos Puntero a la tabla hash de pedidos.
 * @param idPedido ID único del pedido a buscar.
 * @return Puntero al pedido encontrado, o `NULL` si no existe.
 */
Pedido *buscarPedido(PedidosHashTable *tablaPedidos, int idPedido) {
    unsigned int index = hashPedido(idPedido);
    Pedido *actual = tablaPedidos->buckets[index];

    while (actual) {
        if (actual->idPedido == idPedido) {
            return actual;
        }
        actual = actual->next;
    }
    return NULL;
}

/**
 * @brief Elimina un pedido de la tabla hash.
 * @param tablaPedidos Puntero a la tabla hash de pedidos.
 * @param idPedido ID único del pedido a eliminar.
 * @return `true` si el pedido fue eliminado exitosamente, `false` en caso contrario.
 */
bool eliminarPedido(PedidosHashTable *tablaPedidos, int idPedido) {
    unsigned int index = hashPedido(idPedido);
    Pedido *actual = tablaPedidos->buckets[index];
    Pedido *previo = NULL;

    while (actual) {
        if (actual->idPedido == idPedido) {
            if (previo) {
                previo->next = actual->next;
            } else {
                tablaPedidos->buckets[index] = actual->next;
            }
            free(actual);
            printf("\nPedido eliminado: ID=%d\n", idPedido);
            return true;
        }
        previo = actual;
        actual = actual->next;
    }
    printf("\nError: Pedido con ID=%d no encontrado.\n", idPedido);
    return false;
}

/**
 * @brief Actualiza el estado de un pedido.
 * @param tabla Puntero a la tabla hash de pedidos.
 * @param idPedido ID único del pedido a actualizar.
 * @param nuevoEstado Nuevo estado del pedido.
 * @return `true` si el pedido fue actualizado exitosamente, `false` en caso contrario.
 */
bool actualizarPedido(PedidosHashTable *tabla, int idPedido, const char *nuevoEstado) {
    Pedido *pedido = buscarPedido(tabla, idPedido);
    if (!pedido) {
        printf("\nError: Pedido con ID=%d no encontrado.\n", idPedido);
        return false;
    }

    strcpy(pedido->estado, nuevoEstado);
    printf("\nPedido actualizado: ID=%d, Nuevo Estado=%s\n", idPedido, pedido->estado);
    return true;
}

/**
 * @brief Muestra todos los pedidos almacenados en la tabla hash.
 * @param tabla Puntero a la tabla hash de pedidos.
 */
void mostrarTablaPedidos(PedidosHashTable *tabla) {
    printf("\n--- Tabla Hash de Pedidos ---\n");
    for (int i = 0; i < PEDIDOS_TABLE_SIZE; i++) {
        if (tabla->buckets[i] != NULL) {
            printf("Bucket %d: ", i);
            Pedido *actual = tabla->buckets[i];
            while (actual) {
                printf("[ID=%d, Cliente=%d, Fecha=%s, Estado=%s] -> ",
                       actual->idPedido, actual->idCliente, actual->fecha, actual->estado);
                actual = actual->next;
            }
            printf("NULL\n");
        }
    }
}

/**
 * @brief Libera la memoria utilizada por la tabla hash de pedidos.
 * @param tablaPedidos Puntero a la tabla hash de pedidos.
 */
void liberarTablaPedidos(PedidosHashTable *tablaPedidos) {
    for (int i = 0; i < PEDIDOS_TABLE_SIZE; i++) {
        Pedido *actual = tablaPedidos->buckets[i];
        while (actual) {
            Pedido *temp = actual;
            actual = actual->next;
            free(temp);
        }
    }
    free(tablaPedidos);
}
