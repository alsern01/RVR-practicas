#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <thread>
#include <vector>

void haz_mensaje(int sd, int id)
{
    // ---------------------------------------------------------------------- //
    // RECEPCIÓN MENSAJE DE CLIENTE //
    // ---------------------------------------------------------------------- //
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
        return;
    }

    getnameinfo(&client_addr, client_len, host, NI_MAXHOST, service,
                NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

    std::cout << "THREAD: " << id << " IP: " << host << " PUERTO: " << service
              << "MENSAJE: " << buffer << std::endl;

    sleep(2);

    // ---------------------------------------------------------------------- //
    // RESPUESTA AL CLIENTE //
    // ---------------------------------------------------------------------- //
    sendto(sd, buffer, bytes, 0, &client_addr, client_len);
}

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

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if (rc != 0)
    {
        std::cerr << "getaddrinfo: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    // res contiene la representación como sockaddr de dirección + puerto

    int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if (bind(sd, res->ai_addr, res->ai_addrlen) != 0)
    {
        std::cerr << "bind: " << std::endl;
        return -1;
    }

    freeaddrinfo(res);

    // -------------------------------------------------------------------------
    // POOL DE THREADS
    // -------------------------------------------------------------------------
    std::vector<std::thread> pool;

    for (int i = 0; i < 5; ++i)
    {
        pool.push_back(std::thread(haz_mensaje, sd, i));
    }

    for (auto &t : pool)
    {
        t.join();
    }

    close(sd);

    return 0;
}