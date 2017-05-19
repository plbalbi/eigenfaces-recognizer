#include "caraacara.hpp"
#include "tests/unitTesting.hpp"
#include <vector>
#include <cfloat>
#include <ctime>

// -------------- separador de bajo presupuesto --------------

double inline distancia(const VectorXd &a, const VectorXd &b){
    VectorXd temp = a - b; return temp.norm();
};

int fast_knn(const std::vector< std::vector<VectorXd>  > &clase_de_sujetos, const VectorXd &v, int k){
    std::vector< std::pair<int, double> > distances;
    for (int s = 0; s < clase_de_sujetos.size(); s++) {
       for (int i = 0; i < clase_de_sujetos[0].size(); i++) {
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
    for (int i = 0; i < counts.size(); i++) {
        if (counts[i] > max_qty) {
            max_qty = counts[i];
            max_clase = i;
        }
    }
    return max_clase;
}

int weighted_knn(const std::vector< std::vector<VectorXd>  > &clase_de_sujetos, const VectorXd &v, int k){
    std::vector< std::pair<int, double> > distances;
    for (int s = 0; s < clase_de_sujetos.size(); s++) {
       for (int i = 0; i < clase_de_sujetos[0].size(); i++) {
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
    for (int i = 0; i < counts.size(); i++) {
        if (counts[i] > max_qty) {
            max_qty = counts[i];
            max_clase = i;
        }
    }
    return max_clase;
}

// -------------- separador de bajo presupuesto --------------

// Reduccion de espacio

void matrizCovarianza(unsigned int img_alto, unsigned int img_ancho, unsigned int img_por_sujeto, vector<sujeto> sujetos, MatrixXd &X, MatrixXd &M_x, RowVectorXd &media){
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


    // Calculo la matriz de covarianza
    M_x.resize(img_alto*img_ancho,img_alto*img_ancho);
    MatrixXd Xt = X.transpose();
    M_x = Xt*X;
    M_x /= ((double)(sujetos.size()*img_por_sujeto -1));
}

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

void transfCaracteristica(MatrixXd& M_x, unsigned int k, unsigned int its, MatrixXd& V, vector<double> &autovalores){
    V.resize(M_x.rows(),k);
    autovalores.clear();
    for (size_t i = 0; i < k; i++) {
        VectorXd v = M_x.col(0); // un vector cualquiera
        double lambda = metodoPotencia(M_x,v,its);
        autovalores.push_back(lambda);
        deflacionar(M_x,v,lambda);
        V.col(i) = v;
    }
}

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
// -------------- separador de bajo presupuesto --------------
// CARA O NO?
double train_recognizer(const MatrixXd& V, const std::vector< std::vector<VectorXd>  > &clase_de_sujetos){
    std::vector<double> measuring (clase_de_sujetos.size()*clase_de_sujetos[0].size(), 0);
    MatrixXd V_t = V.transpose();
    // Lo hago en el vector por si depues decidimos sacar no el maximo, sino otra cosa?
    for (int s = 0; s < clase_de_sujetos.size(); s++) {
        for (int i = 0; i < clase_de_sujetos[0].size(); i++) {
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
    for (int i = 0; i < measuring.size(); i++) {
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


// -------------- separador de bajo presupuesto --------------


int main(int argc, char const *argv[]) {
    time_t start = time(NULL);

    if ((argc == 2) && argv[1] == std::to_string(42)) {
        // Correr tests si se introdujo el número secreto
        test_metodoPotencia();
        test_metodoPotencia2();
        return 0;
    }else if (argc < 3) {
        // Mostrar help si no hay al menos 2 parámetros
        show_help();
        return 0;
    }

    flags_t flags;
    flags = get_flags(argc, argv);

    char const* in_path;
    char const* out_path;
    unsigned int img_ancho;
    unsigned int img_alto;
    unsigned int img_por_sujeto;
    unsigned int k;
    vector<sujeto> sujetos;
    vector<test> tests;

    in_path = argv[1];
    out_path = argv[2];

    std::cout << "Leyendo entrada de datos...\r" << std::flush;
    read_input(in_path, img_ancho, img_alto, k, sujetos, tests);
    std::cout << "Leyendo entrada de datos...\t\t" << termcolor::green << "OK" << termcolor::reset << std::endl;
    if (sujetos.size()>0) img_por_sujeto = sujetos[0].size();


    // Calculo la matriz de covarianza
    std::cout << "Armando matriz de covarianza...\r" << std::flush;
    MatrixXd X;
    MatrixXd M_x;
    RowVectorXd media;
    matrizCovarianza(img_alto, img_ancho, img_por_sujeto, sujetos, X, M_x, media);
    std::cout << "Armando matriz de covarianza...\t\t" << termcolor::green << "OK" << termcolor::reset << std::endl;


    // Busco la transformación característica
    std::cout << "Armando TL al espacio copado...\r" << std::flush;
    MatrixXd V;
    std::vector<double> autovalores;
    transfCaracteristica(M_x,k,50,V,autovalores);
    MatrixXd Vt = V.transpose();

    ofstream out;
    out.open(out_path);
    out << std::setprecision(9); // 9 dígitos
    for (size_t i = 0; i < autovalores.size(); i++) {
        out << sqrt(autovalores[i]) << '\n';
    }
    out.close();
    std::cout << "Armando TL al espacio copado...\t\t" << termcolor::green << "OK" << termcolor::reset << std::endl;

    // La matrix X tiene la imagenes de entrenamiento

    MatrixXd V_normalized = V;
    save_image("sujetos/media.pgm", img_ancho, img_alto, media);
    for (size_t i = 0; i < V_normalized.cols(); i++) {
      V_normalized.col(i) = V_normalized.col(i) /  V_normalized.col(i).norm();
      //V_normalized.col(i) = V_normalized.col(i).array().abs()*(255/V_normalized.col(i).maxCoeff());
    }

    std::cout << "Pasando imagenes a nuevo espacio...\r" << std::flush;
    MatrixXd Xt = X.transpose();


    // Me guardo las caras centradas
    std::vector< std::vector<VectorXd> > imgs_por_sujeto(sujetos.size());
    for (size_t i = 0; i < sujetos.size(); i++){
        imgs_por_sujeto[i] = std::vector<VectorXd>(img_por_sujeto);
        for (size_t j = 0; j < img_por_sujeto; j++){
            imgs_por_sujeto[i][j] = Xt.col(i*img_por_sujeto+j);
        }
    }

    // Vector que contiene cada cara de cada sujetos en un vector, ya convertida al nuevo espacio
    std::vector< std::vector<VectorXd> > clase_de_sujetos(sujetos.size());

    MatrixXd PXt(k, sujetos.size()*img_por_sujeto);
    PXt = Vt * Xt;

    for (size_t i = 0; i < sujetos.size(); i++){
        clase_de_sujetos[i] = std::vector<VectorXd>(img_por_sujeto);
        for (size_t j = 0; j < img_por_sujeto; j++){
            clase_de_sujetos[i][j] = PXt.col(i*img_por_sujeto+j);
        }
    }
    std::cout << "Pasando imagenes a nuevo espacio...\t" << termcolor::green << "OK" << termcolor::reset << std::endl;

    // Corriendo reconocimiento de caras
    std::cout << "############ RECONOCIENDO CARAS ############" << '\n';
    MatrixXd X_mono = X*V;
    for (size_t i = 0; i < tests.size(); i++) {
        RowVectorXd vt;
        VectorXd v;
        const char* ruta = tests[i].path.c_str();
        get_image(ruta,img_ancho,img_alto,vt, ruta);
        vt -= media;
        v = vt.transpose();
        v = Vt*v;

        /*
        * Las caras de entrenamiento en el nuevo espacio de k dimensiones,
        están en el vector clase_de_sujetos.
        * La cara a reconocer en el nuevo espacio de k dimensiones está en 'v'.

        La idea es que cada método guarde su resultado en el struct test,
        para luego calcular las métricas de todos.
        */

        int res;

        // kNN
        int vecinos = 5;
        res = fast_knn(clase_de_sujetos,v,vecinos);
        tests[i].knn = res;
        std::cout << "(" + tests[i].path + ") ";
        if (tests[i].respuesta == res) {
            std::cout << termcolor::green << tests[i].respuesta << " parece ser " << res<< termcolor::reset << '\n';
        }else{
            std::cout << termcolor::red << tests[i].respuesta << " parece ser " << res<< termcolor::reset << '\n';
        }

        // vecino más cercano
        // TODO

        // otros...
    }

    // Métricas
    // TODO


    time_t end = time(NULL);
    std::cout << "\n Tiempo de ejecución: ~ "<< end - start << " seg" << '\n';



    // A partir de acá comienzan a ejecutarse los flags que agregan cosas.

    if (flags.caraOno != NULL) {
        /*
        "-c"
        Este flag indica que se intente adivinar si la imagen
        pasada como parámetro es una cara o no.
        */
        const char* isImage_route = flags.caraOno;
        std::cout << isImage_route << std::endl;
        double max_norm = train_recognizer(V_normalized, imgs_por_sujeto);
        RowVectorXd target;
        get_image(isImage_route, img_ancho, img_alto, target, isImage_route);
        target = target - media;
        VectorXd target_t_centered = target.transpose();
        if (recognize(V_normalized, max_norm, target_t_centered)) {
            std::cout << termcolor::green << "Esto es una CARA!" << termcolor::reset << std::endl;
        }else{
            std::cout << termcolor::red << "Esto NO es una CARA... :>(!" << termcolor::reset << std::endl;

        }
    }


    if (flags.vecReducidos != NULL) {
        /*
        "-v"
        Este flag indica que se guarden en un archivo los vectores
        de las imágenes de entrenamiento reducidas al espacio
        de k dimensiones con PCA.
        */
        const char* salida_v = flags.vecReducidos;
        ofstream vectores;
        vectores.open(salida_v);

        for (size_t i = 0; i < clase_de_sujetos.size(); i++) {
            for (size_t j = 0; j < clase_de_sujetos[i].size(); j++) {
                vectores << i+1 << '\t' << clase_de_sujetos[i][j].transpose() << '\n';
            }
        }

        vectores.close();
    }


    if (flags.autocaras != NULL) {
        /*
        "-a"
        Este flag indica que se guarden las autocaras en sujetos/*.pgm.
        */
        const char* base_dir = "sujetos/";

        // verificar de una manera poco convencional que exista el directorio sujetos/
        string prueba(base_dir);
        prueba+= "cualEsLaProbabilidadDeQueHayaUnArchivoConEsteNombre";
        std::fstream existe(prueba , std::fstream::out);
        if (!existe) {cerr << "ERROR: no existe el directorio sujetos/" << endl;
        }else{
            remove(prueba.c_str());
            // ofstream autovectores;
            // autovectores.open("autovectores.txt");
            for (size_t i = 0; i < k; i++) {
                std::string save_route = base_dir;
                save_route += "autovector";
                save_route += "_";
                save_route += std::to_string(i+1);
                save_route += ".pgm";
                RowVectorXd sujeto_en_espacio = Vt.row(i);
                RowVectorXd unos(img_alto*img_ancho);
                unos.setOnes();
                unos *= sujeto_en_espacio.minCoeff();
                sujeto_en_espacio -= unos;
                sujeto_en_espacio *= 255.0/(sujeto_en_espacio.maxCoeff());
                // autovectores << sujeto_en_espacio << '\n';
                save_image(save_route.c_str(), img_ancho, img_alto, sujeto_en_espacio);
            }
            // autovectores.close();
        }
        existe.close();
    }

    return 0;
}
