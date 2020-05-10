#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <time.h>

#include <iostream>

#include <thread>
#include <mutex>
#include <condition_variable>

#include <vector>

// Mutex para la seccion critica
//static std::mutex MUTEX;
//static std::condition_variable V_COND;
//static bool END = false;

class Mensaje
{
  private:
    int sd_;
    bool up_ = true;

  public:
    Mensaje(int sd) : sd_(sd){};
    ~Mensaje(){};

    // metodo para gestionar mensajes
    void haz_mensaje(int sd_c)
    {
        // ---------------------------------------------------------------------- //
        // GESTION CONEXION CLIENTE //
        // ---------------------------------------------------------------------- //

        char buffer[80];

        while (up_)
        {
            memset(&buffer, 0, sizeof(buffer));
            ssize_t bytes = recv(sd_c, (void *)buffer, sizeof(char) * 79, 0);

            if (bytes <= 0)
            {
                up_ = false;
                std::cout << "Conexión terminada" << std::endl;
                std::cout << "THREAD: " << std::this_thread::get_id() << std::endl;
                std::cout << "--------------------------" << std::endl;
            }

            if (buffer[0] == 'Q' && strlen(buffer) <= 2)
            {
                up_ = false;
                std::cout << "Conexión terminada" << std::endl;
                return;
            }

            send(sd_c, (void *)buffer, bytes, 0);

            buffer[strlen(buffer) - 1] = '\0';
            std::cout << "MENSAJE: " << buffer << std::endl;
            std::cout << "THREAD: " << std::this_thread::get_id() << std::endl;
            std::cout << "--------------------------" << std::endl;
        }
    };
};

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

    // ---------------------------------------------------------------------- //
    // PUBLICAR EL SERVIDOR (LISTEN) //
    // ---------------------------------------------------------------------- //
    listen(sd, 16);
    // ---------------------------------------------------------------------- //
    // GESTION DE LAS CONEXIONES AL SERVIDOR //
    // ---------------------------------------------------------------------- //
    struct sockaddr client_addr;
    socklen_t client_len = sizeof(struct sockaddr);

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    int sd_client;

    // -------------------------------------------------------------------------
    // POOL DE THREADS
    // -------------------------------------------------------------------------
    std::vector<std::thread> pool;

    do
    {
        sd_client = accept(sd, &client_addr, &client_len);

        getnameinfo(&client_addr, client_len, host, NI_MAXHOST, service,
                    NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        std::cout << "Conexión desde " << host << " " << service << std::endl;

        pool.push_back(std::thread([&]() {
            Mensaje msg(sd);
            msg.haz_mensaje(sd_client);
        }));

    } while (sd_client != -1);

    // MUTEX
    //std::unique_lock<std::mutex> lock(MUTEX);
    //V_COND.wait(lock, [&]() { return END; });

    for (auto &t : pool)
    {
        t.join();
    }

    std::cout << "Conexion terminada" << std::endl;

    close(sd);

    return 0;
}