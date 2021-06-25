#include "MainWindow.hpp"
#include <QtWidgets>
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
#include <QFrame>
#include <QVBoxLayout>
#include <QCategoryAxis>
#include <QBoxLayout>
#include <QAbstractButton>
#include <QtGlobal>
#include <QMessageBox>
#include <QTextStream>
#include <QAbstractAxis>
#include <QPainter>
#include <string>

MainWindow::MainWindow()
{

    setWindowTitle("Application Réseau de neurones"); //nommer la fenetre principal
    resize(1920, 1080);
    //initalisation de param qui est vide lors du lancement de l'app
    p.nbCouches = 0;
    p.nbNeuronesCache = 0;
    p.nbNeuronesSortie = 0;
    p.nbNeuronesEntree = 0;

    cheminDacces = QDir::currentPath();
    QToolBar *bar = new QToolBar(this); //creation d'un toolbar pour quitter l'app
    bar->resize(1920, 50);
    bar->setLayoutDirection(Qt::RightToLeft); // permet d'avoir le button quitter en haut à droite
    auto quitt = new QPushButton("Quitter", this);
    bar->addWidget(quitt);                                 //ajoute le bouton à la toolBar
    connect(quitt, SIGNAL(clicked()), qApp, SLOT(quit())); // donne le signal pour quitter l'application

    //creation d'un widget qui aura toute les Qframe contenant les boutons
    QWidget *boxButton = new QWidget(this);
    boxButton->resize(400, 600);
    boxButton->move(0, 50);

    //Qframe les boutons creer et charger
    QFrame *f1 = new QFrame(boxButton);
    f1->resize(300, 300);
    f1->setFrameShape(QFrame::Panel);   //effet sur la frame
    f1->setFrameShadow(QFrame::Raised); //effet sur la frame
    creer = new QPushButton("Créer", this);
    charger = new QPushButton("Charger", this);
    connect(creer, SIGNAL(clicked()), this, SLOT(saisieParam()));     //donne le signal d'ouverture de la fenetre des paramètres
    connect(charger, SIGNAL(clicked()), this, SLOT(quandOnCharge())); //donne le signal du chargement d'un RN et si l'utilisateur le veut, possibilité de charger le graphe de progression du RN

    boxButton1 = new QVBoxLayout;   //utilisation de la box layout vertical
    boxButton1->addWidget(creer);   //ajout des boutons à la box vertical
    boxButton1->addWidget(charger); //ajout des boutons à la box vertical
    f1->setLayout(boxButton1);      //la Qframe prend la box vertical permet de l'agencement des boutons

    //frame de bouton 2
    QFrame *f2 = new QFrame(boxButton);
    f2->resize(300, 300);
    f2->setFrameShape(QFrame::Box);
    f2->setFrameShadow(QFrame::Raised);
    simulationP = new QPushButton("Simulation", this);
    apprentissage = new QPushButton("Apprentissage", this);
    sauvegardeR = new QPushButton("Sauvegarder le réseau", this);
    sauvegardeS = new QPushButton("Sauvegarder les stats", this);
    connect(simulationP, SIGNAL(clicked()), this, SLOT(quandOnSim()));                  //permet de données l'action de faire une simulation sur le RN
    connect(sauvegardeR, SIGNAL(clicked()), this, SLOT(quandOnSaveR()));                //permet de données l'action de sauvegarder le RN
    connect(sauvegardeS, SIGNAL(clicked()), this, SLOT(quandOnSaveS()));                //permet de données l'action de sauvegarder les stats du RN
    connect(apprentissage, SIGNAL(clicked()), this, SLOT(quandOnLanceApprentissage())); //permet de lancer l'apprentissage
    boxButton2 = new QVBoxLayout;
    boxButton2->addWidget(simulationP);   //ajout les boutons à la frame
    boxButton2->addWidget(apprentissage); //ajout les boutons à la frame
    boxButton2->addWidget(sauvegardeR);   //ajout les boutons à la frame
    boxButton2->addWidget(sauvegardeS);   //ajout les boutons à la frame
    f2->setLayout(boxButton2);

    //utilisation de la vbox pour tous les boutons
    boxAllButton = new QVBoxLayout(boxButton);
    boxAllButton->addWidget(f1);        //ajout de la frame à la vbox
    boxAllButton->addWidget(f2);        //ajout de la frame à la vbox
    boxButton->setLayout(boxAllButton); //le widget ajout la vbox qui contient les frames contenant elle même les boutons, et permet l'agencement final des boutons dans l'interface
}

QChartView *MainWindow::affGraphe(QString f)
{
    QFile file(f);
    //verification sur les fichiers
    if (!file.exists())
    {
        QMessageBox::warning(nullptr, "Attention", "Vous n'avez pas selectionné de fichier de sauvegarde pour le graphe de progression");
    }
    else if (!file.open(QFile::ReadOnly))
    {
        QMessageBox::warning(nullptr, f, "Erreur d'ouverture!");
    }
    //création du graphe
    QChart *graphe = new QChart();

    QLineSeries *q = new QLineSeries();
    QFileInfo filou(f);
    int j(0);
    QTextStream data(&file);
    q->append(0, 0);
    // ajout des statistique recuperer dans le fichiers
    while (!data.atEnd())
    {
        j += 100;
        auto line = data.readLine();
        auto popo = 100-line.toInt();
        line = QString::number(popo);
        *q << QPointF(j, line.toInt());
    }
    file.close();
    file.flush();
    //ajout des stats dans le graphe
    graphe->addSeries(q);
    graphe->createDefaultAxes();
    //donne le nom au graphe
    graphe->setTitle("Graphe de progression du Réseau " + filou.fileName() + " lors de l'apprentissage");
    graphe->legend()->hide();
    q->setPointsVisible(true);

    //Permet d'afficher le graphe visuellement
    QChartView *chartView = new QChartView(graphe);
    chartView->setRenderHint(QPainter::Antialiasing);
    graphe->setFlag(QGraphicsItem::ItemIsMovable); // permet de le déplacer avec la souris
    graphe->resize(700, 350);                      // donne ça taille

    return chartView;
}

void MainWindow::sendToStructParam()
{
    // verification en cas d'oublie de remplir un des champs
    if (champCC->text() == "" || champNC->text() == "" || champNE->text() == "")
    {
        QMessageBox(QMessageBox::Warning, "Attention", "Un de vos champs est vide, veuillez recréer un Réseau", QMessageBox::Ok, this).exec();
    }
    else
    {
        //permet à l'utilisateur de sauvegarder son réseau à ce moment en cas d'oublie
        bool ignore = true;
        if (p.nbCouches != 0)
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Attention");
            msgBox.setText("Un Réseau de neurones existe déjà! En créer un nouveau l'écraserait");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Ignore | QMessageBox::Cancel);
            int ret = msgBox.exec();
            switch (ret)
            {
            case QMessageBox::Save:
                quandOnSaveR();
                break;
            case QMessageBox::Ignore:
                break;
            case QMessageBox::Cancel:
                ignore = false;
            }
        }
        if (ignore)
        {
            //donne les éléments rentré par l'utilisateur à l'attribut Parametres
            p.nbCouches = champCC->text().toInt() + 2;
            p.nbNeuronesCache = champNC->text().toInt();
            if (typeSimulation->currentText() == "image")
            {
                p.typeSim = 0;
                p.nbNeuronesSortie = 2;
                p.nbNeuronesEntree = champNE->text().toInt();
            }
            if (typeSimulation->currentText() == "lettre")
            {
                p.typeSim = 1;
                p.nbNeuronesSortie = 26;
                p.nbNeuronesEntree = 784;
            }
            if (typeSimulation->currentText() == "chiffre")
            {
                p.typeSim = 2;
                p.nbNeuronesSortie = 10;
                p.nbNeuronesEntree = 784;
            }
            p.tauxApprentissage = tauxApp->value();

            //création du reseau de neurones avec le parametres
            shared_ptr<Reseau> temp(new Reseau(p));
            //recupèration par l'attribut Reseau des données du temporels
            rUtilisation = temp;
            temp.reset(); //delete le temp
            paint = true; // permet d'actualiser le paintEvent pour afficher la structure du Reseau
            update();
            QMessageBox(QMessageBox::Information, "Félicitations", "Bravo! Vous venez de créer un Réseau de neurones qui reconnait des " + typeSimulation->currentText(), QMessageBox::Ok, this).exec();
        }
    }
}

QString MainWindow::openDirectory()
{
    QString file = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", cheminDacces);
    
    //verification extension et contenu
    string extension = file.toStdString();
    int position = extension.find_last_of(".");
    extension = extension.substr(position+1);

    if (file == "" || (extension != "txt" && extension != "bmp"))
    {
        QMessageBox mes(this);
        mes.setWindowTitle("Attention");
        mes.setText("Vous avez annulé(e), ou il y a eu un problème d'extension ou de contenu!");
        mes.setIcon(QMessageBox::Warning);
        mes.exec();
        file = "";
    }

    return file; // renvoie le fichier chemin du fichier selectionné
}

void MainWindow::saisieParam()
{
    //créer la fenetre qui va s'afficher lors que l'utilisateur va cliquer sur le bouton creer
    QWidget *fenetrePara = new QWidget;
    fenetrePara->resize(500, 400); //dimensionne la fenetre

    fenetrePara->setWindowTitle("parametrage creation reseau de neurone"); //give title pour fenenetrePara
    QGroupBox *groupBox = new QGroupBox(fenetrePara);                      //va pouvoir contenir tout les element

    //creation des LineEdit à remplir qui on pour parent fenetrePara
    champNE = new QLineEdit(fenetrePara);
    champCC = new QLineEdit(fenetrePara);
    champNC = new QLineEdit(fenetrePara);

    //creation de boite avec un double qui a comme parent fenetrePara
    tauxApp = new QDoubleSpinBox(fenetrePara);

    //creation d'une boite avec des champ precis qstring, qui ont comme parent fenetre
    typeSimulation = new QComboBox(fenetrePara);
    //donne les valeurs possible que l'utilisateur peut choisir à la boite
    typeSimulation->addItem("lettre");
    typeSimulation->addItem("chiffre");
    typeSimulation->addItem("image");

    //creation layout qui va agencer les champs et boites
    QFormLayout *formLayout = new QFormLayout(fenetrePara);
    //infos sur élément champNE
    QString s3("Le nombre de neurones de la couche d'entrée correspond au nombre de pixels,\n");
    s3.push_back("(si vous choissisez une simulation avec des lettres ou des chiffres, ce sera obligatoirement 784)");
    champNE->setToolTip(s3);

    //ajoute l'element au layout qu'on veut agencer en rajoutant un text devant
    formLayout->addRow("Nombre de neurones par couche d'entrée:", champNE);

    //infos sur élément champCC
    champCC->setToolTip("definis le nombre couches cachées");
    formLayout->addRow("Nombre de couches cachées: ", champCC);

    //infos sur élément champNC
    champNC->setToolTip("definis le nombre de neurones par couche cachée");
    formLayout->addRow("Nombre de neurones par couche cachée:", champNC);

    QString s2("Permet de choisir a quelle vitesse votre réseau apprendra\n (valeur comprise entre 0 et 1)\n Conseil : Restez proche de la valeur par défaut");
    tauxApp->setToolTip(s2);
    tauxApp->setRange(0.0, 1.0); // verouille la taille min et max
    //donne des valeurs par defaut
    tauxApp->setValue(0.8);
    tauxApp->setSingleStep(0.1);
    formLayout->addRow("Taux d'apprentissage: ", tauxApp);
    if (typeSimulation->currentText() == "lettre" || typeSimulation->currentText() == "chiffre")
    {
        champNE->setText("784");
    }
    champCC->setText("0");
    champNC->setText("0");

    //texte informatif
    QString s("Choisisser le type de reconnaissance pour votre Simulation\n");
    s.push_back("lettre pour une reconnaissance de lettre.\n");
    s.push_back("chiffre pour une reconnaissance de chiffre. image pour reconnaitre des chiens et des chats.\n");
    s.push_back("definira automatiquement le nombre de neurones dans la couche de sortie");
    typeSimulation->setToolTip(s);
    formLayout->addRow("Type de simulation (lettre, image, chiffre)", typeSimulation);

    //creation d'un autre layout pour toute les boxs pour agencer verticalement les éléments
    QVBoxLayout *layoutPrincipal = new QVBoxLayout();
    layoutPrincipal->addWidget(groupBox);

    //creation d'un troisième layout honrizontal pour les bouttons
    QHBoxLayout *layoutH = new QHBoxLayout();
    QPushButton *close = new QPushButton("Fermer", fenetrePara);    //creer un bouton
    QPushButton *valider = new QPushButton("Valider", fenetrePara); //creer un bouton

    //ajoute les buttons au layout
    layoutH->addWidget(close);
    layoutH->addWidget(valider);
    layoutPrincipal->addLayout(layoutH); //ajout du layut au a l'autre layout

    connect(close, SIGNAL(clicked()), fenetrePara, SLOT(close()));        // ferme la fenetre
    connect(valider, SIGNAL(clicked()), this, SLOT(sendToStructParam())); //appel la fonction sendtostructparam qui ce qui va creer le réseau
    connect(valider, SIGNAL(clicked()), fenetrePara, SLOT(close()));

    groupBox->setLayout(formLayout);         //va permettre de bien positionner la box
    fenetrePara->setLayout(layoutPrincipal); //adapter la taille de la fenetre aux l'elements dedans
    fenetrePara->show();                     // montrer la fenetrePara
}

QString MainWindow::saveFileName()
{
    //permet à l'utilisateur de rentrer le nom du fichier qu'il veut creer*
    QString fileName = QFileDialog::getSaveFileName(this, tr("Enregistrer sous"), cheminDacces);
    //verification extension et contenu
    string extension = fileName.toStdString();
    int position = extension.find_last_of(".");
    extension = extension.substr(position+1);
    if (fileName.isEmpty() || extension != "txt")
    {
        QMessageBox::warning(nullptr, "Erreur", "Ce fichier n'existe pas ou l'extension n'est pas la bonne");
        fileName = "";
    }
    return fileName;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    //permet d'afficher la structure quand le paintEvent est rappeler
    if (paint == true)
    {
        afficherStructure();
    }
    else
    {
        //initialisation du paintEvent lors du lancement de la fonction
        QPainter pinceau(this);
        int largeur = 1020;
        int hauteur = 515;
        QPoint pos(900, 50);
        QRect back(pos, QSize(largeur, hauteur));
        pinceau.drawRect(back);              //couleur fond
        pinceau.setPen(Qt::gray);            //couleur pinceau
        pinceau.setFont(QFont("Arial", 20)); //propriété Font
        pinceau.save();
        pinceau.drawText(back, Qt::AlignCenter, "Vide pour le moment"); //text ecrit au milieu du rectangle
        pinceau.restore();
    }
}

void MainWindow::quandOnCharge()
{
    //actions qui vont se produires quand on clique sur le bouton Charger!!
    /*1*/
    QMessageBox(QMessageBox::Information, "Information", "Vous vous apprêtez à sélectionner un fichier contenant une sauvegarde d'un Réseau", QMessageBox::Ok, this).exec();
    //recuperation du fichier contenant le Réseau à charger
    cheminDacces = openDirectory();
    if (cheminDacces != "")
    {
        /*2*/
        /*On charge le reseau*/
        auto pTemp = chargerRN(cheminDacces, &uPoids, &uBiais);
        /*3*/
        /*On creer le reseau*/
        shared_ptr<Reseau> temp(new Reseau(pTemp, uPoids, uBiais));
        rUtilisation = temp; //le réseau charger est maintanant celui qui sera manipuler par l'utilisateur
        temp.reset();
        /*4*/
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle("Information");
        msgBox.setText("Selectionner, si vous le souhaiter, le fichier de sauvegarde des statistiques du Réseau");
        msgBox.setStandardButtons(QMessageBox::Ignore | QMessageBox::Ok);
        int ret = msgBox.exec();
        bool yes;
        ret == QMessageBox::Ok ? yes = true : yes = false;

        if (yes)
        {
            //Charge le fichier des stats du réseau charger
            cheminDacces = openDirectory();
            /*5*/
            //place resrver pour le graphe de progression dans la fenetre
            QWidget *grapheP = new QWidget(this);
            grapheP->resize(720, 400);
            grapheP->move(900, 600);
            QFrame *f4 = new QFrame(grapheP);
            f4->resize(500, 300);
            f4->setFrameShape(QFrame::Panel);   //effet sur la frame
            f4->setFrameShadow(QFrame::Raised); //effet sur la frame
            graphe = new QBoxLayout(QBoxLayout::LeftToRight);
            /*6*/
            //on lance la fonction qui affiche le graphe
            graphe->addWidget(affGraphe(cheminDacces));
            f4->setLayout(graphe);
            grapheP->setLayout(graphe);
            /*7*/
            //on affiche le Graphe
            grapheP->show();
        }
        /*8*/
        //appel la fonction pour afficher le tableau RN
        //afficheRN(pTemp, &uPoids);
        /*9*/
        //On recupere le parametres qui a ini le Reseau, et on le donne àl'attribut parametres
        p = pTemp;
        /*10*/
        //On modifie le paint pour update le paintevent et afficher la structure du resau chargé
        paint = true; // pour peinte event
        update();
    }
}

Parametres MainWindow::chargerRN(QString fichierTXT, vector<MatrixXd> *mPoids, vector<VectorXd> *vBiais)
{
    Parametres p;
    QFile file(fichierTXT);
    if (!file.exists())
    {
        QMessageBox::warning(nullptr, "Erreur", "Ce fichier n'existe pas");
    }
    if (!file.open(QFile::ReadOnly))
    {
        QMessageBox::warning(nullptr, "Erreur", "Erreur lors de l'ouverture du fichier" + fichierTXT);
    }
    QTextStream data(&file);
    while (!data.atEnd())
    {
        QString param = "";
        QString c = "";

        //typeSim
        while (c != " ")
        {
            param += c;
            c = data.read(1);
        }
        c = "";
        p.typeSim = param.toUInt();
        param = "";

        //nb Couches
        while (c != " ")
        {
            param += c;
            c = data.read(1);
        }
        c = "";
        p.nbCouches = param.toUInt();
        param = "";

        //nbNeuroneEntree
        while (c != " ")
        {
            param += c;
            c = data.read(1);
        }
        c = "";
        p.nbNeuronesEntree = param.toUInt();
        param = "";

        //nbNeuroneCache
        while (c != " ")
        {
            param += c;
            c = data.read(1);
        }
        c = "";
        p.nbNeuronesCache = param.toUInt();
        param = "";

        //nbNeuroneSortie
        while (c != " ")
        {
            param += c;
            c = data.read(1);
        }
        c = "";
        p.nbNeuronesSortie = param.toUInt();
        param = "";

        //tauxApprentissage
        while (c != "\n")
        {
            param += c;
            c = data.read(1);
        }
        c = "";
        p.tauxApprentissage = param.toDouble();
        param = "";
        //poids
        unsigned int j, k;
        j = 0;
        k = 0;
        for (unsigned int i = 0; i < p.nbCouches - 1; i++)
        { //-1 car la couche de sortie n'a pas de poids
            int taille1, taille2;
            if (p.nbCouches == 2)
            {
                if (i == 0)
                {
                    taille1 = p.nbNeuronesEntree;
                    taille2 = p.nbNeuronesSortie;
                }
            }
            else if (i == 0)
            {
                taille1 = p.nbNeuronesEntree;
                taille2 = p.nbNeuronesCache;
            }
            else if (i == p.nbCouches - 2)
            {
                taille1 = p.nbNeuronesCache;
                taille2 = p.nbNeuronesSortie;
            }
            else
            {
                taille1 = p.nbNeuronesCache;
                taille2 = p.nbNeuronesCache;
            }
            MatrixXd h(taille2, taille1);

            while (c != '\n')
            {
                param = "";
                c = "";
                while (c != " " && c != '\n')
                {

                    param += c;
                    c = data.read(1);
                }
                if (c != '\n')
                {

                    h(k, j) = param.toDouble(); //la case j k de la matrice n° i CA MARCHE PAS
                    k++;

                    if ((i == 0) && (k == p.nbNeuronesCache)) //Dans le cas de la première matrice
                    {
                        k = 0;
                        j++;
                    }
                    else
                    {
                        if ((i == p.nbCouches - 2) && (k == p.nbNeuronesSortie)) //Dans le cas de la dernière matrice
                        {
                            k = 0;
                            j++;
                        }
                        else
                        {
                            if (k == p.nbNeuronesCache) //Dans le cas des autres matrices
                            {
                                k = 0;
                                j++;
                            }
                        }
                    }
                }
            }
            mPoids->emplace_back(h);
            c = "";
            param = "";
            k = j = 0;
        }
        for (int i = 0; i < p.nbCouches; i++)
        {
            int taille1;
            if (i == 0)
            {
                taille1 = p.nbNeuronesEntree;
            }
            else if (i == p.nbCouches - 1)
            {
                taille1 = p.nbNeuronesSortie;
            }
            else
            {
                taille1 = p.nbNeuronesCache;
            }
            VectorXd tamp(taille1);
            while (c != '\n')
            {
                param = "";
                c = "";
                while (c != " " && c != '\n')
                {
                    param += c;
                    c = data.read(1);
                }
                if (c != '\n')
                {

                    tamp[j] = param.toDouble();
                    j++;
                }
            }
            vBiais->push_back(tamp);
            c = "";
            param = "";
            j = 0;
        }
    }
    file.close();
    file.flush();

    return p;
}

void MainWindow::afficherStructure()
{
    //element qui vont permettre de donner une echelle pour la representation des cercle
    double compress;
    double entree, cachee, sortie, couche;

    unsigned int neuronesEntree = p.nbNeuronesEntree;
    unsigned int neuronesCaches = p.nbNeuronesCache;
    unsigned int neuronesSortie = p.nbNeuronesSortie;
    unsigned int couches = p.nbCouches;

    //calcul de l'echelle
    entree = (double)neuronesEntree;
    cachee = (double)neuronesCaches;
    if (couches > 2)
        couche = (double)couches - 2;
    else
        couche = 1;

    if (couches > 17)
        couches = 17; //on aura max 15 couches cachees
    couche = couche / (double)(couches - 2);

    if (neuronesEntree > 100)
        neuronesEntree /= 10;
    if ((neuronesEntree > 20) && (neuronesEntree <= 100)) //si il y a plus de 20 neurones d'entree, on aura toujours entre 4 et 20 cercles pour les representer
    {
        compress = neuronesEntree / 25.0;
        neuronesEntree = compress * 5;
    }
    entree = entree / (double)neuronesEntree;

    if (neuronesCaches > 100)
        neuronesCaches /= 10;
    if ((neuronesCaches > 20) && (neuronesCaches <= 100)) //si il y a plus de 20 neurones par couche cachee, on aura toujours entre 4 et 20 cercles pour les representer
    {
        compress = neuronesCaches / 25.0;
        neuronesCaches = compress * 5;
    }
    if (neuronesCaches)
        cachee = cachee / (double)neuronesCaches;
    else
        cachee = 1;

    //creation point de reperes de la zone ou on va dessiner
    int largeur = 1020;
    int hauteur = 515;
    QPoint pos(900, 50);

    //creation d'un Qpainter pour dessiner
    QPainter pinceau(this);

    //affiche le rectangle blanc en fond
    pinceau.setBrush(Qt::white);
    QRect back(pos, QSize(largeur, hauteur));
    pinceau.drawRect(back); //couleur fond

    //ecrit la legende par rapport à l'echelle
    pinceau.setFont(QFont("Arial", 12));
    pinceau.setPen(Qt::black); //couleur pinceau
    pinceau.save();
    if (cachee <= 0)
        cachee = 0;
    if (couche <= 0)
        couche = 0;
    pinceau.drawText(910, 70, "1 cercle représente " + QString::number((int)entree) + " neurones");
    pinceau.drawText(1300, 70, "1 cercle représente " + QString::number((int)cachee) + " neurones");
    pinceau.drawText(1265, 555, "1 colonne représente " + QString::number(couche) + " couches cachées");
    pinceau.restore();

    //algo pour couche entrée
    //repère placer pour texte
    pos.setX(pos.x() + 105);
    pos.setY(pos.y() + 50);
    pinceau.setFont(QFont("Arial", 20)); //proprieterde la police
    pinceau.save();
    pinceau.drawText(pos, "Input");
    pinceau.restore();

    //repère de l'emplacement pour dessiner les neurones d'entré
    QSize in(255, 515);
    double largeurI = 1155.0;
    QPoint CI(1028, 110);
    QRect rect(CI, QSize(15, 15)); //(posX,posy,largeur, hauteur)

    //calcul permettant de placer les couches de la couche cachées
    QSize mid(510, 515);
    double largeurM = 1665.0;
    auto posi = 510.0 / (couches - 2);
    if (!posi % 2 == 0)
    {
        posi /= 2;
    }
    QPoint CM(largeurI + (posi)-10, 110.0);

    //config des repère couche sortie
    QSize out(255, 515);
    int largeurO = 1920;
    QPoint CO(1793, 110);
    QRect rect3(CO, QSize(15, 15)); //(posX,posy,largeur, hauteur)

    //parametres couleur
    pinceau.setBrush(Qt::green);
    pinceau.save();

    for (double i = 110.0; i < 515.0; i += 405.0 / neuronesEntree)
    {
        pinceau.drawEllipse(CI.x(), i, 15, 15);
    }
    pinceau.restore();

    pos.setX(pos.x() + 300);
    pinceau.drawText(pos, "Couches Cachées");

    //parametres couleur
    pinceau.setBrush(Qt::blue);
    pinceau.save();
    auto plus = 405.0 / neuronesCaches;
    //algo pour couche cachées
    for (double i = CM.y(); i < 515.0; i += plus)
    {
        for (double j = CM.x(); j < largeurM; j += 510.0 / (couches - 2))
        {
            pinceau.drawEllipse(j, i, 15, 15);
        }
    }

    pinceau.restore();

    pos.setX(pos.x() + 455);
    pinceau.drawText(pos, "Output");

    //parametres couleur
    pinceau.setBrush(Qt::red);
    pinceau.save();

    //algo pour couche sortie
    for (double i = 110.0; i < 515.0; i += 405.0 / neuronesSortie) //car nombre a , et prend int inf que int sup
    {
        pinceau.drawEllipse(1793, i, 15, 15);
    }
    pinceau.restore();
    //pinceau.drawLine(largeurI, 50, largeurI, 565);
    //pinceau.drawLine(largeurM, 50, largeurM, 565);
}

void MainWindow::quandOnLanceApprentissage()
{

    if (p.nbCouches != 0)
    {
        QMessageBox(QMessageBox::Information, "Information", "Vous avez lancé l'apprentissage.\n Nous vous informerons lorsqu'il sera terminé!", QMessageBox::Ok, this).exec();

        //element globaux
        vector<int> lab;
        vector<VectorXd> all;
        string chemin;
        if (p.typeSim == 0) //si BMP
        {
            chemin = "/Training_set/BMP";
            all = allImage(&lab, chemin, p.nbNeuronesEntree); //recup tous les fichier BMP dans all
        }
        //-------------------------------------------------------------------

        if (p.typeSim == 1) //si lettre
        {
            all = recupDonneesFileMNIST("/TrainingQt/MNIST/MNIST_Letter/emnist-letters-train-images-idx3-ubyte", "/TrainingQt/MNIST/MNIST_Letter/emnist-letters-train-labels-idx1-ubyte", &lab);
        }
        if (p.typeSim == 2) //si chiffre
        {
            all = recupDonneesFileMNIST("/TrainingQt/MNIST/MNIST_Chiffre/emnist-digits-train-images-idx3-ubyte", "/TrainingQt/MNIST/MNIST_Chiffre/emnist-digits-train-labels-idx1-ubyte", &lab);
        }
        //lancer entrainement sur le réseau de l'interface
        rUtilisation->entrainement(all, lab);
        QMessageBox(QMessageBox::Information, "Félicitation", "L'apprentissage s'est terminé avec succès!", QMessageBox::Ok, this).exec();
    }
    else
    {
        QMessageBox(QMessageBox::Warning, "Attention", "Vous ne pouvez pas lancer un apprentissage car aucun Réseau n'a été chargé ou créé", QMessageBox::Ok, this).exec();
    }
}

void MainWindow::quandOnSim()
{
    if (p.nbCouches != 0)
    {
        QMessageBox(QMessageBox::Information, "Information", "Vous vous apprêtez à effectuer une simulation. Pour une simulation de lettre/chiffre, \nselectionner un fichier MNIST. Pour une simulation de reconnaissance chien/chat,\n selectionner un ficher BMP ", QMessageBox::Ok, this).exec();
        /*1*/
        //select le fichier à utiliser
        cheminDacces = openDirectory();
        if (cheminDacces != "")
        {
            /*2*/
            //conversion de l'image choisi en bmp ou MNIST selon type simulation
            VectorXd all;
            if (p.typeSim == 0)
            {
                BitMapFileHeader header;
                BitMapImageHeader image;
                recupAnalyseDonneesBmp(cheminDacces.toStdString(), &header, &image);
                Image img = convertBitmapToImage(image);
                /*4*/
                //transforme image en vecteur
                all = allPixelBitMap(img, p.nbNeuronesEntree);
            }
            //-------------------------------------------------------------------
            if (p.typeSim == 1 || p.typeSim == 2)
            {
                all = recupDonneesFileMNISTSimulation(cheminDacces.toStdString());
            }
            /*5*/
            //lance la simulation
            int reponse = rUtilisation->simulation(all);
            /*6*/
            //recupère la réponse donnée par le reseau
            string s = toString(reponse, p.typeSim);
            /*7*/
            //affichage du resultat sur l'interface
            resultat(s);
        }
    }
    else
    {
        QMessageBox(QMessageBox::Warning, "Attention", "Vous ne pouvez pas faire de simulation car aucun Réseau n'a été chargé ou créé", QMessageBox::Ok, this).exec();
    }
}

void MainWindow::resultat(string resultatMap)
{
    QString qstr = QString::fromStdString(resultatMap); //conversion en Qstring
    QHBoxLayout *aff = new QHBoxLayout;
    res.setText("La réponse de votre simulation est : " + qstr);
    aff->addWidget(&res);
    QFont cara("Times", 18);
    res.setFont(cara);
    res.repaint();
    QFrame *laFrame = new QFrame(this);
    laFrame->resize(420, 150);
    laFrame->setFrameShape(QFrame::Box);
    laFrame->setFrameShadow(QFrame::Raised);
    laFrame->move(10, 650); //placement
    laFrame->setLayout(aff);
    update();
    laFrame->show(); //affiche
}

void MainWindow::quandOnSaveR()
{
    if (p.nbCouches == 0)
    {
        QMessageBox(QMessageBox::Warning, "Attention", "Vous ne pouvez pas sauvegarder car aucun Réseau n'a été chargé ou créé", QMessageBox::Ok, this).exec();
        return;
    }
    /*1*/
    //creer le fichier dans le quelle on veut sauvegarder
    cheminDacces = saveFileName();
    /*2*/
    //lance la fonction de sauvegarde du reseau
    if (cheminDacces != "")
    {
        sauvegardeRN(*rUtilisation, cheminDacces.toStdString());
    }
}

void MainWindow::afficheRN(Parametres param, vector<MatrixXd> *mPoids)
{
    //creation la fenetre
    QMainWindow *fenetreRN = new QMainWindow;
    fenetreRN->resize(1800, 1080);
    fenetreRN->setWindowTitle("Tableau des poids du Réseau");
    //creation des tableaux de poids selon les couches
    QTableWidget *entree = new QTableWidget(param.nbNeuronesEntree, param.nbNeuronesCache, fenetreRN);
    QTableWidget *cachees = new QTableWidget(param.nbNeuronesCache * (param.nbCouches - 2), param.nbNeuronesCache, fenetreRN);
    QTableWidget *sortie = new QTableWidget(param.nbNeuronesCache, param.nbNeuronesSortie, fenetreRN);

    cachees->resize(640, 1080);
    entree->resize(640, 1080);
    sortie->resize(640, 1080);
    cachees->move(640, 0);
    sortie->move(1280, 0);

    QTableWidgetItem *all = nullptr; //pointeur pour les items (pour remplir le tableau)
                                     //matrice qui contiendra les couches une par une
    unsigned int largeur;
    unsigned int hauteur;
    unsigned int bordel;
    auto mPoidsBis = mPoids;
    //ajout des poids dans les tableaux
    for (unsigned int i = 0; i < param.nbCouches - 1; i++) //une iteration par couche
    {

        if (!i) //la matrice vers la couche de sortie
        {
            largeur = param.nbNeuronesSortie;
            hauteur = param.nbNeuronesCache;
        }
        else if (i == param.nbCouches - 2) //la matrice de la couche d'entree
        {
            largeur = param.nbNeuronesCache;
            hauteur = param.nbNeuronesEntree;
        }
        else
        { //couche cachee vers couche cachee
            largeur = hauteur = param.nbNeuronesCache - 2;
        }

        MatrixXd mCouches(hauteur, largeur);

        mCouches = mPoidsBis->back();
        for (unsigned int j = 0; j < hauteur; j++) //une iteration par ligne de la matrice
        {
            for (unsigned int k = 0; k < largeur; k++)
            {                                                                //une iteration par colonne de la matrice
                all = new QTableWidgetItem(QString::number(mCouches(j, k))); //on cree un item pour stocker la valeur de la case de la matrice
                if (!i)                                                      //matrice vers couche de sortie
                {
                    sortie->setItem(j, k, all);
                }
                else if (i == param.nbCouches - 2) //matrice de la couche d'entree
                {
                    entree->setItem(j, k, all);
                }
                else
                {
                    bordel = param.nbNeuronesCache * (param.nbCouches - 2 - i); //la ligne de depart, dans le tableau, de la matrice (c'est un BORDEL sans nom ('fin si, du coup))
                    cachees->setItem(j + bordel, k, all);                       //on additione j a la ligne de depart
                }
                //qDebug() << mCouches(j, k);
            }
        }
        mPoidsBis->pop_back();
    }
    fenetreRN->show();
    //delete all;
}

void MainWindow::quandOnSaveS()

{
    if (p.nbCouches == 0)
    {
        QMessageBox(QMessageBox::Warning, "Attention", "Vous ne pouvez pas sauvegarder car aucun Réseau n'a été chargé ou créé", QMessageBox::Ok, this).exec();
        return;
    }
    /*1*/
    //creer le fichier dans le quelle on veut sauvegarder
    cheminDacces = saveFileName();
    /*2*/
    //lance la fonction de sauvegarde du reseau
    if (cheminDacces != "")
    {
        sauvegardeStat(*rUtilisation, cheminDacces.toStdString());
    }
}

MainWindow::~MainWindow() {}