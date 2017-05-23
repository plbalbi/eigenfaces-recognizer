#ifndef CARAACARA_H
#define CARAACARA_H

#include "PGMDozer.hpp"
#include "parser.hpp"
#include <iostream>
#include <math.h>
#include <iomanip>

//Definiendo includes

//MatrixXd X(img_por_sujeto*sujetos.size(), img_alto*img_ancho);
//RowVectorXd media(1, img_alto*img_ancho);

// -------------- separador de bajo presupuesto --------------
// Reduccion de espacio

double metodoPotencia(const MatrixXd& B, VectorXd& v, int iteraciones);
void deflacionar(MatrixXd& B, VectorXd& v, double lambda);

void cargar_datos(unsigned int img_alto, unsigned int img_ancho, vector<sujeto> sujetos, MatrixXd &X, RowVectorXd &media);

void transfCaracteristica_v1(MatrixXd& X, unsigned int k, unsigned int its, MatrixXd& V, vector<double> &autovalores);
void transfCaracteristica_v2(MatrixXd& X, unsigned int k, unsigned int its, MatrixXd& V, vector<double> &autovalores);
double inline distancia(const VectorXd &a, const VectorXd &b){VectorXd temp = a - b; return temp.norm();};
// -------------- separador de bajo presupuesto --------------
// Clasificacion

int kNN(const vector< vector< VectorXd> > &clase_de_sujetos, const VectorXd &v, int k);
int fast_knn(const std::vector< std::vector<VectorXd>  > &clase_de_sujetos, const VectorXd &v, int k);
int weighted_knn(const std::vector< std::vector<VectorXd>  > &clase_de_sujetos, const VectorXd &v, int k);
int fast_knn(const std::vector< std::vector<VectorXd>  > &clase_de_sujetos, const VectorXd &v, int k);

// -------------- separador de bajo presupuesto --------------

// Es Cara o No?
double train_recognizer(const MatrixXd& V, const std::vector< std::vector<VectorXd>  > &clase_de_sujetos);
bool recognize(const MatrixXd &V, const double& umbral, VectorXd& target);

vector<double> componentes_menos_principales(const MatrixXd& X, unsigned int its, VectorXd &v);

bool esCara_v1(const MatrixXd& X, unsigned int its, unsigned int img_alto, unsigned int img_ancho, const char* img_path, RowVectorXd const &media);

#endif
