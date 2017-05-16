#include "unitTesting.hpp"
#include <cmath>

void test_metodoPotencia(){
    srand(time(NULL));
    float umbral = .1;
    int OK_count = 0;
    int test_qty = 100;
    for (int i = 0; i < test_qty; i++) {
        std::cout << "TEST " << i << ":\t\t";
        MatrixXf M = MatrixXf::Random(10,10);
        VectorXf v = VectorXf::Random(10,1);
        float l = metodoPotencia(M, v, 100);
        VectorXf r = M*v - l*v;
        // std::cout << '\n' << r.transpose() << '\n';
        if (r.norm() < umbral) {
            std::cout << termcolor::green << "OK" << termcolor::reset << '\n';
            OK_count++;
        }else{
            std::cout << termcolor::red << "FALLA" << termcolor::reset << '\n';
        }
        std::cout << "-----------------------------------------" << '\n';
        std::cout << "Paso " << OK_count << " tests de " << test_qty << '\n';
    }
}

void test_metodoPotencia2(){
    std::cout << "Test método de potencia 2... ";

    float umbral;
    MatrixXf M(2, 2);
    VectorXf v;
    float lambda;

    umbral = 0.01;
    M << 1, 5,
         0, 6;
    v = VectorXf::Random(2 ,1);
    lambda = metodoPotencia(M, v, 100);

    if (abs(lambda - 6) < umbral) {
        // Continue...
    }else{
        std::cout << termcolor::red << "FALLA (1)" << termcolor::reset << '\n';
        std::cout << "Resultado obtenido: " << lambda << "\n";
        std::cout << "Resultado esperado: " << 6 << "\n\n";
    }

    deflacionar(M, v, lambda);

    v = VectorXf::Random(2 ,1);
    lambda = metodoPotencia(M, v, 100);

    if (abs(lambda - 1) < umbral) {
        std::cout << termcolor::green << "OK" << termcolor::reset << '\n';
    }else{
        std::cout << termcolor::red << "FALLA (2)" << termcolor::reset << '\n';
        std::cout << "Resultado obtenido: " << lambda << "\n";
        std::cout << "Resultado esperado: " << 6 << "\n\n";
    }
}