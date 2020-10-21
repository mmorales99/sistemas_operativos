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

int main( int argc, char *argv[] ){

    DIM *d;

    if(argc < 2){
        d=menu();
    }
    else
    {
        int x = atoi(argv[1]);
        int y = atoi(argv[2]);
        if(x<=0 || y<=0){
            perror("Por favor, inserte valores entre 1 a INF.\nSintaxys:\nmalla (1-INF) (1-INF)\n");
            fflush(stdin);
            exit(EXIT_FAILURE);
        }
        d = Dimensiones_new(x,y);
    }
    
    generate_grid(*d);
    free(d);
    exit(EXIT_SUCCESS);
    return 0;
}

DIM* Dimensiones_new(unsigned int x, unsigned int y){
    DIM* d = malloc(sizeof(DIM));
    d->x = x;
    d->y = y;
    return d;
}

DIM* menu(){ // extension del modulo
    int x = 0;
    int y = 0;
    printf("Introduce los valores X e Y:");
    do{
        printf("\nX: ");
        scanf("%d", &x);
    }while (x<=0);
    do{
        printf("\nY: ");
        scanf("%d", &y);
    }while (y<=0);
    fflush(stdin);
    return Dimensiones_new(x,y);
}

int generate_grid(DIM d){
    unsigned int x = d.x;
    unsigned int y = d.y;
    unsigned int i = 0;
    unsigned int j = 1; // el padre de la columna cuenta como un proceso de la malla
    while (i<y){
        switch (fork()){
            case -1:
                perror("ERROR: No se ha podido generar un nuevo proceso.\n");
                fflush(stdin);
                exit(EXIT_FAILURE);
            break;

            case 0:
                while(j<x){
                    if(fork()!=0) 
                        pause();
                    else
                        j++;
                }
                pause();
            break;

            default:
                i++;
            break;
        }
    }
    sleep(30);   
    exit(EXIT_SUCCESS);
}