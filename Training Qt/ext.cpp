#include "ext.hpp"


//Fonctions aléatoires
VectorXd aleaBiais(int nbNeurones)
{
	VectorXd vB(nbNeurones);
	std::uniform_real_distribution<double> unif(0, 1);
	std::default_random_engine re;

	for (int i = 0; i < nbNeurones; i++)
	{
		vB(i) = unif(re);
	}
	return vB;
}

MatrixXd aleaPoids(int nbNeurones, int nbNeuronesSuivants)
{
	MatrixXd mP(nbNeurones, nbNeuronesSuivants);
	std::uniform_real_distribution<double> unif(0, 1);
	std::default_random_engine re;

	for (int i = 0; i < nbNeurones; i++)
	{
		for (int j = 0; j < nbNeuronesSuivants; j++)
		{

			mP(i, j) = unif(re);
		}
	}
	return mP;
}



string toString(int resultatSimulation, int typeSim)
{
    if(typeSim==0) //Chien & Chats
    {
        if(resultatSimulation == 0)
          return "chat";
        else return "chien";
    }
    
    else if(typeSim == 1) //Characteres
    {
        char a = ((char)resultatSimulation + 97);
        string val = "";
        val += a;
        return val;
    }
    
    else if(typeSim == 2) //Nombres
    {
        char a = ((char)resultatSimulation + 48);
        string val = "";
        val += a;
        return val;
    }
    
    return "invalide";
}



