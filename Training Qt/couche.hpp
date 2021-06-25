#ifndef COUCHE_HPP
#define COUCHE_HPP

#include "ext.hpp"

#include <iostream>
#include <eigen3/Eigen/Dense>
#include <map>
#include <vector>
#include <string>

using namespace std;
using namespace Eigen;
using Eigen::MatrixXd;
using Eigen::VectorXd;

class Couche
{
	friend class Reseau;
	friend class ext;

	public :
	unsigned int nbNeurones;
	VectorXd vBiais;
	VectorXd vActivation;
	MatrixXd mPoids;

	public:
	//Constructeurs/Destructeur
	Couche (int nbNeurones, MatrixXd mPoids, VectorXd vBiais);
	Couche(int nbNeurones, VectorXd vBiais);
	~Couche();

	//Setters/Getters
	unsigned int getNbNeurones();

	//Méthodes de RNU
	//MatrixXd aleaPoids();
	//MatrixXd aleaBiais();
};

#endif  //Couche_hpp 