// MANUEL MORALES AMAT DNI 48789509T

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h> // comentar si no funciona
#include <math.h>

int count_digits(int);

void throw_error(void);

void my_sync(long unsigned int); // funcion de libreria propia, equivalente a sleep pero en ciclos de reloj, se usa para sincronizar la salida por pantalla

void alarma(){
    /*
    - ¿Qué es más fácil, decir al paralítico: Tus pecados te son perdonados, o decirle: Levántate, toma tu lecho y anda?
    + Pues para que sepáis que el Hijo del Hombre tiene potestad en la tierra para perdonar pecados (dijo al paralítico):
    - A ti te digo: Levántate, toma tu lecho, y vete a tu casa.
    Marcos 2:9
    */
}

int main(int argc, char *argv[]){

    int statusA,statusB,statusZ;
    int ejecPID=0, aPID=0, bPID=0, xPID=0, yPID=0, zPID=0;

    int time;

    if (argc!=2){
        perror("ERROR: Argumentos incorrectos.\nPueba con: ./ejec [0-INF]");
        return 0;
    }else{
        time = atoi(argv[1]);
    }

    switch (fork()){
        case -1:
            throw_error();
        break;
        case 0:
            ejecPID=getppid();
            aPID = getpid();
            switch (fork()){
                case -1:
                    throw_error();
                break;
                case 0:
                    bPID=getpid();
                    printf("Soy el proceso B: mi pid es %d. Mi padre es %d. Mi abuelo es %d\n", bPID, aPID, ejecPID);
                    if((xPID=fork())==0){
                        printf("Soy el proceso X: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n", getpid(), bPID, aPID, ejecPID);
                        pause();
                        exit(EXIT_SUCCESS);
                    }
                    else{
                        my_sync(1);
                        if((yPID=fork())==0){
                            printf("Soy el proceso Y: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n", getpid(), bPID, aPID, ejecPID);
                            pause();
                            exit(EXIT_SUCCESS);
                        }else{
                            my_sync(2);
                            int exec_status;
                            if((zPID=fork())==0){
                                signal(SIGALRM, alarma);
                                alarm(time);
                                printf("Soy el proceso Z: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n", getpid(), bPID, aPID, ejecPID);
                                pause();
                                if(fork()==0){
                                    //if(execlp("ls","ls","-a",NULL)<0){
                                    int leng = count_digits(ejecPID) +1;
                                    char myPID[leng];
                                    sprintf(myPID,"%d",ejecPID);
                                    if(execlp("pstree","pstree","-p",myPID,NULL)<0){ // pstree -p | grep ejec
                                        perror("ERROR, No se ha podido lanzar el exec");
                                        exit(EXIT_FAILURE);
                                    }
                                }else{
                                    wait(&exec_status);
                                    printf("Soy Z (%d) y muero\n", getpid());
                                    exit(EXIT_SUCCESS);
                                }
                            }else{
                                wait(&statusZ);
                                if(statusZ==EXIT_SUCCESS){
                                    printf("Soy Y (%d) y muero\n", yPID);
                                    kill(yPID, SIGKILL);
                                    my_sync(1);
                                    kill(xPID, SIGKILL);
                                    printf("Soy X (%d) y muero\n", xPID);
                                    my_sync(1);
                                }
                                printf("Soy B (%d) y muero\n", getpid());
                                exit(EXIT_SUCCESS);
                            }
                        }
                    }
                break;

                default:
                    printf("Soy el proceso A: mi pid es %d. Mi padre es %d\n",aPID, ejecPID);
                    wait(&statusB);
                    printf("Soy A (%d) y muero\n", getpid());
                    exit(EXIT_SUCCESS);
                break;
            }
        break;

        default:
            printf("Soy el proceso ejec: mi pid es %d\n",getpid());
            wait(&statusA);
            printf("Soy ejec (%d) y muero\n", getpid());
        break;
    }

    return 0;
}

void throw_error(void){
    perror("ERROR No se ha podido crear un nuevo proceso");
    exit(EXIT_FAILURE);
}

void my_sync(long unsigned int time){
    for(long unsigned int i = 0; i<time*1000000;i++); // 1000000 ciclos asegura una impresion ordenada
    return;
}

int count_digits(int n){
    int m = n;
    int i=0;
    while (m>0){
        m=m/10;
        i++;
    }
    return i;
}