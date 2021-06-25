#if !defined(FENETREP_HPP)
#define FENETREP_HPP

#include"mainwindow.hpp"
#include<QSpinBox>
#include<QComboBox>
#include<QDoubleSpinBox>
#include <QWidget>

class Fenetre : public QWidget
{
    Q_OBJECT

private:
    QSpinBox* champNE; 
    QSpinBox* champCC;
    QSpinBox* champNC;
    QComboBox* typeSimulation;
    QDoubleSpinBox* tauxApp;

public:
    Fenetre();
    ~Fenetre();


public slots:
	//Parametres sendToStructParam();
    //void saisieParam();

};

#endif 
