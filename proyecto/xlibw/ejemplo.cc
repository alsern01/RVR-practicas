#include "XLDisplay.h"
#include <unistd.h>

const int WIDTH = 800;
const int HEIGHT = 400;

void draw_forms(int x, int y)
{

    XLDisplay &dpy = XLDisplay::display();

    dpy.clear();

    dpy.set_color(XLDisplay::RED);

    dpy.point(50, 50);

    dpy.line(20, 20, 100, 50);

    dpy.set_color(XLDisplay::BROWN);

    dpy.circle(45, 45, 15);

    dpy.set_color(XLDisplay::BLUE);

    dpy.rectangle(60, 60, 30, 15);

    XPoint pts[] = {{100, 100}, {130, 130}, {100, 130}, {100, 100}};

    dpy.set_color(XLDisplay::YELLOW);

    dpy.lines(pts, 4);

    dpy.set_color(XLDisplay::GREEN);

    dpy.text(x, y, "HOLA MUNDO!");

    dpy.flush();
}

void pintaEscenario(int w, int h)
{
    XLDisplay &dpy = XLDisplay::display();
    dpy.set_color(XLDisplay::GREEN);
    dpy.rectangleFill(0, 0, w / 3, h);
    dpy.rectangleFill(w * 2 / 3, 0, w / 3, h);
    dpy.set_color(XLDisplay::BLACK);
    dpy.rectangleFill(w / 3, 0, w / 3, h);
}

void pintaPersonaje(int x, int y)
{
    XLDisplay &dpy = XLDisplay::display();
    dpy.set_color(XLDisplay::BLUE);
    dpy.rectangleFill(x, y, 50, 50);
}

void pintaBala(int x, int y)
{
    XLDisplay &dpy = XLDisplay::display();
    dpy.set_color(XLDisplay::BROWN);
    dpy.rectangleFill(x, y, 20, 20);
}

void render(int x, int y, bool bala, int xBala)
{
    XLDisplay &dpy = XLDisplay::display();

    dpy.clear();

    pintaEscenario(WIDTH, HEIGHT);
    pintaPersonaje(x, y);

    if (bala)
        pintaBala(xBala + 30, y);

    dpy.flush();
}

bool colisionAABB(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    return true;
}

void wait()
{
    int x = 100;
    int y = 200;
    int xBala = x;

    XLDisplay &dpy = XLDisplay::display();

    char k;
    bool bala = false;
    bool end = false;

    do
    {
        render(x, y, bala, xBala);
        k = dpy.wait_key();
        switch (k)
        {
        case 'w':
            y -= 5;
            break;
        case 'a':
            x -= 5;
            break;
        case 's':
            y += 5;
            break;
        case 'd':
            x += 5;
            break;
        case ' ':
            bala = true;
            break;
        case 'q':
            end = true;
            break;
        }

        if (bala)
            xBala += 20;

        if (xBala >= WIDTH)
        {
            bala = false;
            xBala = x;
        }

        if (x + 50 >= WIDTH / 3)
        {
            x = WIDTH / 3 - 50;
        }
        else if (x < 0)
        {
            x = 0;
        }
        if (y < 0)
        {
            y = 0;
        }
        else if (y + 50 >= HEIGHT)
        {
            y = HEIGHT - 50;
        }

    } while (!end);

    dpy.clear();

    dpy.flush();

    sleep(1);
}

int main()
{
    XLDisplay::init(WIDTH, HEIGHT, "Ejemplo");

    wait();

    return 0;
}
