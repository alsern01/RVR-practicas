#include "Chat.h"

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char *buf = _data;
    // Copia en buf lo que hay en type
    memcpy(buf, &type, sizeof(uint8_t));
    // Mueve el puntero
    buf += sizeof(uint8_t);
    // Copia en buf el nick
    memcpy(buf, nick.c_str(), strlen(nick.c_str()));
    // Mueve el puntero 8 posiciones (la 8 sera para '\0')
    buf += 8 * sizeof(char);
    // Copia en buf el message
    memcpy(buf, message.c_str(), strlen(message.c_str()));
}

int ChatMessage::from_bin(char *bobj)
{
    alloc_data(MESSAGE_SIZE);

    //memcpy(&_data, bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    _data = bobj;

    // Copia en type lo que hay en buffer
    memcpy(&type, _data, sizeof(int8_t));
    // Mueve manualmente el puntero
    _data += sizeof(int8_t);
    // Copia en nick lo que hay en buffer
    char nick_[8];
    memcpy(nick_, _data, 7 * sizeof(char));
    nick = nick_;

    _data += 8 * sizeof(char);
    char msg[80];
    // Copia en msg lo que hay en buffer
    memcpy(msg, _data, 79 * sizeof(char));
    message = msg;

    // Control de errores
    if ((nick.length() + message.length()) < 0)
        return -1;

    return nick.length() + message.length();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        //Recibir Mensajes en y en función del tipo de mensaje
        ChatMessage *msg = new ChatMessage();

        socket.recv(*msg, (Socket *&)*(&socket));

        switch (msg->type)
        {
        // - LOGIN: Añadir al vector clients
        case ChatMessage::LOGIN:
        {
            clients.push_back(&socket);
            std::cout << msg->nick << " se unió al chat" << std::endl;
            break;
        }
        // - LOGOUT: Eliminar del vector clients
        case ChatMessage::LOGOUT:
        {
            std::cout << msg->nick << " abandonó el chat" << std::endl;
            bool erased = false;
            std::vector<Socket *>::iterator it = clients.begin();
            for (int i = 0; i < clients.size() && !erased; ++i)
            {
                if (socket == *clients.at(i))
                {
                    clients.erase(it);
                    erased = true;
                }
                ++it;
            }
            break;
        }
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
        case ChatMessage::MESSAGE:
        {
            std::cout << msg->nick << ": " << msg->message << std::endl;
            for (int i = 0; i < clients.size(); ++i)
            {
                if (socket == *clients.at(i))
                {
                    // Si es el emisor no hace nada
                }
                else
                    socket.send(*msg, *clients.at(i));
            }
            break;
        }
        default:
            break;
        }
    }
}

void ChatClient::login()
{
    connected = true;
    // Crea mensaje de login
    std::string msg = "Usuario " + nick + " se ha conectado.";
    // Saca por pantalla
    std::cout << msg << std::endl;
    // Crea el ChatMessage
    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;
    // Envia al servidor
    socket.send(em, socket);
}

void ChatClient::logout()
{
    // Crea mensaje de logout
    std::string msg = "Usuario " + nick + " se ha desconectado.";
    // Saca por pantalla
    std::cout << msg << std::endl;
    // Crea el ChatMessage
    ChatMessage m(nick, msg);
    m.type = ChatMessage::LOGOUT;
    // Envia al servidor
    socket.send(m, socket);
}

void ChatClient::input_thread()
{
    while (connected)
    {
        // Leer stdin con std::getline
        std::string msg;
        std::getline(std::cin, msg);

        // Enviar al servidor usando socket
        if (msg[0] == 'q' && msg.length() == 1)
        {
            connected = false;
        }
        else
        {
            ChatMessage sm(nick, msg);
            sm.type = ChatMessage::MESSAGE;
            socket.send(sm, socket);
        }
    }
    logout();
}

void ChatClient::net_thread()
{
    while (connected)
    {
        //Recibir Mensajes de red
        ChatMessage rm;
        socket.recv(rm);

        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        if (rm.type == ChatMessage::MESSAGE)
            std::cout << rm.nick << ": " << rm.message << std::endl;
        else
            std::cout << rm.message << std::endl;
    }
}
