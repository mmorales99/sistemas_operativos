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

in_addr_t set_server_addr(char*); // convierte una cadena a ip
void send_command(int,int,char**,char*); // envia el comando y recive el output

/**
 * ! Funcion principal del cleinte, recvide 2 parametros, 1ro la IP_Servidor donde se quiere ejecutar el segundo parametro, 2do el comando que se quiere ejecutar en el servidor
 * @param argc : contador de argumentos, siempre ha de estar entre 1 y 3, en caso de que sean menos de 3 fallara la ejecucion del codigo
 * @param argv : vector donde se almacenan los argumentos del programa al ser llamado, el 1ro es el nombre del propio programa
 * @return 0 : en caso de ejecucion satisfactoria
 */ 
int main(int argc, char** argv){

    //in_addr_t server_address = set_server_addr(argv[1]); // convierte el parametro que se le pase al programa en una ip, si recive localhost la ip es 127.0.0.1
    
    int client_socket = socket(PF_INET, SOCK_STREAM,0); // abrimos el socket del cliente
    struct sockaddr_in *server_meta; // configuramos la conexion con el servidor
    server_meta = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in)); // reservamos memoria para una conexion por si el cliente quisiera conectar con varios servidores a la vez
    server_meta->sin_family = AF_INET; // tipo de conexion IPv4

    server_meta->sin_addr.s_addr = inet_addr(argv[1]); // IP del servidor

    server_meta->sin_port = htons(9999); // puerto de conexion
    //memset(server_meta->sin_zero,'\0',sizeof(server_meta->sin_zero)); // desplazamiento de las cabeceras a 0

    int addr_size = sizeof(struct sockaddr_in); // medimos el tamaño de una configuracion de servidor
    int isConnected = connect(client_socket,(struct sockaddr*)server_meta,addr_size); // conectamos con un socket del servidor configurado
    if(isConnected<0){
        perror("ERROR: No se ha podido conectar con el servidor.\n");
        exit(EXIT_FAILURE);
    }
    char recieved_output[1024]; // buffer donde se va a registar la salida del servidor
    send_command(client_socket,argc,argv,recieved_output); // se envia el comando y se espera a que se reciva una respuesta
    //int recieved_output_leng = strlen(recieved_output);
    printf("El comando lanzado ha recibido el siguiente output en el servidor: \n %s \n", recieved_output); // se imprime la respuesta del servidor
    close(client_socket);
    return 0;
}

/**
 * ! Funcion que dada una cadena trasnsforma la cadena en una direccion IP
 * @param given_ip : cadena de caracteres que contiene la IP, debe tener terminacion
 * @return : devuelve la ip ya convertida, en caso de que se ingrese la direccion localhost, se devuelve la ip 127.0.0.1
 */
in_addr_t set_server_addr(char* given_ip){
    if(strcmp(given_ip,"localhost")==0)
        return inet_addr("127.0.0.1");
    return inet_addr(given_ip);
}

/**
 * ! Funcion que envia el comando pasado por parametro al programa y devuelve el output generado por el servidor
 * @param command : comando que se va a lanzar en el servidor
 * @return output : output generado en el servidor
 */
void send_command(int client_socket, int total_segments, char** command, char* output){
    total_segments-=2;
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
    recv(client_socket,&output, 1024, 0); // recibe de un socket un buffer, permite el uso de flags, ponerlas a 0 equivale al uso de read()
}