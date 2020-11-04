// MANUEL MORALES AMAT DNI 48789509T

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // funciones auxiliares como inet_addr()
#include <string.h> // strtok
#include <sys/wait.h> // wait
#include <unistd.h> // close

#define DEF_PORT 9999 // PUERTO PREDETERMINADO, FINALMENTE SE PODRA CAMBIAR EN LOS PARAMETROS DE LANZAMIENTO DEL SEVIDOR
#define SRVR_META struct sockaddr_in
#define BUFF_SIZE 1024
#define FOREVER TRUE
#define TRUE 1

void throw_listening_error(void); // lanza error si no se puede escuchar el puerto
void throw_fork_error(int); // lanza un error si no se puede generar un hijo
void throw_execute_error(int); // lanza un error si no se puede se hacer el execute

int call_exec(char*,int); // lanza el exec, si falla devuelve -1

/**
 * ! Funcion principal, se lanza con ServidorRemoto, NO RECIVE PARAMETROS
 * @param argc : total de argumentos pasados por parametro
 * @param argv : lista de argumentos pasados por parametro
 */ 
int main(int argc, char** argv){
    int server_socket = socket(PF_INET,SOCK_STREAM, 0); // PF_INET->IPv4 | SOCK_STREAM->Stream De operacion | 0->Tipo de protocolo (0 = TCP | 1 = UDP...)
    struct sockaddr_in *server_meta;    
    server_meta = malloc(sizeof(struct sockaddr_in)); // reservamos memoria para la info del servidor

    server_meta->sin_family = AF_INET; // tipo de conexion | AF_INET->IPv4
    server_meta->sin_port = htons(DEF_PORT); // reservamos el puerto
    server_meta->sin_addr.s_addr = INADDR_ANY; //inet_addr("127.0.0.1"); // reservamos la ip
    //memset(server_meta->sin_zero, '\0', sizeof(server_meta->sin_zero)); // colocamos a 0 la correccion de cabeceras

    bind(server_socket,(struct sockaddr*)server_meta,sizeof(server_meta)); // abrimos el puerto y le cargamos el socket de entrada
    short int isListening = listen(server_socket,5); // escuchamos el trafico del socket
    if(isListening!=0) // 0 si esta escuchando, cualquier otro numero sino
        throw_listening_error(); // lanza un error de lectura y mata el proceso
    struct sockaddr_storage *server_strg; // abre una caja para almacenar la nueva conexion
    server_strg = (struct sockaddr_storage*)malloc(sizeof(struct sockaddr_storage)); // reserva memoria para la nueva conexion
    socklen_t incomming_addr_size = sizeof(struct sockaddr); // calcula el tamaño de la primera conexion
    long unsigned int i = 0; // contador de clientes conectados
    do{
        char incomming_msg[BUFF_SIZE]; // buffer de entrada, si es TERMINATE mata el servidor
        int accept_size = sizeof(struct sockaddr_in);
        int incomming_socket = accept(server_socket, (struct sockaddr*)&server_strg[i], &accept_size); // aceptamos la conexion entrante y genera un nuevo socket para manejarla
        if(incomming_socket==-1) continue; // si el socket recivido da error se relanza la escucha
        recv(incomming_socket,&incomming_msg,BUFF_SIZE, 0); // hace un read del socket, recv es equivalente 
        if(strcmp("terminate", incomming_msg)==0) break; // si recive el comando terminate, sale y ejecuta el protoclo de muerte
        switch (fork()){
            case -1:
                throw_fork_error(incomming_socket);
            break;
            case 0:
                if(call_exec(incomming_msg,incomming_socket)<0){
                    throw_execute_error(incomming_socket);
                }
                close(incomming_socket);
                exit(EXIT_SUCCESS);
            break;
            default:
            i++;
            server_strg = (struct sockaddr_storage*)realloc(&server_strg,(sizeof(server_strg)+incomming_addr_size));
            break;
        }
    }while(FOREVER);
    close(server_socket);
    free(server_meta);
    free(server_strg);
    return 0;
}

/**
 * ! Funcion que lanza un error si no se puede leer el socket
 */ 
void throw_listening_error(void){
    perror("ERROR: Se ha prodicio un error al escuchar en el puerto DEF_PORT, ya hay un proceso escuchando en este puerto.\n");
    exit(EXIT_FAILURE);
}

/**
 * ! Funcion que lanza un error si no se puede generar un hijo
 * @param incomming_socket : socket para reenviar el error
 */ 
void throw_fork_error(int incomming_socket){
    char *errmsg = "ERROR: No se ha podido ejecutar tu peticion, debido a que no se ha podido generar un nuevo proceso, espera un tiempo y vuelve a intentarlo.\n\0";
    perror("ERROR: No se ha podido generar un nuevo proceso.\n");
    send(incomming_socket,errmsg,strlen(errmsg),0);
    exit(EXIT_FAILURE);
}

/**
 * ! Funcion que lanza un error si no se puede ejecutar el comando deseado
 * @param incomming_socket : socket para reenviar el error
 */ 
void throw_execute_error(int incomming_socket){
    char *errmsg = "ERROR: No se ha podido ejecutar tu peticion, el comando no es correcto o no es un ejecutable para este sistema, pruebe con otro comando.\n\0";
    perror("ERROR: No se ha podido ejecutar el programa seleccionado.\n");
    send(incomming_socket,errmsg,strlen(errmsg),0);
    exit(EXIT_FAILURE);
}

/**
 * ! Funcion que ejetuca el comando pedido por parametro
 * @param incomming_msg : mensaje recivido
 * @param incomming_socket : socket para reenviar la salida de la ejecucion
 */ 
int call_exec(char *incomming_msg, int incomming_socket){
    char *tokens = strtok(incomming_msg," \0");
    while (tokens!=NULL)
        tokens = strtok(NULL," \0");
    switch (fork()){
        case -1:
            throw_fork_error(incomming_socket);
        break;
        case 0:
            if(execv(&tokens[0],&tokens)<0){
                throw_execute_error(incomming_socket);
            }
            close(incomming_socket);
            exit(EXIT_SUCCESS);
        break;
        default:
            wait(NULL);
        break;
    }
    return -1;
}