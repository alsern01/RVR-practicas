// Sockets y serializacion
#include "Serializable.h"
#include "Socket.h"
#include "Game.h"

class MirrorClient {
private:
    Socket sc;
public:

    MirrorClient(const char *ip, const char *port) : sc(ip, port) { };
    ~MirrorClient() {};

    // Getters y setters
    Socket getSocket() { return sc; };
};