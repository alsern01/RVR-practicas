#include "Game.h"

// ---------------------------------------------------------------------- //
// --- SERVER ----------------------------------------------------------- //
// ---------------------------------------------------------------------- //

void GameServer::init()
{
    std::cout << "INIT!" << std::endl;
    server.bind();
}

void GameServer::update()
{
    while (!gameover)
    {
        std::cout << "UPDATE!" << std::endl;
        Socket *sock;
        Message *msg = new Message();

        server.recv(*msg, sock);

        switch (msg->type)
        {
        case Message::LOGIN:
        {
            if (clients.size() < 2)
                clients.push_back(sock);
        }
        break;
        case Message::GAME:
        {
            for (int i = 0; i < clients.size(); ++i)
            {
                if (!(*sock == *clients.at(i)))
                    server.send(*msg, *clients.at(i));
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
                    clients.erase(it);
                    erased = true;
                }
                ++it;
            }
        }
        break;

        default:
            break;
        }

        if (msg->isGameOver())
            gameover = true;
    }
}

// ---------------------------------------------------------------------- //
// --- CLIENT ----------------------------------------------------------- //
// ---------------------------------------------------------------------- //

void GameClient::init()
{
    XLDisplay::init(width, height, "MirrorShoot");
    x = 150;
    y = height / 2;
    enemX = x * 2 + width / 3;
    enemY = y;
    w = h = 80;

    dpy.clear();
    render(dpy);

    Message m(gameover, x, y, bullets);
    m.type = Message::LOGIN;
    sock_.send(m, sock_);
}

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
            if (b->x < width)
                b->update();
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

    // Dibujar al jugador
    dpy.set_color(XLDisplay::WHITE);
    dpy.rectangleFill(x, y, w, h);

    // Dibujar al malo
    dpy.set_color(XLDisplay::RED);
    dpy.rectangleFill(enemX, enemY, w, h);

    // Dibujar balas
    drawBullets(dpy);
}

void GameClient::drawBullets(XLDisplay &dpy)
{
    dpy.set_color(XLDisplay::BLACK);
    for (auto b : bullets)
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
        if (!hit)
            y -= 10;
        break;

    case 'a':
        if (!hit)
            x -= 10;
        break;

    case 's':
        if (!hit)
            y += 10;
        break;

    case 'd':
        if (!hit)
            x += 10;
        break;

    case ' ':
    {
        if (!hit)
        {
            Bullet *b = new Bullet(x + w, y + (h / 2));
            bullets.push_back(b);
        }
        break;
    }

    case 'q':
        disconnect();
        gameover = true;
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
            std::cout << "COLISION \n";
            hit = true;
        }
    }
    // bullets del malo con jugador
    for (auto b : bulletsEnem)
    {
        if (b->x < x + w &&
            b->x + b->w > x &&
            b->y < y + h &&
            b->h + b->y > y)
            std::cout << "COLISION \n";
    }
}

void GameClient::disconnect()
{
    Message msg(gameover, x, y, bullets);
    msg.type = Message::LOGOUT;
    sock_.send(msg, sock_);
}

void GameClient::manageMessage()
{
    while (!gameover)
    {
        Message msg;
        sock_.recv(msg);
        if (msg.type == Message::GAME)
        {
            enemX = msg.getX() * 2 + width / 3;
            enemY = msg.getY();
            bulletsEnem = msg.getBullets();
            for (auto b : bullets)
            {
                b->x = width - x;
            }
        }
    }
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
        // para que no avance posiciones el puntero en el ultimo elemento del vector
        if (i < nBullets - 1)
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
        // copia la X de la bala
        memcpy(&bullets[i]->x, tmp, sizeof(int));
        // para que no avance posiciones el puntero en el ultimo elemento del vector
        if (i < nBullets - 1)
            tmp += sizeof(int);
        // copia la Y de la bala
        memcpy(&bullets[i]->y, tmp, sizeof(int));
        // para que no avance posiciones el puntero en el ultimo elemento del vector
        if (i < nBullets - 1)
            tmp += sizeof(int);
    }

    return 0;
}