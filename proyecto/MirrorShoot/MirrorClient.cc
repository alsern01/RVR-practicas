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

    game.update();

    return 0;
}
