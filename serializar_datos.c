#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_alimentos.h"
#include "hash_clientes.h"
#include "hash_menus.h"
#include "hash_pedidos.h"

/**
 * @brief Guarda los alimentos en un archivo en formato YAML.
 *
 * Esta función recorre la tabla hash de alimentos y escribe su contenido
 * en un archivo en formato YAML.
 *
 * @param archivo Puntero al archivo donde se guardarán los datos.
 * @param tablaAlimentos Puntero a la tabla hash de alimentos.
 */
void guardarAlimentos(FILE *archivo, HashTable *tablaAlimentos) {
    fprintf(archivo, "alimentos:\n");
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        Alimento *actual = tablaAlimentos->buckets[i];
        while (actual) {
            fprintf(archivo, "  - id: %d\n", actual->id);
            fprintf(archivo, "    nombre: \"%s\"\n", actual->nombre);
            fprintf(archivo, "    precio: %.2f\n", actual->precio);
            fprintf(archivo, "    disponible: %s\n", actual->disponible ? "Si" : "No");
            actual = actual->next;
        }
    }
}

/**
 * @brief Guarda los clientes en un archivo en formato YAML.
 *
 * Esta función recorre la tabla hash de clientes y escribe su contenido
 * en un archivo en formato YAML.
 *
 * @param archivo Puntero al archivo donde se guardarán los datos.
 * @param tablaClientes Puntero a la tabla hash de clientes.
 */
void guardarClientes(FILE *archivo, ClientesHashTable *tablaClientes) {
    fprintf(archivo, "clientes:\n");
    for (int i = 0; i < CLIENTES_TABLE_SIZE; i++) {
        Cliente *actual = tablaClientes->buckets[i];
        while (actual) {
            fprintf(archivo, "  - id: %d\n", actual->idCliente);
            fprintf(archivo, "    nombre: \"%s\"\n", actual->nombre);
            fprintf(archivo, "    telefono: \"%s\"\n", actual->telefono);
            actual = actual->next;
        }
    }
}

/**
 * @brief Guarda los menús en un archivo en formato YAML.
 *
 * Esta función recorre la tabla hash de menús y escribe su contenido,
 * incluyendo los alimentos asociados, en un archivo en formato YAML.
 *
 * @param archivo Puntero al archivo donde se guardarán los datos.
 * @param tablaMenus Puntero a la tabla hash de menús.
 * @param tablaAlimentos Puntero a la tabla hash de alimentos.
 */
void guardarMenus(FILE *archivo, MenuHashTable *tablaMenus, HashTable *tablaAlimentos) {
    fprintf(archivo, "menus:\n");
    for (int i = 0; i < MENU_TABLE_SIZE; i++) {
        Menu *actual = tablaMenus->buckets[i];
        while (actual) {
            fprintf(archivo, "  - id: %d\n", actual->idMenu);
            fprintf(archivo, "    fecha: \"%s\"\n", actual->fecha);
            fprintf(archivo, "    alimentos:\n");
            for (int j = 0; j < actual->numAlimentos; j++) {
                Alimento *alimento = buscarAlimento(tablaAlimentos, actual->alimentos[j]);
                if (alimento) {
                    fprintf(archivo, "      - id: %d\n", alimento->id);
                    fprintf(archivo, "        nombre: \"%s\"\n", alimento->nombre);
                    fprintf(archivo, "        precio: %.2f\n", alimento->precio);
                    fprintf(archivo, "        disponible: %s\n", alimento->disponible ? "Si" : "No");
                } else {
                    fprintf(archivo, "      - id: %d\n", actual->alimentos[j]);
                    fprintf(archivo, "        error: \"No encontrado\"\n");
                }
            }
            actual = actual->next;
        }
    }
}

/**
 * @brief Guarda los pedidos en un archivo en formato YAML.
 *
 * Esta función recorre la tabla hash de pedidos y escribe su contenido
 * en un archivo en formato YAML.
 *
 * @param archivo Puntero al archivo donde se guardarán los datos.
 * @param tablaPedidos Puntero a la tabla hash de pedidos.
 */
void guardarPedidos(FILE *archivo, PedidosHashTable *tablaPedidos) {
    fprintf(archivo, "pedidos:\n");
    for (int i = 0; i < PEDIDOS_TABLE_SIZE; i++) {
        Pedido *actual = tablaPedidos->buckets[i];
        while (actual) {
            fprintf(archivo, "  - id: %d\n", actual->idPedido);
            fprintf(archivo, "    cliente_id: %d\n", actual->idCliente);
            fprintf(archivo, "    fecha: \"%s\"\n", actual->fecha);
            fprintf(archivo, "    estado: \"%s\"\n", actual->estado);
            fprintf(archivo, "    menus: [");
            for (int j = 0; j < actual->numMenus; j++) {
                fprintf(archivo, "%d", actual->menus[j]);
                if (j < actual->numMenus - 1) fprintf(archivo, ", ");
            }
            fprintf(archivo, "]\n");
            actual = actual->next;
        }
    }
}

/**
 * @brief Serializa todos los datos de las tablas hash en un archivo.
 *
 * Esta función coordina la serialización de alimentos, clientes, menús y pedidos,
 * y los guarda en un archivo en formato YAML.
 *
 * @param nombreArchivo Nombre del archivo donde se guardarán los datos serializados.
 * @param tablaAlimentos Puntero a la tabla hash de alimentos.
 * @param tablaClientes Puntero a la tabla hash de clientes.
 * @param tablaMenus Puntero a la tabla hash de menús.
 * @param tablaPedidos Puntero a la tabla hash de pedidos.
 */
void serializarDatos(const char *nombreArchivo, HashTable *tablaAlimentos, ClientesHashTable *tablaClientes, MenuHashTable *tablaMenus, PedidosHashTable *tablaPedidos) {
    FILE *archivo = fopen(nombreArchivo, "w");
    if (!archivo) {
        printf("\nError: No se pudo abrir el archivo '%s' para escritura.\n", nombreArchivo);
        return;
    }

    // Agregar el indicador de inicio del documento YAML
    fprintf(archivo, "---\n");

    guardarAlimentos(archivo, tablaAlimentos);
    guardarClientes(archivo, tablaClientes);
    guardarMenus(archivo, tablaMenus, tablaAlimentos);
    guardarPedidos(archivo, tablaPedidos);

    fclose(archivo);
    printf("\nDatos guardados exitosamente en '%s'.\n", nombreArchivo);
}
