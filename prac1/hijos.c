// MANUEL MORALES AMAT DNI 48789509T

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h> // esta libreria no esta en los apuntes, sin ella la funcion wait no funciona correctamente

int main(int argc, char *argv[]){

    if(argc != 3){
        perror("ERROR: Argumentos incorrectos.\nPueba con: ./hijos [1-INF] [1-INF]");
        exit(EXIT_FAILURE);
    }
    int x, y;
    x = atoi(argv[1]);
    y = atoi(argv[2]);

    int shmid = shmget(IPC_PRIVATE, (sizeof(int)*(x+1)), IPC_CREAT|0666); // devuelve el identificador del puntero a usar
    if(shmid == -1){
        perror("ERROR: No se ha podido reservar memoria compartida.\n");
        exit(EXIT_FAILURE);
    }
    int *pid_v;
    pid_v = (int*)shmat(shmid, 0, 0);

    int shmid2 = shmget(IPC_PRIVATE, (sizeof(int)*y), IPC_CREAT|0666); // devuelve el identificador del puntero a usar
    if(shmid2 == -1){
        perror("ERROR: No se ha podido reservar memoria compartida.\n");
        exit(EXIT_FAILURE);
    }
    int *pid_h;
    pid_h = (int*)shmat(shmid2, 0, 0);

    pid_v[0] = getpid();
    int fpid = pid_v[0];

    signal(SIGALRM,SIG_IGN);

    // rama vertical
    int i = 0;
    while (i<x){
        switch (fork()){
            case -1:
                perror("ERROR, No se ha podido generar un nuevo proceso.\n");
                exit(EXIT_FAILURE);
            break;
            case 0:
                i++;
                pid_v[i] = getpid();
            break;
            default:
                //if(getpid()!=pid_v[0])
                if(i!=0){
                    wait(NULL);
                    exit(EXIT_SUCCESS);
                }else
                    break;
            break;
        }
        if(i==0) break;
    }

    // ramas horizontales
    if(getpid()!=fpid){
        i=0;
        while(i<y){
            switch (fork()){
                case -1:
                    perror("ERROR, No se ha podido generar un nuevo proceso.\n");
                    exit(EXIT_FAILURE);
                break;
                case 0:
                    pid_h[i] = getpid();
                    printf("Soy el subhijo %d, mis padres son: ",getpid());
                    for(int j = 0; j<(x+1); j++) printf("%d, ", pid_v[j]);
                    printf("\n");
                    //pause();
                    exit(EXIT_SUCCESS);     
                break;
                default:
                    i++;
                break;
            }
        }
        wait(NULL);
        exit(EXIT_SUCCESS);
        //kill(fpid,SIGALRM);
        
    }
    if(getpid()==fpid){
        wait(NULL);
        printf("Soy el superpadre(%d) : mis hijos finales son: ",getpid());
        for(int i = 0; i<y;i++) printf("%d, ",pid_h[i]);
        printf("\n");
        for(int i = 0; i<y; i++) kill(pid_h[i],SIGKILL);
        for(int i = x; i>1; i--) kill(pid_v[i],SIGKILL);
        shmdt(pid_v);
        shmdt(pid_h);
        shmctl(shmid,IPC_RMID,0);
        shmctl(shmid2,IPC_RMID,0);
    }
    exit(EXIT_SUCCESS);
    return 0;
}