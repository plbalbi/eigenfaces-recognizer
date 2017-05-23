#include "caraacara.hpp"
#include "tests/unitTesting.hpp"
#include <vector>
#include <cfloat>
#include "include/Eigen/Core"
#include "include/Eigen/LU"

// -------------- separador de bajo presupuesto --------------
// Clasificacion

int kNN(const vector< vector< VectorXd> > &clase_de_sujetos, const VectorXd &v, int k){
    // X es la matriz cuyas filas son las muestras de entrenamiento
    // Hay 'cant_sujetos' clases, y para cada una, 'img_por_sujeto' muestras
    // A los elementos X[s*img_por_sujeto..s*img_por_sujeto+img_por_sujeto)
    // los vamos a asignar a la clase 'i'. Queremos ver a que clase pertenece v

    int img_por_sujeto = clase_de_sujetos[0].size();

    // Calculo distancias
    int cant_muestras = clase_de_sujetos.size() * img_por_sujeto;
    vector<double> distancias(cant_muestras);
    int indice = 0;
    for (size_t i = 0; i < clase_de_sujetos.size(); i++) {
        for (size_t j = 0; j < clase_de_sujetos[i].size(); j++) {
            VectorXd temp = clase_de_sujetos[i][j] - v;
            distancias[indice] = temp.norm();
            indice++;
        }
    }

    // Me guardo los índices de los k vecinos más cercanos
    // TODO si hubiera distancias iguales no se comporta como debería esta parte
    vector<int> indices(k);
    vector<double> minimos(k, DBL_MAX);
    for (int i = 0; i < k; i++) { // k vecinos
        for (int j = 0; j < cant_muestras; j++) { // busco sobre las muestras
            if (i == 0 || distancias[j] > minimos[i-1]) { // para buscar el i-esimo mínimo
                if (minimos[i] > distancias[j]) {
                    minimos[i] = distancias[j];
                    indices[i] = j;
                }
            }
        }
    }

    // Transformo los índices de los vecinos en las clases de los mismos
    vector<int> clases(k);
    for (int i = 0; i < k; i++) {
        clases[i] = indices[i] % img_por_sujeto;
    }

    // Cuento qué clase tiene más votos
    int max_count = 0;
    int max_clase = 0;
    for (int i = 0; i < k; i++) {
        int clase = clases[i];
        int count = 0; // voy a contar cuantos 'clase' hay en 'clases'
        for (int j = 0; j < k; j++) {
            if (clase == clases[j]) count++;
        }
        if (count > max_count) {
            max_clase = clase;
            max_count = count;
        }
    }

    return max_clase +1;
}

int fast_knn(const std::vector< std::vector<VectorXd>  > &clase_de_sujetos, const VectorXd &v, int k){
    std::vector< std::pair<int, double> > distances;
    for (size_t s = 0; s < clase_de_sujetos.size(); s++) {
       for (size_t i = 0; i < clase_de_sujetos[0].size(); i++) {
           distances.push_back(std::make_pair(s+1, distancia(clase_de_sujetos[s][i], v)));
       }
    }
    std::sort(distances.begin(), distances.end(), [](std::pair<int, double> a, std::pair<int, double> b){ return a.second < b.second; });
    std::vector<int> counts(clase_de_sujetos.size() + 1, 0);
    for (int i = 0; i < k; i++) {
        counts[distances[i].first]++;
    }
    int max_clase;
    int max_qty = 0;
    for (size_t i = 0; i < counts.size(); i++) {
        if (counts[i] > max_qty) {
            max_qty = counts[i];
            max_clase = i;
        }
    }
    return max_clase;
}

int weighted_knn(const std::vector< std::vector<VectorXd>  > &clase_de_sujetos, const VectorXd &v, int k){
    std::vector< std::pair<int, double> > distances;
    for (size_t s = 0; s < clase_de_sujetos.size(); s++) {
       for (size_t i = 0; i < clase_de_sujetos[0].size(); i++) {
           distances.push_back(std::make_pair(s+1, distancia(clase_de_sujetos[s][i], v)));
       }
    }
    std::sort(distances.begin(), distances.end(), [](std::pair<int, double> a, std::pair<int, double> b){ return a.second < b.second; });
    double max_k = distances[k-1].second; // Distancia más lejana de los k más cercanos
    std::vector<double> counts(clase_de_sujetos.size() + 1, 0);
    for (int i = 0; i < k; i++) {
        counts[distances[i].first] += (max_k - distances[i].second);
    }
    int max_clase;
    double max_qty = 0;
    for (size_t i = 0; i < counts.size(); i++) {
        if (counts[i] > max_qty) {
            max_qty = counts[i];
            max_clase = i;
        }
    }
    return max_clase;
}

// -------------- separador de bajo presupuesto --------------
// Reduccion de espacio

double metodoPotencia(const MatrixXd& B, VectorXd& v, int iteraciones){
    for (int i = 0; i < iteraciones; i++) {
        v = B*v;
        v *= (1/v.norm());
    }
    RowVectorXd vt = v.transpose();
    double lambda = vt*(B*v);
    lambda = lambda/(vt*v);
    return lambda;
}

void deflacionar(MatrixXd& B, VectorXd& v, double lambda){
    RowVectorXd vt = v.transpose();
    B = B - lambda*v*vt;
}

void cargar_datos(unsigned int img_alto, unsigned int img_ancho, vector<sujeto> sujetos, MatrixXd &X, RowVectorXd &media){
    unsigned int img_por_sujeto = sujetos[0].size();
    X.resize(img_por_sujeto*sujetos.size(), img_alto*img_ancho);
    media.resize(img_alto*img_ancho);
    media.setZero();
    for (size_t s = 0; s < sujetos.size(); s++) {
        for (size_t i = 0; i < img_por_sujeto; i++) {
            RowVectorXd x_i;
            const char* ruta = sujetos[s][i].c_str();
            get_image(ruta,img_ancho,img_alto,x_i, ruta);
            media = media + x_i;
            X.row(s*sujetos[s].size()+i) = x_i;
        }
    }
    media = media / (double)(img_por_sujeto*sujetos.size());
    // Ahora a cada fila le resto la media
    for (size_t i = 0; i < sujetos.size()*img_por_sujeto; i++) {
        X.row(i) -= media;
    }
}

void transfCaracteristica_v1(MatrixXd& X, unsigned int k, unsigned int its, MatrixXd& V, vector<double> &autovalores){

    // Calculo la matriz de covarianza
    MatrixXd M_x = X.transpose()*X;
    M_x /= (double)(X.rows()-1);

    // Calculo la transformación característica V
    V.resize(X.cols(),k);
    autovalores.clear();
    for (size_t i = 0; i < k; i++) {
        VectorXd v = M_x.col(0); // un vector cualquiera
        double lambda = metodoPotencia(M_x,v,its);
        autovalores.push_back(lambda);
        deflacionar(M_x,v,lambda);
        V.col(i) = v;
    }
}
void transfCaracteristica_v2(MatrixXd& X, unsigned int k, unsigned int its, MatrixXd& V, vector<double> &autovalores){

    // Calculo X*Xt = M_x moño
    MatrixXd Xt = X.transpose();
    MatrixXd Mm_x = X*Xt;;
    Mm_x /= (double)(X.rows()-1);

    // Calculo la transformación característica V
    V.resize(X.cols(),k);
    autovalores.clear();
    for (size_t i = 0; i < k; i++) {
        VectorXd v = Mm_x.col(0); // un vector cualquiera
        double lambda = metodoPotencia(Mm_x,v,its);
        autovalores.push_back(lambda);
        deflacionar(Mm_x,v,lambda);
        V.col(i) = Xt*v;
    }
}

// -------------- separador de bajo presupuesto --------------
// Es Cara o No?

double train_recognizer(const MatrixXd& V, const std::vector< std::vector<VectorXd>  > &clase_de_sujetos){
    std::vector<double> measuring (clase_de_sujetos.size()*clase_de_sujetos[0].size(), 0);
    MatrixXd V_t = V.transpose();
    // Lo hago en el vector por si depues decidimos sacar no el maximo, sino otra cosa?
    for (size_t s = 0; s < clase_de_sujetos.size(); s++) {
        for (size_t i = 0; i < clase_de_sujetos[0].size(); i++) {
            double m;
            VectorXd coordinates = V_t*clase_de_sujetos[s][i];
            VectorXd proyection = V*coordinates;
            VectorXd diff = clase_de_sujetos[s][i] - proyection;
            m = diff.norm();
            measuring[s*clase_de_sujetos[0].size()+i] = m;
        }
    }
    // Get max
    double max = 0;
    for (size_t i = 0; i < measuring.size(); i++) {
        if (measuring[i] > max) {
            max = measuring[i];
        }
    }
    return max;
}

bool recognize(const MatrixXd &V, const double& umbral, VectorXd& target){
    MatrixXd V_t = V.transpose();
    double m = 0;
    VectorXd coordinates = V_t*target;
    VectorXd proyection = V*coordinates;
    VectorXd diff =target - proyection;
    m = diff.norm();
    return m <= umbral;
}

void componente_menos_principal(MatrixXd& X, unsigned int its){

    // Calculo X*Xt = M_x moño
    MatrixXd Xt = X.transpose();
    MatrixXd Mm_x = X*Xt;;
    Mm_x /= (double)(X.rows()-1);

    // Busco el autovector con autovalor de menor magnitud distinto de cero
    Mm_x = Mm_x.inverse();
    vector<double> autovalores;
    for (size_t i = 0; i < X.rows(); i++) {
        VectorXd v = Mm_x.col(0); // un vector cualquiera
        double lambda = metodoPotencia(Mm_x,v,its);
        deflacionar(Mm_x,v,lambda);
    }
}
