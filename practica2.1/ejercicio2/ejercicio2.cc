#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct addrinfo *res;

    // ---------------------------------------------------------------------- //
    // INICIALIZACIÓN SOCKET & BIND //
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

    if (bind(sd, res->ai_addr, res->ai_addrlen) != 0)
    {
        std::cerr << "Error: Could not bind socket" << std::endl;
        return -1;
    }

    freeaddrinfo(res);

    // ---------------------------------------------------------------------- //
    // RECEPCIÓN MENSAJE DE CLIENTE //
    // ---------------------------------------------------------------------- //
    bool server = true;
    while (server)
    {

        char buffer[80];
        char host[NI_MAXHOST];
        char service[NI_MAXSERV];

        struct sockaddr client_addr;
        socklen_t client_len = sizeof(struct sockaddr);

        ssize_t bytes = recvfrom(sd, buffer, 79 * sizeof(char), 0, &client_addr,
                                 &client_len);

        if (bytes == -1)
        {
            std::cerr << "recvfrom: " << std::endl;
            return -1;
        }

        getnameinfo(&client_addr, client_len, host, NI_MAXHOST, service,
                    NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        std::cout << bytes << " bytes de  " << host << ":" << service << std::endl;
        time_t rawtime;
        size_t len;
        struct tm *info;
        char buf[80];

        time(&rawtime);
        info = localtime(&rawtime);

        if (buffer[0] == 't')
        {
            len = strftime(buf, 80, "%I:%M:%S %p", info);
            memset(buf + len, '\0', sizeof(buf));
        }
        else if (buffer[0] == 'd')
        {
            len = strftime(buf, 80, "%F", info);
            memset(buf + len, '\0', sizeof(buf));
        }
        else if (buffer[0] == 'q')
        {
            std::cout << "Saliendo..." << std::endl;
            memset(buf, '\0', sizeof(buf));
            server = false;
        }
        else
        {
            memset(buf, '\0', sizeof(buf));
            std::cout << "Comando no soportado " << buffer[0] << std::endl;
        }

        // ---------------------------------------------------------------------- //
        // RESPUESTA AL CLIENTE //
        // ---------------------------------------------------------------------- //
        sendto(sd, buf, sizeof(buf), 0, &client_addr, client_len);
    }

    return 0;
}