#include "XLDisplay.h"

#include "Serializable.h"
#include "Socket.h"
#include <time.h>

typedef struct timespec timespec;
const int FRAME_RATE = 60;

class GameServer
{
private:
  Socket server;
  std::vector<Socket *> clients;
  bool gameover;

public:
  GameServer(Socket sock)
      : server(sock), gameover(false)
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
    x += 20;
  }
};

class GameClient
{
private:
  // cosas render
  XLDisplay &dpy;

  // Cliente
  Socket sock_;

  // Alto y ancho de la pestaña
  int width, height;

  // Booleano de fin del juego
  bool gameover;
  bool waiting = false;
  // Booleano cuando impacta una bala en jugador
  bool hit = false;
  // Booleano cuando impacta una bala en enemigo
  bool enemyHit = false;

  // atributos player
  int x, y; //  posicion
  int w, h; // ancho-alto

  // vector balas
  std::vector<Bullet *> bullets;
  // vector balas enemigas
  std::vector<Bullet *> bulletsEnem;

  // atributos enemigo
  int enemX, enemY;

  // frame rate
  timespec startFrame;
  timespec endFrame;
  unsigned long targetFrame;

  // Metodos del juego
  void init();
  void render(XLDisplay &dpy);
  void handleInput(XLDisplay &dpy);
  void drawBullets(XLDisplay &dpy);
  void checkBounds();
  void checkCollision();

public:
  GameClient(Socket sock, int w, int h)
      : sock_(sock), width(w), height(h), gameover(false), dpy(XLDisplay::display())
  {
    init();
  };

  ~GameClient(){};

  void update();

  // Gestiona los mensajes recibidos del servidor
  void manageMessage();
  // Manda mensaje al servidor
  void sendMessage(int type_);
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
  static const size_t MESSAGE_SIZE = 20 * sizeof(int);

  // Tipo de mensaje
  int type;
  enum MessageType
  {
    LOGIN = 0,
    GAME = 1,
    LOGOUT = 2,
    COLLISION = 3,
    WAITING = 4,
    START = 5
  };

  Message(){};
  Message(const int &gameOv, const int &x, const int &y, const std::vector<Bullet *> &b)
      : gameOver(gameOv), posX(x), posY(y), bullets(b){};
  ~Message(){};

  void to_bin();

  int from_bin(char *bobj);

  int isGameOver() { return gameOver; };
  int getX() { return posX; };
  int getY() { return posY; };
  std::vector<Bullet *> getBullets() { return bullets; };
};