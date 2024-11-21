/**
 * @file hash_clientes.h
 * @brief Declaraciones de funciones y estructuras para la gestión de clientes usando una tabla hash.
 */

#ifndef HASH_CLIENTES_H
#define HASH_CLIENTES_H

#include <stdbool.h>

/// Tamaño de la tabla hash para clientes.
#define CLIENTES_TABLE_SIZE 64

/**
 * @brief Representa un cliente.
 */
typedef struct Cliente {
    int idCliente;         ///< ID único del cliente.
    char nombre[50];       ///< Nombre completo del cliente.
    char telefono[15];     ///< Número de teléfono del cliente.
    struct Cliente *next;  ///< Puntero al siguiente cliente (manejo de colisiones).
} Cliente;

/**
 * @brief Estructura de la tabla hash para almacenar clientes.
 */
typedef struct {
    Cliente *buckets[CLIENTES_TABLE_SIZE]; ///< Arreglo de buckets que almacena listas enlazadas de clientes.
} ClientesHashTable;

/**
 * @brief Crea una nueva tabla hash para clientes.
 * @return Puntero a la tabla hash creada.
 */
ClientesHashTable *crearTablaClientes();

/**
 * @brief Calcula el índice hash para un ID de cliente.
 * @param idCliente ID del cliente.
 * @return Índice hash calculado.
 */
unsigned int hashCliente(int idCliente);

/**
 * @brief Agrega un cliente a la tabla hash.
 * @param tabla Puntero a la tabla hash.
 * @param idCliente ID único del cliente.
 * @param nombre Nombre del cliente.
 * @param telefono Teléfono del cliente.
 * @return `true` si el cliente se agregó correctamente, `false` si ocurrió un error o si el ID ya existe.
 */
bool agregarCliente(ClientesHashTable *tabla, int idCliente, const char *nombre, const char *telefono);

/**
 * @brief Busca un cliente en la tabla hash por su ID.
 * @param tabla Puntero a la tabla hash.
 * @param idCliente ID del cliente a buscar.
 * @return Puntero al cliente encontrado o `NULL` si no existe.
 */
Cliente *buscarCliente(ClientesHashTable *tabla, int idCliente);

/**
 * @brief Elimina un cliente de la tabla hash por su ID.
 * @param tabla Puntero a la tabla hash.
 * @param idCliente ID del cliente a eliminar.
 * @return `true` si el cliente fue eliminado, `false` si no se encontró.
 */
bool eliminarCliente(ClientesHashTable *tabla, int idCliente);

/**
 * @brief Actualiza la información de un cliente en la tabla hash.
 * @param tabla Puntero a la tabla hash.
 * @param idCliente ID del cliente a actualizar.
 * @param nuevoNombre Nuevo nombre del cliente.
 * @param nuevoTelefono Nuevo teléfono del cliente.
 * @return `true` si el cliente fue actualizado, `false` si no se encontró.
 */
bool actualizarCliente(ClientesHashTable *tabla, int idCliente, const char *nuevoNombre, const char *nuevoTelefono);

/**
 * @brief Muestra el contenido de la tabla hash de clientes.
 * @param tabla Puntero a la tabla hash.
 */
void mostrarTablaClientes(ClientesHashTable *tabla);

/**
 * @brief Libera la memoria ocupada por la tabla hash de clientes.
 * @param tabla Puntero a la tabla hash.
 */
void liberarTablaClientes(ClientesHashTable *tabla);

/**
 * @brief Obtiene el número de clientes almacenados en la tabla hash.
 * @param tabla Puntero a la tabla hash.
 * @return Número de clientes en la tabla.
 */
int obtenerNumeroDeClientes(ClientesHashTable *tabla);

/**
 * @brief Copia los clientes desde la tabla hash a un arreglo.
 * @param tabla Puntero a la tabla hash.
 * @param clientes Arreglo donde se copiarán los clientes.
 */
void copiarClientesDesdeTabla(ClientesHashTable *tabla, Cliente clientes[]);

/**
 * @brief Ordena un arreglo de clientes en paralelo utilizando QuickSort.
 * @param clientes Arreglo de clientes a ordenar.
 * @param low Índice inferior del rango.
 * @param high Índice superior del rango.
 */
void quicksortClientesParallel(Cliente clientes[], int low, int high);

/**
 * @brief Realiza una búsqueda binaria paralelizada en un arreglo de clientes.
 * @param clientes Arreglo de clientes ordenados.
 * @param numClientes Número total de clientes en el arreglo.
 * @param idBuscado ID del cliente a buscar.
 * @return Índice del cliente encontrado o `-1` si no existe.
 */
int busquedaBinariaClientesParallel(Cliente clientes[], int numClientes, int idBuscado);

#endif // HASH_CLIENTES_H
