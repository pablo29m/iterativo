#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9999
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Crear el socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // Conectar al servidor
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error al conectar al servidor");
        exit(EXIT_FAILURE);
    }

    printf("Conexión establecida con el servidor\n");

    // Mensaje a enviar al servidor
    const char *message = "Hola, servidor";

    // Enviar el mensaje al servidor
    if (send(client_socket, message, strlen(message), 0) == -1) {
        perror("Error al enviar datos al servidor");
        exit(EXIT_FAILURE);
    }

    printf("Mensaje enviado al servidor: %s\n", message);

    // Recibir la respuesta del servidor
    ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received == -1) {
        perror("Error al recibir datos del servidor");
        exit(EXIT_FAILURE);
    }

    // Agregar el terminador nulo al final de la cadena recibida
    buffer[bytes_received] = '\0';

    printf("Respuesta del servidor: %s\n", buffer);
    sleep(40);

    // Cerrar el socket del cliente
    close(client_socket);

    return 0;
}
