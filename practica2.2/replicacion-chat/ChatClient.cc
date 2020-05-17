#include <string>
#include <unistd.h>
#include <string.h>
#include "Chat.h"

extern "C" void * _client_thread(void *arg)
{
    ChatClient * server = static_cast<ChatClient *>(arg);

    server->net_thread();

    return 0;
}

int main(int argc, char **argv)
{
    ChatClient ec(argv[1], argv[2], argv[3]);

    pthread_attr_t attr;
    pthread_t id;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&id, &attr, _client_thread, static_cast<void *>(&ec));

    ec.login();

    ec.input_thread();
}

