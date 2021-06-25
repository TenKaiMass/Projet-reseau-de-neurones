#ifndef EXT_HPP
#define EXT_HPP

#include <eigen3/Eigen/Dense>
#include <vector>
#include <string>
#include <random>
#include <map>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <ctime>

using namespace std;

using namespace Eigen;

//Structure Parametres
struct Parametres
{
	unsigned int typeSim;
	unsigned int nbCouches;
	unsigned int nbNeuronesEntree;
	unsigned int nbNeuronesCache;
	unsigned int nbNeuronesSortie;
	double tauxApprentissage;
};




string toString(int resultatSimulation, int typeSim);
VectorXd aleaBiais(int nbNeurones);
MatrixXd aleaPoids(int nbNeurones, int nbNeuronesSuivants);

#endif