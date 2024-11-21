/**
 * @file hash_pedidos.h
 * @brief Definiciones y prototipos para la gestión de pedidos utilizando una tabla hash.
 */

#ifndef HASH_PEDIDOS_H
#define HASH_PEDIDOS_H

#include <stdbool.h>
#include "hash_menus.h"    // Para validar menús
#include "hash_clientes.h" // Para validar clientes

#define PEDIDOS_TABLE_SIZE 64 ///< Tamaño de la tabla hash para pedidos

/**
 * @brief Estructura que representa un pedido.
 */
typedef struct Pedido {
    int idPedido;          ///< ID único del pedido
    int idCliente;         ///< ID del cliente que realizó el pedido
    char fecha[11];        ///< Fecha del pedido (formato YYYY-MM-DD)
    int menus[50];         ///< IDs de los menús solicitados
    int numMenus;          ///< Número de menús en el pedido
    char estado[15];       ///< Estado del pedido (Pendiente, En Proceso, Completado)
    struct Pedido *next;   ///< Puntero al siguiente pedido (para manejar colisiones)
} Pedido;

/**
 * @brief Estructura para la tabla hash de pedidos.
 */
typedef struct {
    Pedido *buckets[PEDIDOS_TABLE_SIZE]; ///< Arreglo de buckets para almacenar pedidos
} PedidosHashTable;

/**
 * @brief Crea una nueva tabla hash para pedidos.
 * @return Puntero a la tabla hash creada.
 */
PedidosHashTable *crearTablaPedidos();

/**
 * @brief Calcula el índice hash para un ID de pedido.
 * @param idPedido ID único del pedido.
 * @return Índice hash calculado.
 */
unsigned int hashPedido(int idPedido);

/**
 * @brief Agrega un pedido a la tabla hash.
 * 
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
                   int idPedido, int idCliente, const char *fecha, int *menus, int numMenus, const char *estado);

/**
 * @brief Busca un pedido por su ID.
 * @param tablaPedidos Puntero a la tabla hash de pedidos.
 * @param idPedido ID único del pedido a buscar.
 * @return Puntero al pedido encontrado, o `NULL` si no existe.
 */
Pedido *buscarPedido(PedidosHashTable *tablaPedidos, int idPedido);

/**
 * @brief Elimina un pedido de la tabla hash.
 * @param tablaPedidos Puntero a la tabla hash de pedidos.
 * @param idPedido ID único del pedido a eliminar.
 * @return `true` si el pedido fue eliminado exitosamente, `false` en caso contrario.
 */
bool eliminarPedido(PedidosHashTable *tablaPedidos, int idPedido);

/**
 * @brief Actualiza el estado de un pedido.
 * @param tabla Puntero a la tabla hash de pedidos.
 * @param idPedido ID único del pedido a actualizar.
 * @param nuevoEstado Nuevo estado del pedido.
 * @return `true` si el pedido fue actualizado exitosamente, `false` en caso contrario.
 */
bool actualizarPedido(PedidosHashTable *tabla, int idPedido, const char *nuevoEstado);

/**
 * @brief Muestra todos los pedidos almacenados en la tabla hash.
 * @param tablaPedidos Puntero a la tabla hash de pedidos.
 */
void mostrarTablaPedidos(PedidosHashTable *tablaPedidos);

/**
 * @brief Libera la memoria utilizada por la tabla hash de pedidos.
 * @param tablaPedidos Puntero a la tabla hash de pedidos.
 */
void liberarTablaPedidos(PedidosHashTable *tablaPedidos);

/**
 * @brief Obtiene el número total de pedidos en la tabla hash.
 * @param tabla Puntero a la tabla hash de pedidos.
 * @return Número total de pedidos en la tabla.
 */
int obtenerNumeroDePedidos(PedidosHashTable *tabla);

/**
 * @brief Copia los pedidos de la tabla hash a un arreglo.
 * @param tabla Puntero a la tabla hash de pedidos.
 * @param pedidos Arreglo donde se copiarán los pedidos.
 */
void copiarPedidosDesdeTabla(PedidosHashTable *tabla, Pedido pedidos[]);

/**
 * @brief Ordena un arreglo de pedidos en paralelo utilizando QuickSort.
 * @param pedidos Arreglo de pedidos a ordenar.
 * @param low Índice inferior del rango.
 * @param high Índice superior del rango.
 */
void quicksortPedidosParallel(Pedido pedidos[], int low, int high);

/**
 * @brief Realiza una búsqueda binaria paralelizada en un arreglo de pedidos.
 * @param pedidos Arreglo de pedidos ordenados.
 * @param numPedidos Número total de pedidos en el arreglo.
 * @param idBuscado ID único del pedido a buscar.
 * @return Índice del pedido encontrado, o `-1` si no existe.
 */
int busquedaBinariaPedidosParallel(Pedido pedidos[], int numPedidos, int idBuscado);

#endif // HASH_PEDIDOS_H
