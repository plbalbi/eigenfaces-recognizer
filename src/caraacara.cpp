#include "caraacara.hpp"
#include <vector>
#include <cfloat>

// -------------- separador de bajo presupuesto --------------
// Reduccion de espacio
int metodoPotencia(const MatrixXf& B, RowVectorXf& v, int iteraciones){
  VectorXf vt = v.transpose();
  for (int i = 0; i < iteraciones; i++) {
    v = B*vt;
    v *= (1/v.norm());
    vt = v.transpose();
  }
  int lambda = v*(B*vt);
  lambda /= (v*vt);
  return lambda;
}

void deflacionar(MatrixXf& B, RowVectorXf& v, int lambda){
  VectorXf vt = v.transpose();
  B = B - lambda*vt*v;
}

void transfCaracteristica(MatrixXf& M_x, unsigned int k, unsigned int its, MatrixXf& Vt){
    Vt.resize(k, M_x.rows());

    for (size_t i = 0; i < k; i++) {
        RowVectorXf v = M_x.row(0); // un vector cualquiera
        int lambda = metodoPotencia(M_x,v,its);
        deflacionar(M_x,v,lambda);
        Vt.row(i) = v;
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
        distancias[i] = (X.row(i) - v).norm();
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

    std::cout << "Leyendo entrada de datos..." << std::flush;
    read_input(in_path, img_ancho, img_alto, k, sujetos, tests);
    std::cout << "Leyendo entrada de datos...  " << termcolor::green << "OK" << termcolor::reset << std::endl;
    if (sujetos.size()>0) img_por_sujeto = sujetos[0].size();
    // Pongo las imágenes como filas de 'X', calculando la media al mismo tiempo
    std::cout << "Armando matriz de covarianza" << std::flush;
    MatrixXf X(img_por_sujeto*sujetos.size(), img_alto*img_ancho);
    RowVectorXf media(1, img_alto*img_ancho);
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
    MatrixXf M_x(img_alto*img_ancho,img_alto*img_ancho);
    MatrixXf Xt = X.transpose();
    M_x = Xt*X;
    M_x *= 1/((double)(sujetos.size()*img_por_sujeto -1));
    std::cout << "Armando matriz de covarianza..." << termcolor::green << "OK" << termcolor::reset << std::endl;


    // Busco la transformación característica
    std::cout << "Armando TL al espacio copado" << std::flush;
    MatrixXf Vt;
    transfCaracteristica(M_x,k,100,Vt);
    std::cout << "Armando TL al espacio copado..." << termcolor::green << "OK" << termcolor::reset << std::endl;

    // imprimo en sujeto las fotitos de los autovectores
    char* base_dir = "sujetos/";
    for (int i = 0; i < k; i++) {
      std::string save_route = base_dir;
      save_route += "autovector";
      save_route += "_";
      save_route += std::to_string(i+1);
      save_route += ".pgm";
      RowVectorXf sujeto_en_espacio = (Vt.row(i))*((double)(sujetos.size()*img_por_sujeto -1))+media;
      save_image(save_route.c_str(), 92, 112, sujeto_en_espacio);
    }


    std::cout << "Pasando imagenes a nuevo espacio" << std::flush;
    
    // Testeando: así queda cada imagen de entrenamiento en el nuevo espacio
    for (size_t s = 0; s < sujetos.size(); s++) {
        std::cout << "Base del sujeto " << s <<":" << '\n';
        for (size_t i = 0; i < img_por_sujeto; i++) {
            VectorXf x_i(img_alto*img_ancho);
            for (size_t j = 0; j < img_alto*img_ancho; j++) {
                x_i(j) = X(s*sujetos[s].size()+i,j);
            }
            std::cout << Vt*x_i << '\n' << '\n';
        }
    }
    std::cout << "Pasando imagenes a nuevo espacio..." << termcolor::green << "OK" << termcolor::reset << std::endl;


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
