// MANUEL MORALES AMAT DNI 48789509T

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct Dimensiones{ // usado para manjear el posible tamaño de la malla
    unsigned int x;
    unsigned int y;
}DIM;

DIM* Dimensiones_new(unsigned int, unsigned int); // constructor para las dimensiones, ayuda para manejarlas
DIM* menu(); // funcion con fin documental y debug, aka verbose mode
int generate_grid(DIM); // metodo que generará el arbol de procesos de dimension x*y

/**
 * ! Funcion principal, que recive como parametros de valores enteros X Y que conjuran las diemnsiones de la malla a generar
 * @param argc : contador de los argumentos nunca podra ser menor que 1 ni 3, sino se lanzara un menu
 * @param argv : vector que contiene los argumentos del programa
 */ 
int main( int argc, char *argv[] ){
    DIM *d; // variable que almacena las dimensiones de la malla
    if(argc < 2){ // si no recibe los suficientes argumentos, lanza un menu, tambien podra lanzar un errors
        d=menu();
    }else{ // sino comprueba los valores
        int x = atoi(argv[1]); // se pasa de char a int
        int y = atoi(argv[2]);
        if(x<=0 || y<=0){ // si alguno de los valores no es correcto se lanza un error
            perror("Por favor, inserte valores entre 1 a INF.\nSintaxys:\nmalla (1-INF) (1-INF)\n");
            fflush(stdin);
            exit(EXIT_FAILURE); // muere el padre con una salida de fallo
        }
        d = Dimensiones_new(x,y); // constructor del las dimensiones
    }
    generate_grid(*d); // genera la malla
    free(d); // elimina la memoria reservada
    exit(EXIT_SUCCESS); // muere con una salida exitosa
    return 0;
}

/**
 * ! Constructor de la estructura que representa las diemnsiones de la malla
 * @param x : dimension X de la malla
 * @param y : dimension Y de la malla
 * @return d : representante de las dimensiones de la malla, puntero
 */ 
DIM* Dimensiones_new(unsigned int x, unsigned int y){
    DIM* d = malloc(sizeof(DIM)); // se reserva memoria para la malla
    d->x = x; // se asignan los valores
    d->y = y;
    return d; // se devuelve el puntero de la malla
}

/**
 * ! Funcion que recauda los valores de las dimensiones de la malla
 * @return devuelve un puntero que contiene un representante de la malla
 */ 
DIM* menu(void){ // extension del modulo
    /**
     * Variables donde se almacenaran los valores pedidos
     */ 
    int x = 0, y = 0;
    printf("Introduce los valores X e Y:"); // se piden los valores de X e Y, necesariamente tienen que ser positivos no nulos
    do{
        printf("\nX: ");
        scanf("%d", &x);
    }while (x<=0);
    do{
        printf("\nY: ");
        scanf("%d", &y);
    }while (y<=0);
    fflush(stdin);
    return Dimensiones_new(x,y); // se devuelve un puntero de tipo DIM
}

/**
 * ! Funcion que genera la malla
 * @param d : representante de las dimensiones de la malla, contiene x e y
 */ 
int generate_grid(DIM d){
    /**
     * ! Variables que almacenan las dimensiones minimas y maximas de la malla
     */ 
    unsigned int x = d.x, y = d.y, i = 0, j = 1; // el padre de la columna cuenta como un proceso de la malla
    while (i<y){ // se generan tantos hijos como niveles necesite la malla
        switch (fork()){
            case -1: // en caso de que el fork falle porque no pueda generar hijos entrara en este caso
                perror("ERROR: No se ha podido generar un nuevo proceso.\n");
                fflush(stdin);
                exit(EXIT_FAILURE); // muere con salida fallida
            break;
            case 0: // en caso de que genere un hijo, el hijo ejecutara este caso
                while(j<x){ // se generan tantos hijos nuevos como sea necesario
                    if(fork()!=0) 
                        pause(); // pausa los hijos
                    else
                        j++; // hace que el padre cuente el resto de hijos
                }
                pause(); // se pausa los hijos para que no degeneren la malla y se puedan ver con el comando pstree
            break;
            default: // el padre ejecutara este caso
                i++; // se siguen contando los hijos
            break;
        }
    }
    sleep(30); // espera 30 segundos para que se pueda ver con el comando pstree
    exit(EXIT_SUCCESS); // muere con una salida exitosa
    return 0;
}