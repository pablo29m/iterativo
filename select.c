#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>
#define PORT 9999
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 30

int main() {
    int server_socket, new_socket, client_socket[MAX_CLIENTS], max_clients = MAX_CLIENTS, activity, i, valread, sd;
    int max_sd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    fd_set readfds;

    // Inicializar todos los client_socket a 0
    for (i = 0; i < max_clients; i++) {
        client_socket[i] = 0;
    }

    // Crear el socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Enlazar el socket a la dirección y el puerto
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al enlazar el socket");
        exit(EXIT_FAILURE);
    }

    // Escuchar conexiones entrantes
    if (listen(server_socket, 3) < 0) {
        perror("Error al escuchar");
        exit(EXIT_FAILURE);
    }

    printf("Servidor escuchando en el puerto %d\n", PORT);

    int addrlen = sizeof(server_addr);

    while (1) {
        // Limpiar el conjunto de descriptores de archivos
        FD_ZERO(&readfds);

        // Agregar el socket maestro al conjunto
        FD_SET(server_socket, &readfds);
        max_sd = server_socket;

        // Agregar sockets hijos al conjunto
        for (i = 0; i < max_clients; i++) {
            // Descriptor de archivo
            sd = client_socket[i];

            // Si la descripción del socket es válida, agregarla al conjunto de lectura
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }

            // El mayor número de descriptor de archivos, necesario para la función select
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        // Esperar la actividad en uno de los sockets
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("Error en select");
        }

        // Si algo sucedió en el socket maestro, entonces es una nueva conexión entrante
        if (FD_ISSET(server_socket, &readfds)) {
            if ((new_socket = accept(server_socket, (struct sockaddr *)&client_addr, (socklen_t*)&addrlen)) < 0) {
                perror("Error al aceptar la conexión");
                exit(EXIT_FAILURE);
            }

            printf("Nueva conexión, socket fd es %d, ip es : %s, puerto : %d\n", new_socket, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            // Agregar nuevo socket al array de sockets
            for (i = 0; i < max_clients; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    printf("Añadiendo a la lista de sockets como %d\n", i);
                    break;
                }
            }
        }

        // Operaciones IO en otros sockets
        for (i = 0; i < max_clients; i++) {
            sd = client_socket[i];

            if (FD_ISSET(sd, &readfds)) {
                // Verificar si se cerró y leer el mensaje entrante
                if ((valread = read(sd, buffer, BUFFER_SIZE)) == 0) {
                    // Alguien se desconectó
                    getpeername(sd, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen);
                    printf("Cliente desconectado, ip %s, puerto %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                    // Cerrar el socket y marcar como 0 en la lista
                    close(sd);
                    client_socket[i] = 0;
                } else {
                    // Responder al cliente con un mensaje de confirmación
                    buffer[valread] = '\0';
                    printf("Mensaje del cliente: %s\n", buffer);
                    const char *response = "Recibido";
                    if (send(sd, response, strlen(response), 0) == -1) {
                        perror("Error al enviar datos al cliente");
                    }
                }
            }
        }
    }

    // Cerrar el socket del servidor
    close(server_socket);

    return 0;
}
