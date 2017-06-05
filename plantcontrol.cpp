#include "iostream"
#include "constraints.h"
#include <limits>
#include "constraints.cpp"
#include "configManager.cpp"
#include "plant.c"
#include "plant.h"

#include <vector>
#include <cmath>
#include <boost/tuple/tuple.hpp>

#include "gnuplot-iostream.h"

double calcQ(Config* cnf, ConstrMap* cmap);
double calcQ(Config* cnf, ConstrMap* cmap, bool full_print);
double calcU(double x1, double x2, Config* cnf, ConstrMap* cmap);

void searchBi(int i, double minval, double maxval, int steps, Config* cnf, ConstrMap* cmap);

void lines();
void graph(std::vector<std::pair<double, double> > vec, int i);

int main(int argc, char const *argv[])
{

  auto cmap = readConstraints("constraints.json");
  //auto cnf = readConfigJson("default_config.json", "constraints.json");
  auto cnf = readConfigFromCommandLine("constraints.json");
  double q = calcQ(&cnf, &cmap, true);
  cout << "Q = " << q << endl;
  cout << "Research config: index of b_i multiplier, minval, maxval and # of steps" << endl;
  int i = readWhileConstraint<int>("Bi?",&cmap);
  while (i > 0){
  double minval = readWhileConstraint<double>("minval",&cmap);
  double maxval = readWhileConstraint<double>("maxval",&cmap);
  int steps = readWhileConstraint<int>("steps",&cmap);

  //i = 6, minval = -0.01, maxval = 0.02, steps = 1000
  searchBi(i - 1, minval, maxval, steps, &cnf, &cmap);

  cout << "Research config: index of b_i multiplier, minval, maxval and # of steps" << endl;
  int i = readWhileConstraint<int>("Bi?",&cmap);

}
  return 0;
}

double calcQ(Config* cnf, ConstrMap* cmap, bool full_print){
  Plant plant;
  plant_init(plant);
  double q = 0;
  double x1,x2,u,y;
  if(full_print){
    lines();
    cout << setw(10) << "x1" << setw(16) << "x2" << setw(16) <<"u" << setw(16) << "y" << setw(16) << "q" << endl; 
    lines();
    cout << endl;
  }
  for (int i = 0; i < cnf->n; ++i)
  {
    //Показания первого датчика
    x1 = plant_measure(cnf->m1, plant);

    //Показания второго датчика
    x2 = plant_measure(cnf->m2, plant);

    //Расчёт управляющего сигнала
    u = calcU(x1, x2, cnf, cmap);

    //Отправка управляющего сигнала
    plant_control(cnf->l, u, plant);

    //Измерение целевого значения
    y = plant_measure(cnf->j,plant);

    //Сохранение статистики
    q += pow((cnf->targetY - y),2);

      if(full_print){
        cout << setw(10) << x1 << setw(16) << x2 << setw(16) << u << setw(16) << y << setw(16) << q/cnf->n << endl; 
      }
  }
  lines();
  return q / cnf->n;
}

double calcU(double x1, double x2, Config* cnf, ConstrMap* cmap){
  auto b = cnf->b;
  double u = b[0] + x1 * b[1] + b[2] * log(cnf->targetY + b[3] + b[4] * exp(b[5]*x2));
  return cmap->at("L" + std::to_string(cnf->l))->fit(u);
}

//Поиск хорошего значения для bi
void searchBi(int i, double minval, double maxval, int steps, Config* cnf, ConstrMap* cmap){
  std::vector<std::pair<double, double> > b_q_pts;
  double db = abs(maxval - minval)/steps;
  double qmin = std::numeric_limits<double>::max();
  double bi_min = minval;
  for(double bi = minval; bi < maxval; bi+=db){
    cnf->b[i] = bi;
    double q = calcQ(cnf,cmap, false);
    if(q < qmin){
      qmin = q;
      bi_min = bi;
    }
    cout << "B" << (i+1) << " = " << bi << "\tQ = " << q << endl; 
    b_q_pts.push_back(std::make_pair(bi, q));
  }
  cnf->b[i] = bi_min;
  cout << "Good multiplier is " << bi_min << "\t Q = " << qmin << endl;
  graph(b_q_pts, i+1);
}

void graph(std::vector<std::pair<double, double> > vec, int i){
  Gnuplot gp;
  gp << "plot" << gp.file1d(vec) << "with lines title 'Q(B" << i << ")'," << std::endl;
}

void lines()
{
  cout << setw(2);
  for (int i = 0; i<80; i++){
    cout << "-";
  }
  cout << endl;
}