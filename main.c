/**
 * @file main.c
 * @brief Sistema de gestión de pedidos para alimentos, menús, clientes y pedidos.
 *
 * Este programa implementa un sistema para gestionar información de alimentos, menús, clientes y pedidos.
 * Incluye operaciones como agregar, buscar, eliminar, actualizar y ordenar datos, con capacidades avanzadas
 * para serializar y deserializar información en formato YAML.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <omp.h>
#include <time.h>

// Compilacion y ejecución pesonal
// gcc -Wall -std=c99 -fopenmp -I "C:\Users\Angel Mijangos\vcpkg\installed\x64-windows\include" -L "C:\Users\Angel Mijangos\vcpkg\installed\x64-windows\lib" -o salidaPF.out main.c deserializar_datos.c serializar_datos.c hash_pedidos.c hash_clientes.c hash_menus.c hash_alimentos.c -lyaml 
// salidaPF.out

// Headers personalizados
#include "hash_alimentos.h"
#include "hash_menus.h"
#include "hash_clientes.h"
#include "hash_pedidos.h"
#include "serializar_datos.h"
#include "deserializar_datos.h"

// Declaración de las tablas hash globales
HashTable *tablaAlimentos;
MenuHashTable *tablaMenus;
ClientesHashTable *tablaClientes;
PedidosHashTable *tablaPedidos;

// Prototipos de funciones
void generarDatosAleatorios(HashTable *tablaAlimentos, MenuHashTable *tablaMenus, ClientesHashTable *tablaClientes, PedidosHashTable *tablaPedidos);
void menuGesAlimentos();
void menuGesMenus();
void menuGesClientes();
void menuGesPedidos();
void menuAdministrador();
void menuCliente();

/**
 * @brief Opciones avanzadas del menú de pedidos.
 *
 * Proporciona funciones para ordenar pedidos utilizando QuickSort paralelizado
 * y búsqueda binaria paralelizada.
 *
 * @param tablaPedidos Puntero a la tabla hash de pedidos.
 */
void menuOpcionesAvanzadasPedidos(PedidosHashTable *tablaPedidos) 
{
    int opcion;
    do 
    {
        printf("\n--- Opciones Avanzadas: Pedidos ---\n");
        printf("1. Ordenar Pedidos (QuickSort Paralelizado)\n");
        printf("2. Buscar Pedido (Búsqueda Binaria Paralelizada)\n");
        printf("3. Volver\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch(opcion) 
        {
            case 1: 
            {
                int numPedidos = obtenerNumeroDePedidos(tablaPedidos);
                Pedido *pedidos = malloc(numPedidos * sizeof(Pedido));
                copiarPedidosDesdeTabla(tablaPedidos, pedidos);

                quicksortPedidosParallel(pedidos, 0, numPedidos - 1);

                printf("\nPedidos ordenados:\n");
                for(int i = 0; i < numPedidos; i++) 
                {
                    printf("[ID=%d, Cliente=%d, Fecha=%s, Estado=%s]\n",
                           pedidos[i].idPedido, pedidos[i].idCliente, pedidos[i].fecha, pedidos[i].estado);
                }

                free(pedidos);
                break;
            }
            case 2: 
            {
                int numPedidos = obtenerNumeroDePedidos(tablaPedidos);
                Pedido *pedidos = malloc(numPedidos * sizeof(Pedido));
                copiarPedidosDesdeTabla(tablaPedidos, pedidos);

                quicksortPedidosParallel(pedidos, 0, numPedidos - 1);

                int idBuscado;
                printf("Ingrese el ID del pedido a buscar: ");
                scanf("%d", &idBuscado);

                int resultado = busquedaBinariaPedidosParallel(pedidos, numPedidos, idBuscado);

                if (resultado != -1) 
                {
                    printf("\nPedido encontrado: [ID=%d, Cliente=%d, Fecha=%s, Estado=%s]\n",
                           pedidos[resultado].idPedido, pedidos[resultado].idCliente, pedidos[resultado].fecha, pedidos[resultado].estado);
                } 
                else 
                {
                    printf("\nPedido no encontrado.\n");
                }

                free(pedidos);
                break;
            }
            case 3:
            {
                printf("\nVolviendo al menú anterior...\n");
                break;
            }
            default:
                printf("\nOpción no válida. Intente de nuevo.\n");
        }
    }while(opcion!=3);
}

/**
 * @brief Menú de gestión de pedidos.
 *
 * Permite realizar operaciones como agregar, buscar, eliminar, actualizar y mostrar pedidos.
 * También incluye opciones avanzadas como ordenar y buscar utilizando algoritmos paralelizados.
 */
void menuGesPedidos()
{
    int opcion, idPedido, idCliente, numMenus, menus[50];
    char fecha[11], estado[15];
    Pedido *pedido;

    do{
        printf("\n--- Gestión de Pedidos ---\n");
        printf("1. Agregar Pedido\n");
        printf("2. Buscar Pedido\n");
        printf("3. Eliminar Pedido\n");
        printf("4. Actualizar Pedido (Estado del Pedido)\n");
        printf("5. Mostrar Tabla de Pedidos\n");
        printf("6. Funciones Avanzadas Menús\n");
        printf("7. Volver al menú anterior\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch(opcion)
        {
            case 1:
            {
                printf("Ingrese ID del Pedido: ");
                scanf("%d", &idPedido);
                printf("Ingrese ID del Cliente: ");
                scanf("%d", &idCliente);
                printf("Ingrese la Fecha del Pedido (YYYY-MM-DD): ");
                scanf("%s", fecha);
                printf("Ingrese el Número de Menús: ");
                scanf("%d", &numMenus);
                for(int i = 0; i < numMenus; i++)
                {
                    printf("Ingrese ID del Menú %d: ", i + 1);
                    scanf("%d", &menus[i]);
                }
                printf("Ingrese el Estado del Pedido (Pendiente/En Proceso/Completado): ");
                scanf("%s", estado);
                agregarPedido(tablaPedidos, tablaMenus, tablaClientes, idPedido, idCliente, fecha, menus, numMenus, estado);
                break;
            }
            case 2:
            {
                printf("Ingrese ID del Pedido: ");
                scanf("%d", &idPedido);
                pedido = buscarPedido(tablaPedidos, idPedido);
                if(pedido)
                {
                    printf("\nPedido encontrado: ID=%d, Cliente=%d, Fecha=%s, Estado=%s\n",
                           pedido->idPedido, pedido->idCliente, pedido->fecha, pedido->estado);
                } 
                else 
                {
                    printf("\nPedido no encontrado.\n");
                }
                break;
            }
            case 3:
            {
                printf("Ingrese ID del Pedido: ");
                scanf("%d", &idPedido);
                eliminarPedido(tablaPedidos, idPedido);
                break;
            }
            case 4:
            {
                printf("Ingrese el ID del Pedido: ");
    			scanf("%d", &idPedido);

    			printf("Ingrese el Nuevo Estado del Pedido (Pendiente/En Proceso/Completado): ");
    			scanf("%s", estado);

    			if (!actualizarPedido(tablaPedidos, idPedido, estado)) 
    			{
       				printf("\nError: No se pudo actualizar el estado del pedido.\n");
    			}
                break;
            }
            case 5:
            {
                mostrarTablaPedidos(tablaPedidos);
                break;
            }
            case 6:
            {
                printf("\nUsted ha seleccionado: Opciones Avanzadas\n");
                menuOpcionesAvanzadasPedidos(tablaPedidos);
                break;
            }
            case 7:
            {
            	printf("\nVolviendo al menú anterior...\n");
            	break;
            }
            default:
                printf("\nOpción no válida. Intente de nuevo.\n");
        }
    }while(opcion!=7);
}

/**
 * @brief Opciones avanzadas del menú de clientes.
 *
 * Proporciona funciones para ordenar clientes utilizando QuickSort paralelizado
 * y búsqueda binaria paralelizada.
 *
 * @param tablaClientes Puntero a la tabla hash de clientes.
 */
void menuOpcionesAvanzadasClientes(ClientesHashTable *tablaClientes) 
{
    int opcion;
    do 
    {
        printf("\n--- Opciones Avanzadas: Clientes ---\n");
        printf("1. Ordenar Clientes (QuickSort Paralelizado)\n");
        printf("2. Buscar Cliente (Búsqueda Binaria Paralelizada)\n");
        printf("3. Volver\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch(opcion) 
        {
            case 1: 
            {
                int numClientes = obtenerNumeroDeClientes(tablaClientes);
                Cliente *clientes = malloc(numClientes * sizeof(Cliente));
                copiarClientesDesdeTabla(tablaClientes, clientes);

                quicksortClientesParallel(clientes, 0, numClientes - 1);

                printf("\nClientes ordenados:\n");
                for(int i = 0; i < numClientes; i++) 
                {
                    printf("[ID=%d, Nombre=%s, Teléfono=%s]\n",
                           clientes[i].idCliente, clientes[i].nombre, clientes[i].telefono);
                }

                free(clientes);
                break;
            }
            case 2: 
            {
                int numClientes = obtenerNumeroDeClientes(tablaClientes);
                Cliente *clientes = malloc(numClientes * sizeof(Cliente));
                copiarClientesDesdeTabla(tablaClientes, clientes);

                quicksortClientesParallel(clientes, 0, numClientes - 1);

                int idBuscado;
                printf("Ingrese el ID del cliente a buscar: ");
                scanf("%d", &idBuscado);

                int resultado = busquedaBinariaClientesParallel(clientes, numClientes, idBuscado);

                if(resultado != -1) 
                {
                    printf("\nCliente encontrado: [ID=%d, Nombre=%s, Teléfono=%s]\n",
                           clientes[resultado].idCliente, clientes[resultado].nombre, clientes[resultado].telefono);
                } 
                else 
                {
                    printf("\nCliente no encontrado.\n");
                }

                free(clientes);
                break;
            }
            case 3:
            {
                printf("\nVolviendo al menú anterior...\n");
                break;
            }
            default:
                printf("\nOpción no válida. Intente de nuevo.\n");
        }
    }while(opcion!=3);
}

/**
 * @brief Menú de gestión de clientes.
 *
 * Permite realizar operaciones como agregar, buscar, eliminar, actualizar y mostrar clientes.
 * También incluye opciones avanzadas como ordenar y buscar utilizando algoritmos paralelizados.
 */
void menuGesClientes()
{
    int opcion, idCliente;
    char nombre[50], telefono[15];
    Cliente *cliente;

    do{
        printf("\n--- Gestión de Clientes ---\n");
        printf("1. Agregar Cliente\n");
        printf("2. Buscar Cliente\n");
        printf("3. Eliminar Cliente\n");
        printf("4. Actualizar Cliente\n");
        printf("5. Mostrar Tabla de Clientes\n");
        printf("6. Funciones Avanzadas Clientes\n");
        printf("7. Volver al menú anterior\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch(opcion)
        {
            case 1:
            {
                printf("Ingrese ID del Cliente: ");
                scanf("%d", &idCliente);
                printf("Ingrese Nombre: ");
                scanf(" %[^\n]", nombre);
                printf("Ingrese Teléfono: ");
                scanf("%s", telefono);
                agregarCliente(tablaClientes, idCliente, nombre, telefono);
                break;
            }
            case 2:
            {
                printf("Ingrese ID del Cliente: ");
                scanf("%d", &idCliente);
                cliente = buscarCliente(tablaClientes, idCliente);
                if(cliente)
                {
                    printf("\nCliente encontrado: ID=%d, Nombre=%s, Teléfono=%s\n",
                           cliente->idCliente, cliente->nombre, cliente->telefono);
                } 
                else 
                {
                    printf("\nCliente no encontrado.\n");
                }
                break;
            }
            case 3:
            {
                printf("Ingrese ID del Cliente: ");
                scanf("%d", &idCliente);
                eliminarCliente(tablaClientes, idCliente);
                break;
            }
            case 4:
            {
                printf("Ingrese ID del Cliente: ");
    			scanf("%d", &idCliente);
    			printf("Ingrese Nuevo Nombre: ");
    			scanf(" %[^\n]", nombre);
    			printf("Ingrese Nuevo Teléfono: ");
    			scanf("%s", telefono);
    			if (!actualizarCliente(tablaClientes, idCliente, nombre, telefono)) 
    			{
        			printf("\nError: No se pudo actualizar el cliente.\n");
    			}
                break;
            }
            case 5:
            {
                mostrarTablaClientes(tablaClientes);
                break;
            }
            case 6:
            {
                printf("\nUsted ha seleccionado: Opciones Avanzadas\n");
                menuOpcionesAvanzadasClientes(tablaClientes);
                break;
            }
            case 7:
            {
            	printf("\nVolviendo al menú anterior...\n");
            	break;
            }
            default:
                printf("\nOpción no válida. Intente de nuevo.\n");
        }
    }while(opcion!=7);
}

/**
 * @brief Opciones avanzadas del menú de menús.
 *
 * Proporciona funciones para ordenar menús utilizando QuickSort paralelizado
 * y búsqueda binaria paralelizada.
 *
 * @param tablaMenus Puntero a la tabla hash de menús.
 */
void menuOpcionesAvanzadasMenus(MenuHashTable *tablaMenus)
{
    int opcion;
    do
    {
        printf("\n--- Opciones Avanzadas: Menús ---\n");
        printf("1. Ordenar Menús (QuickSort Paralelizado)\n");
        printf("2. Buscar Menú (Búsqueda Binaria Paralelizada)\n");
        printf("3. Volver\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch(opcion)
        {
            case 1:
            {
                int numMenus = obtenerNumeroDeMenus(tablaMenus);
                Menu *menus = malloc(numMenus * sizeof(Menu));
                copiarMenusDesdeTabla(tablaMenus, menus);

                quicksortMenusParallel(menus, 0, numMenus - 1);

                printf("\nMenús ordenados:\n");
                for(int i = 0; i < numMenus; i++) 
                {
                    printf("[ID=%d, Fecha=%s, Número de Alimentos=%d]\n",
                           menus[i].idMenu, menus[i].fecha, menus[i].numAlimentos);
                }

                free(menus);
                break;
            }
            case 2:
            {
                int numMenus = obtenerNumeroDeMenus(tablaMenus);
                Menu *menus = malloc(numMenus * sizeof(Menu));
                copiarMenusDesdeTabla(tablaMenus, menus);

                quicksortMenusParallel(menus, 0, numMenus - 1);

                int idBuscado;
                printf("Ingrese el ID del menú a buscar: ");
                scanf("%d", &idBuscado);

                int resultado = busquedaBinariaMenusParallel(menus, numMenus, idBuscado);

                if(resultado != -1) 
                {
                    printf("\nMenú encontrado: [ID=%d, Fecha=%s, Número de Alimentos=%d]\n",
                           menus[resultado].idMenu, menus[resultado].fecha, menus[resultado].numAlimentos);
                } 
                else 
                {
                    printf("\nMenú no encontrado.\n");
                }

                free(menus);
                break;
            }
            case 3:
            {
                printf("\nVolviendo al menú anterior...\n");
                break;
            }
            default:
                printf("\nOpción no válida. Intente de nuevo.\n");
        }
    }while(opcion!=3);
}

/**
 * @brief Menú de gestión de menús.
 *
 * Permite realizar operaciones como agregar, buscar, eliminar, actualizar y mostrar menús.
 * También incluye opciones avanzadas como ordenar y buscar utilizando algoritmos paralelizados.
 */
void menuGesMenus()
{
    int opcion, idMenu, numAlimentos, alimentos[50];
    char fecha[11];
    Menu *menu;

    do{
        printf("\n--- Gestión de Menús ---\n");
        printf("1. Agregar Menú\n");
        printf("2. Buscar Menú\n");
        printf("3. Eliminar Menú\n");
        printf("4. Actualizar Menú\n");
        printf("5. Mostrar Tabla de Menús\n");
        printf("6. Funciones Avanzadas Menús\n");
        printf("7. Volver al menú anterior\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch(opcion)
        {
            case 1:
            {
                printf("Ingrese el ID del Menú: ");
    			scanf("%d", &idMenu);
    			printf("Ingrese la Fecha del Menú (YYYY-MM-DD): ");
    			scanf("%s", fecha);
    			printf("Ingrese el Número de Alimentos: ");
    			scanf("%d", &numAlimentos);

    			int alimentos[numAlimentos];
                for(int i = 0; i < numAlimentos; i++)
                {
                    while(1)
                    { // Bucle para repetir el ingreso hasta que sea válido
                        printf("Ingrese ID del Alimento %d: ", i + 1);
                        scanf("%d", &alimentos[i]);

                        Alimento *alimento = buscarAlimento(tablaAlimentos, alimentos[i]);
                        if (!alimento) 
                        {
                            printf("\nError: El alimento con ID=%d no existe. Intente de nuevo.\n", alimentos[i]);
                        } 
                        else if (alimento->disponible == 0) 
                        {
                            printf("\nError: El alimento con ID=%d no está disponible. Intente de nuevo.\n", alimentos[i]);
                        } 
                        else 
                        {
                        break; // Alimento válido, salir del bucle
                        }
                    }
                }

                if(!agregarMenu(tablaMenus, idMenu, fecha, alimentos, numAlimentos, tablaAlimentos)) 
                {
                    printf("\nError: No se pudo agregar el menú.\n");
                }
                break;
            }
            case 2:
            {
                printf("Ingrese el ID del Menú: ");
                scanf("%d", &idMenu);
                menu = buscarMenuPorID(tablaMenus, idMenu);
                if(menu) 
                {
                    printf("\nMenú encontrado: ID=%d, Fecha=%s, Número de Alimentos=%d\n",
                           menu->idMenu, menu->fecha, menu->numAlimentos);
                } 
                else 
                {
                    printf("\nNo se encontró un menú con el ID %d.\n", idMenu);
                }
                break;
            }
            case 3:
            {
                printf("Ingrese el ID del Menú: ");
                scanf("%d", &idMenu);
                if(!eliminarMenu(tablaMenus, idMenu)) 
                {
                    printf("\nError: No se pudo eliminar el menú.\n");
                }
                break;
            }
            case 4:
            {
                printf("Ingrese el ID del Menú: ");
    			scanf("%d", &idMenu);
    			printf("Ingrese la Nueva Fecha del Menú (YYYY-MM-DD): ");
   	 			scanf("%s", fecha);
    			printf("Ingrese el Nuevo Número de Alimentos: ");
    			scanf("%d", &numAlimentos);
    			for (int i = 0; i < numAlimentos; i++) 
    			{
        			printf("Ingrese ID del Alimento %d: ", i + 1);
        			scanf("%d", &alimentos[i]);
    			}
    			if (!actualizarMenu(tablaMenus, idMenu, fecha, alimentos, numAlimentos)) 
    			{
        			printf("\nError: No se pudo actualizar el menú.\n");
    			}
                break;
            }
            case 5:
            {
                mostrarTablaMenus(tablaMenus, tablaAlimentos);
                break;
            }
            case 6:
            {
                printf("\nUsted ha seleccionado: Opciones Avanzadas\n");
                menuOpcionesAvanzadasMenus(tablaMenus);
                break;
            }
            case 7:
            {
                printf("\nVolviendo al menú anterior...\n");
                break;
            }
            default:
                printf("\nOpción no válida. Intente de nuevo.\n");
        }
    }while(opcion!=7);
}

/**
 * @brief Opciones avanzadas del menú de alimentos.
 *
 * Proporciona funciones para ordenar alimentos utilizando QuickSort paralelizado
 * y búsqueda binaria paralelizada.
 *
 * @param tablaAlimentos Puntero a la tabla hash de alimentos.
 */
void menuOpcionesAvanzadasAlimentos(HashTable *tablaAlimentos)
{
    int opcion;
    do
    {
        printf("\n--- Opciones Avanzadas: Alimentos ---\n");
        printf("1. Ordenar Alimentos (QuickSort Paralelizado)\n");
        printf("2. Buscar Alimento (Búsqueda Binaria Paralelizada)\n");
        printf("3. Volver\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch(opcion)
        {
            case 1: 
            {
                int numAlimentos = obtenerNumeroDeAlimentos(tablaAlimentos);
                Alimento *alimentos = malloc(numAlimentos * sizeof(Alimento));
                copiarAlimentosDesdeTabla(tablaAlimentos, alimentos);

                quicksortAlimentosParallel(alimentos, 0, numAlimentos - 1);

                printf("\nAlimentos ordenados:\n");
                for(int i = 0; i < numAlimentos; i++) 
                {
                    printf("[ID=%d, Nombre=%s, Precio=%.2f, Disponible=%s]\n",
                           alimentos[i].id, alimentos[i].nombre, alimentos[i].precio,
                           alimentos[i].disponible ? "Sí" : "No");
                }

                free(alimentos);
                break;
            }
            case 2:
            {
                int numAlimentos = obtenerNumeroDeAlimentos(tablaAlimentos);
                Alimento *alimentos = malloc(numAlimentos * sizeof(Alimento));
                copiarAlimentosDesdeTabla(tablaAlimentos, alimentos);

                quicksortAlimentosParallel(alimentos, 0, numAlimentos - 1);

                int idBuscado;
                printf("Ingrese el ID del alimento a buscar: ");
                scanf("%d", &idBuscado);

                int resultado = busquedaBinariaAlimentosParallel(alimentos, numAlimentos, idBuscado);

                if(resultado != -1) 
                {
                    printf("\nAlimento encontrado: [ID=%d, Nombre=%s, Precio=%.2f, Disponible=%s]\n",
                           alimentos[resultado].id, alimentos[resultado].nombre, alimentos[resultado].precio,
                           alimentos[resultado].disponible ? "Sí" : "No");
                } 
                else 
                {
                    printf("\nAlimento no encontrado.\n");
                }

                free(alimentos);
                break;
            }
            case 3:
            {
                printf("\nVolviendo al menú anterior...\n");
                break;
            }
            default:
                printf("\nOpción no válida. Intente de nuevo.\n");
        }
    }while(opcion!=3);
}

/**
 * @brief Menú de gestión de alimentos.
 *
 * Permite realizar operaciones como agregar, buscar, eliminar, actualizar y mostrar alimentos.
 * También incluye opciones avanzadas como ordenar y buscar utilizando algoritmos paralelizados.
 */
void menuGesAlimentos()
{
    int opcion, id;
    char nombre[50];
    float precio;
    char disponibleStr[4]; // Para "Sí" o "No"
    Alimento *alimento;

    do{
        printf("\n--- Gestión de Alimentos ---\n");
        printf("1. Agregar Alimento\n");
        printf("2. Buscar Alimento\n");
        printf("3. Eliminar Alimento\n");
        printf("4. Actualizar Alimento\n");
        printf("5. Mostrar Tabla de Alimentos\n");
        printf("6. Funciones Avanzadas Alimentos\n");
        printf("7. Volver al menú anterior\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch(opcion)
        {
            case 1:
            {
                printf("Ingrese ID: ");
                scanf("%d", &id);
                printf("Ingrese Nombre: ");
                scanf(" %[^\n]", nombre);
                printf("Ingrese Precio: ");
                scanf("%f", &precio);
                printf("¿Disponible? (Use estrictamente Si/No): ");
                scanf("%s", disponibleStr);

                if(!insertarAlimento(tablaAlimentos, id, nombre, precio, disponibleStr)) 
                {
                    printf("\nError: No se pudo agregar el alimento.\n");
                }
                break;
            }
            case 2:
            {
                printf("Ingrese ID para buscar: ");
                scanf("%d", &id);
                alimento = buscarAlimento(tablaAlimentos, id);
                if(alimento) 
                {
                    printf("\nEncontrado: ID=%d, Nombre=%s, Precio=%.2f, Disponible=%s\n",
                           alimento->id, alimento->nombre, alimento->precio, alimento->disponible ? "Sí" : "No");
                } 
                else
                {
                    printf("\nAlimento no encontrado.\n");
                }
                break;
            }
            case 3:
            {
                printf("Ingrese ID para eliminar: ");
                scanf("%d", &id);
                eliminarAlimento(tablaAlimentos, id);
                break;
            }
            case 4:
            {
                printf("Ingrese ID del Alimento: ");
                scanf("%d", &id);
                printf("Ingrese Nuevo Nombre: ");
                scanf(" %[^\n]", nombre);
                printf("Ingrese Nuevo Precio: ");
                scanf("%f", &precio);
                printf("Ingrese Nueva Disponibilidad (Use estrictamente Si/No): ");
                scanf("%s", disponibleStr);
                if(!actualizarAlimento(tablaAlimentos, id, nombre, precio, disponibleStr)) 
                {
                    printf("\nError: No se pudo actualizar el alimento.\n");
                }
                break;
            }
            case 5:
            {
                mostrarTabla(tablaAlimentos);
                break;
            }
            case 6:
            {
                printf("\nUsted ha seleccionado: Opciones Avanzadas\n");
                menuOpcionesAvanzadasAlimentos(tablaAlimentos);
                break;
            }
            case 7:
            {
                printf("\nVolviendo al menú anterior...\n");
                break;
            }
            default:
                printf("\nOpción no válida. Intente de nuevo.\n");
        }
    }while(opcion!=7);
}

/**
 * @brief Menú principal para el administrador.
 *
 * Permite gestionar alimentos, menús, clientes y pedidos, con acceso a todas las funcionalidades avanzadas.
 */
void menuAdministrador()
{
    int opcion;

    do{
        printf("\n--- Menú Administrador ---\n");
        printf("1. Gestionar Alimentos\n");
        printf("2. Gestionar Menús\n");
        printf("3. Gestionar Clientes\n");
        printf("4. Gestionar Pedidos\n");
        printf("5. Volver al menú principal\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch(opcion)
        {
            case 1:
            {
                printf("\nUsted a seleccionado: Gestionar Alimentos\n");
                menuGesAlimentos();   
            	break;
            }
            case 2:
            {	
                printf("\nUsted a seleccionado: Gestionar Menús\n");
                menuGesMenus();
            	break;
            }
            case 3:
            {
                printf("\nUsted a seleccionado: Gestionar Clientes\n");
                menuGesClientes();
            	break;
            }
            case 4:
            {
                printf("\nUsted a seleccionado: Gestionar Pedidos\n");
                menuGesPedidos();
            	break;
            }
            case 5:
            {
            	printf("\nUsted a seleccionado: Volver al menú principal\n");
                printf("Volviendo al menú principal...\n");
                break;
            }
            default:
                printf("\nOpción no válida. Intente de nuevo.\n");
        }
    }while(opcion!=5);
}

/**
 * @brief Menú principal para los clientes.
 *
 * Permite a los clientes ver menús disponibles, realizar pedidos y consultar el estado de sus pedidos.
 */
void menuCliente()
{
    int opcion;                    // Para la selección del menú
    int idCliente;                 // ID del cliente
    int idPedido;                  // ID del pedido
    int numMenus;                  // Número de menús en un pedido
    int menus[50];                 // IDs de menús
    char fecha[11];                // Fecha del pedido (YYYY-MM-DD)
    Pedido *pedido;

    do{
        printf("\n--- Menú Cliente ---\n");
        printf("1. Ver Menús Disponibles\n");
        printf("2. Hacer un Pedido\n");
        printf("3. Consultar Pedido\n");
        printf("4. Volver al menú principal\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch(opcion)
        {
            case 1:
            {
                printf("\nUsted a seleccionado: Ver Menús Disponibles\n");
                printf("\n--- Menús Disponibles ---\n");
    			mostrarTablaMenus(tablaMenus, tablaAlimentos);
            	break;
            }
            case 2:
            {
                printf("\nUsted a seleccionado: Hacer un Pedido\n");
                printf("\nIngrese su ID de Cliente: ");
    			scanf("%d", &idCliente);

    			// Verificar que el cliente exista
    			if(!buscarCliente(tablaClientes, idCliente))
    			{
       				printf("Error: Cliente con ID=%d no existe. No se puede hacer el pedido.\n", idCliente);
        			break;
    			}

    			printf("Ingrese ID del Pedido: ");
    			scanf("%d", &idPedido);
    			printf("Ingrese la Fecha del Pedido (YYYY-MM-DD): ");
    			scanf("%s", fecha);
    			printf("Ingrese el Número de Menús que desea: ");
    			scanf("%d", &numMenus);

    			for(int i = 0; i < numMenus; i++)
    			{
        			printf("Ingrese ID del Menú %d: ", i + 1);
        			scanf("%d", &menus[i]);

        			// Verificar que el menú exista
        			if(!buscarMenuPorID(tablaMenus, menus[i]))
        			{
           				printf("Error: El menú con ID=%d no existe. Pedido cancelado.\n", menus[i]);
            			break;
        			}
    			}

    			// Agregar el pedido con estado "Pendiente"
    			if(agregarPedido(tablaPedidos, tablaMenus, tablaClientes, idPedido, idCliente, fecha, menus, numMenus, "Pendiente")) 
    			{
        			printf("Pedido realizado con éxito. ID del Pedido: %d\n", idPedido);
    			} 
    			else 
    			{
        			printf("Error: No se pudo realizar el pedido.\n");
    			}
            	break;
            }
            case 3:
            {
                printf("\nUsted a seleccionado: Consultar Pedido\n");
                printf("\nIngrese el ID del Pedido: ");
    			scanf("%d", &idPedido);

    			pedido = buscarPedido(tablaPedidos, idPedido);

    			if(pedido) 
    			{
        			printf("Pedido encontrado: ID=%d, Cliente=%d, Fecha=%s, Estado=%s, Número de Menús=%d\n",
               			pedido->idPedido, pedido->idCliente, pedido->fecha, pedido->estado, pedido->numMenus);
        			for(int i = 0; i < pedido->numMenus; i++)
        			{
            			printf(" - Menú ID: %d\n", pedido->menus[i]);
        			}
    			} 
    			else 
    			{
        			printf("Error: Pedido con ID=%d no encontrado.\n", idPedido);
    			}
            	break;
            }
            case 4:
            {
            	printf("\nUsted a seleccionado: Volver al menú principal\n");
                printf("Volviendo al menú principal...\n");    	
            	break;
            }
            default:
                printf("\nOpción no válida. Intente de nuevo.\n");
        }
    }while(opcion!=4);
}

/**
 * @brief Genera datos aleatorios para las tablas de alimentos, menús, clientes y pedidos.
 *
 * @param tablaAlimentos Puntero a la tabla hash de alimentos.
 * @param tablaMenus Puntero a la tabla hash de menús.
 * @param tablaClientes Puntero a la tabla hash de clientes.
 * @param tablaPedidos Puntero a la tabla hash de pedidos.
 */
void generarDatosAleatorios(HashTable *tablaAlimentos, MenuHashTable *tablaMenus, ClientesHashTable *tablaClientes, PedidosHashTable *tablaPedidos)
{
    srand(time(NULL));

    int cantidadAlimentos, cantidadClientes, cantidadMenus, cantidadPedidos;

    printf("\nIngrese la cantidad de alimentos a generar: ");
    scanf("%d", &cantidadAlimentos);

    printf("Ingrese la cantidad de clientes a generar: ");
    scanf("%d", &cantidadClientes);

    printf("Ingrese la cantidad de menús a generar: ");
    scanf("%d", &cantidadMenus);

    printf("Ingrese la cantidad de pedidos a generar: ");
    scanf("%d", &cantidadPedidos);

    // Generar alimentos aleatorios
    for (int i = 1; i <= cantidadAlimentos; i++)
    {
        char nombre[50];
        sprintf(nombre, "Alimento%d", i); // Generar nombre como "Alimento1", "Alimento2", etc.
        float precio = (rand() % 10000) / 100.0; // Precio entre 0.00 y 99.99
        const char *disponible = (rand() % 2 == 0) ? "Si" : "Si";
        insertarAlimento(tablaAlimentos, i, nombre, precio, disponible);
    }

    // Generar clientes aleatorios
    for (int i = 1; i <= cantidadClientes; i++)
    {
        char nombre[50];
        sprintf(nombre, "Cliente%d", i); // Generar nombre como "Cliente1", "Cliente2", etc.
        char telefono[15];
        sprintf(telefono, "55%06d", rand() % 100000); // Teléfono como "555-1234"
        agregarCliente(tablaClientes, i, nombre, telefono);
    }

    // Generar menús aleatorios
    for (int i = 1; i <= cantidadMenus; i++)
    {
        char fecha[11];
        sprintf(fecha, "2024-%02d-%02d", (rand() % 12) + 1, (rand() % 28) + 1); // Fecha aleatoria en 2024

        int numAlimentos = (rand() % 3) + 1; // Entre 1 y 5 alimentos
        int alimentos[5];
        for(int j = 0; j < numAlimentos; j++)
        {
            alimentos[j] = (rand() % cantidadAlimentos) + 1; // IDs de alimentos entre 1 y la cantidad generada
        }

        agregarMenu(tablaMenus, i, fecha, alimentos, numAlimentos, tablaAlimentos);
    }

    // Generar pedidos aleatorios
    for(int i = 1; i <= cantidadPedidos; i++)
    {
        char fecha[11];
        sprintf(fecha, "2024-%02d-%02d", (rand() % 12) + 1, (rand() % 28) + 1); // Fecha aleatoria en 2024

        int idCliente = (rand() % cantidadClientes) + 1; // ID de cliente entre 1 y la cantidad generada
        int numMenus = (rand() % 3) + 1;  // Entre 1 y 3 menús
        int menus[3];
        for(int j = 0; j < numMenus; j++)
        {
            menus[j] = (rand() % cantidadMenus) + 1; // IDs de menús entre 1 y la cantidad generada
        }

        const char *estado;
        switch (rand() % 3)
        {
            case 0: estado = "Pendiente"; break;
            case 1: estado = "En Proceso"; break;
            default: estado = "Completado"; break;
        }

        agregarPedido(tablaPedidos, tablaMenus, tablaClientes, i, idCliente, fecha, menus, numMenus, estado);
    }

    printf("\nDatos aleatorios generados exitosamente:\n");
    printf("- %d alimentos\n", cantidadAlimentos);
    printf("- %d clientes\n", cantidadClientes);
    printf("- %d menús\n", cantidadMenus);
    printf("- %d pedidos\n", cantidadPedidos);
}

/**
 * @brief Función principal del programa.
 *
 * Permite al usuario seleccionar entre varias opciones de gestión como administrador o cliente,
 * y realizar operaciones como guardar, cargar o generar datos.
 *
 * @return int Devuelve 0 al finalizar la ejecución correctamente.
 */
int main()
{
	tablaAlimentos = crearTablaHash();
	tablaMenus = crearTablaMenus();
	tablaClientes = crearTablaClientes();
	tablaPedidos = crearTablaPedidos();
	int opcion;

	do{
        printf("\n--- Sistema de Gestión de Pedidos ---\n");
        printf("1. Entrar como Administrador\n");
        printf("2. Entrar como Cliente\n");
        printf("3. Guardar todos los datos actuales (serializar)\n");
        printf("4. Cargar datos (deserializar)\n"); //Aun no funciona
        printf("5. Generar Datos Aleatorios\n");
        printf("6. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch(opcion)
        {
            case 1:
            {
                printf("\nUsted a seleccionado: Entrar como Administrador\n");
                menuAdministrador();
            	break;
            }
            case 2:
            {
                printf("\nUsted a seleccionado: Entrar como Cliente\n");
                menuCliente();
            	break;
            }
            case 3:
            {
                printf("\nUsted a seleccionado: Guardar todos los datos actuales\n");
                char nombreArchivo[100];
                printf("\nIngrese el nombre del archivo para guardar los datos (incluya la extensión, por ejemplo, datos.yaml): ");
                scanf("%s", nombreArchivo);
                serializarDatos(nombreArchivo, tablaAlimentos, tablaClientes, tablaMenus, tablaPedidos);
                printf("\nDatos guardados en '%s'.\n", nombreArchivo);
                break;
            }
            case 4:
            {
                printf("\nUsted a seleccionado: Cargar datos\n");
                printf("\nFunción aún no disponible\n");
                /*
                    No pude terminar la deserialización. La dejo aqui con fines
                    propios, por lo que le pido que no la tome encuenta como parte
                    de la entrega final de mi proyecto.
                */
                /*
                char nombreArchivo[100];
                printf("\nIngrese el nombre del archivo YAML a cargar (incluya la extensión, por ejemplo, datos.yaml): ");
                scanf("%s", nombreArchivo);
                deserializarDatos(nombreArchivo, tablaAlimentos, tablaClientes, tablaMenus, tablaPedidos);
                printf("\nDatos cargados desde '%s'.\n", nombreArchivo);
                */
                break;
            }
            case 5:
            {
                printf("\nUsted ha seleccionado: Generar Datos Aleatorios\n");
                generarDatosAleatorios(tablaAlimentos, tablaMenus, tablaClientes, tablaPedidos);
                break;
            }
            case 6:
            {
                printf("\nSaliendo del sistema...\n");
                liberarTablaMenus(tablaMenus);
                liberarTabla(tablaAlimentos);
                liberarTablaClientes(tablaClientes);
                liberarTablaPedidos(tablaPedidos);
                break;
            }
            default:
                printf("\nOpción no válida. Intente de nuevo.\n");
        }
    }while(opcion!=6);

}
