/*
    Trabajo Practico - Programacion 1
    Sistema de gestion de pedidos de comida

    Funcionalidades:
    1. Agregar un pedido.
    2. Listar todos los pedidos.
    3. Listar pedidos cuyo precio supere un importe ingresado.
    4. Buscar pedidos por un campo a eleccion (numero, cliente, comida,
       tipo o estado).
    5. Separar pedidos entregados y pendientes en dos archivos.
    6. Salir.

    Formato de pedidos.txt (una linea por pedido, separado por ';'):
    numero;cliente;comida;tipo;precio;entregado

    Compilar:
    gcc -Wall -Wextra -std=c99 pedidos.c -o pedidos
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>

#define MAX_TEXTO 60
#define MAX_LINEA 256
#define PRECIO_MAXIMO 1000000000.0
#define ARCHIVO_PEDIDOS "pedidos.txt"
#define ARCHIVO_ENTREGADOS "entregados.txt"
#define ARCHIVO_PENDIENTES "pendientes.txt"

typedef struct {
    int numero;
    char cliente[MAX_TEXTO];
    char comida[MAX_TEXTO];
    char tipo;              /* L = local, D = domicilio */
    float precio;
    bool entregado;
} Pedido;

void mostrarMenu(void);
void leerLinea(char destino[], int tamanio);
void leerTexto(const char mensaje[], char destino[], int tamanio);
int leerEnteroRango(const char mensaje[], int minimo, int maximo);
int leerEntero(const char mensaje[]);
float leerFloat(const char mensaje[]);
char leerTipo(void);
bool leerEstado(void);
bool leerPedido(FILE *archivo, Pedido *pedido);
bool escribirPedido(FILE *archivo, Pedido pedido);
void mostrarPedido(Pedido pedido);
bool contieneTexto(const char texto[], const char buscado[]);
bool coincide(Pedido pedido, int campo, Pedido criterio);
bool numeroExiste(int numero);
bool agregarPedido(void);
void listarPedidos(void);
void listarPorPrecio(void);
void buscarPedido(void);
void dividirPedidos(void);

int main(void) {
    int opcion;

    do {
        mostrarMenu();
        opcion = leerEntero("Seleccione una opcion: ");

        if (opcion == 1) {
            if (agregarPedido()) {
                printf("Pedido guardado correctamente.\n");
            } else {
                printf("No se pudo guardar el pedido.\n");
            }
        } else if (opcion == 2) {
            listarPedidos();
        } else if (opcion == 3) {
            listarPorPrecio();
        } else if (opcion == 4) {
            buscarPedido();
        } else if (opcion == 5) {
            dividirPedidos();
        } else if (opcion == 6) {
            printf("Saliendo del sistema.\n");
        } else {
            printf("Opcion invalida. Intente nuevamente.\n");
        }
        printf("\n");
    } while (opcion != 6);

    return 0;
}

void mostrarMenu(void) {
    printf("====================================\n");
    printf("    SISTEMA DE PEDIDOS DE COMIDA\n");
    printf("====================================\n");
    printf("1. Agregar un pedido\n");
    printf("2. Listar todos los pedidos\n");
    printf("3. Listar pedidos por precio\n");
    printf("4. Buscar pedidos\n");
    printf("5. Separar entregados y pendientes\n");
    printf("6. Salir\n");
    printf("====================================\n");
}

/* Lee una linea completa de teclado, sin el '\n' y descartando lo que no
   entre en el buffer. Si la entrada se cierra (EOF), termina el programa
   para evitar bucles infinitos. */
void leerLinea(char destino[], int tamanio) {
    int longitud;
    int caracter;

    if (fgets(destino, tamanio, stdin) == NULL) {
        printf("\nEntrada cerrada. Saliendo del sistema.\n");
        exit(EXIT_SUCCESS);
    }

    longitud = (int)strlen(destino);
    if (longitud > 0 && destino[longitud - 1] == '\n') {
        destino[longitud - 1] = '\0';
    } else {
        caracter = getchar();
        while (caracter != '\n' && caracter != EOF) {
            caracter = getchar();
        }
    }
}

/* Lee un texto no vacio y sin ';' (el ';' es el separador del archivo). */
void leerTexto(const char mensaje[], char destino[], int tamanio) {
    bool valido = false;

    while (!valido) {
        printf("%s", mensaje);
        leerLinea(destino, tamanio);

        if (destino[0] == '\0') {
            printf("El texto no puede estar vacio.\n");
        } else if (strchr(destino, ';') != NULL) {
            printf("No se permite el caracter ';'.\n");
        } else {
            valido = true;
        }
    }
}

int leerEnteroRango(const char mensaje[], int minimo, int maximo) {
    char linea[MAX_LINEA];
    char *fin;
    long valor = 0;
    bool valido = false;

    while (!valido) {
        printf("%s", mensaje);
        leerLinea(linea, sizeof(linea));

        errno = 0;
        valor = strtol(linea, &fin, 10);
        while (isspace((unsigned char)*fin)) {
            fin++;
        }

        if (fin == linea || *fin != '\0' || errno == ERANGE ||
            valor < minimo || valor > maximo) {
            printf("Entrada invalida. Ingrese un numero entero valido.\n");
        } else {
            valido = true;
        }
    }
    return (int)valor;
}

int leerEntero(const char mensaje[]) {
    return leerEnteroRango(mensaje, INT_MIN, INT_MAX);
}

/* Lee un importe >= 0. Acepta coma o punto como separador decimal. */
float leerFloat(const char mensaje[]) {
    char linea[MAX_LINEA];
    char *fin;
    char *coma;
    double valor = 0;
    bool valido = false;

    while (!valido) {
        printf("%s", mensaje);
        leerLinea(linea, sizeof(linea));

        coma = strchr(linea, ',');
        if (coma != NULL) {
            *coma = '.';
        }

        valor = strtod(linea, &fin);
        while (isspace((unsigned char)*fin)) {
            fin++;
        }

        if (fin == linea || *fin != '\0' ||
            !(valor >= 0 && valor <= PRECIO_MAXIMO)) {
            printf("Entrada invalida. Ingrese un importe mayor o igual a cero.\n");
        } else {
            valido = true;
        }
    }
    return (float)valor;
}

char leerTipo(void) {
    char linea[MAX_LINEA];
    char tipo = ' ';
    bool valido = false;

    while (!valido) {
        printf("Tipo de pedido (L = local, D = domicilio): ");
        leerLinea(linea, sizeof(linea));

        if (strlen(linea) == 1) {
            tipo = (char)toupper((unsigned char)linea[0]);
            valido = (tipo == 'L' || tipo == 'D');
        }
        if (!valido) {
            printf("Opcion invalida. Ingrese L o D.\n");
        }
    }
    return tipo;
}

bool leerEstado(void) {
    return leerEnteroRango("Pedido entregado? (1 = si, 0 = no): ", 0, 1) == 1;
}

/* Lee el proximo pedido valido del archivo. Saltea lineas en blanco o mal
   formadas. Devuelve false solo cuando se termina el archivo. */
bool leerPedido(FILE *archivo, Pedido *pedido) {
    char linea[MAX_LINEA];
    int entregado;
    int resultado;
    bool leido = false;

    while (!leido && fgets(linea, sizeof(linea), archivo) != NULL) {
        resultado = sscanf(linea, "%d;%59[^;];%59[^;];%c;%f;%d",
                           &pedido->numero,
                           pedido->cliente,
                           pedido->comida,
                           &pedido->tipo,
                           &pedido->precio,
                           &entregado);

        if (resultado == 6) {
            pedido->entregado = (entregado == 1);
            leido = true;
        }
    }
    return leido;
}

/* Escribe un pedido con el formato del archivo. */
bool escribirPedido(FILE *archivo, Pedido pedido) {
    return fprintf(archivo, "%d;%s;%s;%c;%.2f;%d\n",
                   pedido.numero, pedido.cliente, pedido.comida,
                   pedido.tipo, pedido.precio,
                   pedido.entregado ? 1 : 0) > 0;
}

void mostrarPedido(Pedido pedido) {
    printf("------------------------------------\n");
    printf("Numero: %d\n", pedido.numero);
    printf("Cliente: %s\n", pedido.cliente);
    printf("Comida: %s\n", pedido.comida);
    printf("Tipo: %s\n", pedido.tipo == 'L' ? "Local" : "Domicilio");
    printf("Precio: $%.2f\n", pedido.precio);
    printf("Estado: %s\n", pedido.entregado ? "Entregado" : "Pendiente");
}

/* Busca 'buscado' dentro de 'texto' sin distinguir mayusculas/minusculas. */
bool contieneTexto(const char texto[], const char buscado[]) {
    int largoTexto = (int)strlen(texto);
    int largoBuscado = (int)strlen(buscado);
    int inicio = 0;
    int j;
    bool hallado = false;

    while (!hallado && inicio + largoBuscado <= largoTexto) {
        j = 0;
        while (j < largoBuscado &&
               tolower((unsigned char)texto[inicio + j]) ==
               tolower((unsigned char)buscado[j])) {
            j++;
        }
        if (j == largoBuscado) {
            hallado = true;
        }
        inicio++;
    }
    return hallado;
}

/* Compara un pedido con el criterio segun el campo elegido:
   1 = numero, 2 = cliente, 3 = comida, 4 = tipo, 5 = estado. */
bool coincide(Pedido pedido, int campo, Pedido criterio) {
    bool resultado = false;

    if (campo == 1) {
        resultado = (pedido.numero == criterio.numero);
    } else if (campo == 2) {
        resultado = contieneTexto(pedido.cliente, criterio.cliente);
    } else if (campo == 3) {
        resultado = contieneTexto(pedido.comida, criterio.comida);
    } else if (campo == 4) {
        resultado = (pedido.tipo == criterio.tipo);
    } else if (campo == 5) {
        resultado = (pedido.entregado == criterio.entregado);
    }
    return resultado;
}

bool numeroExiste(int numero) {
    FILE *archivo;
    Pedido pedido;
    bool encontrado = false;

    archivo = fopen(ARCHIVO_PEDIDOS, "r");
    if (archivo != NULL) {
        while (!encontrado && leerPedido(archivo, &pedido)) {
            if (pedido.numero == numero) {
                encontrado = true;
            }
        }
        fclose(archivo);
    }
    return encontrado;
}

bool agregarPedido(void) {
    FILE *archivo;
    Pedido pedido;
    bool guardado = false;

    pedido.numero = leerEntero("Numero de pedido: ");
    while (pedido.numero <= 0 || numeroExiste(pedido.numero)) {
        if (pedido.numero <= 0) {
            printf("El numero debe ser positivo.\n");
        } else {
            printf("Ese numero de pedido ya existe.\n");
        }
        pedido.numero = leerEntero("Ingrese otro numero: ");
    }

    leerTexto("Nombre y apellido del cliente: ", pedido.cliente, MAX_TEXTO);
    leerTexto("Comida solicitada: ", pedido.comida, MAX_TEXTO);
    pedido.tipo = leerTipo();
    pedido.precio = leerFloat("Precio total: $");
    pedido.entregado = leerEstado();

    archivo = fopen(ARCHIVO_PEDIDOS, "a");
    if (archivo != NULL) {
        guardado = escribirPedido(archivo, pedido);
        fclose(archivo);
    }

    return guardado;
}

void listarPedidos(void) {
    FILE *archivo;
    Pedido pedido;
    int cantidad = 0;

    archivo = fopen(ARCHIVO_PEDIDOS, "r");
    if (archivo == NULL) {
        printf("No hay archivo de pedidos. Agregue un pedido o cree pedidos.txt.\n");
    } else {
        while (leerPedido(archivo, &pedido)) {
            mostrarPedido(pedido);
            cantidad++;
        }
        fclose(archivo);

        if (cantidad == 0) {
            printf("El archivo no contiene pedidos.\n");
        } else {
            printf("------------------------------------\n");
            printf("Cantidad de pedidos: %d\n", cantidad);
        }
    }
}

void listarPorPrecio(void) {
    FILE *archivo;
    Pedido pedido;
    float minimo;
    int cantidad = 0;

    minimo = leerFloat("Mostrar pedidos con precio mayor a: $");
    archivo = fopen(ARCHIVO_PEDIDOS, "r");

    if (archivo == NULL) {
        printf("No se encontro el archivo de pedidos.\n");
    } else {
        while (leerPedido(archivo, &pedido)) {
            if (pedido.precio > minimo) {
                mostrarPedido(pedido);
                cantidad++;
            }
        }
        fclose(archivo);

        if (cantidad == 0) {
            printf("No hay pedidos que superen ese importe.\n");
        } else {
            printf("------------------------------------\n");
            printf("Cantidad de pedidos: %d\n", cantidad);
        }
    }
}

void buscarPedido(void) {
    FILE *archivo;
    Pedido pedido;
    Pedido criterio;
    int campo;
    int cantidad = 0;

    printf("Buscar por:\n");
    printf("1. Numero de pedido\n");
    printf("2. Cliente\n");
    printf("3. Comida\n");
    printf("4. Tipo (local / domicilio)\n");
    printf("5. Estado (entregado / pendiente)\n");
    campo = leerEnteroRango("Seleccione el campo: ", 1, 5);

    if (campo == 1) {
        criterio.numero = leerEntero("Numero de pedido a buscar: ");
    } else if (campo == 2) {
        leerTexto("Cliente a buscar: ", criterio.cliente, MAX_TEXTO);
    } else if (campo == 3) {
        leerTexto("Comida a buscar: ", criterio.comida, MAX_TEXTO);
    } else if (campo == 4) {
        criterio.tipo = leerTipo();
    } else {
        criterio.entregado = leerEstado();
    }

    archivo = fopen(ARCHIVO_PEDIDOS, "r");
    if (archivo == NULL) {
        printf("No se encontro el archivo de pedidos.\n");
    } else {
        while (leerPedido(archivo, &pedido)) {
            if (coincide(pedido, campo, criterio)) {
                mostrarPedido(pedido);
                cantidad++;
            }
        }
        fclose(archivo);

        if (cantidad == 0) {
            printf("No se encontraron pedidos con ese dato.\n");
        } else {
            printf("------------------------------------\n");
            printf("Pedidos encontrados: %d\n", cantidad);
        }
    }
}

void dividirPedidos(void) {
    FILE *origen;
    FILE *entregados;
    FILE *pendientes;
    Pedido pedido;
    int cantEntregados = 0;
    int cantPendientes = 0;

    origen = fopen(ARCHIVO_PEDIDOS, "r");
    if (origen == NULL) {
        printf("No se encontro el archivo de pedidos.\n");
    } else {
        entregados = fopen(ARCHIVO_ENTREGADOS, "w");
        pendientes = fopen(ARCHIVO_PENDIENTES, "w");

        if (entregados == NULL || pendientes == NULL) {
            printf("No se pudieron crear los archivos de salida.\n");
        } else {
            while (leerPedido(origen, &pedido)) {
                if (pedido.entregado) {
                    escribirPedido(entregados, pedido);
                    cantEntregados++;
                } else {
                    escribirPedido(pendientes, pedido);
                    cantPendientes++;
                }
            }
            printf("Se crearon %s (%d pedidos) y %s (%d pedidos).\n",
                   ARCHIVO_ENTREGADOS, cantEntregados,
                   ARCHIVO_PENDIENTES, cantPendientes);
        }

        if (entregados != NULL) {
            fclose(entregados);
        }
        if (pendientes != NULL) {
            fclose(pendientes);
        }
        fclose(origen);
    }
}
