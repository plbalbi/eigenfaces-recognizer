#include <iostream>
#include <fstream>
#include "include/Eigen/Core"
#include "include/termcolor.hpp"

using namespace Eigen;

// Si bien los pixels son ints, hacemos que convierta la imagen
// en Matriz (en realidad vector) de floats porque después
// vamos a hacer cuentas que se van de enteros

// typedef Matrix< float , 1, Dynamic > Eigen::RowVectorXf
// typedef Matrix< double , 1, Dynamic > Eigen::RowVectorXd
void get_image(const char* image_route, unsigned int ancho, unsigned int alto, RowVectorXd& imagen,const char* error_msg);
void save_image(const char* image_route, unsigned int ancho, unsigned int alto, RowVectorXd& imagen);
void show_help();
