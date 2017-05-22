#include "PGMDozer.hpp"
#include <string>

using namespace std;
using namespace Eigen;

// MAtrixxXi <- matrix de tamaño random, con eltos integer

void get_image(const char* image_route, unsigned int ancho, unsigned int alto, RowVectorXd& imagen,const char* error_msg){
    //Abro el archivo de la imagen
    std::ifstream source(image_route, std::fstream::in);
    if (!source) {
        errno = 1;
        std::cerr << "ERROR: Hubo un problema al abrir la imagen '" << error_msg << "'"<< std::endl;
        std::cerr << "ERROR CODE: " << strerror(errno) << std::endl;
        exit(errno);
    }
    /* DEBUG
    source.seekg(0, ios::end); // set the pointer to the end
    size_t bytes = source.tellg() ; // get the length of the file
    source.seekg(0, ios::beg); // set the pointer to the beginning
    */

    //La imagen ya fue abierta aca
    string version, comment, maxvalue;
    unsigned int ancho_r, alto_r;
    source >> version; // numero mágico: formato de archivo
    source >> std::ws; // salto espacios
    if (source.peek()=='#') getline(source, comment); // si hay commentario
    source >> ancho_r ; // ancho
    source >> std::ws; // salto espacios
    if (source.peek()=='#') getline(source, comment); // si hay commentario
    source >> alto_r ; // alto
    if (ancho_r != ancho || alto_r != alto) {
        std::cerr << "ERROR: la imagen '" << error_msg << "' no mide "<<ancho<<"x"<<alto << std::endl;
        exit(3);
    }
    source >> std::ws; // salto espacios
    if (source.peek()=='#') getline(source, comment); // si hay commentario
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
        imagen(i) = pixel;
    }
    //Cierro la imagen
    source.close();
}


void save_image(const char* image_route, unsigned int ancho, unsigned int alto, RowVectorXd& imagen){
    std::fstream stream(image_route, std::fstream::out);
    if (!stream) {
        std::cerr << "ERROR: Hubo un problema al guardar la imagen" << std::endl;
        std::cerr << "ERROR CODE: " << strerror(errno) << std::endl;
        exit(errno);
    }
    stream << "P5" << std::endl;
    stream << "# Hecha por el grupo de tp mas copado" << std::endl;
    stream << ancho << " " << alto << std::endl;
    stream << 255 << std::endl;
    for (int i = 0; i < ancho*alto; i++) {
        unsigned char temp = char(imagen(0,i));
        stream << temp;
    }
    stream.close();
}

void show_help(){

    std::string caras_art = "     ___          ___          ___          ___          ___           \n";
    caras_art += "    /  /\\        /  /\\        /  /\\        /  /\\        /  /\\                \n";
    caras_art += "   /  /:/       /  /::\\      /  /::\\      /  /::\\      /  /:/_                 \n";
    caras_art += "  /  /:/       /  /:/\\:\\    /  /:/\\:\\    /  /:/\\:\\    /  /:/ /\\            \n";
    caras_art += " /  /:/  ___  /  /:/~/::\\  /  /:/~/:/   /  /:/~/::\\  /  /:/ /::\\               \n";
    caras_art += "/__/:/  /  /\\/__/:/ /:/\\:\\/__/:/ /:/___/__/:/ /:/\\:\\/__/:/ /:/\\:\\          \n";
    caras_art += "\\  \\:\\ /  /:/\\  \\:\\/:/__\\/\\  \\:\\/:::::/\\  \\:\\/:/__\\/\\  \\:\\/:/~/:/\n";
    caras_art += " \\  \\:\\  /:/  \\  \\::/      \\  \\::/~~~~  \\  \\::/      \\  \\::/ /:/       \n";
    caras_art += "  \\  \\:\\/:/    \\  \\:\\       \\  \\:\\       \\  \\:\\       \\__\\/ /:/     \n";
    caras_art += "   \\  \\::/      \\  \\:\\       \\  \\:\\       \\  \\:\\        /__/:/         \n";
    caras_art += "    \\__\\/        \\__\\/        \\__\\/        \\__\\/        \\__\\/         \n\n";
    std::string message = "Uso de este super reconocedor de caras (no la revista):\n";
    message += "\n";
    message += "./tp <ruta-parámetros> <ruta-salida> <flags>\n";
    message += "\n";
    message += "El campo <flags> puede ser alguno/s de los siguientes: \n";
    message += "\t -c <ruta-imagen> (determinar si la imagen de la ruta es o no una cara) \n";
    message += "\t -v <ruta-salida> (guardar vectores de entrenamiento reducidos con PCA) \n";
    message += "\t -a  (guardar en sujetos/ las autocaras en .pgm) \n";
    message += "\t -i o --its  Número de iteraciones que hace método potencia \n";
    message += "Salu2\n";
    std::cout << termcolor::red << caras_art << termcolor::reset ;
    std::cout << termcolor::bold << termcolor::red << "\t\t\tRECOGNIZER\n\n" << termcolor:: reset << std::endl;
    std::cout << termcolor::yellow << message << std::endl;
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
