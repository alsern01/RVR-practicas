#include "Chat.h"

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char *tmp = _data;
    // Copia en tmp lo que hay en type
    memcpy(tmp, &type, sizeof(uint8_t));
    // Mueve el puntero
    tmp += sizeof(uint8_t);
    // Copia en tmp el nick
    memcpy(tmp, nick.c_str(), 7 * sizeof(char));
    // Mueve el puntero 8 posiciones (la 8 sera para '\0')
    tmp += 8 * sizeof(char);
    // Copia en tmp el message
    memcpy(tmp, message.c_str(), 79 * sizeof(char));
}

int ChatMessage::from_bin(char *bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char *tmp = _data;

    // Copia en type lo que hay en tmp
    memcpy(&type, tmp, sizeof(int8_t));
    // Mueve manualmente el puntero
    tmp += sizeof(int8_t);
    // Copia en nick lo que hay en tmp
    char nick_[8];
    memcpy(nick_, tmp, 8 * sizeof(char));
    nick = nick_;

    tmp += 8 * sizeof(char);
    char msg[80];
    // Copia en msg lo que hay en tmp
    memcpy(msg, tmp, 80 * sizeof(char));
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

        // - LOGIN: Añadir al vector clients

        // - LOGOUT: Eliminar del vector clients

        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)

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

}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
    }
}
