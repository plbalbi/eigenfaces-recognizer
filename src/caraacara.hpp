#ifndef CARAACARA_H
#define CARAACARA_H

#include "PGMDozer.hpp"
#include "parser.hpp"
#include <iostream>
#include <math.h>
//Definiendo includes

//MatrixXf X(img_por_sujeto*sujetos.size(), img_alto*img_ancho);
//RowVectorXf media(1, img_alto*img_ancho);

// -------------- separador de bajo presupuesto --------------
// Reduccion de espacio
int metodoPotencia(const MatrixXf& B, RowVectorXf& v, int iteraciones);
void deflacionar(MatrixXf& B, RowVectorXf& v, int lambda);
void transfCaracteristica(MatrixXf& M_x, unsigned int k, unsigned int its, MatrixXf& Vt);
int inline norma2(const RowVectorXf& v){ return sqrt(v.dot(v)); };

// -------------- separador de bajo presupuesto --------------
// Clasificacion
int kNN(const MatrixXf& X, const RowVectorXf& v, int img_por_sujeto, int k);
// inline porque solo sería multiplicar un vector
// Definirla aca mismo
void inline reducirEspacio(RowVectorXf& imagen, MatrixXf& TL);

#endif
