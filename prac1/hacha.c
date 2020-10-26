// MANUEL MORALES AMAT DNI 48789509T

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/wait.h> // esta libreria no esta en los apuntes, sin ella la funcion wait no funciona correctamente
#include <fcntl.h>
void generate_file(int tam, char *new_file_name, char* name, int childs_born, int* pipe);
int main(int argc, char *argv[]){
    if(argc!=3){
        perror("ERROR: Argumentos incorrectos.\nPueba con: ./hacha [Nombre archivo] [Tamanyo de los nuevos archivos]\n");
        exit(EXIT_FAILURE);
    }
    int tam = atoi(argv[2]);
    char name[strlen(argv[1])]; strcpy(name,argv[1]);
    int file = open(name,O_RDONLY);
    if(file==-1){
        perror("ERROR: El archivo no se ha abierto correctamente o no existe.\n");
        exit(EXIT_FAILURE);
    }
    int file_start = lseek(file,0,SEEK_CUR);
    int file_end = lseek(file, 0, SEEK_END);
    int file_size = file_end-file_start;
    lseek(file,file_start,SEEK_SET);
    struct stat file_stats;
    fstat(file,&file_stats);
    double total_childs = file_size/(double)tam;
    total_childs++;
    int childs_born = 0;
    int pipes[(int)total_childs][2];
    while(childs_born<(int)total_childs){
        char buff[tam+1];
        char new_file_name[strlen(name)+5];
        if(pipe(pipes[childs_born])){
            perror("ERROR: No se ha podido generar la pipe correspondiente al proceso.\n");
            exit(EXIT_FAILURE);
        }
        if(lseek(file,childs_born*tam,SEEK_SET)<0){
            perror("ERROR: No se ha podido operar el archivo.\n");
            exit(EXIT_FAILURE);
        }
        if(read(file,&buff,tam)<0){
            perror("ERROR: No se ha podido leer el archivo.\n");
            exit(EXIT_FAILURE);
        }
        if(write(pipes[childs_born][1],&buff,tam)<0){
            perror("ERROR: No se ha podido operar con la pipe.\n");
            exit(EXIT_FAILURE);
        }
        close(pipes[childs_born][1]); // cerramos el descriptor de escritura por si las moscas
        switch (fork()){
            case -1:
                perror("ERROR: No se ha podido generar el hijo.");
                exit(EXIT_FAILURE);
            break;
            case 0:
                generate_file(tam, new_file_name,name,childs_born,pipes[childs_born]);
            break;
        }
        childs_born++;
    }
    close(file);
    return 0;
}

void generate_file(int tam, char *new_file_name, char* name, int childs_born, int* pipe){
    char child_buff[tam+1];
    sprintf(new_file_name,"%s.h%d",name,childs_born);
    int new_file = creat(new_file_name,0666);
    read(pipe[0],&child_buff,tam);
    close(pipe[0]);
    write(new_file,&child_buff,tam);
    close(new_file);
    exit(EXIT_SUCCESS);
}