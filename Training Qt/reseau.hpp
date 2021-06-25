#ifndef RESEAU_HPP
#define RESEAU_HPP

#include "couche.hpp"

//#include "gestionnaireMemoire.hpp"
//#include "MainWindow.hpp"

#include <iostream>
#include <eigen3/Eigen/Dense>
#include <map>
#include <vector>
#include <string>

using namespace Eigen;
using namespace std;


class Reseau
{
	public :
	unsigned int nbCouches;
	vector<Couche> vCouches;
	vector<unsigned int> stats;
	unsigned int typeSim;
	double tauxApprentissage;


	public:
	//Constructeurs/Destructeur
	Reseau(Parametres p);
	Reseau(Parametres p, vector<MatrixXd> mpoids, vector<VectorXd> vbiais);
	~Reseau();

	//Setters/Getters
	unsigned int getNbCouches();
	vector<MatrixXd> getPoids();
	vector<unsigned int> getStats();

	//Methode de tests
	void printReseau();
	
	//Méthodes de RNU
	//double sigmoide(double entree);
	int max(VectorXd sorties);
	VectorXd propagation(VectorXd entrees);
	int simulation(VectorXd entrees);
	
	//Parametres chargerRN(string fichierTXT , vector<MatriXd> ∗mPoids, vector<VectorXd> ∗vBiais ); 

	//Méthode d'apprentissage
	double deriveeSigmoide(double sig);
	VectorXd calculDelta1(VectorXd resultatAttendu);
	MatrixXd calculDelta2(VectorXd delta, MatrixXd poids);
	void miseAJour(MatrixXd delta, VectorXd activation);
	bool retropropagation(VectorXd entree, VectorXd resultatattendu);
	void entrainement(vector<VectorXd> setFichiers, vector<int> reponsesAttendues);
	VectorXd retourLabel(int label);
	vector<VectorXd> allLabels(vector<int> labels);

};

#endif /* Reseau_hpp */