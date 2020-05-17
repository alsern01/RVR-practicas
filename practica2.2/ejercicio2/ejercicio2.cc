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

        // Desplazar el puntero tmp en 20 posiciones

        tmp += 20 * sizeof(char);
        // Copiar la posicionX y posicionY
        memcpy(tmp, &x, sizeof(int16_t)); // tmp = posicion 20
        tmp += sizeof(int16_t);           // Mueve 2 bytes la posicion
        memcpy(tmp, &y, sizeof(int16_t)); // tmp = 20+2
    }

    int from_bin(char *data)
    {
        //
        return 0;
    }
};

int main(int argc, char **argv)
{
    Jugador one("Player_ONE", 123, 987);

    // Serializa los datos
    one.to_bin();
    // Abre el archivo file
    int o = open("file", O_CREAT | O_RDWR, 0666);
    // Escribe en el archivo los datos de one
    int err = write(o, one.data(), one.size());
    // Comprueba que la escritura sea correcta
    if (err == -1)
    {
        std::cout << "Error write: " << strerror(errno) << std::endl;
        return -1;
    }
    // Cierra el archivo
    close(o);

    return 0;
}

/*
El comando od -sa nombrearchivo muestra en la consola el contenido de un archivo binario
aparece en cada byte del archivo el contenido y el valor que tiene ese byte
*/
