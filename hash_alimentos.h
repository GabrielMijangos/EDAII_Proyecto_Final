#ifndef HASH_ALIMENTOS_H
#define HASH_ALIMENTOS_H

#include <stdbool.h> // Para usar el tipo bool
#include <stdlib.h>  // Para malloc, free

/** 
 * @def HASH_TABLE_SIZE
 * @brief Tamaño de la tabla hash.
 */
#define HASH_TABLE_SIZE 64

/**
 * @struct Alimento
 * @brief Estructura que representa un alimento en la tabla hash.
 * 
 * @var Alimento::id
 * ID único del alimento.
 * @var Alimento::nombre
 * Nombre del alimento.
 * @var Alimento::precio
 * Precio del alimento.
 * @var Alimento::disponible
 * Disponibilidad del alimento (1 para disponible, 0 para no disponible).
 * @var Alimento::next
 * Puntero al siguiente alimento (para manejar colisiones).
 */
typedef struct Alimento {
    int id;
    char nombre[50];
    float precio;
    int disponible;
    struct Alimento *next; // Encadenamiento para colisiones
} Alimento;

/**
 * @struct HashTable
 * @brief Estructura que representa la tabla hash para almacenar alimentos.
 * 
 * @var HashTable::buckets
 * Arreglo de punteros a los buckets de la tabla hash.
 */
typedef struct {
    Alimento *buckets[HASH_TABLE_SIZE];
} HashTable;

/**
 * @brief Crea e inicializa una tabla hash.
 * @return Puntero a la tabla hash creada.
 */
HashTable *crearTablaHash();

/**
 * @brief Calcula la posición hash utilizando la técnica de Mid-Square.
 * @param id ID del alimento.
 * @return Índice hash en la tabla.
 */
unsigned int hashFunctionMidSquare(int id);

/**
 * @brief Inserta un alimento en la tabla hash.
 * @param tabla Puntero a la tabla hash.
 * @param id ID único del alimento.
 * @param nombre Nombre del alimento.
 * @param precio Precio del alimento.
 * @param disponibleStr Disponibilidad del alimento ("Si" o "No").
 * @return true si el alimento se insertó correctamente, false en caso contrario.
 */
bool insertarAlimento(HashTable *tabla, int id, const char *nombre, float precio, const char *disponibleStr);

/**
 * @brief Busca un alimento en la tabla hash.
 * @param tabla Puntero a la tabla hash.
 * @param id ID del alimento a buscar.
 * @return Puntero al alimento encontrado, o NULL si no existe.
 */
Alimento *buscarAlimento(HashTable *tabla, int id);

/**
 * @brief Elimina un alimento de la tabla hash.
 * @param tabla Puntero a la tabla hash.
 * @param id ID del alimento a eliminar.
 * @return true si el alimento se eliminó correctamente, false en caso contrario.
 */
bool eliminarAlimento(HashTable *tabla, int id);

/**
 * @brief Actualiza la información de un alimento en la tabla hash.
 * @param tabla Puntero a la tabla hash.
 * @param idAlimento ID del alimento a actualizar.
 * @param nuevoNombre Nuevo nombre del alimento.
 * @param nuevoPrecio Nuevo precio del alimento.
 * @param nuevaDisponibilidadStr Nueva disponibilidad ("Si" o "No").
 * @return true si el alimento se actualizó correctamente, false en caso contrario.
 */
bool actualizarAlimento(HashTable *tabla, int idAlimento, const char *nuevoNombre, float nuevoPrecio, const char *nuevaDisponibilidadStr);

/**
 * @brief Muestra todos los alimentos almacenados en la tabla hash.
 * @param tabla Puntero a la tabla hash.
 */
void mostrarTabla(HashTable *tabla);

/**
 * @brief Libera la memoria ocupada por la tabla hash.
 * @param tabla Puntero a la tabla hash a liberar.
 */
void liberarTabla(HashTable *tabla);

/**
 * @brief Ordena un arreglo de alimentos en paralelo utilizando QuickSort.
 * @param alimentos Arreglo de alimentos a ordenar.
 * @param low Índice inicial del arreglo.
 * @param high Índice final del arreglo.
 */
void quicksortAlimentosParallel(Alimento alimentos[], int low, int high);

/**
 * @brief Realiza una búsqueda binaria en paralelo para encontrar un alimento por su ID.
 * @param alimentos Arreglo de alimentos ordenados.
 * @param numAlimentos Número de alimentos en el arreglo.
 * @param idBuscado ID del alimento a buscar.
 * @return Índice del alimento encontrado, o -1 si no existe.
 */
int busquedaBinariaAlimentosParallel(Alimento alimentos[], int numAlimentos, int idBuscado);

/**
 * @brief Copia los alimentos desde la tabla hash a un arreglo.
 * @param tabla Puntero a la tabla hash.
 * @param alimentos Arreglo donde se almacenarán los alimentos copiados.
 */
void copiarAlimentosDesdeTabla(HashTable *tabla, Alimento alimentos[]);

/**
 * @brief Obtiene el número de alimentos almacenados en la tabla hash.
 * @param tabla Puntero a la tabla hash.
 * @return Número de alimentos en la tabla.
 */
int obtenerNumeroDeAlimentos(HashTable *tabla);

#endif // HASH_ALIMENTOS_H
