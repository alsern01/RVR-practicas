#include "XLDisplay.h"

#include "Serializable.h"
#include "Socket.h"

class GameServer
{
private:
  Socket server;
  Socket *p1_, *p2_;
  bool gameover;

public:
  GameServer(Socket sock, Socket *player_ONE, Socket *player_TWO)
      : server(sock), p1_(player_ONE), p2_(player_TWO), gameover(false)
  {
    init();
  };

  ~GameServer(){};

  void init();
  void update();

  bool gameOver() { return gameover; };
};

// atributos balas
struct Bullet
{
  int x, y, w, h;
  Bullet(int x_, int y_) : x(x_), y(y_), w(20), h(20)
  {
  }
  void update()
  {
    x += 10;
  }
};

class GameClient
{
private:
  // cosas render
  XLDisplay &dpy;

  // Servidor
  Socket server;

  // Alto y ancho de la pestaña
  int width, height;

  // Booleano de fin del juego
  bool gameover;
  bool hit = false;

  // atributos player
  int x, y;
  int w, h;

  // vector balas
  std::vector<Bullet *> bullets;
  // vector balas enemigas
  std::vector<Bullet *> bulletsEnem;

  // atributos enemigo
  int enemX, enemY;

  void drawBullets(XLDisplay &dpy);
  void checkBounds();
  void checkCollision();

public:
  GameClient(Socket sock, int w, int h)
      : server(sock), width(w), height(h), gameover(false), dpy(XLDisplay::display())
  {
    init();
  };

  ~GameClient(){};

  void init();
  void render(XLDisplay &dpy);
  void handleInput(XLDisplay &dpy);

  void update();

  // Getters y Setters
  bool gameOver() { return gameover; };
};

class Message : public Serializable
{
private:
  // info juego
  int gameOver, nBullets;
  // info jugador
  int posX, posY;
  std::vector<Bullet *> bullets;

public:
  // Tamaño del mensaje
  static const size_t MESSAGE_SIZE = 9 * sizeof(int);

  Message(){};
  Message(const int &gameOv, const int &x, const int &y, const std::vector<Bullet *> &b)
      : gameOver(gameOv), posX(x), posY(y), bullets(b){};
  ~Message(){};

  void to_bin();

  int from_bin(char *bobj);

  int isGameOver() { return gameOver; };
};