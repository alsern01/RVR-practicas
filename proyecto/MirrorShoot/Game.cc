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
    std::cout << "UPDATE!" << std::endl;
    Socket *sock;
    Message *msg = new Message();

    // recibe de jugador 1 y manda a jugador 2
    server.recv(*msg, sock);
    server.send(*msg, *p2_);

    // recibe de jugador 2 y manda jugador 1
    server.recv(*msg, sock);
    server.send(*msg, *p1_);

    if (msg->isGameOver())
        gameover = true;
}

// ---------------------------------------------------------------------- //
// --- CLIENT ----------------------------------------------------------- //
// ---------------------------------------------------------------------- //

void GameClient::init()
{
    sock_.bind();
    XLDisplay::init(width, height, "MirrorShoot");
    x = 150;
    y = height / 2;
    enemX = x * 2 + width / 3;
    enemY = y;
    w = h = 80;

    dpy.clear();
    render(dpy);
}

void GameClient::update()
{
    // Recibe info del server

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

// ---------------------------------------------------------------------- //
// --- MESSAGE ---------------------------------------------------------- //
// ---------------------------------------------------------------------- //

void Message::to_bin()
{
    alloc_data(MESSAGE_SIZE);
    memset(_data, 0, MESSAGE_SIZE);
    char *tmp = _data;

    // copia info de fin del juego y avanza puntero
    memcpy(tmp, &gameOver, sizeof(uint8_t));
    tmp += sizeof(uint8_t);
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

    // copia info de fin del juego y avanza puntero
    memcpy(&gameOver, tmp, sizeof(uint8_t));
    tmp += sizeof(uint8_t);
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