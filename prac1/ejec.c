// MANUEL MORALES AMAT DNI 48789509T

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h> // comentar si no funciona
#include <math.h>

int count_digits(int); // funcion que cuenta los digitos de un numero, usada mas adelante

void throw_error(void); // funcion usada para lanzar errores sin reescribirlos

void my_sync(long unsigned int); // funcion de libreria propia, equivalente a sleep pero en ciclos de reloj, se usa para sincronizar la salida por pantalla

void alarma(){ // esta función es un handler de una señal
    /*
    - ¿Qué es más fácil, decir al paralítico: Tus pecados te son perdonados, o decirle: Levántate, toma tu lecho y anda?
    + Pues para que sepáis que el Hijo del Hombre tiene potestad en la tierra para perdonar pecados (dijo al paralítico):
    - A ti te digo: Levántate, toma tu lecho, y vete a tu casa.
    Marcos 2:9
    */
}

int main(int argc, char *argv[]){ // funcion principal, debe recivir un parámetro(sera el tiempo de espera entre las partes de la ejecucion, y el tiempo de la alarma)

    /**
     * ! Las variables marcadas como *STATUS* sirven para poder llevar control sobre los wait que se realizan
     */
    int statusA,statusB,statusZ;

    /**
     * ! Las variables marcadas como *PID* sirven para almacenar el PID de cada proceso generado a lo largo del programa
     * estas variables seran usadas para mostrarse cuando sean necesarias.
     */
    int ejecPID=0, aPID=0, bPID=0, xPID=0, yPID=0, zPID=0;

    /**
     * Esta variable almacena el tiempo que manda el usuario para esperar antes de ejecutar el comando mediante el uso de exec
     */
    int time;

    if (argc!=2){ // si hay menos de 2 argumentos quiere decir que no hay los argumentos necesarios o son incorrectos
        perror("ERROR: Argumentos incorrectos.\nPueba con: ./ejec [0-INF]");
        return 0;
    }else{
        time = atoi(argv[1]); // la funcion atoi convierte un array de caracteres a un entero simple {char* -> int}
    }

    switch (fork()){ // este switch parte la ejecucion del codigo en 2 partes cuando ejecuta el fork
        case -1: // en caso de que fork devuelva -1 es que ha habido un error
            throw_error();
        break;
        case 0: // en caso de que devuelva 0, se ha generado un hijo, el cual ejecutara la siguiente seccion de cogido
            ejecPID=getppid(); // se guarda el PID del proceso padre (PID o Progam Identificator, es un nombre que asigna el sistema operativo al proceso que alberga la ejecucion del programa)
            aPID = getpid(); // se guarda el pid del proceso A
            switch (fork()){ // repite el proceso anterior con el fin de concebir mas hijos
                case -1:
                    throw_error();
                break;
                case 0:
                    bPID=getpid();
                    printf("Soy el proceso B: mi pid es %d. Mi padre es %d. Mi abuelo es %d\n", bPID, aPID, ejecPID);
                    if((xPID=fork())==0){ // empieza a generar los procesos hermanos
                        printf("Soy el proceso X: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n", getpid(), bPID, aPID, ejecPID);
                        pause(); // esta orden pausa el proceso a la espera de una senyal que lo mate
                        exit(EXIT_SUCCESS); // tras recibir la senyal el proceso muere de manera satisfactoria
                    }
                    else{
                        my_sync(1); // lanzamos la funcion que sincroniza la salida
                        if((yPID=fork())==0){ // se genera el proceso y
                            printf("Soy el proceso Y: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n", getpid(), bPID, aPID, ejecPID);
                            pause();
                            exit(EXIT_SUCCESS);
                        }else{
                            my_sync(2); // lanzamos otra vez la funcion de sincro
                            int exec_status; // aguardamos el status del exec 
                            if((zPID=fork())==0){
                                signal(SIGALRM, alarma); // se prepara la senyal que va ser soportada por alarma como handlder
                                alarm(time); // se programa la alarma para que se lance la senyal pasado time tiempo
                                printf("Soy el proceso Z: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n", getpid(), bPID, aPID, ejecPID);
                                pause(); // se pausa el codigo a la espera de la senyal
                                if(fork()==0){
                                    //if(execlp("ls","ls","-a",NULL)<0){
                                    int leng = count_digits(ejecPID) +1; // contamos la longitud del numero del pid del proceso padre
                                    char myPID[leng]; // generamos la variable donde lo guardaremos
                                    sprintf(myPID,"%d",ejecPID); // lo almacenamos en forma de cadena de texto
                                    if(execlp("pstree","pstree","-p",myPID,NULL)<0){ // pstree -p | grep ejec // se lanza dentro del hijo z1 la orden mediante exec, esto destruye el proceso z1
                                        perror("ERROR, No se ha podido lanzar el exec");
                                        exit(EXIT_FAILURE);
                                    }
                                }else{
                                    wait(&exec_status); // zOrigen o z2 espera que exec acabe, cuando lo haga, continuara hasta su muerte
                                    printf("Soy Z (%d) y muero\n", getpid());
                                    exit(EXIT_SUCCESS);
                                }
                            }else{
                                wait(&statusZ); // una vez muerto Z, B puede continuar
                                if(statusZ==EXIT_SUCCESS){
                                    printf("Soy Y (%d) y muero\n", yPID);
                                    kill(yPID, SIGKILL); // mata a Y
                                    my_sync(1); // sincro de la salida
                                    kill(xPID, SIGKILL); // mata a X
                                    printf("Soy X (%d) y muero\n", xPID);
                                    my_sync(1); // sincro de la salida
                                }
                                printf("Soy B (%d) y muero\n", getpid());
                                exit(EXIT_SUCCESS); // muerto los hijos de B, este muere
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

        default: // en el caso default se ejecuta el codigo correspondiente al padre o proceso original(en este caso)
            printf("Soy el proceso ejec: mi pid es %d\n",getpid());
            wait(&statusA);
            printf("Soy ejec (%d) y muero\n", getpid());
        break;
    }

    return 0;
}

/**
 * Funcion auxiliar para imprimir errores
 */ 
void throw_error(void){
    perror("ERROR No se ha podido crear un nuevo proceso");
    exit(EXIT_FAILURE);
}

/**
 * Funcion auxuliar que sirve para sincronizar la salida estandar por consola, sirve para que se impriman en orden los mensajes de los hijos hermanos X Y Z
 * @param time
 */
void my_sync(long unsigned int time){
    for(long unsigned int i = 0; i<time*1000000;i++); // 1000000 ciclos asegura una impresion ordenada
    return;
}

/**
 * Cuenta los digitos que componen un entero
 * ! n solo puede tomar valores de 0 a Infinitos, siempre positivo
 * @param n : entero a analizar
 * @return i : total de digitos que componen n
 */ 
int count_digits(int n){
    int m = n; // copia 'defensiva' para evitar a toda costa de que n se modifique, realmente no es necesario
    int i=0;
    while (m>0){ // cuenta los digitos dividiendo entre 10 el numero hasta que sea 0        \
                    se podría hacer mas facil si simplemnte se hiciera el log10             \
                    pero la version del compilador y la libreria math de C no permiten su uso
        m=m/10;
        i++;
    }
    return i;
}