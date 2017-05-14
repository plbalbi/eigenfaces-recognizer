#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

// Tipos
struct test{
    // path a la imagen de la cara a identificar
    string path;

    // nro de sujeto al que pertenece la cara
    int respuesta;
};

// un sujeto es una lista de las imágenes (paths) que lo identifican
typedef vector<string> sujeto ;


// Declaraciones
void read_input(char* input_path, unsigned int &img_ancho, unsigned int &img_alto,
                unsigned int &k, vector<sujeto> &sujetos, vector<test> &tests);
