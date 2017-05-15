#ifndef CARAACARA_H
#define CARAACARA_H

#include "PGMDozer.hpp"
#include "parser.hpp"
#include <iostream>
//Definiendo includes

//MatrixXf X(img_por_sujeto*sujetos.size(), img_alto*img_ancho);
//RowVectorXf media(1, img_alto*img_ancho);

// -------------- separador de bajo presupuesto --------------
// Reduccion de espacio
int metodoPotencia(MatrixXf& B, RowVectorXf& v, int iteraciones);
void deflacionar(MatrixXf& B, RowVectorXf& v, int lambda);
void transfCaracteristica(MatrixXf& M_x, MatrixXf& Vt);

// -------------- separador de bajo presupuesto --------------
// Clasificacion
int kNN();
// inline porque solo sería multiplicar un vector
void inline reducirEspacio(RowVectorXf& imagen, MatrixXf& TL);

#endif
