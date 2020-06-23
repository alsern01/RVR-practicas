#include <string>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include <thread>
#include <mutex>
#include <condition_variable>

#include "Serializable.h"
#include "Socket.h"
#include "Game.h"

// Mutex para la seccion critica
static std::mutex MUTEX;
static std::condition_variable V_COND;
static bool END = false;

int main(int argc, char **argv)
{
    GameServer server(Socket(argv[1], argv[2]));

    server.update();

    // MUTEX
    std::unique_lock<std::mutex> lock(MUTEX);
    V_COND.wait(lock, [&]() { return END; });

    return 0;
}
