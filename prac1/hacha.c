// MANUEL MORALES AMAT DNI 48789509T

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h> // esta libreria no esta en los apuntes, sin ella la funcion wait no funciona correctamente
#include <fcntl.h>

int main(int argc, char *argv[]){

    if(argc!=3){
        perror("ERROR: Argumentos incorrectos.\n Pueba con: ./hacha [Nombre archivo] [Tamanyo de los nuevos archivos]\n");
        exit(EXIT_FAILURE);
    }
    int tam = atoi(argv[2]);
    char *name; strcpy(name,argv[1]);

    int file = open(name,O_RDONLY);
    if(file==-1){
        perror("ERROR: El archivo no se ha abierto correctamente o no existe.\n");
        exit(EXIT_FAILURE);
    }

    char *buff;
    int readed_size;

    close(file);

    return 0;
}