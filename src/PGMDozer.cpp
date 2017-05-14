#include "PGMDozer.hpp"
#include <string>

using namespace std;
using namespace Eigen;

// MAtrixxXi <- matrix de tamaño random, con eltos integer

void get_image(char* image_route, unsigned int ancho, unsigned int alto, RowVectorXf& imagen){
    //Abro el archivo de la imagen
    std::ifstream source(image_route, std::fstream::in);
    if (!source) {
        std::cerr << "ERROR: Hubo un problema al abrir la imagen" << std::endl;
    }

    /* DEBUG
    source.seekg(0, ios::end); // set the pointer to the end
    size_t bytes = source.tellg() ; // get the length of the file
    source.seekg(0, ios::beg); // set the pointer to the beginning
    */

    //La imagen ya fue abierta aca
    string filetype, comment, size, maxvalue;
    getline(source, filetype); // numero mágico: formato de archivo
    getline(source, comment); // commentario
    getline(source, size); // ancho alto
    getline(source, maxvalue); // maxValue

    /* DEBUG
    bytes = bytes - source.tellg() ;
    cout << "Bytes restantes para pixels: " << bytes << '\n';
    std::cout << ancho*alto << '\n';
    */

    // parseando pixels
    imagen.resize(1, ancho*alto);
    unsigned char pixel;
    for (size_t i = 0; i < ancho*alto; i++) {
        pixel = source.get(); // El .get() avanza de a un char (byte)
        imagen(0,i) = pixel;
    }

    //Cierro la imagen
    source.close();
}

void show_help(){
   
  std::string caras_art = "     ___          ___          ___          ___          ___     \n";
  caras_art += "    /  /\\        /  /\\        /  /\\        /  /\\        /  /\\    \n";
  caras_art += "   /  /:/       /  /::\\      /  /::\\      /  /::\\      /  /:/_   \n";
  caras_art += "  /  /:/       /  /:/\\:\\    /  /:/\\:\\    /  /:/\\:\\    /  /:/ /\\  \n";
  caras_art += " /  /:/  ___  /  /:/~/::\\  /  /:/~/:/   /  /:/~/::\\  /  /:/ /::\\ \n";
  caras_art += "/__/:/  /  /\\/__/:/ /:/\\:\\/__/:/ /:/___/__/:/ /:/\\:\\/__/:/ /:/\\:\\ \n";
  caras_art += "\\  \\:\\ /  /:/\\  \\:\\/:/__\\/\\  \\:\\/:::::/\\  \\:\\/:/__\\/\\  \\:\\/:/~/:/\n";
  caras_art += " \\  \\:\\  /:/  \\  \\::/      \\  \\::/~~~~  \\  \\::/      \\  \\::/ /:/ \n";
  caras_art += "  \\  \\:\\/:/    \\  \\:\\       \\  \\:\\       \\  \\:\\       \\__\\/ /:/  \n";
  caras_art += "   \\  \\::/      \\  \\:\\       \\  \\:\\       \\  \\:\\        /__/:/   \n";
  caras_art += "    \\__\\/        \\__\\/        \\__\\/        \\__\\/        \\__\\/    \n\n";
  std::string message = "Uso de este super reconocedor de caras (no la revista):\n";
  message += "\n";
  message += "./tp [ruta al archivo de config]\n";
  message += "\n";
  message += "Salu2\n";
  std::cout << termcolor::red << caras_art << termcolor::reset ;
  std::cout << termcolor::bold << termcolor::red << "\t\t\tRECOGNIZER\n\n" << termcolor:: reset << std::endl;
  std::cout << termcolor::blue << message << std::endl;
}

// TESING
/*
int main(int argc, char *argv[]){
    if (argc != 2) {
        std::cerr << "ERROR: Faltaron/sobraron argumentos" << std::endl;
    }
    char* file = argv[1];
    RowVectorXf imagen;
    get_image(file, 92, 112, imagen);
    return 0;
}
*/
