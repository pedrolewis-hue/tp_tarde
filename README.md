Funcionalidades

El programa muestra un menu con estas opciones:

1. Agregar un pedido: pide los datos, los valida y los guarda al final de pedidos.txt.
2. Listar todos los pedidos: muestra todo el archivo y la cantidad total.
3. Listar pedidos por precio: muestra solo los pedidos cuyo precio supera un importe ingresado.
4. Buscar pedidos: el usuario elige el campo (numero, cliente, comida, tipo o estado) y el dato a buscar. En cliente y comida la busqueda es parcial y no distingue mayusculas.
5. Separar entregados y pendientes: genera entregados.txt y pendientes.txt segun el campo booleano.
6. Salir.

Estructura de datos

typedef struct {
    int numero;
    char cliente[MAX_TEXTO];
    char comida[MAX_TEXTO];
    char tipo;          /* L = local, D = domicilio */
    float precio;
    bool entregado;
} Pedido;

Formato del archivo
Un pedido por linea, con los campos separados por ;:

numero;cliente;comida;tipo;precio;entregado
1;Juan Perez;Pizza muzzarella;L;8500.00;1
2;Maria Gomez;Hamburguesa doble;D;6200.50;0

tipo: L (local) o D (domicilio).
entregado: 1 (entregado) o 0 (pendiente).
Las lineas en blanco o mal formadas se ignoran al leer.


En Windows el ejecutable se llama pedidos.exe. El archivo pedidos.txt debe estar en la misma carpeta desde donde se ejecuta el programa. Si no existe, se crea al agregar el primer pedido.

Validaciones
El numero de pedido debe ser positivo y no repetirse.
Cliente y comida no pueden estar vacios ni contener ;.
El tipo solo acepta L o D (en mayuscula o minuscula).
El precio debe ser mayor o igual a cero (acepta coma o punto decimal).
Cualquier entrada numerica invalida se vuelve a pedir.
Pruebas sugeridas
Listar todos los pedidos (deberian aparecer 12).
Listar por precio con 9000 (deberian aparecer 6).
Buscar por cliente maria (2 resultados) y por comida pizza (3 resultados).
Agregar un pedido nuevo y verificar que aparece en el listado.
Intentar agregar un numero ya existente (debe rechazarlo).
Escribir letras donde se pide un numero (debe volver a preguntar).
Separar entregados y pendientes y abrir los dos archivos generados.
Notas
Lenguaje: C estandar (C99), sin variables globales.
Los datos se leen del archivo en cada operacion; el programa no los guarda en memoria.
