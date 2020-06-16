#include "Game.h"

// ---------------------------------------------------------------------- //
// --- SERVER ----------------------------------------------------------- //
// ---------------------------------------------------------------------- //

void GameServer::init()
{
    std::cout << "INIT!" << std::endl;
}

void GameServer::update()
{
    std::cout << "UPDATE!" << std::endl;
}

// ---------------------------------------------------------------------- //
// --- CLIENT ----------------------------------------------------------- //
// ---------------------------------------------------------------------- //

void GameClient::init()
{
    XLDisplay::init(width, height, "MirrorShoot");
    x = 150;
    y = height / 2;
    w = h = 80;

    dpy.clear();
    render(dpy);
}

void GameClient::update()
{
    // Coger input
    handleInput(dpy);

    // Actualizar elementos
    checkBounds();

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
    if (shoot)
    {
        dpy.set_color(XLDisplay::BLACK);
        dpy.rectangleFill(bulX, bulX, bulW, bulH);
    }
}

void GameClient::handleInput(XLDisplay &dpy)
{
    char k = dpy.wait_key();
    switch (k)
    {
    case 'w':
        y -= 10;
        break;

    case 'a':
        x -= 10;
        break;

    case 's':
        y += 10;
        break;

    case 'd':
        x += 10;
        break;

    case ' ':
        shoot = true;
        break;

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

// ---------------------------------------------------------------------- //
// --- MESSAGE ---------------------------------------------------------- //
// ---------------------------------------------------------------------- //

void Message::to_bin()
{
    alloc_data(MESSAGE_SIZE);
    memset(_data, 0, MESSAGE_SIZE);
    char *tmp = _data;
}

int Message::from_bin(char *bobj)
{
    alloc_data(MESSAGE_SIZE);
    char *tmp = bobj;

    return 0;
}