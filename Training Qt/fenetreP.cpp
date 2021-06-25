#include "mainwindow.hpp"
#include "fenetreP.hpp"
#include <QPushButton>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QLibraryInfo>
#include <QGridLayout>
#include <QFormLayout>
#include <QWidget>
#include <iostream>
#include <QGraphicsWidget>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <Eigen/Dense>
#include <QFrame>
#include <QVBoxLayout>
#include <QCategoryAxis>
#include <QBoxLayout>
#include <QAbstractButton>
#include <QDebug>



Fenetre::Fenetre(){

    this->resize(500,400);
    this->setWindowTitle("parametrage creation reseau de neurone"); //give title pour fenenetrePara
    QGroupBox* groupBox = new QGroupBox(this); //va pouvoir contenir tout les element 

    //creation boite avec des int qui on pour parent fenetre
    champNE = new QSpinBox(this); 
    champCC = new QSpinBox(this);
    champNC = new QSpinBox(this);
    
    //creation de boite avec un double qui a comme parents fenetre
    tauxApp = new QDoubleSpinBox(this);

    //creation d'une boite avec des champ precis qstring, qui ont comme dad fenetre
    typeSimulation = new QComboBox(this);
    //add les champ precis avec addItem
    typeSimulation->addItem("lettre");
    typeSimulation->addItem("chiffre");
    typeSimulation->addItem("image");

    //creation layout qui va agencer verticalement avec parent fenetre
    QFormLayout* formLayout = new QFormLayout(this); 
    QString s3 ("Le nombre de neurones dans la couche d'entrée definira le nombre de pixels,\n");
    s3.push_back("à combien seront compresser ou remplis les images");
    champNE->setToolTip(s3);
    
    //add l'element au layout qu'on veut agencer en rajoutant un blase devant
    formLayout->addRow("Nombre de neurones par couches d'entrées:",champNE);

    champCC->setToolTip("definis le nombre couches cachées");
    formLayout->addRow("Nombre de couches cachées: ",champCC);

    champNC->setToolTip("definis le nombre de neurones par couche cachée");
    formLayout->addRow("Nombre de neurones par couches cachées:", champNC);

    QString s2 ("vous permet de choisir a quelle vitesse votre reseau apprendra\n");
    s2.push_back("valeur comprise entre 0 et 1) conseil rester proche de la valeur par defaut");
    tauxApp->setToolTip(s2);
    tauxApp->setRange(0.0, 1.0);
    tauxApp->setValue(0.8);
    tauxApp->setSingleStep(0.1);
    formLayout->addRow("Taux d'apprentissage: ",tauxApp);

    QString s("vous permet de choisir le type de reconnaissance de votre reseau\n"); 
    s.push_back("lettre pour une reconnaissance de de lettre.\n");
    s.push_back("pour une reconnaissance de chiffre. image pour reconnaitre des chat.\n");
    s.push_back("definira automatiquement le nombre de neurones dans la couche de sortie");
    typeSimulation->setToolTip(s);
    formLayout->addRow("Type de simulation (lettre, image, chiffre)", typeSimulation);

    //creation d'un autre layout pour toute les boxs de manière vertical de la this; dad fenetre
    QVBoxLayout *layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->addWidget(groupBox); // add box 2ième layout

    //creation d'un troisième layout honrizontal pour les bouttons
    QHBoxLayout* layoutH = new QHBoxLayout(this);
    QPushButton *valider = new QPushButton("Valider", this);//creer un bouton
    QPushButton *close = new QPushButton("close", this);//creer un bouton
   
    //add les buttons au layout
    layoutH->addWidget(close);
    layoutH->addWidget(valider);
    layoutPrincipal->addLayout(layoutH);
    
    connect(close, SIGNAL(clicked()),this, SLOT(close()));
    connect(valider,SIGNAL(clicked()),this,SLOT(sendToStructParam()));
    groupBox->setLayout(formLayout);//va permettre de bien positionner la box (je crois en disant que cette box respecte ce layout)
    this->setLayout(layoutPrincipal);//adapter la taille de la fentre au element dedans
    this->show(); // montrer la this 
 

}

Fenetre::~Fenetre(){

}
/*
Parametres Fenetre::sendToStructParam()
{
	Parametres param;
	
	param.nbCouches = champCC->value();
	param.nbNeuronesEntree = champNE->value();
	param.nbNeuronesCache = champNC->value();
	if(typeSimulation->currentText() == "image") 
	{
		param.typeSim = 0;
		param.nbNeuronesSortie = 2;
	}
	if(typeSimulation->currentText() == "lettre")
	{
		param.typeSim = 1;
		param.nbNeuronesSortie = 26;
	}
	if(typeSimulation->currentText() == "chiffre")
	{
		param.typeSim = 2;
		param.nbNeuronesSortie = 10;
	}
	
	param.tauxApprentissage = tauxApp->value();
	qDebug() << "nombre couche cacher" << param.nbCouches;
	return param;
}

void Fenetre::saisieParam(){

    
    
 
    
    
}*/