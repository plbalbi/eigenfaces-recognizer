#include "caraacara.hpp"

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

void transfCaracteristica(MatrixXf& M_x, unsigned int k, MatrixXf& Vt){
    Vt.resize(k, M_x.rows());

    int iteraciones = 100;
    for (size_t i = 0; i < k; i++) {
        RowVectorXf v = M_x.row(0); // un vector cualquiera
        int lambda = metodoPotencia(M_x,v,iteraciones);
        deflacionar(M_x,v,lambda);
        Vt.row(i) = v;
    }
}

// -------------- separador de bajo presupuesto --------------
// Clasificacion
int kNN();
// -------------- separador de bajo presupuesto --------------



int main(int argc, char const *argv[]) {

    // Mostrar help si no hay 1 parametro
    if (argc != 2) {
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

    read_input(in_path, img_ancho, img_alto, k, sujetos, tests);
    if (sujetos.size()>0) img_por_sujeto = sujetos[0].size();

    // Pongo las imágenes como filas de 'X', calculando la media al mismo tiempo
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


    // Busco la transformación característica
    MatrixXf Vt;
    transfCaracteristica(M_x,k,Vt);


    // Testeando: así queda cada imagen de entrenamiento en el nuevo espacio
    for (size_t s = 0; s < sujetos.size(); s++) {
        std::cout << "Base del sujeto " << s <<":" << '\n';
        for (size_t i = 0; i < img_por_sujeto; i++) {
            RowVectorXf x_i;
            const char* ruta = sujetos[s][i].c_str();
            get_image(ruta,img_ancho,img_alto,x_i);
            x_i -= media;
            VectorXf x_it = x_i.transpose();
            std::cout << Vt*x_it << '\n' << '\n';
        }
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
