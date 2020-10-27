// MANUEL MORALES AMAT DNI 48789509T

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

int set_server_addr(char*); // convierte una cadena a ip
char* send_command(int,int,char*); // envia el comando y recive el output

int main(int argc, char* argv){
    in_addr_t server_address = set_server_addr(argv[1]); // convierte el parametro que se le pase al programa en una ip, si recive localhost la ip es 127.0.0.1
    int client_socket = socket(PF_INET, SOCK_STREAM,0); // abrimos el socket del cliente
    struct sockaddr_in *server_meta; // configuramos la conexion con el servidor
    server_meta = malloc(sizeof(struct sockaddr_in)); // reservamos memoria para una conexion por si el cliente quisiera conectar con varios servidores a la vez
    server_meta->sin_family = AF_INET; // tipo de conexion IPv4
    server_meta->sin_addr.s_addr = server_address; // IP del servidor
    server_meta->sin_port = htons(9999); // puerto de conexion
    memset(server_meta->sin_zero,'\0',sizeof(server_meta->sin_zero)); // desplazamiento de las cabeceras a 0
    socklen_t addr_size = sizeof(*server_meta); // medimos el tamaño de una configuracion de servidor
    connect(client_socket,(struct sockadrr*)server_meta,addr_size); // conectamos con un socket del servidor configurado

    char *recived_output = send_command(client_socket,argc-2,argv);

    /**
     * 
     * TODO
     *  - anyadir el send del comando y el recive de la ejecucion
     * 
     */ 

    return 0;
}

/**
 * ! Funcion que dada una cadena trasnsforma la cadena en una direccion IP
 * @param given_ip : cadena de caracteres que contiene la IP, debe tener terminacion
 * @return : devuelve la ip ya convertida, en caso de que se ingrese la direccion localhost, se devuelve la ip 127.0.0.1
 */
int set_server_addr(char* given_ip){
    char server_ip[strlen(given_ip)];
    strcpy(server_ip,given_ip);
    if(strcmp(server_ip,"localhost")==0)
        return in_addr("127.0.0.1");
    return in_addr(server_ip);
}


/**
 * ! Funcion que envia el comando pasado por parametro al programa y devuelve el output generado por el servidor
 * @param command : comando que se va a lanzar en el servidor
 * @return output : output generado en el servidor
 */
char* send_command(int client_socket, int total_segments, char* command){
    char* output;
    int total_command_length = 0;
    for(int i = 0;i<total_segments;i++) total_command_length+=(1+strlen(command[i+2]));
    char sending_command[total_command_length];
    for(int i = 0;i<total_segments;i++){
        strcat(sending_command,command[i+2]);
        strcat(sending_command," ");
    }
    int last_char = strlen(sending_command);
    sending_command[last_char] = '\0';
    send(client_socket,&sending_command,strlen(sending_command), 0); // envia a un socket un buffer, permite el uso de flags, ponerlas a 0 hace que equivalga a write()
    recv(client_socket,&output,1024, 0); // recibe de un socket un buffer, permite el uso de flags, ponerlas a 0 equivale al uso de read()
    return output;
}