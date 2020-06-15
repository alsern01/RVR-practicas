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
    dpy.clear();
    render(dpy);
}

void GameClient::update()
{
    dpy.clear();
    render(dpy);

    dpy.set_color(XLDisplay::BLUE);
    dpy.text(150, 150, "JUGANDO");
    dpy.flush();
}

void GameClient::render(XLDisplay &dpy)
{
    // Dibujar el fondo del tablero
    dpy.set_color(XLDisplay::GREEN);
    dpy.rectangleFill(0, 0, 265, 200);
    dpy.set_color(XLDisplay::GREEN);
    dpy.rectangleFill(530, 0, 265, 200);

    // Dibujar al jugador
    dpy.set_color(XLDisplay::WHITE);
    dpy.rectangleFill(x, y, 20, 20);

    // Dibujar al malo
    dpy.set_color(XLDisplay::RED);
    dpy.rectangleFill(enemX, enemY, 20, 20);

    // Dibujar balas
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