#include "parser.hpp"

using namespace std;

void read_input(const char* input_path, unsigned int &img_ancho, unsigned int &img_alto, unsigned int &k, vector<sujeto> &sujetos, vector<test> &tests){

    ifstream input;
    input.open(input_path, std::ifstream::in);

    string base;
    unsigned int n; // cantidad de sujetos
    unsigned int m; // cantidad de imágenes por sujeto

    input >> base;
    input >> img_alto;
    input >> img_ancho;
    input >> n;
    input >> m;
    input >> k;

    // empiezan las líneas de las imágenes de los sujetos
    for (size_t i = 0; i < n; i++) {
        sujeto s;
        string dir;
        input >> dir;
        dir = base + dir;
        for (size_t j = 0; j < m; j++) {
            string img;
            input >> img;
            img = dir + img + ".pgm";
            const char* gola = img.c_str();
            s.push_back(gola);
        }
        sujetos.push_back(s);
    }

    // empiezan las líneas de los tests
    string path;
    while (input >> path) {
        int res;
        input >> res;
        test un_test;
        un_test.path = path;
        un_test.respuesta = res;
        tests.push_back(un_test);
    }

    input.close();
}
