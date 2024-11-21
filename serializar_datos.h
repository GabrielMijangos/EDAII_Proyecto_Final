#ifndef SERIALIZAR_DATOS_H
#define SERIALIZAR_DATOS_H

#include "hash_alimentos.h"
#include "hash_clientes.h"
#include "hash_menus.h"
#include "hash_pedidos.h"

/**
 * @brief Serializa los datos de las tablas hash (alimentos, clientes, menús y pedidos) y los guarda en un archivo.
 *
 * Esta función toma las tablas hash de alimentos, clientes, menús y pedidos, 
 * y las serializa en un archivo para su almacenamiento persistente.
 *
 * @param nombreArchivo Nombre del archivo donde se guardarán los datos serializados.
 * @param tablaAlimentos Puntero a la tabla hash de alimentos.
 * @param tablaClientes Puntero a la tabla hash de clientes.
 * @param tablaMenus Puntero a la tabla hash de menús.
 * @param tablaPedidos Puntero a la tabla hash de pedidos.
 */
void serializarDatos(const char *nombreArchivo, HashTable *tablaAlimentos, ClientesHashTable *tablaClientes, MenuHashTable *tablaMenus, PedidosHashTable *tablaPedidos);

#endif // SERIALIZAR_DATOS_H
