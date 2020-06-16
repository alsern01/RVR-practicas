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

class GameClient
{
private:
  XLDisplay &dpy;
  // Servidor
  Socket server;
  // Alto y ancho de la pestaña
  int width, height;
  // Booleano de fin del juego
  bool gameover;
  // atributos player
  int x, y, w, h;
  // atributos bala
  int bulX, bulY, bulW, bulH;
  bool shoot = false;
  // atributos enemigo
  int enemX, enemY;

  void checkBounds();

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
  int gameOver;

public:
  // Tamaño del mensaje
  static const size_t MESSAGE_SIZE = 9 * sizeof(int);

  Message(){};
  ~Message(){};

  void to_bin();

  int from_bin(char *bobj);

  int isGameOver() { return gameOver; };
};