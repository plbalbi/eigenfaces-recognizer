#include "caraacara.hpp"
#include "tests/unitTesting.hpp"
#include <vector>
#include <cfloat>

// -------------- separador de bajo presupuesto --------------

// Reduccion de espacio

void matrizCovarianza(unsigned int img_alto, unsigned int img_ancho, unsigned int img_por_sujeto, vector<sujeto> sujetos, MatrixXf &X, MatrixXf &M_x, RowVectorXf &media){
    X.resize(img_por_sujeto*sujetos.size(), img_alto*img_ancho);
    media.resize(1, img_alto*img_ancho);
    media.setZero();
    for (size_t s = 0; s < sujetos.size(); s++) {
        for (size_t i = 0; i < img_por_sujeto; i++) {
            RowVectorXf x_i;
            const char* ruta = sujetos[s][i].c_str();
            get_image(ruta,img_ancho,img_alto,x_i);
            media = media + x_i;
            X.row(s*sujetos[s].size()+i) = x_i;
        }
    }
    media = media / (img_por_sujeto*sujetos.size());
    // Ahora a cada fila le resto la media
    for (size_t i = 0; i < sujetos.size()*img_por_sujeto; i++) {
        X.row(i) -= media;
    }


    // Calculo la matriz de covarianza
    M_x.resize(img_alto*img_ancho,img_alto*img_ancho);
    MatrixXf Xt = X.transpose();
    M_x = Xt*X;
    M_x *= 1/((double)(sujetos.size()*img_por_sujeto -1));
}

int metodoPotencia(const MatrixXf& B, VectorXf& v, int iteraciones){
    for (int i = 0; i < iteraciones; i++) {
        v = B*v;
        v *= (1/v.norm());
    }
    RowVectorXf vt = v.transpose();
    int lambda = vt*(B*v);
    lambda = lambda/(vt*v);
    return lambda;
}

void deflacionar(MatrixXf& B, VectorXf& v, int lambda){
    RowVectorXf vt = v.transpose();
    B = B - lambda*v*vt;
}

void transfCaracteristica(MatrixXf& M_x, unsigned int k, unsigned int its, MatrixXf& V){
    V.resize(M_x.rows(),k);

    for (size_t i = 0; i < k; i++) {
        VectorXf v = M_x.col(0); // un vector cualquiera
        int lambda = metodoPotencia(M_x,v,its);
        deflacionar(M_x,v,lambda);
        V.col(i) = v;
    }
}

// -------------- separador de bajo presupuesto --------------
// Clasificacion
int kNN(const MatrixXf& X, const RowVectorXf& v, int img_por_sujeto, int k){
    // X es la matriz cuyas filas son las muestras de entrenamiento
    // Hay 'cant_sujetos' clases, y para cada una, 'img_por_sujeto' muestras
    // A los elementos X[s*img_por_sujeto..s*img_por_sujeto+img_por_sujeto)
    // los vamos a asignar a la clase 'i'. Queremos ver a que clase pertenece v

    // Calculo distancias
    int cant_muestras = X.rows();
    vector<double> distancias(cant_muestras);
    for (int i = 0; i < cant_muestras; i++) {
        RowVectorXf temp(X.cols());
        for (size_t j = 0; j < temp.cols(); j++) {
            temp(j) = X(i,j) - v(j);
        }
        distancias[i] = temp.norm();
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

    return max_clase;
}

// -------------- separador de bajo presupuesto --------------


int main(int argc, char const *argv[]) {

    // Mostrar help si no hay 1 parametro
    if ((argc == 2) && argv[1] == std::to_string(42)) {
        //test_metodoPotencia();
        test_metodoPotencia2();
        return 0;
    }else
    if (argc != 3) {
        show_help();
        return 0;
    }

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
    MatrixXf X;
    MatrixXf M_x;
    RowVectorXf media;
    matrizCovarianza(img_alto, img_ancho, img_por_sujeto, sujetos, X, M_x, media);
    std::cout << "Armando matriz de covarianza...\t\t" << termcolor::green << "OK" << termcolor::reset << std::endl;


    // Busco la transformación característica
    std::cout << "Armando TL al espacio copado...\r" << std::flush;
    MatrixXf V;
    transfCaracteristica(M_x,k,100,V);
    MatrixXf Vt = V.transpose();
    std::cout << "Armando TL al espacio copado...\t\t" << termcolor::green << "OK" << termcolor::reset << std::endl;

    // imprimo en sujeto las fotitos de los autovectores
    char* base_dir = "sujetos/";
    ofstream autovectores;
    autovectores.open("autovectores.txt");
    autovectores << V*(double)(sujetos.size()*img_por_sujeto -1) << '\n';
    for (size_t i = 0; i < k; i++) {
      std::string save_route = base_dir;
      save_route += "autovector";
      save_route += "_";
      save_route += std::to_string(i+1);
      save_route += ".pgm";
      RowVectorXf sujeto_en_espacio = (Vt.row(i))*((double)(sujetos.size()*img_por_sujeto -1))+media;
      save_image(save_route.c_str(), 92, 112, sujeto_en_espacio);
    }
    autovectores.close();


    std::cout << "Pasando imagenes a nuevo espacio...\r" << std::flush;
    MatrixXf Xt = X.transpose();
    // Vector que contiene cada cara de cada sujetos en un vector, ya convertida al nuevo espacio

    std::vector< std::vector<VectorXf> > clase_de_sujetos(sujetos.size());
    MatrixXf PXt = MatrixXf(k, sujetos.size()*img_por_sujeto);
    PXt = Vt * Xt;

    for (size_t i = 0; i < sujetos.size(); i++){
        clase_de_sujetos[i] = std::vector<VectorXf>(img_por_sujeto);
        for (size_t j = 0; j < img_por_sujeto; j++){
            clase_de_sujetos[i][j] = PXt.col(i*img_por_sujeto+j);
        }
    }


    // Testeando: así queda cada imagen de entrenamiento en el nuevo espacio
    //for (size_t s = 0; s < sujetos.size(); s++) {
        //std::cout << "Base del sujeto " << s <<":" << '\n';
        //for (size_t i = 0; i < img_por_sujeto; i++) {
            //VectorXf x_i(img_alto*img_ancho);
            //for (size_t j = 0; j < img_alto*img_ancho; j++) {
                //x_i = X.col(s*sujetos[s].size()+i);
            //}
            //std::cout << Vt.transpose()*x_i << '\n' << '\n';
        //}
    //}
    //
    for (size_t i = 0; i < sujetos.size(); i++) {
        for (size_t j = 0; j < img_por_sujeto; j++) {
            std::cout << "Sujeto " << i << " | imagen " << j << ":" << std::endl;
            std::cout << clase_de_sujetos[i][j] << std::endl;
        }
    }

    std::cout << "Pasando imagenes a nuevo espacio...\t\t" << termcolor::green << "OK" << termcolor::reset << std::endl;

    // Corriendo reconocimiento de caras
    std::cout << "------- RECONOCIENDO CARAS ------------" << '\n';
    int vecinos = 5;
    MatrixXf X_mono = X*V;
    for (size_t i = 0; i < tests.size(); i++) {
        RowVectorXf v;
        const char* ruta = tests[i].path.c_str();
        get_image(ruta,img_ancho,img_alto,v);
        v -= media;
        v /= sqrt((double)(sujetos.size()*img_por_sujeto -1));
        v = v*V;
        std::cout << "Imagen convertida: " << v << '\n';
        int res = kNN(X_mono,v,img_por_sujeto,vecinos);
        std::cout << "A " << tests[i].respuesta << " se lo identificó como " << res<< '\n';
    }

    // testeando metodo potencia
    /*
    RowVectorXf test_vector = RowVectorXf(img_alto*img_ancho);
    test_vector = M_x.row(1); // uno random cualquiera
    int test_eigenValue = metodoPotencia(M_x, test_vector, 10);

    std::cout << "Prueba metodo potencia: " << std::endl;
    std::cout << test_vector << std::endl;
    std::cout << test_eigenValue << std::endl;
    */




    return 0;
}
