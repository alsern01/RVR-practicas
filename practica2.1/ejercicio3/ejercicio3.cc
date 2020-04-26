#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

const char *command = NULL;

int main(int argc, char **argv)
{
    command = argv[3];

    struct addrinfo hints;
    struct addrinfo *res;

    // ---------------------------------------------------------------------- //
    // INICIALIZACIÓN SOCKET//
    // ---------------------------------------------------------------------- //

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if (rc != 0)
    {
        std::cerr << "Error: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    // res contiene la representación como sockaddr de dirección + puerto

    int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    // ---------------------------------------------------------------------- //
    // MENSAJE AL SERVIDOR //
    // ---------------------------------------------------------------------- //
    struct sockaddr server_addr;
    socklen_t server_len = sizeof(struct sockaddr);

    sendto(sd, command, sizeof(command), 0, res->ai_addr, res->ai_addrlen);

    // ---------------------------------------------------------------------- //
    // RESPUESTA DEL SERVIDOR //
    // ---------------------------------------------------------------------- //
    char buffer[80];
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    ssize_t bytes = recvfrom(sd, buffer, sizeof(buffer), 0, res->ai_addr,
                             &res->ai_addrlen);

    if (bytes == -1)
    {
        std::cerr << "recvfrom: " << std::endl;
        return -1;
    }

    std::cout << buffer;

    freeaddrinfo(res);

    return 0;
}