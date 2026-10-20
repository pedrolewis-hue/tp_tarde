# tp_tarde
Repositorio para el tp del turno tarde Lewis Buschi
/*
    Trabajo Practico - Programacion 1
    Sistema de gestion de pedidos de comida

    Funcionalidades:
    1. Agregar un pedido.
    2. Listar todos los pedidos.
    3. Listar pedidos cuyo precio supere un importe ingresado.
    4. Buscar un pedido por numero.
    5. Separar pedidos entregados y pendientes en dos archivos.
    6. Salir.

    Compilar:
    gcc pedidos.c -o pedidos
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TEXTO 60
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
void limpiarEntrada(void);
void leerTexto(const char mensaje[], char destino[], int tamanio);
int leerEntero(const char mensaje[]);
float leerFloat(const char mensaje[]);
char leerTipo(void);
bool leerEstado(void);
bool leerPedido(FILE *archivo, Pedido *pedido);
void mostrarPedido(Pedido pedido);
bool agregarPedido(void);
void listarPedidos(void);
void listarPorPrecio(void);
void buscarPedido(void);
void dividirPedidos(void);
bool numeroExiste(int numero);

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
    printf("4. Buscar pedido por numero\n");
    printf("5. Separar entregados y pendientes\n");
    printf("6. Salir\n");
    printf("====================================\n");
}

void limpiarEntrada(void) {
    int caracter;
    caracter = getchar();
    while (caracter != '\n' && caracter != EOF) {
        caracter = getchar();
    }
}

void leerTexto(const char mensaje[], char destino[], int tamanio) {
    int resultado;
    printf("%s", mensaje);
    resultado = scanf(" %59[^\n]", destino);
    if (resultado != 1) {
        destino[0] = '\0';
    }
    limpiarEntrada();
    destino[tamanio - 1] = '\0';
}

int leerEntero(const char mensaje[]) {
    int valor;
    int resultado;

    printf("%s", mensaje);
    resultado = scanf("%d", &valor);
    while (resultado != 1) {
        printf("Entrada invalida. Ingrese un numero entero: ");
        limpiarEntrada();
        resultado = scanf("%d", &valor);
    }
    limpiarEntrada();
    return valor;
}

float leerFloat(const char mensaje[]) {
    float valor;
    int resultado;

    printf("%s", mensaje);
    resultado = scanf("%f", &valor);
    while (resultado != 1 || valor < 0) {
        printf("Entrada invalida. Ingrese un importe mayor o igual a cero: ");
        limpiarEntrada();
        resultado = scanf("%f", &valor);
    }
    limpiarEntrada();
    return valor;
}

char leerTipo(void) {
    char tipo;
    int resultado;

    printf("Tipo de pedido (L = local, D = domicilio): ");
    resultado = scanf(" %c", &tipo);
    while (resultado != 1 ||
           (tipo != 'L' && tipo != 'l' && tipo != 'D' && tipo != 'd')) {
        printf("Opcion invalida. Ingrese L o D: ");
        limpiarEntrada();
        resultado = scanf(" %c", &tipo);
    }
    limpiarEntrada();

    if (tipo == 'l') {
        tipo = 'L';
    }
    if (tipo == 'd') {
        tipo = 'D';
    }
    return tipo;
}

bool leerEstado(void) {
    int estado;
    int resultado;

    printf("Pedido entregado? (1 = si, 0 = no): ");
    resultado = scanf("%d", &estado);
    while (resultado != 1 || (estado != 0 && estado != 1)) {
        printf("Ingrese 1 para entregado o 0 para pendiente: ");
        limpiarEntrada();
        resultado = scanf("%d", &estado);
    }
    limpiarEntrada();
    return estado == 1;
}

/* Lee un registro separado por punto y coma. */
bool leerPedido(FILE *archivo, Pedido *pedido) {
    int entregado;
    int resultado;
    char linea[256];

    bool leido = false;

    if (fgets(linea, sizeof(linea), archivo) != NULL) {
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

void mostrarPedido(Pedido pedido) {
    printf("------------------------------------\n");
    printf("Numero: %d\n", pedido.numero);
    printf("Cliente: %s\n", pedido.cliente);
    printf("Comida: %s\n", pedido.comida);
    printf("Tipo: %s\n", pedido.tipo == 'L' ? "Local" : "Domicilio");
    printf("Precio: $%.2f\n", pedido.precio);
    printf("Estado: %s\n", pedido.entregado ? "Entregado" : "Pendiente");
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
        if (fprintf(archivo, "%d;%s;%s;%c;%.2f;%d\n",
                    pedido.numero, pedido.cliente, pedido.comida,
                    pedido.tipo, pedido.precio,
                    pedido.entregado ? 1 : 0) > 0) {
            guardado = true;
        }
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
        }
    }
}

void buscarPedido(void) {
    FILE *archivo;
    Pedido pedido;
    int numero;
    bool encontrado = false;

    numero = leerEntero("Ingrese el numero de pedido a buscar: ");
    archivo = fopen(ARCHIVO_PEDIDOS, "r");

    if (archivo == NULL) {
        printf("No se encontro el archivo de pedidos.\n");
    } else {
        while (!encontrado && leerPedido(archivo, &pedido)) {
            if (pedido.numero == numero) {
                mostrarPedido(pedido);
                encontrado = true;
            }
        }
        fclose(archivo);

        if (!encontrado) {
            printf("No existe un pedido con ese numero.\n");
        }
    }
}

void dividirPedidos(void) {
    FILE *origen;
    FILE *entregados;
    FILE *pendientes;
    Pedido pedido;
    bool error = false;

    origen = fopen(ARCHIVO_PEDIDOS, "r");
    if (origen == NULL) {
        printf("No se encontro el archivo de pedidos.\n");
    } else {
        entregados = fopen(ARCHIVO_ENTREGADOS, "w");
        pendientes = fopen(ARCHIVO_PENDIENTES, "w");

        if (entregados == NULL || pendientes == NULL) {
            printf("No se pudieron crear los archivos de salida.\n");
            error = true;
        }

        if (entregados != NULL && pendientes != NULL) {
            while (leerPedido(origen, &pedido)) {
                if (pedido.entregado) {
                    fprintf(entregados, "%d;%s;%s;%c;%.2f;1\n",
                            pedido.numero, pedido.cliente, pedido.comida,
                            pedido.tipo, pedido.precio);
                } else {
                    fprintf(pendientes, "%d;%s;%s;%c;%.2f;0\n",
                            pedido.numero, pedido.cliente, pedido.comida,
                            pedido.tipo, pedido.precio);
                }
            }
        }

        if (entregados != NULL) {
            fclose(entregados);
        }
        if (pendientes != NULL) {
            fclose(pendientes);
        }
        fclose(origen);

        if (!error) {
            printf("Se crearon entregados.txt y pendientes.txt correctamente.\n");
        }
    }
}
