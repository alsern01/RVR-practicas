#include "Game.h"

// ---------------------------------------------------------------------- //
// --- SERVER ----------------------------------------------------------- //
// ---------------------------------------------------------------------- //

void GameServer::init()
{
    server.bind();
}

void GameServer::update()
{
    int jugadores = 0;

    while (!gameover)
    {
        Socket *sock;
        Message *msg = new Message();

        server.recv(*msg, sock);

        switch (msg->type)
        {
        case Message::LOGIN:
        {
            if (clients.size() < 2)
            {
                clients.push_back(sock);
                jugadores++;
                std::cout << "Jugador " << jugadores << "  conectado\n";
                msg->type = Message::WAITING;
                server.send(*msg, *sock);
            }
            // si hay 2 jugadores en la partida empieza
            if (clients.size() == 2)
            {
                // si hay dos clientes conectados empieza partida
                msg->type = Message::START;
                for (int i = 0; i < clients.size(); i++)
                {
                    server.send(*msg, *clients.at(i));
                }
            }
        }
        break;
        case Message::GAME:
        case Message::COLLISION:
        {
            for (int i = 0; i < clients.size(); ++i)
            {
                if (!(*sock == *clients.at(i)))
                {
                    server.send(*msg, *clients.at(i));
                }
            }
        }
        break;
        case Message::LOGOUT:
        {
            bool erased = false;
            std::vector<Socket *>::iterator it = clients.begin();
            for (int i = 0; i < clients.size() && !erased; ++i)
            {
                if (*sock == *clients.at(i))
                {
                    std::cout << "Jugador desconectado\n";
                    clients.erase(it);
                    erased = true;
                    jugadores--;
                }
                ++it;
            }

            if (msg->isGameOver() == 1 && clients.size() == 0)
            {
                gameover = true;
            }

            for (int i = 0; i < clients.size(); i++)
            {
                msg->type = Message::WAITING;
                server.send(*msg, *clients.at(i));
            }
        }
        break;

        default:
            break;
        }
    }
}

// ---------------------------------------------------------------------- //
// --- CLIENT ----------------------------------------------------------- //
// ---------------------------------------------------------------------- //

void GameClient::init()
{
    XLDisplay::init(width, height, "MirrorShoot");
    x = 150;
    y = (height / 2) - h;
    enemX = width - x - w;
    enemY = y;
    w = h = 80;

    dpy.clear();
    render(dpy);

    sendMessage(Message::LOGIN);
}

// Bucle principal del juego actualiza todos los elementos y procesa input
// Envia la informacion necesaria al servidor
void GameClient::update()
{
    while (!gameover)
    {
        // Coger input
        handleInput(dpy);

        // Actualizar elementos
        checkBounds();
        checkCollision();

        for (auto b : bullets)
        {
            if (b->x < width && !hit && !enemyHit && !waiting)
                b->update();
        }

        // Enviar info al servidor
        if (!hit)
        {
            sendMessage(Message::GAME);
        }

        if (enemyHit)
        {
            sendMessage(Message::COLLISION);
        }

        // Renderizado
        render(dpy);

        dpy.flush();
    }
}

// pone todos los elementos del juego en pantalla
void GameClient::render(XLDisplay &dpy)
{
    dpy.clear();

    // Dibujar el fondo del tablero
    dpy.set_color(XLDisplay::GREEN);
    dpy.rectangleFill(0, 0, width / 3, height);
    dpy.rectangleFill(width * 2 / 3, 0, width / 3, height);
    dpy.set_color(XLDisplay::BLUE);
    dpy.rectangleFill(width / 3, 0, width / 3, height);

    // Dibujar balas
    drawBullets(dpy);

    // Dibujar al jugador
    dpy.set_color(XLDisplay::WHITE);
    dpy.rectangleFill(x, y, w, h);

    // Dibujar al malo
    dpy.set_color(XLDisplay::RED);
    dpy.rectangleFill(enemX, enemY, w, h);

    std::string t = "Q PARA CERRAR";
    dpy.set_color(XLDisplay::BLACK);
    if (hit)
    {
        std::string s = "PIERDES! TE HAN DADO!";
        dpy.text(width / 2 - 100, height / 2, s);
        dpy.text(width / 2 - 80, height / 2 + 20, t);
    }

    else if (enemyHit)
    {
        std::string s = "GANAS! HAS ACERTADO AL RIVAL!";
        dpy.text(width / 2 - 145, height / 2, s);
        dpy.text(width / 2 - 80, height / 2 + 20, t);
    }
    if (waiting)
    {
        t = "ESPERANDO AL OTRO JUGADOR";
        dpy.text(width / 2 - 135, height / 2, t);
        t = "MOVIMIENTO: W,A,S,D | DISPARO: ESPACIO";
        dpy.text(width / 2 - 165, height / 2 + 20, t);
        t = "Q PARA CERRAR";
        dpy.text(width / 2 - 80, height / 2 + 40, t);
    }
}

void GameClient::drawBullets(XLDisplay &dpy)
{
    dpy.set_color(XLDisplay::BLACK);
    for (auto b : bullets)
    {
        dpy.rectangleFill(b->x, b->y, b->w, b->h);
    }

    dpy.set_color(XLDisplay::BROWN);
    for (auto b : bulletsEnem)
    {
        dpy.rectangleFill(b->x, b->y, b->w, b->h);
    }
}

void GameClient::handleInput(XLDisplay &dpy)
{
    char k = dpy.wait_key();
    switch (k)
    {
    case 'w':
        if (!hit && !enemyHit && !waiting)
            y -= 10;
        break;

    case 'a':
        if (!hit && !enemyHit && !waiting)
            x -= 10;
        break;

    case 's':
        if (!hit && !enemyHit && !waiting)
            y += 10;
        break;

    case 'd':
        if (!hit && !enemyHit && !waiting)
            x += 10;
        break;

    case ' ':
    {
        if (!hit && !enemyHit && !waiting)
        {
            Bullet *b = new Bullet(x + w, y + (h / 2));
            bullets.push_back(b);
        }
        break;
    }

    case 'q':
        gameover = true;
        sendMessage(Message::LOGOUT);
        break;
    }
}

void GameClient::checkBounds()
{
    if (x + w >= width / 3)
    {
        x = width / 3 - w;
    }
    else if (x < 0)
    {
        x = 0;
    }
    if (y < 0)
    {
        y = 0;
    }
    else if (y + h >= height)
    {
        y = height - h;
    }
}

void GameClient::checkCollision()
{
    // bullets con enemigo
    for (auto b : bullets)
    {
        if (b->x < enemX + w &&
            b->x + b->w > enemX &&
            b->y < enemY + h &&
            b->h + b->y > enemY)
        {
            enemyHit = true;
        }
    }
    // bullets del malo con jugador
    for (auto b : bulletsEnem)
    {
        if (b->x < x + w &&
            b->x + b->w > x &&
            b->y < y + h &&
            b->h + b->y > y)
        {
            hit = true;
        }
    }
}

void GameClient::manageMessage()
{
    while (!gameover)
    {
        Message msg;
        sock_.recv(msg);

        // Actualiza los elementos del otro jugador
        enemX = width - msg.getX() - w;
        enemY = msg.getY();
        bulletsEnem = msg.getBullets();
        for (auto b : bulletsEnem)
        {
            b->x = width - b->x - b->w;
        }

        if (msg.type == Message::COLLISION)
        {
            hit = true;
        }
        else if (msg.type == Message::WAITING)
        {
            waiting = !waiting;
        }
        else if (msg.type == Message::START)
        {
            waiting = false;
        }
    }
}

void GameClient::sendMessage(int type_)
{
    Message m(gameover, x, y, bullets);
    m.type = type_;
    sock_.send(m, sock_);
}

// ---------------------------------------------------------------------- //
// --- MESSAGE ---------------------------------------------------------- //
// ---------------------------------------------------------------------- //

void Message::to_bin()
{
    alloc_data(MESSAGE_SIZE);
    memset(_data, 0, MESSAGE_SIZE);
    char *tmp = _data;

    // copia info de tipo del mensaje y avanza puntero
    memcpy(tmp, &type, sizeof(int));
    tmp += sizeof(int);
    // copia info de fin del juego y avanza puntero
    memcpy(tmp, &gameOver, sizeof(int));
    tmp += sizeof(int);
    // copia info posX y avanza
    memcpy(tmp, &posX, sizeof(int));
    tmp += sizeof(int);
    // copia info posY y avanza
    memcpy(tmp, &posY, sizeof(int));
    tmp += sizeof(int);
    // copia numero de balas y avanza
    nBullets = bullets.size();
    memcpy(tmp, &nBullets, sizeof(int));
    tmp += sizeof(int);
    // recorre vector de balas para copiar posicion x e y
    // de cada bala mientras avanza el puntero
    for (int i = 0; i < nBullets; i++)
    {
        // copia la X de la bala
        memcpy(tmp, &bullets[i]->x, sizeof(int));
        tmp += sizeof(int);
        // copia la Y de la bala
        memcpy(tmp, &bullets[i]->y, sizeof(int));
        // para que no avance posiciones el puntero en el ultimo elemento del vector
        if (i < nBullets - 1)
            tmp += sizeof(int);
    }
}

int Message::from_bin(char *bobj)
{
    alloc_data(MESSAGE_SIZE);
    char *tmp = bobj;

    // copia info de tipo del mensaje y avanza puntero
    memcpy(&type, tmp, sizeof(int));
    tmp += sizeof(int);
    // copia info de fin del juego y avanza puntero
    memcpy(&gameOver, tmp, sizeof(int));
    tmp += sizeof(int);
    // copia info posX y avanza
    memcpy(&posX, tmp, sizeof(int));
    tmp += sizeof(int);
    // copia info posY y avanza
    memcpy(&posY, tmp, sizeof(int));
    tmp += sizeof(int);
    // copia el numero de balas y avanza
    memcpy(&nBullets, tmp, sizeof(int));
    tmp += sizeof(int);
    // recorre vector de balas para copiar posicion x e y
    // de cada bala mientras avanza el puntero
    for (int i = 0; i < nBullets; i++)
    {
        Bullet *b = new Bullet(0, 0);
        // copia la X de la bala
        memcpy(&b->x, tmp, sizeof(int));
        tmp += sizeof(int);
        // copia la Y de la bala
        memcpy(&b->y, tmp, sizeof(int));
        // para que no avance posiciones el puntero en el ultimo elemento del vector
        if (i < nBullets - 1)
            tmp += sizeof(int);

        bullets.push_back(b);
    }

    return 0;
}