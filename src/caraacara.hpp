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
void transfCaracteristica(MatrixXd& M_x, unsigned int k, unsigned int its, MatrixXd& V, vector<double> &autovalores);
int inline norma2(const RowVectorXd& v){ return sqrt(v.dot(v)); };

// -------------- separador de bajo presupuesto --------------
// Clasificacion
int kNN(const vector< vector< VectorXd> > &clase_de_sujetos, const VectorXd &v, int k);
// inline porque solo sería multiplicar un vector
// Definirla aca mismo
void inline reducirEspacio(RowVectorXd& imagen, MatrixXd& TL);

#endif
