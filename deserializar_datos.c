/**
 * @file deserializar_datos.c
 * @brief Implementación de funciones para deserializar datos en formato YAML y cargarlos en tablas hash.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <yaml.h>
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
 * @brief Valida y corrige el nombre del archivo para asegurar que tenga la extensión .yaml.
 * 
 * @param nombreArchivo Nombre del archivo proporcionado por el usuario.
 */
void validarNombreArchivo(char *nombreArchivo) {
    const char *extension = strrchr(nombreArchivo, '.');
    if (!extension || strcmp(extension, ".yaml") != 0) {
        strcat(nombreArchivo, ".yaml");
    }
}

/**
 * @brief Manejo genérico para cargar secuencias en YAML.
 */
bool avanzarEnSecuencia(yaml_parser_t *parser) {
    yaml_event_t event;
    if (!yaml_parser_parse(parser, &event)) return false;
    bool resultado = (event.type == YAML_SEQUENCE_START_EVENT);
    yaml_event_delete(&event);
    return resultado;
}

/**
 * @brief Carga los alimentos desde un archivo YAML a una tabla hash.
 */
void cargarAlimentos(yaml_parser_t *parser, HashTable *tablaAlimentos) {
    yaml_event_t event;
    int id = -1;
    char nombre[50];
    float precio = 0.0;
    char disponibleStr[3] = "No";

    while (yaml_parser_parse(parser, &event) && event.type != YAML_SEQUENCE_END_EVENT) {
        if (event.type == YAML_MAPPING_START_EVENT) {
            id = -1;
            nombre[0] = '\0';
            precio = 0.0f;
            strcpy(disponibleStr, "No");
        } else if (event.type == YAML_SCALAR_EVENT) {
            const char *clave = (char *)event.data.scalar.value;

            yaml_event_delete(&event);
            yaml_parser_parse(parser, &event);
            if (strcmp(clave, "id") == 0) {
                id = atoi((char *)event.data.scalar.value);
            } else if (strcmp(clave, "nombre") == 0) {
                strcpy(nombre, (char *)event.data.scalar.value);
            } else if (strcmp(clave, "precio") == 0) {
                precio = atof((char *)event.data.scalar.value);
            } else if (strcmp(clave, "disponible") == 0) {
                strcpy(disponibleStr, (char *)event.data.scalar.value);
            }
        } else if (event.type == YAML_MAPPING_END_EVENT) {
            if (!insertarAlimento(tablaAlimentos, id, nombre, precio, disponibleStr)) {
                printf("\nError al insertar el alimento con ID=%d\n", id);
            }
        }
        yaml_event_delete(&event);
    }
}

/**
 * @brief Carga los clientes desde un archivo YAML a una tabla hash.
 */
void cargarClientes(yaml_parser_t *parser, ClientesHashTable *tablaClientes) {
    yaml_event_t event;
    int id = -1;
    char nombre[50];
    char telefono[15];

    while (yaml_parser_parse(parser, &event) && event.type != YAML_SEQUENCE_END_EVENT) {
        if (event.type == YAML_MAPPING_START_EVENT) {
            id = -1;
            nombre[0] = '\0';
            telefono[0] = '\0';
        } else if (event.type == YAML_SCALAR_EVENT) {
            const char *clave = (char *)event.data.scalar.value;

            yaml_event_delete(&event);
            yaml_parser_parse(parser, &event);
            if (strcmp(clave, "id") == 0) {
                id = atoi((char *)event.data.scalar.value);
            } else if (strcmp(clave, "nombre") == 0) {
                strcpy(nombre, (char *)event.data.scalar.value);
            } else if (strcmp(clave, "telefono") == 0) {
                strcpy(telefono, (char *)event.data.scalar.value);
            }
        } else if (event.type == YAML_MAPPING_END_EVENT) {
            if (!agregarCliente(tablaClientes, id, nombre, telefono)) {
                printf("\nError al insertar el cliente con ID=%d\n", id);
            }
        }
        yaml_event_delete(&event);
    }
}

/**
 * @brief Carga los menús desde un archivo YAML a una tabla hash.
 */
void cargarMenus(yaml_parser_t *parser, MenuHashTable *tablaMenus, HashTable *tablaAlimentos) {
    yaml_event_t event;
    int idMenu = -1;
    char fecha[11];
    int alimentos[50];
    int numAlimentos = 0;

    while (yaml_parser_parse(parser, &event) && event.type != YAML_SEQUENCE_END_EVENT) {
        if (event.type == YAML_MAPPING_START_EVENT) {
            idMenu = -1;
            fecha[0] = '\0';
            numAlimentos = 0;
        } else if (event.type == YAML_SCALAR_EVENT) {
            const char *clave = (char *)event.data.scalar.value;

            yaml_event_delete(&event);
            yaml_parser_parse(parser, &event);
            if (strcmp(clave, "id") == 0) {
                idMenu = atoi((char *)event.data.scalar.value);
            } else if (strcmp(clave, "fecha") == 0) {
                strcpy(fecha, (char *)event.data.scalar.value);
            } else if (strcmp(clave, "alimentos") == 0) {
                if (avanzarEnSecuencia(parser)) {
                    numAlimentos = 0;
                    while (yaml_parser_parse(parser, &event) && event.type != YAML_SEQUENCE_END_EVENT) {
                        if (event.type == YAML_SCALAR_EVENT) {
                            alimentos[numAlimentos++] = atoi((char *)event.data.scalar.value);
                        }
                        yaml_event_delete(&event);
                    }
                }
            }
        } else if (event.type == YAML_MAPPING_END_EVENT) {
            if (!agregarMenu(tablaMenus, idMenu, fecha, alimentos, numAlimentos, tablaAlimentos)) {
                printf("\nError al insertar el menú con ID=%d\n", idMenu);
            }
        }
        yaml_event_delete(&event);
    }
}

/**
 * @brief Deserializa los datos desde un archivo YAML y los carga en las tablas hash.
 */
void deserializarDatos(const char *nombreArchivoOriginal, HashTable *tablaAlimentos, ClientesHashTable *tablaClientes, MenuHashTable *tablaMenus, PedidosHashTable *tablaPedidos) {
    char nombreArchivo[100];
    strncpy(nombreArchivo, nombreArchivoOriginal, sizeof(nombreArchivo) - 1);
    nombreArchivo[sizeof(nombreArchivo) - 1] = '\0';
    validarNombreArchivo(nombreArchivo);

    FILE *archivo = fopen(nombreArchivo, "r");
    if (!archivo) {
        printf("\nError: No se pudo abrir el archivo '%s'.\n", nombreArchivo);
        return;
    }

    yaml_parser_t parser;
    if (!yaml_parser_initialize(&parser)) {
        printf("\nError: No se pudo inicializar el parser de YAML.\n");
        fclose(archivo);
        return;
    }
    yaml_parser_set_input_file(&parser, archivo);

    yaml_event_t event;

    while (yaml_parser_parse(&parser, &event)) {
        if (event.type == YAML_SCALAR_EVENT) {
            const char *clave = (char *)event.data.scalar.value;
            if (strcmp(clave, "alimentos") == 0) {
                cargarAlimentos(&parser, tablaAlimentos);
            } else if (strcmp(clave, "clientes") == 0) {
                cargarClientes(&parser, tablaClientes);
            } else if (strcmp(clave, "menus") == 0) {
                cargarMenus(&parser, tablaMenus, tablaAlimentos);
            }
        } else if (event.type == YAML_STREAM_END_EVENT) {
            break;
        }
        yaml_event_delete(&event);
    }

    yaml_parser_delete(&parser);
    fclose(archivo);
    printf("\nDeserialización finalizada con éxito.\n");
}
