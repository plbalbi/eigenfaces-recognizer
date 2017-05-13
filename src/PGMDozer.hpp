#include <iostream>
#include <fstream>
#include "include/Eigen/Core"

using Eigen::MatrixXi;

// MAtrixxXi <- matrix de tamaño random, con eltos integer

MatrixXi& get_image(char* image_route){
  //Abro el archivo de la imagen
  std::ifstream source(image_route, std::fstream::in);
  if (!source) {
    std::cerr << "ERROR: Hubo un problema al abrir la imagen" << std::endl;
  }
  //La imagen ya fue abierta aca
  char* filetype;
  source.getline(filetype, '\n');
  std::cout << filetype << std::endl;
  //Cierro la imagen
  source.close();
}


// TESING
int main(int argc, char *argv[])
{
  if (argc != 2) {
    std::cerr << "ERROR: Faltaron/sobraron argumentos" << std::endl;
  }
  char* file = argv[2];
  get_image(file);
  return 0;
}

