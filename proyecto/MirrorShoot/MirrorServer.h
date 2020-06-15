// Sockets y serializacion
#include "Serializable.h"
#include "Socket.h"

#include <thread>

class MirrorServer {
private:
    Socket sd;
public:

    MirrorServer(const char *ip, const char *port) : sd(ip, port) { };
    ~MirrorServer() {};

    // Getters y setters
    Socket getSocket() { return sd; };
};