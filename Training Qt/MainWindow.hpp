#if !defined(MAINWINDOW_HPP)
#define MAINWINDOW_HPP


#include "gestionnaireMemoire.hpp"


#include <QMainWindow>
#include <QPushButton>
#include <QGroupBox>
#include <string>
#include <QVBoxLayout>
#include <QtCharts/QChart>
#include <QBrush>
#include <string>
#include <QChartView>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <eigen3/Eigen/Dense>
#include <vector>
#include <memory>
#include <QLabel>


using namespace QtCharts;
using namespace Eigen;


class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QPushButton* creer;//slot fait
    QPushButton* charger;//slot fait
    QVBoxLayout* boxButton1;
    QPushButton* simulationP;//slot fait
    QPushButton* apprentissage;//slot fait
    QPushButton* sauvegardeR;//slot fait
    QPushButton* sauvegardeS;//slot fait
    QVBoxLayout* boxButton2;
    QVBoxLayout* boxAllButton;
    QBoxLayout* graphe;
    QString cheminDacces;    
    QLabel res;




//Attribut ajouter par rapport au cahier des spec

    bool paint;// pour maj le paintevent
    vector<VectorXd> uBiais;// biais du reseau
    vector<MatrixXd> uPoids;// poids du reseau
    shared_ptr<Reseau> rUtilisation;//le reseau manipuler par l'utilisateur 
    Parametres p;//parametres du reseau

    //fenetrepara
    QLineEdit* champNE; 
    QLineEdit* champCC;
    QLineEdit* champNC;
    QComboBox* typeSimulation;
    QDoubleSpinBox* tauxApp;

    
public:
    MainWindow();
    ~MainWindow(); 
	QChartView* affGraphe(QString f);
    void resultat(string resultatMap);
	void afficherStructure();
    Parametres chargerRN(QString fichierTXT, vector<MatrixXd>* mPoids, vector<VectorXd>* vBiais );
    void afficheRN(Parametres param, vector<MatrixXd>* mPoids);
    
protected:
    void paintEvent(QPaintEvent *event) override;
    
    

public slots:
    

    //slot initial du cahier des specs
    void sendToStructParam(); 
    void saisieParam();
	QString openDirectory();
    QString saveFileName();

    //les slot qui n'ont pas été ajouté au cahier des specs
    void quandOnCharge();// lorsque qu'on charge un rn
    void quandOnSim();
    void quandOnLanceApprentissage();
    void quandOnSaveR();
    void quandOnSaveS();
       
};


#endif // MAINWINDOW_HPP
