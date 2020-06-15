#include <string>
#include <unistd.h>
#include <string.h>
#include "MirrorClient.h"

int main(int argc, char **argv)
{
    MirrorClient client(argv[1], argv[2]);

    client.getSocket().bind();

    return 0;
}

