#include <string>
#include <unistd.h>
#include <thread>
#include <string.h>

#include "Game.h"

extern "C" void *_client_thread(void *arg)
{
    GameClient *client = static_cast<GameClient *>(arg);

    client->manageMessage();

    return 0;
}

int main(int argc, char **argv)
{
    //MirrorClient client(argv[1], argv[2]);

    GameClient game(Socket(argv[1], argv[2]), 800, 400);

    pthread_attr_t attr;
    pthread_t id;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    // crea un hilo de ejecucion para el cliente
    pthread_create(&id, &attr, _client_thread, static_cast<void *>(&game));

    game.update();

    return 0;
}
