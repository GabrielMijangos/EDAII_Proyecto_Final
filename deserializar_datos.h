#ifndef DESERIALIZAR_DATOS_H
#define DESERIALIZAR_DATOS_H

#include "hash_alimentos.h"
#include "hash_clientes.h"
#include "hash_menus.h"
#include "hash_pedidos.h"


/*
    No pude terminar la deserialización. La dejo aqui con fines
    propios, por lo que le pido que no la tome encuenta como parte
    de la entrega final de mi proyecto.
*/


/**
 * @brief Deserializa los datos desde un archivo en formato YAML y los carga en las tablas hash correspondientes.
 *
 * Esta función lee el contenido de un archivo YAML y llena las tablas hash de alimentos,
 * clientes, menús y pedidos con los datos deserializados.
 *
 * @param nombreArchivo Nombre del archivo YAML que contiene los datos serializados.
 * @param tablaAlimentos Puntero a la tabla hash de alimentos que se llenará con los datos deserializados.
 * @param tablaClientes Puntero a la tabla hash de clientes que se llenará con los datos deserializados.
 * @param tablaMenus Puntero a la tabla hash de menús que se llenará con los datos deserializados.
 * @param tablaPedidos Puntero a la tabla hash de pedidos que se llenará con los datos deserializados.
 */
void deserializarDatos(const char *nombreArchivo, HashTable *tablaAlimentos, ClientesHashTable *tablaClientes, MenuHashTable *tablaMenus, PedidosHashTable *tablaPedidos);

#endif // DESERIALIZAR_DATOS_H
