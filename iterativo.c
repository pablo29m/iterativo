#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9999
#define BUFFER_SIZE 1024

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];

    // Crear el socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // Enlazar el socket a la dirección y el puerto
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error al enlazar el socket");
        exit(EXIT_FAILURE);
    }

    // Escuchar conexiones entrantes
    if (listen(server_socket, 5) == -1) {
        perror("Error al escuchar");
        exit(EXIT_FAILURE);
    }

    printf("Servidor escuchando en el puerto %d\n", PORT);

    while (1) {
        socklen_t client_addr_len = sizeof(client_addr);
        
        // Aceptar conexiones entrantes
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Error al aceptar la conexión");
            exit(EXIT_FAILURE);
        }

        printf("Conexión entrante de: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        while (1) {
            // Recibir datos del cliente
            ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
            if (bytes_received == -1) {
                perror("Error al recibir datos del cliente");
                exit(EXIT_FAILURE);
            } else if (bytes_received == 0) {
                // El cliente ha cerrado la conexión
                printf("El cliente ha cerrado la conexión\n");
                break;
            }

            // Imprimir el mensaje recibido
            printf("Mensaje del cliente: %s\n", buffer);

            // Responder al cliente con un mensaje de confirmación
            const char *response = "Recibido";
            if (send(client_socket, response, strlen(response), 0) == -1) {
                perror("Error al enviar datos al cliente");
                exit(EXIT_FAILURE);
            }
        }

        // Cerrar el socket del cliente
        close(client_socket);
    }

    // Cerrar el socket del servidor
    close(server_socket);

    return 0;
}
