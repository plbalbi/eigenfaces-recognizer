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
            s.push_back(img);
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

}


int main_test(int argc, char const *argv[]) {
    unsigned int img_ancho;
    unsigned int img_alto;
    unsigned int k;

    vector<sujeto> sujetos;
    vector<test> tests;
    read_input(argv[1], img_ancho, img_alto, k, sujetos, tests);

    std::cout << "Ancho imágenes: " << img_ancho << '\n';
    std::cout << "Alto imágenes: " << img_alto << '\n';
    std::cout << "# Componentes principales: " << k << '\n';

    for (size_t i = 0; i < sujetos.size(); i++) {
        std::cout << "Imágenes sujeto " << i << '\n';
        for (size_t j = 0; j < sujetos[i].size(); j++) {
            std::cout << sujetos[i][j] << '\n';
        }
    }

    std::cout << "Tests:" << '\n';
    for (size_t i = 0; i < tests.size(); i++) {
        std::cout << tests[i].path << " " << tests[i].respuesta << '\n';
    }
    return 0;
}
