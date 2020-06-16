#include <string>
#include <unistd.h>
#include <string.h>
#include "MirrorClient.h"

int main(int argc, char **argv)
{
    MirrorClient client(argv[1], argv[2]);

    client.getSocket().bind();

    GameClient game(client.getSocket(), 800, 400);

    while (!game.gameOver())
    {
        game.update();
    }

    return 0;
}
