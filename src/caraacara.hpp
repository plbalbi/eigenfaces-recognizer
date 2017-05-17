#ifndef CARAACARA_H
#define CARAACARA_H

#include "PGMDozer.hpp"
#include "parser.hpp"
#include <iostream>
#include <math.h>
#include <iomanip>
//Definiendo includes

//MatrixXf X(img_por_sujeto*sujetos.size(), img_alto*img_ancho);
//RowVectorXf media(1, img_alto*img_ancho);

// -------------- separador de bajo presupuesto --------------
// Reduccion de espacio
float metodoPotencia(const MatrixXf& B, VectorXf& v, int iteraciones);
void deflacionar(MatrixXf& B, VectorXf& v, float lambda);
void transfCaracteristica(MatrixXf& M_x, unsigned int k, unsigned int its, MatrixXf& V, vector<float> &autovalores);
int inline norma2(const RowVectorXf& v){ return sqrt(v.dot(v)); };

// -------------- separador de bajo presupuesto --------------
// Clasificacion
int kNN(const vector< vector< VectorXf> > &clase_de_sujetos, const VectorXf &v, int k);
// inline porque solo sería multiplicar un vector
// Definirla aca mismo
void inline reducirEspacio(RowVectorXf& imagen, MatrixXf& TL);

#endif
