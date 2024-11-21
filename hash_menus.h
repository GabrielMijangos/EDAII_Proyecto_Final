/**
 * @file hash_menus.h
 * @brief Declaraciones de funciones y estructuras para la gestión de menús utilizando una tabla hash.
 */

#ifndef HASH_MENUS_H
#define HASH_MENUS_H

#include <stdbool.h>
#include "hash_alimentos.h"

/// Tamaño de la tabla hash para menús.
#define MENU_TABLE_SIZE 64

/**
 * @brief Representa un menú.
 */
typedef struct Menu {
    int idMenu;             ///< ID único del menú.
    char fecha[11];         ///< Fecha del menú (formato YYYY-MM-DD).
    int alimentos[50];      ///< IDs de alimentos disponibles en el menú.
    int numAlimentos;       ///< Número total de alimentos en el menú.
    struct Menu *next;      ///< Puntero al siguiente menú (manejo de colisiones).
} Menu;

/**
 * @brief Estructura de la tabla hash para almacenar menús.
 */
typedef struct {
    Menu *buckets[MENU_TABLE_SIZE]; ///< Arreglo de buckets que almacena listas enlazadas de menús.
} MenuHashTable;

/**
 * @brief Crea una nueva tabla hash para menús.
 * @return Puntero a la tabla hash creada.
 */
MenuHashTable *crearTablaMenus();

/**
 * @brief Calcula el índice hash para un ID de menú.
 * @param idMenu ID del menú.
 * @return Índice hash calculado.
 */
unsigned int hashIDMenu(int idMenu);

/**
 * @brief Busca un menú por su ID en la tabla hash.
 * @param tabla Puntero a la tabla hash.
 * @param idMenu ID del menú a buscar.
 * @return Puntero al menú encontrado o `NULL` si no existe.
 */
Menu *buscarMenuPorID(MenuHashTable *tabla, int idMenu);

/**
 * @brief Agrega un menú a la tabla hash.
 * @param tabla Puntero a la tabla hash.
 * @param idMenu ID único del menú.
 * @param fecha Fecha del menú (formato YYYY-MM-DD).
 * @param alimentos Arreglo de IDs de alimentos en el menú.
 * @param numAlimentos Número de alimentos en el menú.
 * @param tablaAlimentos Puntero a la tabla hash de alimentos para validar los alimentos.
 * @return `true` si el menú se agregó correctamente, `false` si ocurrió un error o si el ID ya existe.
 */
bool agregarMenu(MenuHashTable *tabla, int idMenu, const char *fecha, int *alimentos, int numAlimentos, HashTable *tablaAlimentos);

/**
 * @brief Elimina un menú de la tabla hash por su ID.
 * @param tabla Puntero a la tabla hash.
 * @param idMenu ID del menú a eliminar.
 * @return `true` si el menú fue eliminado, `false` si no se encontró.
 */
bool eliminarMenu(MenuHashTable *tabla, int idMenu);

/**
 * @brief Actualiza la información de un menú en la tabla hash.
 * @param tabla Puntero a la tabla hash.
 * @param idMenu ID del menú a actualizar.
 * @param nuevaFecha Nueva fecha del menú.
 * @param nuevosAlimentos Arreglo de IDs de los nuevos alimentos del menú.
 * @param numAlimentos Número de alimentos en el menú.
 * @return `true` si el menú fue actualizado, `false` si no se encontró.
 */
bool actualizarMenu(MenuHashTable *tabla, int idMenu, const char *nuevaFecha, int *nuevosAlimentos, int numAlimentos);

/**
 * @brief Muestra el contenido de la tabla hash de menús.
 * @param tablaMenus Puntero a la tabla hash de menús.
 * @param tablaAlimentos Puntero a la tabla hash de alimentos para mostrar detalles de los alimentos.
 */
void mostrarTablaMenus(MenuHashTable *tablaMenus, HashTable *tablaAlimentos);

/**
 * @brief Libera la memoria ocupada por la tabla hash de menús.
 * @param tabla Puntero a la tabla hash.
 */
void liberarTablaMenus(MenuHashTable *tabla);

/**
 * @brief Obtiene el número de menús almacenados en la tabla hash.
 * @param tabla Puntero a la tabla hash.
 * @return Número de menús en la tabla.
 */
int obtenerNumeroDeMenus(MenuHashTable *tabla);

/**
 * @brief Copia los menús desde la tabla hash a un arreglo.
 * @param tabla Puntero a la tabla hash.
 * @param menus Arreglo donde se copiarán los menús.
 */
void copiarMenusDesdeTabla(MenuHashTable *tabla, Menu menus[]);

/**
 * @brief Ordena un arreglo de menús en paralelo utilizando QuickSort.
 * @param menus Arreglo de menús a ordenar.
 * @param low Índice inferior del rango.
 * @param high Índice superior del rango.
 */
void quicksortMenusParallel(Menu menus[], int low, int high);

/**
 * @brief Realiza una búsqueda binaria paralelizada en un arreglo de menús.
 * @param menus Arreglo de menús ordenados.
 * @param numMenus Número total de menús en el arreglo.
 * @param idBuscado ID del menú a buscar.
 * @return Índice del menú encontrado o `-1` si no existe.
 */
int busquedaBinariaMenusParallel(Menu menus[], int numMenus, int idBuscado);

#endif // HASH_MENUS_H
