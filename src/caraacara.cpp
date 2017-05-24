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

bool esCara_v1(const MatrixXd& X, unsigned int its, unsigned int img_alto, unsigned int img_ancho, const char* img_path, RowVectorXd const &media){

    VectorXd v = X.row(0);
    vector<double> coors = componentes_menos_principales(X, its,v);

    // Las siguientes líneas guardan la autocara
    /*
    RowVectorXd sujeto_en_espacio = v.transpose();
    RowVectorXd unos(img_alto*img_ancho);
    unos.setOnes();
    unos *= sujeto_en_espacio.minCoeff();
    sujeto_en_espacio -= unos;
    sujeto_en_espacio *= 255.0/(sujeto_en_espacio.maxCoeff());
    save_image("componente_menos_principal.pgm", img_ancho, img_alto, sujeto_en_espacio);
    */

    /* Ahora que tengo la distribución de la coordenada de la última componente
    en las imágenes de entrenamiento, la supongo normal y me fijo si la coordenada
    de la imagen que me dieron está a +- 2*sigma de la media.
    */

    double mean = 0;
    for (size_t i = 0; i < coors.size(); i++) mean+=coors[i];
    mean /= (double)coors.size();

    double sigma = 0;
    for (size_t i = 0; i < coors.size(); i++) sigma+=(coors[i]-mean)*(coors[i]-mean);
    sigma /= (double)coors.size();
    sigma = sqrt(sigma);

    double min_lim = mean - 2*sigma;
    double max_lim = mean + 2*sigma;

    RowVectorXd imgt;
    VectorXd img_espacio;
    get_image(img_path, img_ancho, img_alto, imgt, img_path);
    imgt -= media;
    img_espacio = imgt.transpose();
    double coor = v.transpose()*img_espacio;

    return (min_lim < coor) && (max_lim > coor);

}


vector<double> componentes_menos_principales(const MatrixXd& X, unsigned int its, VectorXd &v){
    /*
    Esta función devuelve la coordenada de la última componente principal
    de cada imagen de entrenamiento.

    NOTA IMPORTANTE
    El método de la potencia inversa no es practicable acá.
    La inversa de X*Xt que me tira Eigen está lejos de ser la inversa (no dan
    ni ahí la identidad multiplicadas). Se me ocurren dos explicaciones:
        * XXt resulta no ser inversible: parece poco probable, pero es posible.
         Significaría algo como que las imágenes de entrenamiento son ld.
        * Como las filas son muy parecidas, el número de condición de XXt no es
        bueno y el cálculo de la inversa da cualquier cosa. Me suena raro también,
        pero yo que se...

    UPDATE: creo que, evidentemente, XXt no es inversible; ya sea matemáticamente
    (lo cual me resulta poco probable) o para la computadora por error numérico.
    */

    v.resize(X.rows());

    // Calculo X*Xt = M_x moño = Mm_x
    MatrixXd Xt = X.transpose();
    MatrixXd Mm_x = X*Xt;;
    Mm_x /= (double)(X.rows()-1);

    // Busco el autovalor de menor magnitud distinto a cero y su autovector asociado
    vector<double> autovalores;
    vector<VectorXd> autovectores;
    VectorXd x = Mm_x.col(0); // un vector cualquiera
    for (int i = 0; i < X.rows(); i++) {
        double lambda = metodoPotencia(Mm_x,x,its);
        autovalores.push_back(lambda);
        autovectores.push_back(x);
        deflacionar(Mm_x,x,lambda);
    }
    assert(X.rows() > 0);
    int indice = 0;
    while (autovalores[indice+1] > exp(-8)) {
        indice++;
    }
    v = Xt*autovectores[indice]; // Para convertir autovector de Mm_x a uno de M_x

    // Calculo la coordenada de cada imagen de entrenamiento
    VectorXd coors_e = v.transpose()*Xt;
    vector<double> coors(coors_e.data(), coors_e.data() + coors_e.rows() * coors_e.cols());

    return coors;
}



bool esCara_v2(const MatrixXd &V_normalized,const MatrixXd &Xt, unsigned int img_alto, unsigned int img_ancho, const char* img_path, RowVectorXd const &media, const vector<sujeto> &sujetos){

    unsigned int img_por_sujeto = sujetos[0].size();

    std::vector< std::vector<VectorXd> > imgs_por_sujeto(sujetos.size());
    for (size_t i = 0; i < sujetos.size(); i++){
        imgs_por_sujeto[i] = std::vector<VectorXd>(img_por_sujeto);
        for (size_t j = 0; j < img_por_sujeto; j++){
            imgs_por_sujeto[i][j] = Xt.col(i*img_por_sujeto+j);
        }
    }

    double max_norm = train_recognizer(V_normalized, imgs_por_sujeto);

    RowVectorXd target;
    get_image(img_path, img_ancho, img_alto, target, img_path);
    target = target - media;
    VectorXd target_t_centered = target.transpose();

    MatrixXd V_t = V_normalized.transpose();
    double m = 0;
    VectorXd coordinates = V_t*target_t_centered;
    VectorXd proyection = V_normalized*coordinates;
    VectorXd diff =target_t_centered - proyection;
    m = diff.norm();

    return (m <= max_norm*0.65);

}

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
