#ifndef GESTIONNAIRE_HPP
#define GESTIONNAIRE_HPP

#include "reseau.hpp"
#include "couche.hpp"

#include "stdio.h"
#include "stdlib.h"
#include "iostream"
#include "fstream"

using namespace std;

//Structure BitmapFileHeader
struct BitMapFileHeader
{
	char* type;
	int size;
	char* reserved;
	int offset;
};

//Structure BitmapImageHeader
struct BitMapImageHeader
{
	int Size ;
	int Width ;
	int Height ;
	int Planes ;
	char bitCount ;
	int Compression ;
	int imageSize ;
	int widthPixelsPerMeter ;
	int heightPixelsPerMeter ;
	int pixelsUsed ;
	int importantpixels ;
	unsigned int* R;
	unsigned int* G;
	unsigned int* B;
	int etiquette;
};

//Structure Image
struct Image
{
	unsigned  long Width ;
	unsigned  long Height ;
	double* pixel;
	int etiquette;
};
//Méthodes du gestionnaire de mémoire
int reverseInt(int i);
vector<VectorXd> recupDonneesFileMNIST(string fImage, string fLabel, vector<int> *labels);
VectorXd recupDonneesFileMNISTSimulation(string fImage);
void recupAnalyseDonneesBmp (string f, BitMapFileHeader *header , BitMapImageHeader *image);
Image convertBitmapToImage(BitMapImageHeader b);
Image compression(Image i, int nbNeurones);
VectorXd allPixelBitMap(Image i, int nbNeurones);
vector<VectorXd> allImage(vector<int> *labels, string f, int nbneurones);
void sauvegardeRN(Reseau r, string chemin);
void sauvegardeStat(Reseau r, string chemin);

#endif /* GestionnaireMemoire_hpp */