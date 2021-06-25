#include <iostream>
#include <QApplication>
#include "MainWindow.hpp"
#include "couche.hpp"
#include "reseau.hpp"
#include "gestionnaireMemoire.hpp"
#include <QDebug>
#include <chrono>
#include <thread>


int main(int argc, char **argv)
{
   QApplication app(argc, argv);
   MainWindow w;
   w.show();
  

   return app.exec();
}