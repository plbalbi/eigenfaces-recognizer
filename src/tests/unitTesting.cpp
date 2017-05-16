#include "unitTesting.hpp"

void test_metodoPotencia(){
  srand(time(NULL));
  float umbral = 1;
  for (int i = 0; i < 100; i++) {
    std::cout << "TEST " << i << ":\t\t";
    MatrixXf M = MatrixXf::Random(10,10);
    RowVectorXf v = RowVectorXf::Random(1,10);
    int l = metodoPotencia(M, v, 100);
    VectorXf v_trans = v.transpose();
    VectorXf r = M*v_trans - l*v_trans;
    std::cout << '\n' << r.transpose() << '\n';
    if (r.norm() < umbral) {
      std::cout << termcolor::green << "OK" << termcolor::reset << '\n';
    }else{
      std::cout << termcolor::red << "FALLA" << termcolor::reset << '\n';
    }
  }
}
