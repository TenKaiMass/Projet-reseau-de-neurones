#include <eigen3/Eigen/Dense>
#include "reseau.hpp"
#include "couche.hpp"

#include "gestionnaireMemoire.hpp"
#include "MainWindow.hpp"

#include <iostream>

#include <map>
#include <vector>
#include <cmath>
using namespace Eigen;
using namespace std;

Reseau::Reseau(Parametres p)
{
	typeSim = p.typeSim;
	nbCouches = p.nbCouches;
	tauxApprentissage = p.tauxApprentissage;
	vCouches.reserve(nbCouches);

	//Couche d'entrée
	if (nbCouches == 2)
		vCouches.emplace_back(Couche(p.nbNeuronesEntree, aleaPoids(p.nbNeuronesSortie, p.nbNeuronesEntree), aleaBiais(p.nbNeuronesEntree)));
	else
		vCouches.emplace_back(Couche(p.nbNeuronesEntree, aleaPoids(p.nbNeuronesCache, p.nbNeuronesEntree), aleaBiais(p.nbNeuronesEntree))); //Couches cachées-1
	
	for (int i = 1; i < (nbCouches - 2); i++)
		vCouches.emplace_back(Couche(p.nbNeuronesCache, aleaPoids(p.nbNeuronesCache, p.nbNeuronesCache), aleaBiais(p.nbNeuronesCache)));

	//Couche cachée; avant-dernière
	int i = 1; //ajout
	if (nbCouches > 2)
		vCouches.emplace_back(Couche(p.nbNeuronesCache, aleaPoids(p.nbNeuronesSortie, p.nbNeuronesCache), aleaBiais(p.nbNeuronesCache)));

	//Couche de sortie
	vCouches.emplace_back(Couche(p.nbNeuronesSortie, aleaBiais(p.nbNeuronesSortie)));
}
Reseau::Reseau(Parametres p, vector<MatrixXd> mPoids, vector<VectorXd> vBiais)
{
	typeSim = p.typeSim;
	nbCouches = p.nbCouches;
	tauxApprentissage = p.tauxApprentissage;
	vCouches.reserve(nbCouches);

	//Couche d'entrée
	
	vCouches.emplace_back(Couche(p.nbNeuronesEntree, mPoids[0], vBiais[0]));

	//Couches cachées-1
	for (int i = 1; i < (nbCouches - 2); i++)
	{
		vCouches.emplace_back(Couche(p.nbNeuronesCache, mPoids[i], vBiais[i]));
	}

	//Couche cachée; avant-dernière
	if (nbCouches > 2)
		vCouches.emplace_back(Couche(p.nbNeuronesCache, mPoids[nbCouches - 2], vBiais[nbCouches - 2]));

	//Couche de sortie
	vCouches.emplace_back(Couche(p.nbNeuronesSortie, vBiais[nbCouches - 1]));
}

//Destructeur
Reseau::~Reseau() {}

//Getteurs
unsigned int Reseau::getNbCouches() { return nbCouches; };

vector<MatrixXd> Reseau::getPoids()
{
	vector<MatrixXd> poids;
	poids.resize(nbCouches); //mettre taille  au vec

	for (int i = 0; i < nbCouches; i++)
	{
		poids.push_back(vCouches[i].mPoids);
	}
	return poids;
};
vector<unsigned int> Reseau::getStats() { return stats; }
//Méthodes du RNU
int Reseau::max(VectorXd sorties) //permet d'avoir l'indice de la valeur max
{
	int max = 0;
	int indice = -1;
	for (int i = 0; i < (sorties.size()); i++)
	{
		if (max < sorties[i])
		{
			max = sorties[i];
			indice = i;
		}
	}
	return indice;
}

void Reseau::printReseau()
{
	for (int i = 0; i < nbCouches; i++)
	{
		std::cout << "Matrice " << i << " :\n"
				  << vCouches[i].mPoids << std::endl
				  << std::endl;
	}

	for (int i = 0; i < nbCouches; i++)
	{
		std::cout << "Vecteur " << i << " :\n"
				  << vCouches[i].vBiais << std::endl
				  << std::endl;
	}
}

int Reseau::simulation(VectorXd entrees) /*applique la propa + max */
{
	VectorXd v;
	v = propagation(entrees);
	return max(v);
}

VectorXd sigmoide(VectorXd entree) //apparently slow
{
	VectorXd sortie(entree.size());
	for (int i = 0; i < entree.size(); i++)
	{
		double dividend = exp(-entree[i]);
		dividend += 1;
		sortie[i] = 1 / dividend;
	}
	return sortie;
}

double fast_sigmoide(double entree)
{
	double sortie;
	double diviseur = abs(entree);
	diviseur += 1;
	sortie = 1 / diviseur;

	return sortie;
}

double Reseau::deriveeSigmoide(double sigmo)
{
	double sortie;
	sortie = 1 - sigmo;
	sortie *= sigmo;
	return sigmo;
}

VectorXd Reseau::propagation(VectorXd entrees)
{
	MatrixXd mult;

	vCouches[0].vActivation = sigmoide(entrees + vCouches[0].vBiais); //modify the activation vector

	for (int i = 1; i < nbCouches; i++) //boucle pour TOUTES les couches cachees
	{
		mult = vCouches[i - 1].mPoids * vCouches[i - 1].vActivation;

		for (int n = 0; n < vCouches[i].nbNeurones; n++)
		{
			vCouches[i].vActivation = sigmoide(mult + vCouches[i].vBiais); //modify the activation vector
		}
	}

	return vCouches[nbCouches - 1].vActivation; //retourne le calcul sigmoide de la derniere couche
}

// calcul delta1
VectorXd Reseau::calculDelta1(VectorXd resultatAttendu)
{
	VectorXd error = resultatAttendu - vCouches[nbCouches - 1].vActivation;

	return error;
}

MatrixXd Reseau::calculDelta2(VectorXd delta, MatrixXd poids)
{
	int nbNeuronesMax = vCouches[0].nbNeurones;
	nbNeuronesMax = (vCouches[1].nbNeurones > nbNeuronesMax) ? vCouches[1].nbNeurones : (vCouches[nbCouches - 1].nbNeurones > nbNeuronesMax ? vCouches[nbCouches - 1].nbNeurones : nbNeuronesMax);

	MatrixXd deltaMatrice(nbCouches, nbNeuronesMax); // matrice d'erreur
	//vector<VectorXd> deltaMatrice;
	//deltaMatrice.

	// copier le vecteur delta du calcul delta1
	deltaMatrice.row(nbCouches - 1) = delta; //on colle delta dans la derniere colonne

	cout << "On a copie la colonnnnnne !\n";
	for (int boucle = nbCouches - 2; boucle >= 0; boucle++)
	{
		VectorXd mult;
		// calcul somme
		for (int i = 0; i < nbCouches; i++) //boucle pour TOUTES les couches cachees
		{
			mult += vCouches[i - 1].mPoids * vCouches[i - 1].vActivation;
		}

		for (int j = nbCouches - 2; j >= 0; j--)
		{
			for (int i = 0; i < vCouches[i].nbNeurones; i++)
			{
				deltaMatrice(j, i) = ((deriveeSigmoide(vCouches[j].vActivation[i])) * deltaMatrice(j, i - 1) * mult[i]);
			}
		}
	}
	return deltaMatrice;
}

void Reseau::miseAJour(MatrixXd delta, VectorXd activation)
{

	int indexDerniereCouche;
	int nbNeuronesCouche;

	double gradient;

	for (int j = 0; j < nbCouches; j++)
	{
		for (int i = 0; i < vCouches[i].nbNeurones; i++)
		{
			gradient = tauxApprentissage * vCouches[j].vActivation[i - 1] * delta(i, j);
			vCouches[j].mPoids(i, j) = vCouches[j].mPoids(i, j) + gradient;
			vCouches[j].vBiais(i) = vCouches[j].vBiais(i) + gradient;
		}
	}
}

bool Reseau::retropropagation(VectorXd entree, VectorXd resultatAttendu)
{
	// propagation
	VectorXd propa;
	propa = propagation(entree);
	VectorXd delta = calculDelta1(resultatAttendu);

	cout << "Delta 1 fonctionne :\n"
		 << delta << endl
		 << endl;

	// récupération du resultatattendu
	if (max(delta) == max(resultatAttendu))
	{
		cout << "Verification Delta" << endl;

		// calculdelta2
		MatrixXd d2;
		for (int i = nbCouches - 1; i >= 0; i++)
		{
			d2 = calculDelta2(delta, vCouches[i].mPoids);
			cout << "Delta 2 num " << i << " : " << d2 << endl;
			// miseàjour
			miseAJour(d2, vCouches[i].vActivation);
		}

		return false;
	}
	else
	{
		cout << "On est pas passe par Mise a Jour\n";
		return true;
	}
}

VectorXd Reseau::retourLabel(int label)
{
	// récupération du resultat attendu
	VectorXd repAttendues;
	if (typeSim == 1) // bmp chien chat
		repAttendues.Zero(2);
	else if (typeSim == 2) //chiffres
		repAttendues.Zero(10);
	else if (typeSim == 3) //lettres
		repAttendues.Zero(26);

	repAttendues[label] = 1;
	return repAttendues;
}

vector<VectorXd> Reseau::allLabels(vector<int> labels)
{
	vector<VectorXd> vReponses;
	for (int i = 0; i < labels.size(); i++)
	{
		vReponses[i] = retourLabel(labels[i]);
	}

	return vReponses;
}

void Reseau::entrainement(vector<VectorXd> setFichiers, vector<int> labels)
{
    bool verifRetro = false;

    while(setFichiers.size()) //TOUS les fichiers
    {
        VectorXd image = setFichiers.back();
        VectorXd label = retourLabel(labels.back());
        setFichiers.pop_back(); labels.pop_back();

        unsigned int tauxEchec = -1;
        while(!verifRetro)
        {
            verifRetro = this->retropropagation(image, label);
            tauxEchec++;
        }
        stats.push_back(tauxEchec);
    }
}