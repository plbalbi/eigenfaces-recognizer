#include "caraacara.hpp"
#include "tests/unitTesting.hpp"
#include <vector>
#include <cfloat>
#include <ctime>


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
        int vecinos;

        // kNN
        vecinos = 5;
        res = fast_knn(clase_de_sujetos,v,vecinos);
        tests[i].knn = res;
        std::cout << "(" + tests[i].path + ") ";
        if (tests[i].respuesta == res) {
            std::cout << termcolor::green << tests[i].respuesta << " parece ser " << res<< termcolor::reset << '\n';
        }else{
            std::cout << termcolor::red << tests[i].respuesta << " parece ser " << res<< termcolor::reset << '\n';
        }
        std::cout << "\n";

        // vecino más cercano
        vecinos = 1;
        res = fast_knn(clase_de_sujetos,v,vecinos);
        tests[i].vecino_mas_cercano = res;

        // weighted knn
        vecinos = 5;
        res = weighted_knn(clase_de_sujetos,v,vecinos);
        tests[i].weighted_knn = res;
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

        // Me guardo las caras centradas
        std::vector< std::vector<VectorXd> > imgs_por_sujeto(sujetos.size());
        for (size_t i = 0; i < sujetos.size(); i++){
            imgs_por_sujeto[i] = std::vector<VectorXd>(img_por_sujeto);
            for (size_t j = 0; j < img_por_sujeto; j++){
                imgs_por_sujeto[i][j] = Xt.col(i*img_por_sujeto+j);
            }
        }

        const char* isImage_route = flags.caraOno;
        std::cout << isImage_route << std::endl;
        double max_norm = train_recognizer(V_normalized, imgs_por_sujeto);
        RowVectorXd target;
        get_image(flags.caraOno, img_ancho, img_alto, target, flags.caraOno);
        target = target - media;
        VectorXd target_t_centered = target.transpose();
        if (recognize(V_normalized, max_norm*1, target_t_centered)) {
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
