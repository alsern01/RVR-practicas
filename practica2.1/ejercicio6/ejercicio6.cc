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
static std::mutex MUTEX;
static std::condition_variable V_COND;
static bool END = false;

class Mensaje
{
  private:
    int sd_;
    bool up_ = true;

  public:
    Mensaje(int sd) : sd_(sd){};
    ~Mensaje(){};

    // metodo para gestionar mensajes
    void haz_mensaje()
    {
        // ---------------------------------------------------------------------- //
        // RECEPCIÓN MENSAJE DE CLIENTE //
        // ---------------------------------------------------------------------- //
        char buffer[80];
        char host[NI_MAXHOST];
        char service[NI_MAXSERV];

        struct sockaddr client_addr;
        socklen_t client_len = sizeof(struct sockaddr);

        while (up_)
        {
            memset(&buffer, 0, sizeof(buffer));
            ssize_t bytes = recvfrom(sd_, buffer, 79 * sizeof(char), 0, &client_addr,
                                     &client_len);

            if (bytes == -1)
            {
                std::cerr << "recvfrom: " << std::endl;
                return;
            }

            int err = getnameinfo(&client_addr, client_len, host, NI_MAXHOST, service,
                                  NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

            std::cout << bytes << " bytes de  " << host << ":" << service << std::endl;

            if (err != 0)
            {
                std::cerr << "getnameinfo: " << std::endl;
                return;
            }

            time_t rawtime;
            size_t len;
            struct tm *info;
            char buf[sizeof(buffer)];
            memset(&buf, 0, sizeof(buf));

            time(&rawtime);
            info = localtime(&rawtime);

            if (buffer[0] == 't')
            {
                strftime(buf, 80, "%I:%M:%S %p", info);
                sendto(sd_, buf, strlen(buf), 0, &client_addr, client_len);
            }

            else if (buffer[0] == 'd')
            {
                len = strftime(buf, 80, "%F", info);
                sendto(sd_, buf, strlen(buf), 0, &client_addr, client_len);
            }

            else if (buffer[0] == 'q')
            {
                std::cout << "Saliendo..." << std::endl;
                up_ = false;
                END = true;
                std::unique_lock<std::mutex> lock(MUTEX);
                V_COND.notify_one();
            }

            else
            {
                std::cout << "Comando no soportado " << buffer[0] << std::endl;
            }

            std::cout << "THREAD: " << std::this_thread::get_id() << std::endl;
            std::cout << "--------------------------" << std::endl;
            sleep(10);
        }
    }
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
        pool.push_back(std::thread([&]() {
            Mensaje msg(sd);
            msg.haz_mensaje();
        }));
    }

    // MUTEX
    std::unique_lock<std::mutex> lock(MUTEX);
    V_COND.wait(lock, [&]() { return END; });

    for (auto &t : pool)
    {
        t.join();
    }

    close(sd);

    return 0;
}