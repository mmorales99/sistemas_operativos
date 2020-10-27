// MANUEL MORALES AMAT DNI 48789509T

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define DEF_PORT 9999
#define SRVR_META struct sockaddr_in

void throw_listening_error(void);

int main(int argc, char* argv){
    int server_socket = socket(PF_INET,SOCK_STREAM, 0); // PF_INET->IPv4 | SOCK_STREAM->Stream De operacion | 0->Tipo de protocolo (0 = TCP | 1 = UDP...)
    SRVR_META *server_meta;    
    server_meta = malloc(sizeof(SRVR_META)); // reservamos memoria para la info del servidor
    server_meta->sin_family = AF_INET; // tipo de conexion | AF_INET->IPv4
    server_meta->sin_port = htons(DEF_PORT); // reservamos el puerto
    server_meta->sin_addr.s_addr = inet_addr("127.0.0.1"); // reservamos la ip
    memset(server_meta->sin_zero, '\0', sizeof(server_meta->sin_zero)); // colocamos a 0 la correccion de cabeceras
    bind(server_socket,(struct sockaddr*)server_meta,sizeof(server_meta)); // abrimos el puerto y le cargamos el socket de entrada
    short int isListening = listen(server_socket,10); // escuchamos el trafico del socket
    if(isListening!=0) // 0 si esta escuchando, cualquier otro numero sino
        throw_listening_error(); // lanza un error de lectura y mata el proceso
    struct sockaddr_storage *server_strg; // abre una caja para almacenar la nueva conexion
    server_strg = malloc(sizeof(struct sockaddr_storage)); // reserva memoria para la nueva conexion
    socklen_t incomming_addr_size = sizeof(*server_strg); // calcula el tamaño de la primera conexion
    int incomming_socket = accept(server_socket, (struct sockaddr*)server_strg, &incomming_addr_size); // aceptamos la conexion entrante y genera un nuevo socket para manejarla

    /**
     * 
     * TODO
     *  - añadir loopback de escucha
     *  - fork del proceso para ejecutar los exec necesarios
     */ 

    return 0;
}

void throw_listening_error(void){
    perror("Se ha prodicio un error al escuchar en el puerto DEF_PORT, ya hay un proceso escuchando en este puerto.\n");
    exit(EXIT_FAILURE);
}