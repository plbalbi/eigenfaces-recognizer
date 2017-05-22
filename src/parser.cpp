#include "parser.hpp"

using namespace std;


flags_t get_flags(int argc, char const *argv[]){
    flags_t res;
    // setear todos en NULL antes de empezar;
    res.caraOno = NULL;
    res.vecReducidos = NULL;
    res.autocaras = NULL;
    res.matriz = NULL;
    res.tiempo = NULL;

    for (int i = 0; i < argc; i++) {
        const char* param = argv[i];

        if (strcmp(param, "-c") == 0) {
            res.caraOno = argv[i+1];
        }else if(strcmp(param, "-v") == 0){
            res.vecReducidos = argv[i+1];
        }else if(strcmp(param, "-a") == 0){
            res.autocaras = "Blah"; // no requiere nada
        }else if (strcmp(param, "--its") == 0 || strcmp(param, "-i") == 0) {
            res.its = stoi(argv[i+1]);
        }else if (strcmp(param, "-m") == 0){
            res.matriz = "Tu javie";
        }else if (strcmp(param, "-t") == 0){
            res.tiempo = "tiempo.dat";
        }else if (strcmp(param, "-s") == 0){
            res.silent = true;
        }
    }

    return res;
}


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
    int cant_tests;
    input >> cant_tests;
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
