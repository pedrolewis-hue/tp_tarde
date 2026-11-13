#include <stdio.h>
#include <string.h>

/* Formato de pedidos.txt: numero;cliente;comida;tipo;precio;entregado */

typedef struct {
    int numero;
    char cliente[60];
    char comida[60];
    char tipo;          /* L = local, D = domicilio */
    float precio;
    int entregado;      /* 1 = entregado, 0 = pendiente */
} Pedido;

/* Prototipos */
void mostrarMenu(void);
void agregarPedido(void);
void listarPedidos(void);
void listarPorPrecio(void);
void buscarPedido(void);
void separarPedidos(void);
int leerPedido(FILE *f, Pedido *p);
void escribirPedido(FILE *f, Pedido p);
void mostrarPedido(Pedido p);
void mostrarArchivo(const char nombre[]);

int main(void) {
    int opcion;

    do {
        mostrarMenu();
        scanf("%d", &opcion);

        if (opcion == 1) agregarPedido();
        else if (opcion == 2) listarPedidos();
        else if (opcion == 3) listarPorPrecio();
        else if (opcion == 4) buscarPedido();
        else if (opcion == 5) separarPedidos();
        else if (opcion != 6) printf("Opcion invalida.\n");
    } while (opcion != 6);

    return 0;
}

/* ---------- Menu ---------- */

void mostrarMenu(void) {
    printf("\n====================================\n");
    printf("    SISTEMA DE PEDIDOS DE COMIDA\n");
    printf("====================================\n");
    printf("1. Agregar pedido\n");
    printf("2. Listar pedidos\n");
    printf("3. Listar por precio\n");
    printf("4. Buscar pedido\n");
    printf("5. Separar entregados y pendientes\n");
    printf("6. Salir\n");
    printf("====================================\n");
    printf("Seleccione una opcion: ");
}

/* ---------- Opciones del menu (en orden) ---------- */

void agregarPedido(void) {
    Pedido p;
    FILE *f;

    printf("Numero: ");
    scanf("%d", &p.numero);
    printf("Cliente: ");
    scanf(" %59[^\n]", p.cliente);
    printf("Comida: ");
    scanf(" %59[^\n]", p.comida);
    printf("Tipo (L = local, D = domicilio): ");
    scanf(" %c", &p.tipo);
    printf("Precio: ");
    scanf("%f", &p.precio);
    printf("Entregado? (1 = si, 0 = no): ");
    scanf("%d", &p.entregado);

    f = fopen("pedidos.txt", "a");
    if (f == NULL) {
        printf("No se pudo abrir el archivo.\n");
    } else {
        escribirPedido(f, p);
        fclose(f);
        printf("Pedido guardado.\n");
    }
}

void listarPedidos(void) {
    Pedido p;
    FILE *f = fopen("pedidos.txt", "r");

    if (f == NULL) {
        printf("No hay pedidos.\n");
    } else {
        while (leerPedido(f, &p)) {
            mostrarPedido(p);
        }
        fclose(f);
    }
}

void listarPorPrecio(void) {
    Pedido p;
    float minimo;
    FILE *f = fopen("pedidos.txt", "r");

    printf("Mostrar pedidos con precio mayor a: ");
    scanf("%f", &minimo);

    if (f == NULL) {
        printf("No hay pedidos.\n");
    } else {
        while (leerPedido(f, &p)) {
            if (p.precio > minimo) {
                mostrarPedido(p);
            }
        }
        fclose(f);
    }
}

void buscarPedido(void) {
    Pedido p;
    int campo, numero = 0, estado = 0;
    char texto[60] = "", tipo = ' ';
    int coincide;
    FILE *f;

    printf("Buscar por: 1.Numero 2.Cliente 3.Comida 4.Tipo 5.Estado\n");
    scanf("%d", &campo);

    if (campo == 1) {
        printf("Numero: ");
        scanf("%d", &numero);
    } else if (campo == 2 || campo == 3) {
        printf("Texto a buscar: ");
        scanf(" %59[^\n]", texto);
    } else if (campo == 4) {
        printf("Tipo (L/D): ");
        scanf(" %c", &tipo);
    } else if (campo == 5) {
        printf("Entregado? (1 = si, 0 = no): ");
        scanf("%d", &estado);
    } else {
        printf("Campo invalido.\n");
        return;
    }

       f = fopen("pedidos.txt", "r");
    if (f == NULL) {
        printf("No hay pedidos.\n");
    } else {
        while (leerPedido(f, &p)) {
            switch (campo) {
                case 1:
                    if (p.numero == numero) mostrarPedido(p);
                    break;
                case 2:
                    if (strcmp(p.cliente, texto) == 0) mostrarPedido(p);
                    break;
                case 3:
                    if (strcmp(p.comida, texto) == 0) mostrarPedido(p);
                    break;
                case 4:
                    if (p.tipo == tipo) mostrarPedido(p);
                    break;
                case 5:
                    if (p.entregado == estado) mostrarPedido(p);
                    break;
            }
        }
        fclose(f);
    }
}

void separarPedidos(void) {
    Pedido p;
    FILE *origen = fopen("pedidos.txt", "r");
    FILE *entregados, *pendientes;

    if (origen == NULL) {
        printf("No hay pedidos.\n");
        return;
    }

    entregados = fopen("entregados.txt", "w");
    pendientes = fopen("pendientes.txt", "w");

    while (leerPedido(origen, &p)) {
        if (p.entregado) {
            escribirPedido(entregados, p);
        } else {
            escribirPedido(pendientes, p);
        }
    }

    fclose(origen);
    fclose(entregados);
    fclose(pendientes);
    printf("Archivos entregados.txt y pendientes.txt creados.\n");

    printf("\n===== PEDIDOS ENTREGADOS =====\n");
    mostrarArchivo("entregados.txt");
    printf("\n===== PEDIDOS PENDIENTES =====\n");
    mostrarArchivo("pendientes.txt");
}

/* ---------- Funciones auxiliares ---------- */

int leerPedido(FILE *f, Pedido *p) {
    return fscanf(f, "%d;%59[^;];%59[^;];%c;%f;%d ",
                  &p->numero, p->cliente, p->comida,
                  &p->tipo, &p->precio, &p->entregado) == 6;
}

void escribirPedido(FILE *f, Pedido p) {
    fprintf(f, "%d;%s;%s;%c;%.2f;%d\n",
            p.numero, p.cliente, p.comida, p.tipo, p.precio, p.entregado);
}

void mostrarPedido(Pedido p) {
    printf("------------------------------\n");
    printf("Numero: %d\n", p.numero);
    printf("Cliente: %s\n", p.cliente);
    printf("Comida: %s\n", p.comida);

    if (p.tipo == 'L') {
        printf("Tipo: Local\n");
    } else {
        printf("Tipo: Domicilio\n");
    }

    printf("Precio: $%.2f\n", p.precio);

    if (p.entregado) {
        printf("Estado: Entregado\n");
    } else {
        printf("Estado: Pendiente\n");
    }
}

void mostrarArchivo(const char nombre[]) {
    Pedido p;
    int cantidad = 0;
    FILE *f = fopen(nombre, "r");

    if (f != NULL) {
        while (leerPedido(f, &p)) {
            mostrarPedido(p);
            cantidad++;
        }
        fclose(f);
    }
    if (cantidad == 0) {
        printf("No hay pedidos.\n");
    }
}
