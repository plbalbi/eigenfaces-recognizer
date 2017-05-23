#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>

using namespace std;

// este struct indica los flags de la ejecución.
// si el parámetro es un puntero a NULL, no se activó ese flag.
// si el parámetro no es un puntero a null, se activó ese flag
// y de ser necesario se puede leer algo de ese char* (como un path)
struct flags_t{
    const char * caraOno; // -c
    const char * vecReducidos; // -v
    const char * autocaras; // -a
    int its = -1; // -i | --its
    const char * matriz; // -m
    const char * tiempo; // -t
    bool silent = false; // -s
    int vecinos = 5; // -k
};

// Tipos
struct test{
    // path a la imagen de la cara a identificar
    string path;

    // nro de sujeto al que pertenece la cara
    int respuesta;

    // respuestas que dio cada método
    int vecino_mas_cercano;
    int knn;
    int weighted_knn;

};

// un sujeto es una lista de las imágenes (paths) que lo identifican
typedef vector<string> sujeto ;


// Declaraciones
void read_input(const char* input_path, unsigned int &img_ancho, unsigned int &img_alto,
                unsigned int &k, vector<sujeto> &sujetos, vector<test> &tests);

flags_t get_flags(int argc, char const *argv[]);
