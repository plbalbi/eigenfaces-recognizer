#include "PGMDozer.hpp"
#include "parser.hpp"
#include <iostream>


int main(int argc, char const *argv[]) {
    
    if (argc != 2) {
      show_help();
      return 0;
    }

    char const* in_path;
    char const* out_path;
    unsigned int img_ancho;
    unsigned int img_alto;
    unsigned int k;
    vector<sujeto> sujetos;
    vector<test> tests;

    in_path = argv[1];
    out_path = argv[2];

    read_input(in_path, img_ancho, img_alto, k, sujetos, tests);

    return 0;
}
