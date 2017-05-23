#include "caraacara.hpp"
#include "tests/unitTesting.hpp"
#include <vector>
#include <string>
#include <cfloat>
#include <ctime>


int main(int argc, char const *argv[]) {

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
    unsigned int iterations = 500;

    // Para los experimentos en los (Ja, es 1 nomás) necesitemos usar un numero de its custom
    if (flags.its != -1) {
        iterations = flags.its;
    }

    // Modo silencioso
    if (flags.silent) {
        std::cout.setstate(std::ios_base::failbit);
    }

    vector<sujeto> sujetos;
    vector<test> tests;

    in_path = argv[1];
    out_path = argv[2];

    std::cout << "Leyendo parámetros de entrada...\r" << std::flush;
    read_input(in_path, img_ancho, img_alto, k, sujetos, tests);
    if (sujetos.size()>0) img_por_sujeto = sujetos[0].size();
    std::cout << "Leyendo parámetros de entrada...\t" << termcolor::green << "OK" << termcolor::reset << std::endl;


    clock_t start_clock = clock();
    // -------------- separador de bajo presupuesto --------------

    std::cout << "Armando TL al espacio copado...\r" << std::flush;
    MatrixXd X;
    RowVectorXd media;
    cargar_datos(img_alto, img_ancho, sujetos, X, media);

    MatrixXd V;
    std::vector<double> autovalores;
    if (flags.matriz != NULL) {
        //calcula los autovectores con la matriz Xt*X
        transfCaracteristica_v1(X, k, 500, V, autovalores);
    }else{
        //calcula los autovectores con la matriz X*Xt
        transfCaracteristica_v2(X, k, iterations, V, autovalores);
    }
    MatrixXd Vt = V.transpose();

    MatrixXd V_normalized = V;
    for (size_t i = 0; i < V_normalized.cols(); i++) {
        V_normalized.col(i) = V_normalized.col(i) /  V_normalized.col(i).norm();
        //V_normalized.col(i) = V_normalized.col(i).array().abs()*(255/V_normalized.col(i).maxCoeff());
    }
    std::cout << "Armando TL al espacio copado...\t\t" << termcolor::green << "OK" << termcolor::reset << std::endl;

    // -------------- separador de bajo presupuesto --------------

    std::cout << "Guardando autovalores...\r" << std::flush;
    ofstream out;
    out.open(out_path);
    out << std::setprecision(9); // 9 dígitos
    for (size_t i = 0; i < autovalores.size(); i++) {
        out << sqrt(autovalores[i]) << '\n';
    }
    out.close();
    std::cout << "Guardando autovalores...\t\t" << termcolor::green << "OK" << termcolor::reset << std::endl;

    // -------------- separador de bajo presupuesto --------------

    std::cout << "Pasando imágenes a nuevo espacio...\r" << std::flush;
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
    std::cout << "Pasando imágenes a nuevo espacio...\t" << termcolor::green << "OK" << termcolor::reset << std::endl;

    // -------------- separador de bajo presupuesto --------------

    std::cout << "############ RECONOCIENDO CARAS ############" << '\n';
    MatrixXd X_mono = X*V;

    // Clasificación
    unsigned int classification_methods = 3;
    for (size_t c = 0; c < classification_methods; c++){

        string method_name;
        if (c == 0) {
            method_name = "KNN";
        } else if (c == 1) {
            method_name = "VECINO";
        } else if (c == 2) {
            method_name = "WEIGHTED";
        }
        std::cout << method_name;
        if (c != 1) std::cout << " (" << flags.vecinos << ")";
        std::cout << " ####\n";

        vector<int> res(tests.size());
        vector< vector<int> > confusion(sujetos.size(), vector<int>(sujetos.size()));
        //  en la posición (i,j) está la cantidad de i's clasificados como j's,
        //  las filas son las clases reales y las columnas son las predicciones
        for (size_t i = 0; i < tests.size(); i++) {
            // Clasificación
            RowVectorXd vt;
            VectorXd v;
            const char* ruta = tests[i].path.c_str();
            get_image(ruta,img_ancho,img_alto,vt, ruta);
            vt -= media;
            v = vt.transpose();
            v = Vt*v;

            if (c == 0) {
                res[i] = fast_knn(clase_de_sujetos,v,flags.vecinos);
            } else if (c == 1) {
                res[i] = fast_knn(clase_de_sujetos,v,1);
            } else if (c == 2) {
                res[i] = weighted_knn(clase_de_sujetos,v,flags.vecinos);
            }

            // Resultados
            confusion[tests[i].respuesta-1][res[i]-1]++;
            std::cout << "(" + tests[i].path + ") ";
            if (tests[i].respuesta == res[i]) {
                std::cout << termcolor::green << tests[i].respuesta << " parece ser " << res[i] << termcolor::reset << '\n';
            }else{
                std::cout << termcolor::red << tests[i].respuesta << " parece ser " << res[i] << termcolor::reset << '\n';
            }
        }

        vector<float> precision(sujetos.size());
        string precision_path = "./metricas/";
        precision_path += method_name;
        precision_path += "_precision.out";
        ofstream precision_file;
        precision_file.open(precision_path);

        vector<float> recall(sujetos.size());
        string recall_path = "./metricas/";
        recall_path += method_name;
        recall_path += "_recall.out";
        ofstream recall_file;
        recall_file.open(recall_path);

        vector<float> f1(sujetos.size());
        string f1_path = "./metricas/";
        f1_path += method_name;
        f1_path += "_f1.out";
        ofstream f1_file;
        f1_file.open(f1_path);

        for (size_t s = 0; s < sujetos.size(); s++) {
             // Precálculos
            int verdaderos_positivos = confusion[s][s];
            int falsos_negativos = 0;
            for (size_t i = 0; i < sujetos.size(); i++) {
                if (i != s) falsos_negativos += confusion[s][i];
            }
            int falsos_positivos = 0;
            for (size_t i = 0; i < sujetos.size(); i++) {
                if (i != s) falsos_positivos += confusion[i][s];
            }

            // Metriques
            if (verdaderos_positivos + falsos_positivos != 0) {
                precision[s] = (float)verdaderos_positivos / ((float)verdaderos_positivos + (float)falsos_positivos);
            } else {
                precision[s] = -1;
            }
            if (verdaderos_positivos + falsos_positivos != 0) {
                recall[s] = (float)verdaderos_positivos / ((float)verdaderos_positivos + (float)falsos_negativos);
            } else {
                recall[s] = -1;
            }
            if (precision[s] + recall[s] != 0 && precision[s] != -1 && recall[s] != -1) {
                f1[s] = 2 * precision[s] * recall[s] / (precision[s] + recall[s]);
            } else {
                f1[s] = -1;
            }

            // Guardar
            precision_file << s+1 << " " << precision[s] << "\n";
            recall_file << s+1 << " " << recall[s] << "\n";
            f1_file << s+1 << " " << f1[s] << "\n";
        }
        precision_file.close();
        recall_file.close();
        f1_file.close();

        printf("\n");
    }


    clock_t end_clock = clock();
    double segundos = (double)(end_clock - start_clock)/CLOCKS_PER_SEC;
    std::cout << "\n Tiempo de ejecución: ~ "<< segundos << " seg" << '\n';


    // -------------- separador de bajo presupuesto --------------


    // A partir de acá comienzan a ejecutarse los flags que agregan cosas

    if (flags.caraOno != NULL) {
        /*
        "-c"
        Este flag indica que se intente adivinar si la imagen
        pasada como parámetro es una cara o no.
        */
        ofstream caraOno;
        caraOno.open("caraOno.dat");

        bool res = esCara_v1(X, iterations, img_alto, img_ancho, flags.caraOno, media);
        if (res) {
            std::cout << "ES CARA!" << '\n';
            caraOno << res;
        }else{
            std::cout << "NO ES CARA!" << '\n';
            caraOno << res;
        }


        caraOno.close();
        /*
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
        */
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

    if (flags.tiempo != NULL) {
        /*
        "-t"
        Este flag indica que se guarde en un archivo el tiempo
        de ejecución del método.
        */
        const char* salida_t = flags.tiempo;
        ofstream tiempo;
        tiempo.open(salida_t);
        tiempo << std::setprecision(10);
        tiempo << segundos;
        tiempo.close();
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
            save_image("sujetos/media.pgm", img_ancho, img_alto, media);
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
