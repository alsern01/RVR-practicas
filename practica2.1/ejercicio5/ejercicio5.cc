#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

/*
./tcp 0.0.0.0 7777
*/

int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct addrinfo *res;

    // ---------------------------------------------------------------------- //
    // INICIALIZACIÓN SOCKET & BIND //
    // ---------------------------------------------------------------------- //

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if (rc != 0)
    {
        std::cerr << "getaddrinfo: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    // res contiene la representación como sockaddr de dirección + puerto

    int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    // ---------------------------------------------------------------------- //
    // CONEXION AL SERVIDOR //
    // ---------------------------------------------------------------------- //
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    int sd_server = connect(sd, res->ai_addr, res->ai_addrlen);

    // ---------------------------------------------------------------------- //
    // GESTION DE LA CONEXION SERVIDOR //
    // ---------------------------------------------------------------------- //
    bool connection = true;

    while (connection)
    {
        char buffer[80];

        std::cin >> buffer;

        if (buffer[0] == 'Q' && buffer[1] == '\0')
        {
            connection = false;
        }

        else
        {
            send(sd_server, (void *)buffer, sizeof(buffer), 0);

            recv(sd_server, (void *)buffer, sizeof(char) * 79, 0);

            std::cout << buffer << std::endl;
        }
    }

    close(sd_server);
    freeaddrinfo(res);

    return 0;
}