// MANUEL MORALES AMAT DNI 48789509T

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h> // esta libreria no esta en los apuntes, sin ella la funcion wait no funciona correctamente

/**
 * ! Funcion principal, genera un arbol de procesos que tiene una rama vertical con N hijos y una rama horizontal de M hijos
 *  Recibe por parametro los valores de N y M
 *  @param argc : contador de los argumentos, nunca sera menor 1 y deberia ser 3
 *  @param argv : vector donde se almacenan los argumentos del programa
 */ 
int main(int argc, char *argv[]){
    if(argc != 3){ // si no hay los suficientes argumentos el proceso padre muere con una salida de fallo
        perror("ERROR: Argumentos incorrectos.\nPueba con: ./hijos [1-INF] [1-INF]");
        exit(EXIT_FAILURE);
    }

    /**
     * ! X e Y almacenan los valores pasados por parametro, sera la profundidad del arbol vertical(X) y la longitud del arbol horizontal(Y)
     */ 
    int x, y;
    x = atoi(argv[1]); // se convierte de array a entero
    y = atoi(argv[2]);
    int shmid = shmget(IPC_PRIVATE, (sizeof(int)*(x+1)), IPC_CREAT|0666); // devuelve el identificador del puntero a usar, reservando tanta memoria como profundidad tenga que tener el arbol mas uno para el proceso padre
    if(shmid == -1){ // cuando la direccion es -1 es porque no se ha podido reservar la memoria
        perror("ERROR: No se ha podido reservar memoria compartida.\n");
        exit(EXIT_FAILURE); // muere el padre con salida de fallo
    }
    int *pid_v; // vector donde se almacenaran los PIDs de la rama vertical
    pid_v = (int*)shmat(shmid, 0, 0); // direccion de memoria donde se encuentra la memoria reservada
    // Repetimos el proceso anterior para la memoria donde se guardara los PIDs de la rama horizontal
    int shmid2 = shmget(IPC_PRIVATE, (sizeof(int)*y), IPC_CREAT|0666); // devuelve el identificador del puntero a usar
    if(shmid2 == -1){
        perror("ERROR: No se ha podido reservar memoria compartida.\n");
        exit(EXIT_FAILURE);
    }
    int *pid_h; // vector donde se almacenan los PIDs de la rama horizontal
    pid_h = (int*)shmat(shmid2, 0, 0);
    pid_v[0] = getpid(); // se guarda el PID del proceso padre
    int fpid = pid_v[0]; // lo copiamos para mayor facilidad de uso
    signal(SIGALRM,SIG_IGN); // configuramos la senyal de la alarma para que no mate al padre

    // rama vertical
    int i = 0;
    while (i<x){ // genera tantos hijos como X hayamos pasado por parametro empezando por 0
        switch (fork()){
            case -1:
                perror("ERROR, No se ha podido generar un nuevo proceso.\n");
                exit(EXIT_FAILURE);
            break;
            case 0:
                i++;
                pid_v[i] = getpid(); // guardamos el PID del proceso hijo
            break;
            default:
                if(i!=0){ // hacemos esperar a todos los hijos, nietos y demas del proceso original
                    wait(NULL);
                    exit(EXIT_SUCCESS);
                }else
                    break; // el proceso original sale del switch 
            break;
        }
        if(i==0) break; // el poceso original sale del bucle, el ultimo proceso tambien
    }

    // ramas horizontales
    if(getpid()!=fpid){ // el ultimo proceso hijo entra en este if 
        i=0; 
        while(i<y){ // genera tantos hijos como Y hayamos puesto
            switch (fork()){
                case -1:
                    perror("ERROR, No se ha podido generar un nuevo proceso.\n");
                    exit(EXIT_FAILURE);
                break;
                case 0:
                    pid_h[i] = getpid(); // guardamos el valor del pid
                    printf("Soy el subhijo %d, mis padres son: ",getpid());
                    for(int j = 0; j<(x+1); j++) printf("%d, ", pid_v[j]); // muestra tantos antecesores como tenga
                    printf("\n"); 
                    exit(EXIT_SUCCESS); // muere con una salida exitosa
                break;
                default:
                    i++; // sigue contando hijos
                break;
            }
        }
        wait(NULL); // espera al padre
        exit(EXIT_SUCCESS); // sale con una salida exitosa
    }
    if(getpid()==fpid){ // el proceso padre entra en este if
        wait(NULL); // espera a que mueran todos los hijos
        printf("Soy el superpadre(%d) : mis hijos finales son: ",getpid()); // imprime tantos sucesores como tenga
        for(int i = 0; i<y;i++) printf("%d, ",pid_h[i]);
        printf("\n");
        for(int i = 0; i<y; i++) kill(pid_h[i],SIGKILL); // mata todos los sucesores del proceso
        for(int i = x; i>1; i--) kill(pid_v[i],SIGKILL);
        shmdt(pid_v); // limpia la memoria reservada
        shmdt(pid_h);
        shmctl(shmid,IPC_RMID,0); // elimina la memoria reservada
        shmctl(shmid2,IPC_RMID,0);
    }
    exit(EXIT_SUCCESS);
    return 0;
}