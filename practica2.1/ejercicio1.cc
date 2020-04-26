#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct addrinfo *res, *r;
    int sfd;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_UNSPEC;
    hints.ai_next = NULL;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if (rc != 0)
    {
        std::cerr << "Error: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    for (r = res; r != NULL; r = r->ai_next)
    {
        getnameinfo(r->ai_addr, r->ai_addrlen, host, NI_MAXHOST,
                    service, NI_MAXSERV, NI_NUMERICHOST);

        std::cout << host << "    " << r->ai_family << "    " << r->ai_socktype << std::endl;
    }

    freeaddrinfo(res);

    return 0;
}