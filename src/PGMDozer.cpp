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
  //La imagen ya fue abierta aca
  string filetype, comment, size, maxvalue;
  getline(source, filetype); // numero mágico: formato de archivo
  getline(source, comment); // commentario
  getline(source, size); // ancho alto
  getline(source, maxvalue); // maxValue

  // parseando pixels
  imagen.resize(1, ancho*alto);
  unsigned char pixel;
  unsigned int i(0);
  for (size_t i = 0; i < ancho*alto; i++) {
      source >> pixel;
      imagen(i) = pixel;
  }
  //Cierro la imagen
  source.close();
}


// TESING
int main(int argc, char *argv[])
{
  if (argc != 2) {
    std::cerr << "ERROR: Faltaron/sobraron argumentos" << std::endl;
  }
  char* file = argv[1];
  RowVectorXf imagen;
  get_image(file, 92, 112, imagen);
  return 0;
}
