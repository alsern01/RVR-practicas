#include "Serializable.h"

#include <iostream>
#include <fstream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

static const size_t MAX_LENGTH = 80;

class Jugador : public Serializable
{

  private:
    char name[MAX_LENGTH];

    int16_t x;
    int16_t y;

  public:
    Jugador(const char *_n, int16_t _x, int16_t _y) : x(_x), y(_y)
    {
        strncpy(name, _n, MAX_LENGTH);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        // Calcula el tamano del string
        int32_t totalSize = MAX_LENGTH * sizeof(char) + 2 * sizeof(int16_t);

        // Reservar memoria del tamano del string
        alloc_data(totalSize);

        // Copia en un puntero el contenido de name
        char *tmp = _data;
        memcpy(tmp, name, MAX_LENGTH);

        // Desplazar el puntero tmp en 80 posiciones
        tmp += MAX_LENGTH * sizeof(char);
        // Copiar la posicionX y posicionY
        memcpy(tmp, &x, sizeof(int16_t)); // tmp = posicion 80
        tmp += sizeof(int16_t);           // Mueve 2 bytes la posicion
        memcpy(tmp, &y, sizeof(int16_t)); // tmp = 80+2
    }

    int from_bin(char *data)
    {
        // Apunta al primer dato
        char *tmp = data;
        // Copia los primeros MAX_LENGTH caracteres en name
        memcpy(name, tmp, MAX_LENGTH);
        // Mueve el puntero MAX_LENGTH posiciones
        tmp += MAX_LENGTH * sizeof(char);
        // Copia en x el valor de la posicion del puntero
        memcpy(&x, tmp, sizeof(int16_t));
        // Mueve una posicion el puntero y lo mismo con y
        tmp += sizeof(int16_t);
        memcpy(&y, tmp, sizeof(int16_t));

        //Control de errores al copiar datos
        if (strlen(name) < 0)
        {
            return -1;
        }

        return strlen(name);
    }

    int16_t getX() { return x; };
    int16_t getY() { return y; };
    char *getName() { return name; };
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);               // leer los datos de archivo
    Jugador one_w("Player_ONE", 123, 987); // escribir los datos en archivo

    // Serializa los datos
    one_w.to_bin();

    // Abre el archivo file
    int wFile = open("file", O_CREAT | O_RDWR, 0666);

    // Escribe en el archivo los datos de one_w
    int err_w = write(wFile, one_w.data(), one_w.size());
    // Comprueba que la escritura sea correcta
    if (err_w == -1)
    {
        std::cout << "Error write: " << strerror(errno) << std::endl;
        return -1;
    }

    // cierra el archivo de escritura
    close(wFile);

    // Crea un buffer para guardar la info del archivo
    int size = one_w.size();
    char data[size];

    // Abre archivo de lectura
    int rFile = open("file", O_CREAT | O_RDWR, 0666);

    // Lee el archivo
    int err_r = read(rFile, data, size);
    // Copia los datos en el buffer
    if (err_r == -1)
    {
        std::cerr << "Error read: " << strerror(errno) << std::endl;
        return -1;
    }

    // Escribir los datos del buffer en one_r
    int dat = one_r.from_bin(data);
    if (dat < 0)
    {
        std::cout << "Error data: " << std::endl;
        return -1;
    }

    std::cout << one_r.getName() << " " << one_r.getX() << " " << one_r.getY() << std::endl;

    // Cierra el archivo
    close(rFile);

    return 0;
}

/*
El comando od -sa nombrearchivo muestra en la consola el contenido de un archivo binario
aparece en cada byte del archivo el contenido y el valor que tiene ese byte
*/
