#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "centereditemdelegate.h" //Centrer les contenus du tableau
#include "NiveauItemDelegate.h" //Ajuster le Niveau dans la gestion d'élève


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

 // *********************** Page Ecolage ********************************//
    handCursor = QCursor(Qt::PointingHandCursor);  //Curseur indiquant les boutons cliquables
    //logo
    InsertImage(ui->label, ":/Images/header_logo.png");

    //QListeWidget
    ui->navigation->setStyleSheet("QListWidget::item { color: white; font-weight: bold; font-size: 45px; }");
    ui->navigation->setSpacing(10);

    InsertNav(ui->navigation, ":/C:/Users/AVOTRA/Downloads/pie-chart (1).png", "STATISTIQUES");          //Ajout des menu de navigation à l'aide de
    InsertNav(ui->navigation, ":/Images/settings.png", "ADMINISTRATION");                                //la fonction personnalisé InsertNav()
    InsertNav(ui->navigation, ":/Images/paid.png", "STATUT DE PAIEMENT");

    //Icon Accueil
    InsertImage(ui->img_accueil, ":/C:/Users/AVOTRA/Downloads/list.png");

    //Changement de page
    connect(ui->navigation, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(CliquementItem(QListWidgetItem*)));


//*************************** Page de login d'Administration ******************************//
    //Icône d'administrateur
    InsertImage(ui->icon_admin, ":/Images/user.png");

    //Icône du mot de passe
    InsertImage(ui->icon_password, ":/Images/padlock (1).png");

    //Figure pour remplir le login
    InsertImage(ui->remplissage, ":/Images/blogger.png");

    //Connexion Base de donnée
    bd = QSqlDatabase::addDatabase("QSQLITE");
    bd.setDatabaseName("E:/SQLiteDB/EcoManagement.db");
    if(!bd.open())
        QMessageBox::critical(this, "Erreur", "Base de données pas ouverte");

    //Permet de vérifier l'authentification en appuyant sur Entrée
    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Return), this);// Créer un raccourci clavier pour la touche "Entrée" (Qt::Key_Enter)

    // Connecter le raccourci au slot de clic du bouton
    connect(shortcut, &QShortcut::activated, this, [this]() {
        ui->pushButton_2->click();
    });

    //Cureseur de main indiquant que le bouton "Connexion" est cliquable
    ui->pushButton_2->setCursor(Qt::PointingHandCursor);


//*************************** Page d'Administration ******************************//
    //Insertion de l'image d'un laptop pour l'illustration de la page
    InsertImage(ui->laptop, ":/Images/design-software.png");

    // Installez un gestionnaire d'événements pour le bouton
    ui->Bouton->installEventFilter(this);

    //Stickers d'illustration
    InsertImage(ui->emoji, ":/Images/kind.png");

    //Cureseur de main indiquant que les deux boutons sont cliquables
    ui->Bouton->setCursor(Qt::PointingHandCursor);
    ui->Bouton_2->setCursor(Qt::PointingHandCursor);

//*************************** Gestion des élèves ******************************//

    //Création d' un tableau généré par SQLite qui est éditable
    gestionEleve();

    /* if(bd.open()){
        QSqlTableModel *editableModel = new QSqlTableModel(this);
        editableModel->setTable("ELEVE");
        editableModel->setEditStrategy(QSqlTableModel::OnFieldChange);

        if (editableModel->select()) {
            ui->tableau->setModel(editableModel);
            ui->tableau->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);

            // Appliquez le CentreItemDelegate pour centrer le contenu des cellules
            ui->tableau->setItemDelegate(new CenteredItemDelegate);
            // Ajuster les colonnes pour qu'elles aient la même taille
            int numCols = editableModel->columnCount();
            for (int col = 0; col < numCols; ++col) {
                ui->tableau->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
            }
        }
    }
*/
    // Chemin vers mon fichier QSS pour personnaliser le tableau
    QString qssPath = "E:/SQLiteDB/style.qss";

    // Lire le contenu du fichier QSS
    QFile qssFile(qssPath);
    if (qssFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&qssFile);
        QString styleSheet = stream.readAll();
        qssFile.close();

        // Appliquer le style au QTableWidget
        ui->tableau->setStyleSheet(styleSheet);

        // Supprimer la colonne de numérotation des lignes
        ui->tableau->verticalHeader()->setVisible(false);       
    }

    //Icône du titre
    InsertImage(ui->principalIcon, ":/Images/learning.png");

    //Cureseur de main indiquant que l'Icône de loupe, d'ajout, de suppression et d'enregistrement sont cliquables
    ui->pushButton_14->setCursor(Qt::PointingHandCursor);
    ui->ajoutLigne->setCursor(Qt::PointingHandCursor);
    ui->supprimer->setCursor(Qt::PointingHandCursor);
    ui->Enregistrer->setCursor(Qt::PointingHandCursor);

    //Empêcher la saisie des caractères non numériques pour le cas d'une recherche par matricule
    if(ui->comboBox_5->currentText() == "Matricule"){
         QIntValidator *validator = new QIntValidator();
         ui->recherche_bar->setValidator(validator);
         ui->recherche_bar->setPlaceholderText(QString("Recherche par matricule"));
    }else{
         ui->recherche_bar->setValidator(nullptr);
         ui->recherche_bar->setPlaceholderText(QString("Recherche par nom"));
    }

 //*************************** Gestion d'écolage---->Paramètre d'écolage******************************//
    //Cureseur de main indiquant que boutons cliquables
    ui->pushButton_5->setCursor(handCursor);
    ui->pushButton_6->setCursor(handCursor);
    ui->pushButton_15->setCursor(handCursor);
    ui->pushButton_16->setCursor(handCursor);
    ui->pushButton_17->setCursor(handCursor);
    ui->pushButton_24->setCursor(handCursor);

    //Détermination du mois de début et de fin d'année scolaire
    mois[0] = "Janvier"; mois[1] = "Février"; mois[2] = "Mars"; mois[3] = "Avril"; mois[4] = "Mai";
    mois[5] = "Juin"; mois[6] = "Juillet"; mois[7] = "Août"; mois[8] = "Septembre"; mois[9] = "Octobre";
    mois[10] = "Novembre"; mois[11] = "Décembre";

    //Ajouter tous les mois
    for(int l = 0; l < 12; l++){
        ui->fin->setItemText(l, mois[l]);            //ui->fin->addItem(mois[l]);
    }

    //Initialisée l'année scolaire à celle contenue dans la base de données
    QSqlQuery anneeSco;
    //ui->annee2->setText("Bonjour");
    anneeSco.prepare("SELECT anneeScolaire, Debut, Fin FROM ANNEE_SCOLAIRE");
    if(anneeSco.exec() && anneeSco.last()){
            QString Ascolaire = anneeSco.value(0).toString(), debut = anneeSco.value(1).toString();
            QString fin = anneeSco.value(2).toString();
            // QMessageBox::information(this,"Test", fin);
            ui->annee2->setText(Ascolaire);
            ui->debut->setCurrentText(debut);
            ui->fin->setCurrentText(fin);
    }else{
         QMessageBox::critical(this, "Erreur", "Impossible de charger l'année scolaire depuis la base de données\n"
                                               "" + anneeSco.lastError().text());
    }

    for(int v = 0; v < 12; v++){
        if(ui->debut->currentText() == mois[v]){
             for(int h = 0; h < 12; h++){
                if(ui->fin->itemText(h) == mois[v]){
                    ui->fin->removeItem(h);
                }
             }
        }
    }
    //Création du modèle de tableau pour l'affichage de la table
    QSqlTableModel *historiqueModel = new QSqlTableModel(this);
    historiqueModel->setTable("NIVEAU_ACADEMIQUE");
    historiqueModel->setEditStrategy(QSqlTableModel::OnFieldChange);

    if (historiqueModel->select()) {
        ui->tableauParametre->setModel(historiqueModel);
        ui->tableauParametre->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);

        // Supprimer la colonne de numérotation des lignes
        ui->tableauParametre->verticalHeader()->setVisible(false);

        // Appliquez le CentreItemDelegate pour centrer le contenu des cellules
        ui->tableauParametre->setItemDelegate(new CenteredItemDelegate);

        // Ajuster les colonnes pour qu'elles aient la même taille
        int numCols = historiqueModel->columnCount();
        for (int col = 0; col < numCols; ++col) {
            ui->tableauParametre->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
        }
    }

    //Personnalisation  du tableau de montant de l'écolage
    ui->tableauParametre->setAlternatingRowColors(true);
    ui->tableauParametre->setStyleSheet("QHeaderView::section{ background-color: #016B60; font-weight: bold; color: white;}"
                                        "QTableView::item:alternate { background-color: #DFE9E8;}"
                                        "QTableView::item { background-color: #FFFFFF;}");
    //Icône de paramètre pour le titre
    InsertImage(ui->setting, ":/Images/setting.png");

//Gestion des remises et des réductions

    //Caché le nombre de ligne
    ui->tableWidget_2->verticalHeader()->hide();

    // Ajuster les colonnes pour qu'elles aient la même taille
    int nbCol = ui->tableWidget_2->columnCount();
    for (int colonne = 0; colonne < nbCol; ++colonne) {
        ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(colonne, QHeaderView::Stretch);
    }

    // Appliquez le CentreItemDelegate pour centrer le contenu des cellules
    ui->tableWidget_2->setItemDelegate(new CenteredItemDelegate);

    //Charger le tableau des réductions avec les données qui existent déjà
    afficheReduction();

    //Gestion de réduction
    //Création du modèle de tableau pour l'affichage de la table
    QSqlTableModel *reduction = new QSqlTableModel(this);
    reduction->setTable("REDUCTION");
    reduction->setEditStrategy(QSqlTableModel::OnFieldChange);

    if (reduction->select()) {
        ui->tableReduction->setModel(reduction);
        ui->tableReduction->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);

        // Supprimer la colonne de numérotation des lignes
        ui->tableReduction->verticalHeader()->setVisible(false);

        // Appliquez le CentreItemDelegate pour centrer le contenu des cellules
        ui->tableReduction->setItemDelegate(new CenteredItemDelegate);

        // Ajuster les colonnes pour qu'elles aient la même taille
        int numCols = reduction->columnCount();
        for (int col = 0; col < numCols; ++col) {
            ui->tableReduction->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
        }

        reduction->setHeaderData(3, Qt::Horizontal, "Durée de Validité");

        ui->tableReduction->hideColumn(0);

        setStyleSheet("QHeaderView::section{ background-color: #23A0DE; font-weight: bold; color: white;}");
    }

    //ComboBox raison
    QSqlQuery existeRaison;
    existeRaison.prepare("SELECT Raison FROM REDUCTION ORDER BY idREDUCTION ASC");
    if(existeRaison.exec()){
        while(existeRaison.next()){
            QString raison = existeRaison.value(0).toString();
            ui->comboBox_6->addItem(raison);
        }
    }

    //Icône paramètre réduction
    InsertImage(ui->reductionParameter, ":/C:/Users/AVOTRA/Downloads/adjust.png");

    //Pour filtrer les caractères qui peuvent être saisit dans la duree et le taux dans le paramètre de réduction
    QIntValidator *intFilter = new QIntValidator(this);
    ui->duree2->setValidator(intFilter);
    ui->taux2->setValidator(intFilter);

    //Pour éviter la saisi des caractères non numériques dans le champ qui est déstiné pour les matricules initialement
    QIntValidator *validator = new QIntValidator(this);
    ui->recherche_bar_2->setValidator(validator);


    //*************************** Gestion d'écolage---->Enregistrement de Paiement******************************//

    //Pour que l'année scolaire dans l'enregistrement de paiement correspond à celle qui est règler dans le Paramètre
    ui->annee->setText(ui->annee2->text());

    //Empêcher la saisie des caractères non numériques dans le champ de la matricule
    QIntValidator *validation = new QIntValidator();
    ui->matricule->setValidator(validation);

    //Curseur de main au survole des boutons cliquables
    ui->pushButton_10->setCursor(Qt::PointingHandCursor);
    ui->pushButton_11->setCursor(Qt::PointingHandCursor);
    ui->pushButton_9->setCursor(Qt::PointingHandCursor);

    //Isolé la date à la date d'aujourd'hui
    // Obtenez la date d'aujourd'hui
    QDate currentDate = QDate::currentDate();

    // Définissez la date d'aujourd'hui dans le QDateEdit
    ui->date->setDate(currentDate);

    // Désactivez la possibilité de modification de la date
    ui->date->setDisabled(true);

    //Désactiver le check box initialement
    ui->check_1->setCheckable(false);
    ui->check_2->setCheckable(false);
    ui->check_3->setCheckable(false);
    ui->check_4->setCheckable(false);
    ui->check_5->setCheckable(false);
    ui->check_6->setCheckable(false);
    ui->check_7->setCheckable(false);
    ui->check_8->setCheckable(false);
    ui->check_9->setCheckable(false);
    ui->check_10->setCheckable(false);
    ui->check_11->setCheckable(false);
    ui->check_12->setCheckable(false);

    //Récupération des noms du checkbox des mois
    //  QCheckBox *checkBoxes[10];
    checkBoxes[0] = ui->check_1;
    checkBoxes[1] = ui->check_2;
    checkBoxes[2] = ui->check_3;
    checkBoxes[3] = ui->check_4;
    checkBoxes[4] = ui->check_5;
    checkBoxes[5] = ui->check_6;
    checkBoxes[6] = ui->check_7;
    checkBoxes[7] = ui->check_8;
    checkBoxes[8] = ui->check_9;
    checkBoxes[9] = ui->check_10;
    checkBoxes[10] = ui->check_11;
    checkBoxes[11] = ui->check_12;

    afficheCheckbox();

    //Icône de Paiement d'écolage
    InsertImage(ui->iconPaiement, ":/Images/money.png");

//*************************** Gestion d'écolage---->tableau de suivi******************************//
    //Définir l'année scolaire en fonction de ce qui est conservée dans le paramètre
    ui->anneeSco->setText(ui->annee2->text());

    //Curseur de main au survole des boutons cliquables
    ui->paiement1->setCursor(handCursor);
    ui->paramEco1->setCursor(handCursor);

    InsertImage(ui->tableIcon, ":/C:/Users/AVOTRA/Downloads/stats.png");

    afficheTableau(2, 2, 14, ui->tableWidget);

    ui->anneeSco2->setText(ui->annee->text()); //Année Scolaire Paramètre réduction

//******************************** Statistiques **********************************//
    //Icône d'illustration
    InsertImage(ui->student, ":/Images/Eleve.png"); //Icône élève
    InsertImage(ui->payee, ":/C:/Users/AVOTRA/Downloads/paid (1).png"); //Icône payé
    InsertImage(ui->impayee, ":/C:/Users/AVOTRA/Downloads/bankruptcy.png"); //Icône impayé
    InsertImage(ui->stat, ":/C:/Users/AVOTRA/Downloads/pie-chart.png"); //Icône statistique
    InsertImage(ui->paidMensuel, ":/C:/Users/AVOTRA/Downloads/money-bag.png"); //Icône revenu mensuel
    InsertImage(ui->paidAnnuel, ":/C:/Users/AVOTRA/Downloads/money-bag (1).png"); //Icône revenu annuel
    InsertImage(ui->Status, ":/C:/Users/AVOTRA/Downloads/delete.png"); //Icône status

    //Pour cacher le calendrier par initialement
    ui->calendarWidget->hide();
    nbClic = 0; //Initialiser à 0 le nombre de clic

    // Créez un QPieSeries initial avec des valeurs fictives
    series = new QPieSeries;
    series->setHoleSize(0.5);
    mettreAJourDonutChart(40, 60); // Exemple : 40% Payés, 60% Impayés

    chart = new QChart();
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setTitle("Taux de paiement d'écolage");

    QChartView *chartview = new QChartView(chart);
    chartview->setRenderHint(QPainter::Antialiasing);
    chartview->setFixedSize(771, 391);
    chartview->setParent(ui->horizontalFrame);


    //Remplissage de l'année scolaire
    QSqlQuery comboAnnee;
    comboAnnee.prepare("SELECT anneeScolaire FROM ANNEE_SCOLAIRE ORDER BY anneeScolaire DESC");
    if(comboAnnee.exec()){
        while(comboAnnee.next()){
            QString item = comboAnnee.value(0).toString();
            ui->anneeCourant->addItem(item);
        }
    }else QMessageBox::critical(this, "Erreur", "Anneé Scolaire non sélectionnée\n" + comboAnnee.lastError().text());

    //Arranger le calendrier
    date();

    //Afficher le nombre d'élève, payé et non payé
    statistique();

    QLineSeries *line = new QLineSeries();

    line->append(0, 6);
    line->append(2, 4);
    line->append(3, 8);
    line->append(7, 4);
    line->append(10, 5);

    *line << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(12, 4)
          << QPointF(15, 2) ;
    QChart *graph = new QChart();
    //graph->legend()->hide();
    graph->legend()->setVisible(true);
    graph->legend()->setAlignment(Qt::AlignBottom);
    graph->addSeries(line);
    graph->createDefaultAxes();
    graph->setTitle("Fréquence de paiement");

    QChartView *vue = new QChartView(graph);
    vue->setRenderHint(QPainter::Antialiasing);
    vue->setFixedSize(481, 391);
   // vue->setParent(ui->horizontalFrame2);

//******************************** Statut de Paiement **********************************//
    //Personnalisation du tableau
    int nbColonnes = ui->historique->columnCount();
    for (int col = 0; col < nbColonnes; ++col) {
        ui->historique->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
    }
    ui->historique->verticalHeader()->hide(); //Cacher le verticalheader
    // Appliquez le CentreItemDelegate pour centrer le contenu des cellules
    ui->historique->setItemDelegate(new CenteredItemDelegate);
    //Personnalisation  du tableau de montant de l'écolage
    ui->historique->setAlternatingRowColors(true);
    ui->historique->setStyleSheet("QHeaderView::section{ background-color: #0B2048; font-weight: bold; color: white;}"
                                        "QTableView::item:alternate { background-color: #DFE9E8;}"
                                        "QTableView::item { background-color: #FFFFFF;}");

    ui->pushButton->setCursor(handCursor); //Curseur indiquant un élément cliquable

    //Remplissage de l'année scolaire
    historiqueAnneeScolaire();

    //Remplissage du mois
    historiqueMois();

    //Remplissage global du tableau
    historiqueTableau();

    ui->pushButton_20->setCursor(handCursor);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//******************************** Définition des fonctions **********************************//

//Fonction pour insérer un menu dans le QListWidget (la barre de navigation)
void MainWindow::InsertNav(QListWidget *etiquette, const QString &ficPath, const QString menu)
{
    QListWidgetItem *item = new QListWidgetItem(QIcon(ficPath),menu);
    etiquette->addItem(item);
}

//Fonction pour ajouter un image sur un QLbel
void MainWindow::InsertImage(QLabel *label, const QString &imagePath)
{
    QPixmap picture(imagePath);
    int width = label->width();
    int height = label->height();
    label->setPixmap(picture.scaled(width, height, Qt::KeepAspectRatio));
}

//Définition de la fonction pour le changement de page au cliquement sur les menus dans le QStackedWidget
void MainWindow::CliquementItem(QListWidgetItem *item)
{
    // Récupérez le texte de l'élément de menu sélectionné
    QString menuText = item->text();

    if (menuText == "STATISTIQUES")
    {
        //Remplissage de l'année scolaire
        QSqlQuery comboAnnee;
        bool anneeExistDeja = false;
        comboAnnee.prepare("SELECT anneeScolaire FROM ANNEE_SCOLAIRE ORDER BY anneeScolaire DESC");
        if(comboAnnee.exec()){
            while(comboAnnee.next()){
                QString item = comboAnnee.value(0).toString();
                for(int i = 0; i < ui->anneeCourant->count(); ++i){
                    if(ui->anneeCourant->itemText(i) == item){
                        anneeExistDeja = true;
                        break;
                    }
                }
                if(anneeExistDeja == false){
                    ui->anneeCourant->addItem(item);
                    if(item == ui->annee->text()){
                        ui->anneeCourant->setCurrentText(item);
                    }
                }
            }
        }else QMessageBox::critical(this, "Erreur", "Anneé Scolaire non sélectionnée\n" + comboAnnee.lastError().text());

        //Arranger le calendrier
        date();

        //Afficher le nombre d'élève, payé et non payé
        statistique();
       ui->stackedWidget->setCurrentIndex(0);
    }

    else if (menuText == "ADMINISTRATION")
    {
        // Changez de page dans le QStackedWidget
       ui->stackedWidget->setCurrentIndex(2);
    }

    else
    {
       ui->Annee_Scolaire->clear();
       ui->historique->clearContents();
       //Remplissage de l'année scolaire
       historiqueAnneeScolaire();

       //Remplissage du mois
       historiqueMois();

       //Remplissage global du tableau
       historiqueTableau();
       // Changez de page dans le QStackedWidget

       ui->stackedWidget->setCurrentIndex(1);
    }
}

//Affichage du mot de passe
void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if(arg1)
        ui->password->setEchoMode(QLineEdit::Normal);
    else
        ui->password->setEchoMode(QLineEdit::Password);
}


//Pour cacher les icônes d'administrateur et de mot de passe lorsque le curseur entre dans le QLineEdit
void MainWindow::on_NomAcces_cursorPositionChanged()
{
        ui->icon_admin->hide();
        ui->NomAcces->setPlaceholderText("");
}

void MainWindow::on_password_cursorPositionChanged()
{
    ui->icon_password->hide();
}

//Fonction pour Vérifier l'authentification dans la section Administration
void MainWindow::on_pushButton_2_clicked()
{
    if(bd.open())
    {
        QString username = ui->NomAcces->text();
        QString password = ui->password->text();

        QSqlQuery query;
        query.prepare("SELECT * FROM UTILISATEUR WHERE NiveauAcces = :username AND Password = :password");
        query.bindValue(":username", username);
        query.bindValue(":password", password);

        if (query.exec()) {
            if (query.next()) {
                ui->stackedWidget->setCurrentIndex(4);
                ui->NomAcces->setText("");
                ui->password->setText("");
                ui->ErrorMessage->setText("");
            } else {
                ui->ErrorMessage->setText("Echec d'authentification");
            }
        }
    }

}


void MainWindow::on_Bouton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}

//Remplir l'Année Scolaire dans le statut de paiement
void MainWindow::historiqueAnneeScolaire(){
    QSqlQuery anneeScolaire;
    anneeScolaire.prepare("SELECT anneeScolaire, Debut, Fin, Duree FROM ANNEE_SCOLAIRE ORDER BY anneeScolaire DESC");
    if(anneeScolaire.exec()){
        while(anneeScolaire.next()){
            QString item = anneeScolaire.value(0).toString();
            ui->Annee_Scolaire->addItem(item);
        }
    }else QMessageBox::critical(this, "Erreur", "Anneé Scolaire non sélectionnée\n" + anneeScolaire.lastError().text());
}

//Remplir le mois dans le statut de paiement
void MainWindow::historiqueMois(){
    QSqlQuery moisCorrespondant;
    QString debutAS, finAS;
    int dureeAS;
    QString anneeCourant = ui->Annee_Scolaire->currentText();
    moisCorrespondant.prepare("SELECT Debut, Fin, Duree FROM ANNEE_SCOLAIRE WHERE anneeScolaire = :anneeCourante");
    moisCorrespondant.bindValue(":anneeCourante", anneeCourant);
    if(moisCorrespondant.exec() && moisCorrespondant.first()){
        debutAS = moisCorrespondant.value(0).toString();
        finAS = moisCorrespondant.value(1).toString();
        dureeAS = moisCorrespondant.value(2).toInt();
        //QMessageBox::information(this, "Information",debutAS + " " + finAS + " " + QString::number(dureeAS));
    }else QMessageBox::critical(this, "Erreur", "Année Scolaire non sélectionnée\n" + moisCorrespondant.lastError().text());
    int indiceDebutAS = 0;
    for(int j = 0; j < 12; j++){ //Récupérer l'indicce du début de l'année scolaire
        if(mois[j] == debutAS) indiceDebutAS = j;
    }
    ui->MoisPaiement->addItem("Tous les mois"); //Premier item du comboBox du mois
    for(int k = 0; k < dureeAS; k++){ //Remplir le comboBox du mois
        if(indiceDebutAS == 12) indiceDebutAS = 0;
        ui->MoisPaiement->addItem(mois[indiceDebutAS]);
        indiceDebutAS++;
    }
}

//Permet de remplir le tableau dans le statut de paiement
void MainWindow::historiqueTableau(){
    InsertImage(ui->paid, ":/Images/check.png"); //Icon pour le statut payé
    InsertImage(ui->unpaid, ":/Images/close.png"); // Icone Non Payé
    InsertImage(ui->dashed, ":/C:/Users/AVOTRA/Downloads/remove (1).png"); // Icone Sans information
    ui->paid->hide();
    ui->unpaid->hide();
    ui->dashed->hide();
    int ligne = 0; //Initialiser à 0 l'indice de la ligne
    QString annee_scolaire = ui->Annee_Scolaire->currentText();
    QString moisSpecifie = ui->MoisPaiement->currentText();

    if(annee_scolaire == ui->annee->text()){ //Vérifier si l'Année Scolaire affichée correspond à celle de l'année scolaire actuelle
        if(moisSpecifie != "Tous les mois"){ //Vérifier si le mois est spécifié
            QSqlQuery eleve;
            eleve.prepare("SELECT Matricule, Nom, Prénom, Classe, Statut, idREDUCTION FROM ELEVE");
                if(eleve.exec()){
                while(eleve.next()){
                    QString matricule = eleve.value(0).toString(), name = eleve.value(1).toString(), prenom = eleve.value(2).toString();
                    QString classe = eleve.value(3).toString(), statut = eleve.value(4).toString();
                    QString id = eleve.value(5).toString();
                    if(statut != "parti") statut = "actif";
                    else statut = "inactif";
                    ui->historique->insertRow(ligne); //Insérer une nouvelle ligne
                    ui->historique->setItem(ligne, 0, new QTableWidgetItem(matricule)); //Ajout de la matricule
                    ui->historique->setItem(ligne, 1, new QTableWidgetItem(name)); //Ajout du nom
                    ui->historique->setItem(ligne, 2, new QTableWidgetItem(prenom)); //Ajout du prénom
                    ui->historique->setItem(ligne, 3, new QTableWidgetItem(statut)); //Ajout du statut scolaire
                    ui->historique->setItem(ligne, 5, new QTableWidgetItem(moisSpecifie)); //Ajout du mois
                    QString valeur = "Non";
                    ui->historique->setItem(ligne, 9, new QTableWidgetItem(valeur)); //Pour la colonne réduction

                    //Vérifier si l'idREDUCTION n'est pas null et affiché les informations de cette réduction
                    bool moisValide = false; //Pour savoir si le mois fait partie de la durée de validité de la réduction
                    if(!id.isNull()){
                        //Récupérer le début de l'année scolaire pour initialiser le compte de la durée de validitée
                        QSqlQuery debut;
                        QString moisDebut;
                        int indexDebut;
                        debut.prepare("SELECT Debut FROM ANNEE_SCOLAIRE WHERE anneeScolaire = :annee");
                        debut.bindValue(":annee", annee_scolaire);
                        if(debut.exec() && debut.first()) moisDebut = debut.value(0).toString();
                        //Récupérer l'indice du début de l'année scolaire
                        for(int n = 0; n < 12; n++){
                            if(mois[n] == moisDebut) indexDebut = n;
                        }

                        QSqlQuery infoReduction;
                        infoReduction.prepare("SELECT Raison, Taux, DureeValiditee FROM REDUCTION WHERE idREDUCTION = :id");
                        infoReduction.bindValue(":id", id);
                        if(infoReduction.exec() && infoReduction.first()){
                            QString raison = infoReduction.value(0).toString(), taux = infoReduction.value(1).toString();
                            int dureeValiditee = infoReduction.value(2).toInt();
                            QString valeur = "Oui";
                            //Vérifier si le mois est valide pour la réduction
                            for(int k = 0; k < dureeValiditee; k++){
                                if(indexDebut == 12) indexDebut = 0;
                                if(moisSpecifie == mois[indexDebut]){
                                    moisValide = true;
                                    break;
                                }
                                indexDebut++;
                            }
                            if(moisValide){
                                ui->historique->setItem(ligne, 9, new QTableWidgetItem(valeur)); //Pour la colonne réduction
                                ui->historique->setItem(ligne, 10, new QTableWidgetItem(raison)); //Ajout de la Raison
                                ui->historique->setItem(ligne, 11, new QTableWidgetItem(taux + " %")); //Ajout du taux
                            }
                        }
                    }

                    //Récupérer les informations de l'élève sur le mois payé
                    QSqlQuery information;
                    information.prepare("SELECT DatePaiement, Montant FROM HISTORIQUE_PAIEMENT WHERE Matricule = :matricule AND MoisPayes = :mois AND AnneeScolaire = :annee");
                    information.bindValue(":matricule", matricule);
                    information.bindValue(":mois", moisSpecifie);
                    information.bindValue(":annee", annee_scolaire);
                    if(information.exec()){
                        if(information.first()){
                            QString date = information.value(0).toString(), montant = information.value(1).toString();
                            ui->historique->setItem(ligne, 4, new QTableWidgetItem(classe)); //Ajout de la classe
                            QLabel *statutPaiement = new QLabel(); //Pour afficher les icônes ci-dessus
                            statutPaiement->setPixmap(ui->paid->pixmap());
                            statutPaiement->setAlignment(Qt::AlignHCenter);
                            ui->historique->setCellWidget(ligne, 6, statutPaiement); //Ajout de l'icône payé
                            ui->historique->setItem(ligne, 7, new QTableWidgetItem(date)); //Ajout de la date de paiement
                            ui->historique->setItem(ligne, 8, new QTableWidgetItem(montant)); //Ajout du montant
                            //Ajouter un trait si les informations des réductions sont vides
                            QLabel *noInformation[2];
                            for(int m = 0; m < 2; m++){
                                noInformation[m] = new QLabel;
                                noInformation[m]->setPixmap(ui->dashed->pixmap());
                                noInformation[m]->setAlignment(Qt::AlignHCenter);
                            }
                            if(id.isNull() || moisValide == false){
                                ui->historique->setCellWidget(ligne, 10, noInformation[0]); //Ajout Raison
                                ui->historique->setCellWidget(ligne, 11, noInformation[1]); //Ajout Taux
                            }
                        }else{
                            if(statut == "actif"){
                                ui->historique->setItem(ligne, 4, new QTableWidgetItem(classe)); //Ajout de la classe
                                QLabel *statutPaiement = new QLabel(); //Pour afficher les icônes ci-dessus
                                statutPaiement->setPixmap(ui->unpaid->pixmap());
                                statutPaiement->setAlignment(Qt::AlignHCenter);
                                ui->historique->setCellWidget(ligne, 6, statutPaiement); //Ajout de l'icône payé
                                QLabel *Paiements[4];
                                for(int m = 0; m < 4; m++){
                                    Paiements[m] = new QLabel;
                                    Paiements[m]->setPixmap(ui->dashed->pixmap());
                                    Paiements[m]->setAlignment(Qt::AlignHCenter);
                                }
                                ui->historique->setCellWidget(ligne, 7, Paiements[0]); //Ajout Date paiement
                                ui->historique->setCellWidget(ligne, 8, Paiements[1]); //Ajout Montant
                                if(id.isNull() || moisValide == false){
                                    ui->historique->setCellWidget(ligne, 10, Paiements[2]); //Ajout Raison
                                    ui->historique->setCellWidget(ligne, 11, Paiements[3]); //Ajout Taux
                                }
                            }else{
                                QLabel *Paiements[7];
                                for(int m = 0; m < 7; m++){
                                    Paiements[m] = new QLabel;
                                    Paiements[m]->setPixmap(ui->dashed->pixmap());
                                    Paiements[m]->setAlignment(Qt::AlignHCenter);
                                }
                                ui->historique->setCellWidget(ligne, 4, Paiements[0]); //Ajout Classe
                                ui->historique->setCellWidget(ligne, 6, Paiements[1]); //Ajout Statut Ecolage
                                ui->historique->setCellWidget(ligne, 7, Paiements[2]); //Ajout Date paiement
                                ui->historique->setCellWidget(ligne, 8, Paiements[3]); //Ajout Montant
                                ui->historique->setItem(ligne, 9, nullptr); //Enlever le "non" par défaut dans la réduction
                                ui->historique->setCellWidget(ligne, 9, Paiements[4]); //Ajout Réduction
                                ui->historique->setCellWidget(ligne, 10, Paiements[5]); //Ajout Raison
                                ui->historique->setCellWidget(ligne, 11, Paiements[6]); //Ajout Taux
                            }
                        }
                    }
                    ligne++;
                }
            }
        }else{ //Le mois spécifié correspond à "Tous les mois"
            //Récupérer la durée de l'année scolaire
            QSqlQuery dureeAnnee;
            int duree;
            dureeAnnee.prepare("SELECT Duree FROM ANNEE_SCOLAIRE WHERE anneeScolaire = :annee");
            dureeAnnee.bindValue(":annee", annee_scolaire);
            if(dureeAnnee.exec() && dureeAnnee.first()){
                duree = dureeAnnee.value(0).toInt();
            }

            //Récupérer les informations de l'élève
            QSqlQuery eleve;
            int ligne = 0; //Initialiser l'indice de la ligne à 0
            eleve.prepare("SELECT Matricule, Nom, Prénom, Classe, Statut, idREDUCTION FROM ELEVE");
                if(eleve.exec()){
                while(eleve.next()){
                    for(int i = 0; i < duree; ++i) {
                        QString matricule = eleve.value(0).toString(), name = eleve.value(1).toString(), prenom = eleve.value(2).toString();
                        QString classe = eleve.value(3).toString(), statut = eleve.value(4).toString();
                        QString id = eleve.value(5).toString();
                        moisSpecifie = checkBoxes[i]->text(); //Le mois correspondant aux checkbox dans l'enregistrement de paiement
                        if(statut != "parti") statut = "actif";
                        else statut = "inactif";
                        ui->historique->insertRow(ligne); //Insérer une nouvelle ligne
                        ui->historique->setItem(ligne, 0, new QTableWidgetItem(matricule)); //Ajout de la matricule
                        ui->historique->setItem(ligne, 1, new QTableWidgetItem(name)); //Ajout du nom
                        ui->historique->setItem(ligne, 2, new QTableWidgetItem(prenom)); //Ajout du prénom
                        ui->historique->setItem(ligne, 3, new QTableWidgetItem(statut)); //Ajout du statut scolaire
                        ui->historique->setItem(ligne, 5, new QTableWidgetItem(moisSpecifie)); //Ajout du mois
                        QString valeur = "Non";
                        ui->historique->setItem(ligne, 9, new QTableWidgetItem(valeur)); //Mettre "Non" par défaut sur la réduction

                        //Vérifier si l'idREDUCTION n'est pas null et affiché les informations de cette réduction
                        bool moisValide = false; //Pour savoir si le mois fait partie de la durée de validité de la réduction
                        if(!id.isNull()){
                            //Récupérer le début de l'année scolaire pour initialiser le compte de la durée de validitée
                            QSqlQuery debut;
                            QString moisDebut;
                            int indexDebut;
                            debut.prepare("SELECT Debut FROM ANNEE_SCOLAIRE WHERE anneeScolaire = :annee");
                            debut.bindValue(":annee", annee_scolaire);
                            if(debut.exec() && debut.first()) moisDebut = debut.value(0).toString();
                            //Récupérer l'indice du début de l'année scolaire
                            for(int n = 0; n < 12; n++){
                                if(mois[n] == moisDebut) indexDebut = n;
                            }

                            QSqlQuery infoReduction;
                            infoReduction.prepare("SELECT Raison, Taux, DureeValiditee FROM REDUCTION WHERE idREDUCTION = :id");
                            infoReduction.bindValue(":id", id);
                            if(infoReduction.exec() && infoReduction.first()){
                                QString raison = infoReduction.value(0).toString(), taux = infoReduction.value(1).toString();
                                int dureeValiditee = infoReduction.value(2).toInt();
                                QString valeur = "Oui";
                                //Vérifier si le mois est valide pour la réduction
                                for(int k = 0; k < dureeValiditee; k++){
                                    if(indexDebut == 12) indexDebut = 0;
                                    if(moisSpecifie == mois[indexDebut]){
                                        moisValide = true;
                                        break;
                                    }
                                    indexDebut++;
                                }
                                if(moisValide){
                                    ui->historique->setItem(ligne, 9, new QTableWidgetItem(valeur)); //Pour la colonne réduction
                                    ui->historique->setItem(ligne, 10, new QTableWidgetItem(raison)); //Ajout de la Raison
                                    ui->historique->setItem(ligne, 11, new QTableWidgetItem(taux + " %")); //Ajout du taux
                                }
                            }
                        }

                        //Récupérer les informations de l'élève sur le mois payé
                        QSqlQuery information;
                        information.prepare("SELECT DatePaiement, Montant FROM HISTORIQUE_PAIEMENT WHERE Matricule = :matricule AND MoisPayes = :mois AND AnneeScolaire = :annee");
                        information.bindValue(":matricule", matricule);
                        information.bindValue(":mois", moisSpecifie);
                        information.bindValue(":annee", annee_scolaire);
                        if(information.exec()){
                            if(information.first()){
                                QString date = information.value(0).toString(), montant = information.value(1).toString();
                                ui->historique->setItem(ligne, 4, new QTableWidgetItem(classe)); //Ajout de la classe
                                QLabel *statutPaiement = new QLabel(); //Pour afficher les icônes ci-dessus
                                statutPaiement->setPixmap(ui->paid->pixmap());
                                statutPaiement->setAlignment(Qt::AlignHCenter);
                                ui->historique->setCellWidget(ligne, 6, statutPaiement); //Ajout de l'icône payé
                                ui->historique->setItem(ligne, 7, new QTableWidgetItem(date)); //Ajout de la date de paiement
                                ui->historique->setItem(ligne, 8, new QTableWidgetItem(montant)); //Ajout du montant
                                //Ajouter un trait si les informations des réductions sont vides
                                QLabel *noInformation[2];
                                for(int m = 0; m < 2; m++){
                                    noInformation[m] = new QLabel;
                                    noInformation[m]->setPixmap(ui->dashed->pixmap());
                                    noInformation[m]->setAlignment(Qt::AlignHCenter);
                                }
                                if(id.isNull() || moisValide == false){
                                    ui->historique->setCellWidget(ligne, 10, noInformation[0]); //Ajout Raison
                                    ui->historique->setCellWidget(ligne, 11, noInformation[1]); //Ajout Taux
                                }
                            }else{
                                if(statut == "actif"){
                                    ui->historique->setItem(ligne, 4, new QTableWidgetItem(classe)); //Ajout de la classe
                                    QLabel *statutPaiement = new QLabel(); //Pour afficher les icônes ci-dessus
                                    statutPaiement->setPixmap(ui->unpaid->pixmap());
                                    statutPaiement->setAlignment(Qt::AlignHCenter);
                                    ui->historique->setCellWidget(ligne, 6, statutPaiement); //Ajout de l'icône payé
                                    QLabel *Paiements[4];
                                    for(int m = 0; m < 4; m++){
                                        Paiements[m] = new QLabel;
                                        Paiements[m]->setPixmap(ui->dashed->pixmap());
                                        Paiements[m]->setAlignment(Qt::AlignHCenter);
                                    }
                                    ui->historique->setCellWidget(ligne, 7, Paiements[0]); //Ajout Date paiement
                                    ui->historique->setCellWidget(ligne, 8, Paiements[1]); //Ajout Montant
                                    if(id.isNull() || moisValide == false){
                                        ui->historique->setCellWidget(ligne, 10, Paiements[2]); //Ajout Raison
                                        ui->historique->setCellWidget(ligne, 11, Paiements[3]); //Ajout Taux
                                    }
                                }else{
                                    QLabel *Paiements[7];
                                    for(int m = 0; m < 7; m++){
                                        Paiements[m] = new QLabel;
                                        Paiements[m]->setPixmap(ui->dashed->pixmap());
                                        Paiements[m]->setAlignment(Qt::AlignHCenter);
                                    }
                                    ui->historique->setCellWidget(ligne, 4, Paiements[0]); //Ajout Classe
                                    ui->historique->setCellWidget(ligne, 6, Paiements[1]); //Ajout Statut Ecolage
                                    ui->historique->setCellWidget(ligne, 7, Paiements[2]); //Ajout Date paiement
                                    ui->historique->setCellWidget(ligne, 8, Paiements[3]); //Ajout Montant
                                    ui->historique->setItem(ligne, 9, nullptr); //Enlever le "non" par défaut dans la réduction
                                    ui->historique->setCellWidget(ligne, 9, Paiements[4]); //Ajout Réduction
                                    ui->historique->setCellWidget(ligne, 10, Paiements[5]); //Ajout Raison
                                    ui->historique->setCellWidget(ligne, 11, Paiements[6]); //Ajout Taux
                                }
                            }
                        }
                        ligne++;
                    }
                }
            }
        }
    }else{  //Pour les anciennes années scolaires
        if(moisSpecifie != "Tous les mois"){
            QSqlQuery eleve;
            int ligne = 0; //Initialiser l'indice de la ligne à 0
            eleve.prepare("SELECT Matricule, Nom, Prénom, Statut FROM ELEVE");
                if(eleve.exec()){
                while(eleve.next()){
                    QString matricule = eleve.value(0).toString(), name = eleve.value(1).toString(), prenom = eleve.value(2).toString();
                    QString statut = eleve.value(3).toString();
                    if(statut != "parti") statut = "actif";
                    else statut = "inactif";
                    QSqlQuery verifieHistorique;
                    verifieHistorique.prepare("SELECT Classe, Reduction, RaisonReduction, TauxReduction, Montant, DatePaiement FROM HISTORIQUE_PAIEMENT WHERE Matricule = :matricule AND MoisPayes = :mois AND AnneeScolaire = :annee");
                    verifieHistorique.bindValue(":matricule", matricule);
                    verifieHistorique.bindValue(":mois", moisSpecifie);
                    verifieHistorique.bindValue(":annee", annee_scolaire);
                    if(verifieHistorique.exec() && verifieHistorique.next()){
                        QString classe = verifieHistorique.value(0).toString(), reduction = verifieHistorique.value(1).toString();
                        QString raison = verifieHistorique.value(2).toString(), taux = verifieHistorique.value(3).toString() + " %";
                        QString montant = verifieHistorique.value(4).toString(), date = verifieHistorique.value(5).toString();
                        QLabel *statutPaiement = new QLabel();
                        ui->historique->insertRow(ligne); //Insérer une nouvelle ligne

                        if(reduction == "oui"){
                            ui->historique->setItem(ligne, 0, new QTableWidgetItem(matricule)); //Ajout de la matricule
                            ui->historique->setItem(ligne, 1, new QTableWidgetItem(name)); //Ajout du nom
                            ui->historique->setItem(ligne, 2, new QTableWidgetItem(prenom)); //Ajout du prénom
                            ui->historique->setItem(ligne, 3, new QTableWidgetItem(statut)); //Ajout du statut scolaire
                            ui->historique->setItem(ligne, 4, new QTableWidgetItem(classe)); //Ajout de la classe
                            ui->historique->setItem(ligne, 5, new QTableWidgetItem(moisSpecifie)); //Ajout du mois
                            ui->historique->setItem(ligne, 9, new QTableWidgetItem(reduction)); //Ajout de la reduction
                            ui->historique->setItem(ligne, 10, new QTableWidgetItem(raison)); //Ajout de la raison
                            ui->historique->setItem(ligne, 11, new QTableWidgetItem(taux)); //Ajout du taux de réduction
                            statutPaiement->setPixmap(ui->paid->pixmap());
                            statutPaiement->setAlignment(Qt::AlignHCenter);
                            ui->historique->setCellWidget(ligne, 6, statutPaiement); //Ajout de l'icône payé
                            ui->historique->setItem(ligne, 7, new QTableWidgetItem(date)); //Ajout de la date de paiement
                            ui->historique->setItem(ligne, 8, new QTableWidgetItem(montant)); //Ajout du montant
                            ligne++;
                        }else{
                            ui->historique->setItem(ligne, 0, new QTableWidgetItem(matricule)); //Ajout de la matricule
                            ui->historique->setItem(ligne, 1, new QTableWidgetItem(name)); //Ajout du nom
                            ui->historique->setItem(ligne, 2, new QTableWidgetItem(prenom)); //Ajout du prénom
                            ui->historique->setItem(ligne, 3, new QTableWidgetItem(statut)); //Ajout du statut scolaire
                            ui->historique->setItem(ligne, 4, new QTableWidgetItem(classe)); //Ajout de la classe
                            ui->historique->setItem(ligne, 5, new QTableWidgetItem(moisSpecifie)); //Ajout du mois
                            ui->historique->setItem(ligne, 9, new QTableWidgetItem(reduction)); //Ajout de la reduction
                            statutPaiement->setPixmap(ui->paid->pixmap());
                            statutPaiement->setAlignment(Qt::AlignHCenter);
                            ui->historique->setCellWidget(ligne, 6, statutPaiement); //Ajout de l'icône payé
                            ui->historique->setItem(ligne, 7, new QTableWidgetItem(date)); //Ajout de la date de paiement
                            ui->historique->setItem(ligne, 8, new QTableWidgetItem(montant)); //Ajout du montant
                            QLabel *statutPaiements[2];
                            for(int m = 0; m < 2; m++){
                                statutPaiements[m] = new QLabel;
                                statutPaiements[m]->setPixmap(ui->dashed->pixmap());
                                statutPaiements[m]->setAlignment(Qt::AlignHCenter);
                            }
                            ui->historique->setCellWidget(ligne, 10, statutPaiements[0]); //Ajout Raison
                            ui->historique->setCellWidget(ligne, 11, statutPaiements[1]); //Ajout Taux
                            ligne++;
                        }
                    }else{
                        ui->historique->insertRow(ligne); //Insérer une nouvelle ligne
                        ui->historique->setItem(ligne, 0, new QTableWidgetItem(matricule)); //Ajout de la matricule
                        ui->historique->setItem(ligne, 1, new QTableWidgetItem(name)); //Ajout du nom
                        ui->historique->setItem(ligne, 2, new QTableWidgetItem(prenom)); //Ajout du prénom
                        ui->historique->setItem(ligne, 3, new QTableWidgetItem(statut)); //Ajout du statut scolaire
                        //Ajout de l'icône d'un tiret si on n'a plus d'information sur l'élève
                        QLabel *statutPaiement[8];
                        for(int m = 0; m < 7; m++){
                            statutPaiement[m] = new QLabel;
                            statutPaiement[m]->setPixmap(ui->dashed->pixmap());
                            statutPaiement[m]->setAlignment(Qt::AlignHCenter);
                        }
                        ui->historique->setCellWidget(ligne, 4, statutPaiement[0]); //Ajout de la classe
                        ui->historique->setItem(ligne, 5, new QTableWidgetItem(moisSpecifie)); //Ajout du mois
                        ui->historique->setCellWidget(ligne, 6, statutPaiement[1]); //Ajout du statut d'écolage
                        ui->historique->setCellWidget(ligne, 7, statutPaiement[2]); //Ajout de la date de paiement
                        ui->historique->setCellWidget(ligne, 8, statutPaiement[3]); //Ajout du montant
                        ui->historique->setCellWidget(ligne, 9, statutPaiement[4]); //Ajout de la réduction
                        ui->historique->setCellWidget(ligne, 10, statutPaiement[5]); //Ajout de la raison
                        ui->historique->setCellWidget(ligne, 11, statutPaiement[6]); //Ajout Taux
                        ligne++;
                    }

                }
            }
        }else{  //Le mois concerné est "Tous les mois"
            QSqlQuery eleve;
            int ligne = 0; //Initialiser l'indice de la ligne à 0
            eleve.prepare("SELECT Matricule, Nom, Prénom, Statut FROM ELEVE");
                if(eleve.exec()){
                while(eleve.next()){
                    int indice = 1; //Commencer à 1 l'indice du comboBox à parcourir
                    for(int p = indice; p < (ui->MoisPaiement->count()); p++){
                        moisSpecifie = ui->MoisPaiement->itemText(p);
                        QString matricule = eleve.value(0).toString(), name = eleve.value(1).toString(), prenom = eleve.value(2).toString();
                        QString statut = eleve.value(3).toString();
                        if(statut != "parti") statut = "actif";
                        else statut = "inactif";
                        QSqlQuery verifieHistorique;
                        verifieHistorique.prepare("SELECT Classe, Reduction, RaisonReduction, TauxReduction, Montant, DatePaiement FROM HISTORIQUE_PAIEMENT WHERE Matricule = :matricule AND MoisPayes = :mois AND AnneeScolaire = :annee");
                        verifieHistorique.bindValue(":matricule", matricule);
                        verifieHistorique.bindValue(":mois", moisSpecifie);
                        verifieHistorique.bindValue(":annee", annee_scolaire);
                        if(verifieHistorique.exec() && verifieHistorique.next()){
                            QString classe = verifieHistorique.value(0).toString(), reduction = verifieHistorique.value(1).toString();
                            QString raison = verifieHistorique.value(2).toString(), taux = verifieHistorique.value(3).toString() + " %";
                            QString montant = verifieHistorique.value(4).toString(), date = verifieHistorique.value(5).toString();
                            QLabel *statutPaiement = new QLabel();
                            statutPaiement->setPixmap(ui->paid->pixmap());
                            statutPaiement->setAlignment(Qt::AlignHCenter);
                            ui->historique->insertRow(ligne); //Insérer une nouvelle ligne
                            ui->historique->setCellWidget(ligne, 6, statutPaiement); //Ajout de l'icône payé
                            ui->historique->setItem(ligne, 7, new QTableWidgetItem(date)); //Ajout de la date de paiement
                            ui->historique->setItem(ligne, 8, new QTableWidgetItem(montant)); //Ajout du montant

                            if(reduction == "oui"){
                                ui->historique->setItem(ligne, 0, new QTableWidgetItem(matricule)); //Ajout de la matricule
                                ui->historique->setItem(ligne, 1, new QTableWidgetItem(name)); //Ajout du nom
                                ui->historique->setItem(ligne, 2, new QTableWidgetItem(prenom)); //Ajout du prénom
                                ui->historique->setItem(ligne, 3, new QTableWidgetItem(statut)); //Ajout du statut scolaire
                                ui->historique->setItem(ligne, 4, new QTableWidgetItem(classe)); //Ajout de la classe
                                ui->historique->setItem(ligne, 5, new QTableWidgetItem(moisSpecifie)); //Ajout du mois
                                ui->historique->setItem(ligne, 9, new QTableWidgetItem(reduction)); //Ajout de la reduction
                                ui->historique->setItem(ligne, 10, new QTableWidgetItem(raison)); //Ajout de la raison
                                ui->historique->setItem(ligne, 11, new QTableWidgetItem(taux)); //Ajout du taux de réduction
                            }else{
                                ui->historique->setItem(ligne, 0, new QTableWidgetItem(matricule)); //Ajout de la matricule
                                ui->historique->setItem(ligne, 1, new QTableWidgetItem(name)); //Ajout du nom
                                ui->historique->setItem(ligne, 2, new QTableWidgetItem(prenom)); //Ajout du prénom
                                ui->historique->setItem(ligne, 3, new QTableWidgetItem(statut)); //Ajout du statut scolaire
                                ui->historique->setItem(ligne, 4, new QTableWidgetItem(classe)); //Ajout de la classe
                                ui->historique->setItem(ligne, 5, new QTableWidgetItem(moisSpecifie)); //Ajout du mois
                                ui->historique->setItem(ligne, 9, new QTableWidgetItem(reduction)); //Ajout de la reduction
                            }
                            ligne++;
                        }else{
                            ui->historique->insertRow(ligne); //Insérer une nouvelle ligne
                            ui->historique->setItem(ligne, 0, new QTableWidgetItem(matricule)); //Ajout de la matricule
                            ui->historique->setItem(ligne, 1, new QTableWidgetItem(name)); //Ajout du nom
                            ui->historique->setItem(ligne, 2, new QTableWidgetItem(prenom)); //Ajout du prénom
                            ui->historique->setItem(ligne, 3, new QTableWidgetItem(statut)); //Ajout du statut scolaire
                            //Ajout de l'icône d'un tiret si on n'a plus d'information sur l'élève
                            QLabel *statutPaiement[8];
                            for(int m = 0; m < 8; m++){
                                statutPaiement[m] = new QLabel;
                                statutPaiement[m]->setPixmap(ui->dashed->pixmap());
                                statutPaiement[m]->setAlignment(Qt::AlignHCenter);
                            }
                            ui->historique->setCellWidget(ligne, 4, statutPaiement[0]); //Ajout de la classe
                            ui->historique->setItem(ligne, 5, new QTableWidgetItem(moisSpecifie)); //Ajout du mois
                            ui->historique->setCellWidget(ligne, 6, statutPaiement[2]); //Ajout du statut d'écolage
                            ui->historique->setCellWidget(ligne, 7, statutPaiement[3]); //Ajout de la date de paiement
                            ui->historique->setCellWidget(ligne, 8, statutPaiement[4]); //Ajout du montant
                            ui->historique->setCellWidget(ligne, 9, statutPaiement[5]); //Ajout de la réduction
                            ui->historique->setCellWidget(ligne, 10, statutPaiement[6]); //Ajout de la raison
                            ui->historique->setCellWidget(ligne, 11, statutPaiement[7]); //Ajout Taux
                            ligne++;
                        }
                    }
                }
            }
        }

    }
}


//Permet d'afficher le tableau du gestion des élèves
void MainWindow::gestionEleve(){
    if (bd.open()){
        QSqlTableModel *editableModel = new QSqlTableModel(this);
        editableModel->setTable("ELEVE");
        editableModel->setEditStrategy(QSqlTableModel::OnFieldChange);

        // Appliquer un filtre pour sélectionner uniquement les élèves dont le statut est différent de "renvoyé"
        editableModel->setFilter("Statut = 'retenu'");


        QSqlTableModel *niveauModel = new QSqlTableModel(this); // Modèle pour la table "NIVEAU_ACADEMIQUE"
        niveauModel->setTable("NIVEAU_ACADEMIQUE");

        if (editableModel->select() && niveauModel->select()) {
            ui->tableau->setModel(editableModel);
            ui->tableau->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);

            // Ajuster les colonnes pour qu'elles aient la même taille
            int numCols = editableModel->columnCount();
            for (int col = 0; col < numCols; ++col) {
                ui->tableau->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
            }


            // Appliquer le délégué personnalisé à la colonne "Niveau" en passant le modèle "niveauModel"
            ui->tableau->setItemDelegateForColumn(3, new NiveauItemDelegate(niveauModel, ui->tableau));

            // Appliquez le CentreItemDelegate pour centrer le contenu des cellules
            ui->tableau->setItemDelegate(new CenteredItemDelegate);

            //Caché les colonnes non nécessaire
            ui->tableau->hideColumn(8); ui->tableau->hideColumn(9); ui->tableau->hideColumn(3);
            ui->tableau->hideColumn(10); ui->tableau->hideColumn(11);
        }
    }
}

//Permet d'enregistrer les modifications dans le tableau de gestion des élèves dans la base de données(dans ELEVE)
void MainWindow::on_Enregistrer_clicked()
{
    //Pour mettre à jour le statut de l'élève renvoyé
    QSqlTableModel *editableModel = qobject_cast<QSqlTableModel*>(ui->tableau->model());
    QSqlQuery renvoi;
    QDate currentDate = QDate::currentDate();
    QString date = currentDate.toString("dd/MM/yyyy");
    QString statut = "parti"; int nbLigne = editableModel->rowCount();
    renvoi.prepare("UPDATE ELEVE SET Statut = :statut, DateSortie = :date WHERE Matricule = :matricule");
    for(int i = 0; i < nbLigne; i++){
        if(ui->tableau->isRowHidden(i)){
            renvoi.bindValue(":statut", statut);
            renvoi.bindValue(":matricule", renvoiMatricule);
            renvoi.bindValue(":date", date);

            if(!renvoi.exec()){
                QMessageBox::critical(this, "Erreur", "Le renvoi de la matricule " + renvoiMatricule + " a échoué");
            }
        }
    }

    if (editableModel) {
        // Accédez au modèle de données associé à votre QTableView.
        QAbstractItemModel* model = ui->tableau->model();

        // Assurez-vous que le modèle est non nul et qu'il a des lignes.
        if (model && model->rowCount() > 0) {
            // Parcourez toutes les lignes du modèle.
            for (int row = 0; row < model->rowCount(); ++row) {
                // Récupérez les valeurs de la colonne "Classe" et "Niveau" pour la ligne actuelle.
                QModelIndex classeIndex = model->index(row, 4);
                QModelIndex niveauIndex = model->index(row, 3);

                // Récupérez le contenu de la colonne "Classe".
                QVariant classeValue = model->data(classeIndex);

                // Convertissez le contenu de la colonne "Classe" en QString.
                QString classe = classeValue.toString();
                int longueur = classe.length();

                if(longueur == 4 || longueur == 11){
                    int espaceIndex = classe.indexOf(' ');
                    QString niveau = classe.left(espaceIndex);
                    if(niveau == "PS") niveau = "Petite Section";
                    if(niveau == "MS") niveau = "Moins Section";
                    // Mettez à jour la valeur de la colonne "Niveau" dans le modèle.
                    model->setData(niveauIndex, niveau);
                }
                if(longueur == 7){
                    QString niveau = classe.left(5);
                    model->setData(niveauIndex, niveau);
                }
                if(longueur == 8){
                    QString niveau = classe.left(6);
                    model->setData(niveauIndex, niveau);
                }
            }

            if (!editableModel->submitAll()) {
                QMessageBox::warning(this, "Erreur d'enregistrement", "Impossible d'enregistrer les modifications\n"
                                                                          + editableModel->lastError().text());
            } else {
                QMessageBox::information(this, "Enregistrement", "Enregistrement effectué");
            }
        }
    }

    editableModel->clear();
    RefreshTable();
    gestionEleve();
}

//Permet l'ajout d'une nouvelle ligne dans le tableau de gestion des élèves
void MainWindow::on_ajoutLigne_clicked()
{
        QSqlTableModel *editableModel = new QSqlTableModel(this);
        editableModel->setTable("ELEVE");
        editableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        // Appliquer un filtre pour sélectionner uniquement les élèves dont le statut est différent de "renvoyé"
        editableModel->setFilter("statut = 'retenu'");


        if (editableModel->select()) {
            int rowCount = editableModel->rowCount();
            editableModel->insertRow(rowCount); // Insérer une nouvelle ligne à la fin du modèle

            // Indiquez au modèle que la nouvelle ligne a été insérée
            QModelIndex newIndex = editableModel->index(rowCount, 0);
            ui->tableau->setCurrentIndex(newIndex);

            // Déclenchez l'édition de la nouvelle ligne
            ui->tableau->edit(newIndex);

            // Mettez à jour la vue
            ui->tableau->setModel(editableModel);

            //Remplir la colonne de la DateEntree
            QAbstractItemModel* model = ui->tableau->model();
            QDate currentDate = QDate::currentDate();
            QString date = currentDate.toString("dd/MM/yyyy");
            QModelIndex dateEntree = model->index(model->rowCount() - 1, 10);
            model->setData(dateEntree, date);

        }
}

//Permet de supprimer une ligne sur le tableau
void MainWindow::on_supprimer_clicked()
{
        QModelIndex currentIndex = ui->tableau->currentIndex();
        // Récupérez l'index de la cellule souhaitée (par exemple, ligne 2, colonne 3)
        QModelIndex cellIndex = ui->tableau->model()->index(currentIndex.row(), 0);

        // Utilisez la méthode data() pour récupérer le contenu de la cellule
        QVariant cellValue = ui->tableau->model()->data(cellIndex);

        // Convertissez la valeur en QString si nécessaire
         renvoiMatricule = cellValue.toString();

        int reponse = QMessageBox::question(this, "Avertissement", "Voulez-vous vraiment supprimer la matricule " + renvoiMatricule,
                              QMessageBox::Yes | QMessageBox::No);

        if(reponse == QMessageBox::Yes){
            if (currentIndex.isValid()) {
                ui->tableau->hideRow(currentIndex.row());
            }
        }
}

//Permet de filtrer les caractères utilisables à chaque choix dans le combo box sur la barre de recherche
void MainWindow::on_comboBox_5_currentIndexChanged(int index)
{
        QStringList comboBoxChoices;
        // Créez un QIntValidator pour limiter la saisie à des nombres entiers
        QIntValidator *validator = new QIntValidator();
        comboBoxChoices << "Matricule" << "Nom";

        if (index > 0 && index < comboBoxChoices.size()) {
            // Utilisez le choix sélectionné pour mettre à jour le texte de placeholder du QLineEdit
            delete validator; //Pour libérer la mémoire du validateur précédent
            ui->recherche_bar->setValidator(nullptr);
            ui->recherche_bar->setPlaceholderText(QString("Recherche par nom"));
        }
        else{
            // Appliquez le validateur à la QLineEdit
            ui->recherche_bar->setValidator(validator);
            ui->recherche_bar->setPlaceholderText(QString("Recherche par matricule"));
        }
}

//Permet de faire les recherches dans le menu gestion d'élève
void MainWindow::on_recherche_bar_textChanged()
{
    if (!bd.open()) {
        QMessageBox::warning(this, "Attention", "La connexion à la base de données a échoué");
        return;
    }

    QString texte = ui->recherche_bar->text();
    QSqlTableModel *editable = new QSqlTableModel(this);
    editable->setTable("ELEVE");
    editable->setEditStrategy(QSqlTableModel::OnFieldChange);
    editable->setFilter("Matricule LIKE '%" + texte + "%' OR Nom LIKE '%" + texte + "%' OR Prénom LIKE '%" + texte + "%' OR (Nom || ' ' || Prénom) LIKE '%" + texte + "%'");
    editable->select(); // Sélectionnez les données filtrées de la table

    if (editable->select()) {
        ui->tableau->setModel(editable);
        ui->tableau->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    }
}


//Changer de page vers la Gestion des élèves
void MainWindow::on_pushButton_4_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}

//Pour montrer la classe et le montant à l-aide de la matricule ainsi que la désactivation du checckbox qui était
//déjà coché auparavant
void MainWindow::on_matricule_textEdited(const QString &arg1)
{
    if(bd.open()){
        //Vérifier si le statut de l'élève est 'retenu'
        QSqlQuery statut;
        QString Statut;
        statut.prepare("SELECT Statut FROM ELEVE WHERE Matricule = :Matricule");
        statut.bindValue(":Matricule", arg1);
        if(statut.exec() && statut.first()) Statut = statut.value(0).toString();

        if(Statut == "retenu"){
                //Apparition de la classe
                QSqlQuery demandes;
                demandes.prepare("SELECT Classe FROM ELEVE WHERE Matricule = :matricule");
                demandes.bindValue(":matricule", arg1);
                if (demandes.exec() && demandes.first()) {
                QString classe = demandes.value(0).toString();
                ui->classe->setText(classe);
                } else {
                ui->classe->setText("");
                }

                QSqlQuery identifiant;
                int id = 0, nbMois = 0, compteMois = 0;
                identifiant.prepare("SELECT idREDUCTION FROM ELEVE WHERE Matricule = :matr");
                identifiant.bindValue(":matr", arg1);
                if(identifiant.exec()){
                if(identifiant.next()){
                    id = identifiant.value(0).toInt();
                   // QMessageBox::information(this, "Test", QString::number(id));
                    if(id > 1){

                        //Récupérer la durée de Validation de la réduction
                        for(int k = 0; k < ui->tableWidget_2->rowCount(); k++){
                            if(ui->tableWidget_2->item(k, 0)->text() == arg1){
                                nbMois = ui->tableWidget_2->item(k, 4)->text().toInt();
                            }
                        }                       

                        //Compter le nombre d'id existante pour la réduction
                        QSqlQuery nbId;
                        int compteID = 0;
                        nbId.prepare("SELECT idREDUCTION FROM REDUCTION");
                        if(nbId.exec()){
                            while(nbId.next()){
                                compteID++;
                            }
                        }                        

                        //Compter le nombre de mois qui était déjà payé(s)
                        QSqlQuery mois;
                        mois.prepare("SELECT MoisPayes FROM HISTORIQUE_PAIEMENT WHERE Matricule = :matri AND AnneeScolaire = :annee");
                        mois.bindValue(":matri", arg1);
                        mois.bindValue(":annee", ui->annee->text());
                        if(mois.exec()){
                            while(mois.next()){
                                compteMois++;
                            }
                            for(int idn = 1; idn <= compteID; idn++){
                                if(id == idn){
                                    if(compteMois < nbMois){
                                        for(int k = 0; k < ui->tableWidget_2->rowCount(); k++){
                                            if(ui->tableWidget_2->item(k, 0)->text() == arg1){
                                                ui->ecolage->setText(ui->tableWidget_2->item(k, 3)->text());
                                                ui->mois->setMaximum(nbMois - compteMois); //Pour régler le nombre de mois maximum en fonction de la durée de validation de la réduction
                                            }
                                        }
                                    }
                                }
                            }
                        }

                    }//else QMessageBox::critical(this, "Erreur", "Problème sur l'enregistrement des réductions");
                }else{
                    ui->ecolage->setText("");
                }
                }

                //Apparition du montant
                if(id < 1 || compteMois >= nbMois){
                QSqlQuery demandes_2;
                demandes_2.prepare("SELECT Niveau FROM ELEVE WHERE Matricule = :mat");
                demandes_2.bindValue(":mat", arg1);
                if(demandes_2.exec()){
                    if(demandes_2.first()){
                        QString niveau = demandes_2.value(0).toString();
                        QSqlQuery demandes_3;
                        demandes_3.prepare("SELECT \"Montant (en Ar)\" FROM NIVEAU_ACADEMIQUE WHERE Niveau = :niveau");
                        demandes_3.bindValue(":niveau", niveau);
                        if(demandes_3.exec() && demandes_3.first()){
                            QString montant = demandes_3.value(0).toString();
                            ui->ecolage->setText(montant);
                        }
                    }
                }else QMessageBox::critical(this, "Erreur", "La requête pour afficher le montant des élèves sans réduction a échoué");
                }


                //Permet de désactiver les checkbox qui ont été déjà coché auparavant
                QSqlQuery demandes_3;
                int checked = 0; //récupération du nb de checbox coché
                QString annee = ui->annee->text();
                bool auMoinsUnEnregistrement = false;
                for(int i = 0; i < 12; i++){
                QString moisCheck = checkBoxes[i]->text();
                demandes_3.prepare("SELECT MoisPayes FROM HISTORIQUE_PAIEMENT WHERE Matricule = :matric AND AnneeScolaire = :annee");
                demandes_3.bindValue(":matric", arg1);
                demandes_3.bindValue(":annee", annee);
                if(demandes_3.exec()){
                    while(demandes_3.next()){
                        QString mois = demandes_3.value(0).toString();
                        if(moisCheck == mois){
                            checkBoxes[i]->setCheckable(true);
                            checkBoxes[i]->setChecked(true);
                            checkBoxes[i]->setEnabled(false);
                            checked++;
                            //Permet de limiter la valeur maximale du spin box
                            if(id < 1 || compteMois >= nbMois){
                                int nbChecboxAffiche = 0;
                                for(int v = 0; v < 12; ++v){
                                    if(checkBoxes[v]->isVisible()) nbChecboxAffiche++;
                                }
                                ui->mois->setMaximum(nbChecboxAffiche - checked);
                            }
                        }
                        auMoinsUnEnregistrement = true;
                    }
                    if(!auMoinsUnEnregistrement){
                        for(int v = 0; v < 12; v++ ){
                            if(!checkBoxes[v]->isEnabled()){
                                checkBoxes[v]->setEnabled(true);
                                checkBoxes[v]->setChecked(false);
                                checkBoxes[v]->setCheckable(false);
                            }else if(checkBoxes[v]->isChecked()){
                                checkBoxes[v]->setChecked(false);
                                checkBoxes[v]->setCheckable(false);
                            }
                        }

                    }

                }else QMessageBox::critical(this, "Erreur", "Problème");
            }
        }
        else{
            ui->classe->setText("");
            ui->ecolage->setText("");
            ui->total->setText("");
            int nbCheckboxVisible = 0;
            for(int v = 0; v < 12; ++v){
              checkBoxes[v]->setEnabled(true);
              checkBoxes[v]->setChecked(false);
              checkBoxes[v]->setCheckable(false);
              if(!checkBoxes[v]->isHidden()) nbCheckboxVisible++;
            }
            ui->mois->setMaximum(nbCheckboxVisible);
        }


    }
}

//Calculer le total du prix d'écolage à payer à partir du nombre de mois et du montant
void MainWindow::on_mois_textChanged(const QString &arg1)
{
    // Initialiser les checkboxes à leurs état d'origine
    for(int i = 0; i < 12; i++){
        if(checkBoxes[i]->isEnabled() || (checkBoxes[i]->isEnabled() && checkBoxes[i]->isChecked())){
            checkBoxes[i]->setChecked(false);
            checkBoxes[i]->setCheckable(false);
        }
    }
    int nbMois = arg1.toInt(), montant = ui->ecolage->text().toInt(), result = nbMois * montant;
    QString conversion = QString::number(result);
     ui->total->setText(conversion);

    //Acivation des cases à cocher non checker en fonction de la valeur du spin box

    int i = 0, compteur = 0;
    while(compteur < nbMois){
        if(checkBoxes[i]->isEnabled()){
            checkBoxes[i]->setCheckable(true);
            compteur++;
        }
        i++;
    }
}

//Permet de mettre à jour la liste des mois de l'année scolaire dans l'enregistrement de paiement
void MainWindow::afficheCheckbox(){
    //Rendre visible tous les checkbox avant le triage
    for(int m = 0; m < 12; m++){
        checkBoxes[m]->setVisible(true);
    }

    //Récupérer le début de l'année scolaire et la fin;
    QSqlQuery recuperation;
    QString start, end;
    recuperation.prepare("SELECT Debut, Fin FROM ANNEE_SCOLAIRE");
    if(recuperation.exec() && recuperation.last()){
        start = recuperation.value(0).toString();
        end = recuperation.value(1).toString();
    }

    int indiceDebut = 0, indiceCheckbox = 0, checkboxUsed = 0;

    //Récupérer l'indice du début de l'année scolaire pour pouvoir l'affiché en 1er dans le checkbox
    for(int g = 0; g < 12; g++){
        if(mois[g] == start){
            indiceDebut = g;
            break;
        }
    }

    //Arranger le checkbox suivant le début et la fin du mois
    while(true){
        if(indiceDebut == 12) indiceDebut = 0; //Réinitialiser à 0 l'indice pour que l'ajout continue tant qu'on n'a pas encore atteint la fin de l'année scolaire
        if(mois[indiceDebut] == end){
            checkBoxes[indiceCheckbox]->setText(end);
            indiceCheckbox++;
            checkboxUsed++;
            break;
        }else{
            checkBoxes[indiceCheckbox]->setText(mois[indiceDebut]);
        }
        indiceDebut++;
        indiceCheckbox++;
        checkboxUsed++;
    }

    for(int t = 0; t < (12 - checkboxUsed); t++)
    {
        checkBoxes[indiceCheckbox]->hide();
        indiceCheckbox++;
    }

    //Limiter la valeur du maximale du spin Box pour le nombre de Mois Payé
    ui->mois->setMaximum(checkboxUsed);
}

//Permet d'enregistrer le paiement dans l'historique
void MainWindow::on_pushButton_9_clicked()
{
    int matricule = ui->matricule->text().toInt(), compte = 0;
    QString classe = ui->classe->text();
    QString date = ui->date->text();
    int montant = ui->ecolage->text().toInt();
    QString annee = ui->annee->text();


    //Récupération du nombre de checkbox cocher pour le mettre dans la condition ci-après
    for(int i = 0; i < 12; i++){
        if(checkBoxes[i]->isEnabled() && checkBoxes[i]->isChecked())
            compte++;
    }
    QString compter = QString::number(compte);

    //Condition avant de pouvoir enregistrer le paiement
    if(ui->matricule->text().isEmpty() || ui->classe->text().isEmpty() || ui->ecolage->text().isEmpty() ||ui->mois->text().isEmpty() || ui->mois->text() == '0' || ui->mois->text() != compter){
        QMessageBox::warning(this, "Avertissement", "Veuiller remplir correctement les champs");
    }else{

            if(bd.open()){
                QSqlQuery requete;
                for(int j = 0; j < 12; j++){
                    if(checkBoxes[j]->isEnabled() && checkBoxes[j]->isChecked()){
                        int revenuID;
                        QString moiS = checkBoxes[j]->text();
                        //Récupérer l'identifiant du revenu pour pouvoir l'insérer dans la table Historique de Paiement(foreign Key)
                        QSqlQuery idRevenu;
                        idRevenu.prepare("SELECT idRevenu FROM HISTORIQUE_REVENU WHERE Mois = :month AND anneeScolaire = :year");
                        idRevenu.bindValue(":month", moiS);
                        idRevenu.bindValue(":year", annee);
                        if(idRevenu.exec() && idRevenu.first()){
                            revenuID = idRevenu.value(0).toInt();
                        }

                        //Vérifier si l'élève obtient une réduction et si le mois est encore valide
                        QString Raison,Taux;
                        QSqlQuery idReduction;
                        idReduction.prepare("SELECT idREDUCTION FROM ELEVE WHERE Matricule = :matricule");
                        idReduction.bindValue(":matricule", matricule);
                        if(idReduction.exec() && idReduction.first()){
                            QString id = idReduction.value(0).toString();
                            bool moisValide = false;
                            if(!id.isNull()){
                                //Récupérer le début de l'année scolaire pour initialiser le compte de la durée de validitée
                                QSqlQuery debut;
                                QString moisDebut;
                                int start = 0;
                                debut.prepare("SELECT Debut FROM ANNEE_SCOLAIRE WHERE anneeScolaire = :annee");
                                debut.bindValue(":annee", annee);
                                if(debut.exec() && debut.first()) moisDebut = debut.value(0).toString();
                                //Récupérer l'indice du début de l'année scolaire
                                for(int f = 0; f < 12; f++){
                                    if(moisDebut == mois[f]) {
                                        start = f;
                                    }
                                }


                                QSqlQuery infoReduction;
                                infoReduction.prepare("SELECT Raison, Taux, DureeValiditee FROM REDUCTION WHERE idREDUCTION = :id");
                                infoReduction.bindValue(":id", id);
                                if(infoReduction.exec() && infoReduction.first()){
                                    QString raison = infoReduction.value(0).toString(), taux = infoReduction.value(1).toString();
                                    int dureeValiditee = infoReduction.value(2).toInt();
                                    //Vérifier si le mois est valide pour la réduction
                                    for(int k = 0; k < dureeValiditee; k++){
                                        if(start == 12) start = 0;
                                        if(moiS == mois[start]){
                                            moisValide = true;
                                            break;
                                        }
                                        start++;
                                    }
                                    if(moisValide){
                                        Raison = raison;
                                        Taux = taux;
                                        //Enregistrement du paiement dans HISTORIQUE_PAIEMENT
                                        requete.prepare("INSERT INTO HISTORIQUE_PAIEMENT (Matricule, Classe, DatePaiement, MoisPayes, Montant, RaisonReduction, TauxReduction, AnneeScolaire, idRevenu, Reduction) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
                                        requete.bindValue(0, matricule);
                                        requete.bindValue(1, classe);
                                        requete.bindValue(2, date);
                                        requete.bindValue(3, checkBoxes[j]->text());
                                        requete.bindValue(4, montant);
                                        requete.bindValue(5, Raison);
                                        requete.bindValue(6, Taux);
                                        requete.bindValue(7, annee);
                                        requete.bindValue(8, revenuID);
                                        requete.bindValue(9, "oui");

                                        // Ne Pas Exécutez la requête SQL
                                        if (!requete.exec()) {
                                            // L'insertion ne s'est pas bien passée
                                            QMessageBox::critical(this, "Erreur", "Historique de Paiement non enregistré\n" + requete.lastError().text());
                                        }
                                    }
                                }
                            } if(id.isNull() || moisValide == false){
                                //Enregistrement du paiement dans HISTORIQUE_PAIEMENT
                                requete.prepare("INSERT INTO HISTORIQUE_PAIEMENT (Matricule, Classe, DatePaiement, MoisPayes, Montant, AnneeScolaire, idRevenu) VALUES (?, ?, ?, ?, ?, ?, ?)");
                                requete.bindValue(0, matricule);
                                requete.bindValue(1, classe);
                                requete.bindValue(2, date);
                                requete.bindValue(3, checkBoxes[j]->text());
                                requete.bindValue(4, montant);
                                requete.bindValue(5, annee);
                                requete.bindValue(6, revenuID);

                                // Ne Pas Exécutez la requête SQL
                                if (!requete.exec()) {
                                    // L'insertion ne s'est pas bien passée
                                    QMessageBox::critical(this, "Erreur", "Historique de Paiement non enregistré\n" + requete.lastError().text());
                                }
                            }
                        }

                        //Total de revenu mensuel
                        QSqlQuery revenu;
                        revenu.prepare("UPDATE HISTORIQUE_REVENU SET Revenu = Revenu + :montant WHERE Mois = :mois AND anneeScolaire = :annee");
                        revenu.bindValue(":montant", montant);
                        revenu.bindValue(":mois", moiS);
                        revenu.bindValue(":annee", annee);
                        if(!revenu.exec()) QMessageBox::critical(this,"Erreur", "Le calcul des revenus a échoué\n" + revenu.lastError().text());
                    }
                }
                QMessageBox::information(this, "Information", "Enregistrement effectué");
                ui->matricule->setText("");
                ui->classe->setText("");
                ui->ecolage->setText("");
                ui->mois->setValue(0);
                ui->total->setText("");
                for(int v = 0; v < 10; v++ ){
                    if(!checkBoxes[v]->isEnabled()){
                        checkBoxes[v]->setEnabled(true);
                        checkBoxes[v]->setChecked(false);
                        checkBoxes[v]->setCheckable(false);
                    }else if(checkBoxes[v]->isChecked()){
                        checkBoxes[v]->setChecked(false);
                        checkBoxes[v]->setCheckable(false);
                    }
                }
            }
        }
    RefreshTable();
}

void MainWindow::afficheTableau(int m, int indiceColonne, int stoppeur, QTableWidget *tableWidget){
    //Rendre visible tous les colonnes avant le triage
    for(int l = m; l < stoppeur; l++){
        tableWidget->showColumn(l);
    }

    //Récupérer le début de l'année scolaire et la fin;
    QSqlQuery recuperation;
    QString start, end;
    recuperation.prepare("SELECT Debut, Fin FROM ANNEE_SCOLAIRE");
    if(recuperation.exec() && recuperation.last()){
            start = recuperation.value(0).toString();
            end = recuperation.value(1).toString();
    }

    int indiceDebut = 0, columnUsed = 0;

    //Récupérer l'indice du début de l'année scolaire pour pouvoir l'affiché en 1er
    for(int g = 0; g < 12; g++){
            if(mois[g] == start){
                indiceDebut = g;
                break;
            }
    }

    //Arrangement suivant le début et la fin du mois
    while(true){
            if(indiceDebut == 12) indiceDebut = 0; //Réinitialiser à 0 l'indice pour que l'ajout continue tant qu'on n'a pas encore atteint la fin de l'année scolaire
            QTableWidgetItem *headerItem = new QTableWidgetItem(mois[indiceDebut]);
            if(mois[indiceDebut] == end){
                tableWidget->setHorizontalHeaderItem(indiceColonne, headerItem);
                indiceColonne++;
                columnUsed++;
                break;
            }else{
                tableWidget->setHorizontalHeaderItem(indiceColonne, headerItem);
            }
            indiceDebut++;
            indiceColonne++;
            columnUsed++;
    }

    for(int t = 0; t < (14 - columnUsed); t++)
    {
            tableWidget->hideColumn(indiceColonne);;
            indiceColonne++;
    }

    // Style pour les en-têtes de colonnes
    QString headerStyle = "QHeaderView::section { background-color: #1D3C26; color: white}"; // Exemple de style
    ui->tableWidget->horizontalHeader()->setStyleSheet(headerStyle);

    // Ajuster les colonnes pour qu'elles aient la même taille
    int nbColumn = ui->tableWidget->columnCount();
    for (int col = 0; col < nbColumn; ++col) {
            ui->tableWidget->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);

    }

    // Supprimer la colonne de numérotation des lignes
    ui->tableWidget->verticalHeader()->setVisible(false);

    // Appliquez le CentreItemDelegate pour centrer le contenu des cellules
    ui->tableWidget->setItemDelegate(new CenteredItemDelegate);

    RefreshTable();
}

//Permet de faire un refresh sur le tableau de suivi
void MainWindow::RefreshTable(){
    ui->tableWidget->setRowCount(0); //Effacer les anciens données du tableWidget
    //Remplissage du QTableWidget
    // Exécutez une requête SQL pour récupérer les matricules de la table ELEVE
    QSqlQuery queryEleve("SELECT Matricule, Classe, Statut FROM ELEVE");
    row = 0; // Compteur de ligne pour le QTableWidget
    QString statut[100];
    QString annees = ui->anneeSco->text();
    InsertImage(ui->yes, ":/Images/check.png"); //Icon pour le statut payé
    InsertImage(ui->no, ":/Images/close.png"); // Icone Non Payé
    InsertImage(ui->noInformation, ":/C:/Users/AVOTRA/Downloads/remove (1).png"); // Icone Non Payé
    ui->yes->hide();
    ui->no->hide();
    ui->noInformation->hide();

    while (queryEleve.next()) {
            QString matricule = queryEleve.value(0).toString();
            QString classe = queryEleve.value(1).toString();
            statut[row] = queryEleve.value(2).toString();
            // Ajoutez une nouvelle ligne dans le QTableWidget
            ui->tableWidget->insertRow(row);

            // Remplissez la première colonne avec le matricule
            ui->tableWidget->setItem(row, 0, new QTableWidgetItem(matricule));

            // Remplissez les autres colonnes avec les données correspondantes
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(classe));
            row++; // Incrémentez le compteur de ligne
    }
    //Mettre à jour le tableau de suivi

        QSqlQuery queryHistorique;
        for(int r = 0; r < row; r++){
            bool inscritAS = false; //Pour vérifier si l'élève qui n'étudie à l'école a bosser pendant quelque mois ou pas avant d'aller
            for(int l = 2; l < 14; l++){
                QString month = ui->tableWidget->horizontalHeaderItem(l)->text();
                QString matricule = ui->tableWidget->item(r, 0)->text();
                queryHistorique.prepare("SELECT * FROM HISTORIQUE_PAIEMENT WHERE Matricule = :matricule AND AnneeScolaire = :annee AND MoisPayes = :month");
                    // "AND AnneeScolaire = :annee");
                queryHistorique.bindValue(":matricule", matricule);
                queryHistorique.bindValue(":annee", annees);
                queryHistorique.bindValue(":month", month);

                if(queryHistorique.exec()){
                    if(queryHistorique.next()){
                        if(statut[r] == "parti") inscritAS = true;
                        QLabel *lbl_item = new QLabel();
                        lbl_item ->setPixmap(ui->yes->pixmap());
                        lbl_item ->setAlignment(Qt::AlignHCenter);
                        ui->tableWidget->setCellWidget(r, l, lbl_item);
                    }else{                       
                        if(statut[r] == "parti"){
                            QLabel *lbl_item2 = new QLabel();
                            lbl_item2 ->setPixmap(ui->noInformation->pixmap());
                            lbl_item2 ->setAlignment(Qt::AlignHCenter);
                            ui->tableWidget->setCellWidget(r, l, lbl_item2);
                        }else{
                            QLabel *lbl_item2 = new QLabel();
                            lbl_item2 ->setPixmap(ui->no->pixmap());
                            lbl_item2 ->setAlignment(Qt::AlignHCenter);
                            ui->tableWidget->setCellWidget(r, l, lbl_item2);
                        }
                    }
                }
                else{
                    QMessageBox::critical(this, "Error", "Query not executed");
                }
            }
            if(inscritAS == false && statut[r] == "parti"){
                delete ui->tableWidget->item(r, 1);
                QLabel *lbl_item2 = new QLabel();
                lbl_item2 ->setPixmap(ui->noInformation->pixmap());
                lbl_item2 ->setAlignment(Qt::AlignHCenter);
                ui->tableWidget->setCellWidget(r, 1, lbl_item2);
            }
        }
}

//Pour faire apparaître la page d'Enregistrement de paiement
void MainWindow::on_Bouton_clicked()
{
    ui->stackedWidget->setCurrentIndex(7);
}

//Pour revenir à la page de choix d'opération dans la page dEnregistrement de paiement
void MainWindow::on_pushButton_12_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}

//Pour afficher la page contenant le tableau de vue depuis l'enregistrement de paiement
void MainWindow::on_pushButton_11_clicked()
{
    afficheTableau(2, 2, 14, ui->tableWidget);
    ui->stackedWidget->setCurrentIndex(6);
}

//Pour afficher la page contenant l'Enregistrement de paiement depuis le tableau de vue
void MainWindow::on_paiement1_clicked()
{
    afficheCheckbox(); //Pour mettre à jour l'année scolaire
    ui->stackedWidget->setCurrentIndex(7);
}



//Permet d'effectuer des recherches à partir de la matricule dans le tableau de suivi
void MainWindow::on_recherche_bar2_textChanged(const QString &arg1)
{
    // Parcourez toutes les lignes de votre QTableWidget
    for (int lin = 0; lin < ui->tableWidget->rowCount(); ++lin) {
            QTableWidgetItem* item = ui->tableWidget->item(lin, 0); // Supposons que la matricule est dans la colonne 0

            if (item) {
                QString matriculeLigne = item->text();

                // Vérifiez si la matricule de cette ligne contient la matricule de recherche
                if (matriculeLigne.contains(arg1, Qt::CaseInsensitive)) {
                    // Affichez la ligne
                    ui->tableWidget->showRow(lin);
                } else {
                    // Cachez la ligne si aucune correspondance n'est trouvée
                    ui->tableWidget->hideRow(lin);
                }
            }
    }

}

//Pour aller à la page de Paramàtre d'écolage à partir d'Enregistrement de Paiement
void MainWindow::on_pushButton_10_clicked()
{
    //Recharger le tableau de montant en fonction du Niveau
    QSqlTableModel *historiqueModel = new QSqlTableModel(this);
    historiqueModel->setTable("NIVEAU_ACADEMIQUE");
    historiqueModel->setEditStrategy(QSqlTableModel::OnFieldChange);

    if (historiqueModel->select()) {
            ui->tableauParametre->setModel(historiqueModel);
            ui->tableauParametre->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);

            // Supprimer la colonne de numérotation des lignes
            ui->tableauParametre->verticalHeader()->setVisible(false);

            // Appliquez le CentreItemDelegate pour centrer le contenu des cellules
            ui->tableauParametre->setItemDelegate(new CenteredItemDelegate);

            // Ajuster les colonnes pour qu'elles aient la même taille
            int numCols = historiqueModel->columnCount();
            for (int col = 0; col < numCols; ++col) {
                ui->tableauParametre->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
            }
    }

    ui->stackedWidget->setCurrentIndex(8);
}


void MainWindow::on_pushButton_8_clicked()
{
     ui->stackedWidget->setCurrentIndex(4);
}

//Permet de gérer le formulaire dans le Paramètre d'écolage (Réduction)
void MainWindow::on_matricule_2_textChanged(const QString &arg1)
{
     if(bd.open()){
            //Permet d'afficher automatiquement la classe quand la matricule est saisit
            QSqlQuery demandes;
            demandes.prepare("SELECT Classe, Niveau FROM ELEVE WHERE Matricule = :matricule AND Statut = 'retenu'");
            demandes.bindValue(":matricule", arg1);
            if (demandes.exec() && demandes.first()) {
                QString classe = demandes.value(0).toString();
                level = demandes.value(1).toString();
                ui->classe_2->setText(classe);
            } else {
                ui->classe_2->setText("");
                ui->taux->setText("");
                ui->duree->setText("");
                ui->comboBox_6->setCurrentIndex(0);
                ui->prix->setText("");
            }
     }else{
            QMessageBox::critical(this, "Error", "Connection failed");
     }

}

void MainWindow::afficheReduction(){
    //Charger les données dans le tableau de gestion des réductions
    QString mat[20], classes[20], ID[20], niveau[20];
    int compte = 0;
    QSqlQuery selection;
    selection.prepare("SELECT Matricule, Classe, idREDUCTION, Niveau FROM ELEVE WHERE idREDUCTION > 0");
    if(selection.exec()){
        while(selection.next()){
            mat[compte] = selection.value(0).toString();
            classes[compte] = selection.value(1).toString();
            ID[compte] = selection.value(2).toString();
            niveau[compte] = selection.value(3).toString();
            compte++;
        }
    }else{
        QMessageBox::critical(this, "Erreur", "La requête n'est pas exécutée");
    }

    QSqlQuery reduction;
    reduction.prepare("SELECT Raison, Taux, DureeValiditee FROM REDUCTION WHERE idREDUCTION = :id");

    for(int lines = 0; lines < compte; lines++){
        reduction.bindValue(":id", ID[lines]);
        if(reduction.exec() && reduction.first()){
            QString raisons = reduction.value(0).toString();
            int taux = reduction.value(1).toInt();
            QString duree = reduction.value(2).toString();
            QSqlQuery montant;
            montant.prepare("SELECT \"Montant (en Ar)\" FROM NIVEAU_ACADEMIQUE WHERE Niveau = :niv");
            montant.bindValue(":niv", niveau[lines]);
            if(montant.exec() && montant.first()){
                int montants = montant.value(0).toInt();
                int valeurReduction = montants * taux / 100;
                int valeurEcolage = montants - valeurReduction;
                QString ecolage = QString::number(valeurEcolage);
                int ligneCompte = ui->tableWidget_2->rowCount();
                ui->tableWidget_2->insertRow(ligneCompte);
                ui->tableWidget_2->setItem(ligneCompte, 0, new QTableWidgetItem(mat[lines]));
                ui->tableWidget_2->setItem(ligneCompte, 1, new QTableWidgetItem(classes[lines]));
                ui->tableWidget_2->setItem(ligneCompte, 2, new QTableWidgetItem(raisons));
                ui->tableWidget_2->setItem(ligneCompte, 3, new QTableWidgetItem(ecolage));
                ui->tableWidget_2->setItem(ligneCompte, 4, new QTableWidgetItem(duree));

            }else{
                QMessageBox::critical(this, "Erreur", "Le montant de l'écolage en fonction du niveau"
                                                      "n'est pas sélectioné");
            }
        }else{
            QMessageBox::critical(this, "Erreur", "La sélection des données dans la table REDUCTION a échoué\n" + reduction.lastError().text());

        }
    }
}

//Permet de faire apparître automatiquement
void MainWindow::on_comboBox_6_currentTextChanged(const QString &arg1)
{
     if(!ui->matricule_2->text().isEmpty() && !ui->classe_2->text().isEmpty()){
            QSqlQuery requete;
            requete.prepare("SELECT Taux, DureeValiditee FROM REDUCTION WHERE Raison = :raison");
            requete.bindValue(":raison", arg1);
            if(requete.exec() && requete.first()){
                QString taux = requete.value(0).toString(), duree = requete.value(1).toString();
                ui->taux->setText(taux);
                ui->duree->setText(duree);
            }else{
                ui->taux->setText("");
                ui->duree->setText("");
            }

            int taux_2 = ui->taux->text().toInt();
            QSqlQuery requete_2;
            requete_2.prepare("SELECT \"Montant (en Ar)\" FROM NIVEAU_ACADEMIQUE WHERE Niveau = :niv");
            requete_2.bindValue(":niv", level);
            if(requete_2.exec() && requete_2.first()){
                int ecolage = requete_2.value(0).toInt();
                int reduction = ecolage * taux_2 / 100;
                int montant = ecolage - reduction;
                QString montantEcolage = QString::number(montant);
                ui->prix->setText(montantEcolage);
            }else{
                QMessageBox::critical(this, "Error", requete_2.lastError().text());
            }
        }
}

//Enregistrer les modifications dans la table NIVEAU_ACADEMIQUE et ANNEE_SCOLAIRE
void MainWindow::on_pushButton_16_clicked()
{
        QString annee = ui->annee2->text(), debut = ui->debut->currentText(), fin = ui->fin->currentText();
        ui->annee->setText(annee); ui->anneeSco->setText(annee); //Mettre à jour l'année scolaire dans l'enregistrement de paiement et le tableau de suivi
        QSqlQuery verifQuery;
        //Récupérer le début de l'année scolaire et la fin;
        int indiceDebut = 0, checkboxUsed = 0;

        //Récupérer l'indice du début de l'année scolaire pour pouvoir l'affiché en 1er dans le checkbox
        for(int g = 0; g < 12; g++){
            if(mois[g] == debut){
                indiceDebut = g;
                break;
            }
        }

        //Arranger le checkbox suivant le début et la fin du mois
        while(true){
            if(indiceDebut == 12) indiceDebut = 0; //Réinitialiser à 0 l'indice pour que l'ajout continue tant qu'on n'a pas encore atteint la fin de l'année scolaire
            if(mois[indiceDebut] == fin){
                checkboxUsed++;
                break;
            }
            indiceDebut++;
            checkboxUsed++;
        }

        //Permet de mettre à jour l'année scolaire si elle existe déjà ou d'insérer la nouvelle année scolaire
        verifQuery.prepare("SELECT anneeScolaire FROM ANNEE_SCOLAIRE WHERE anneeScolaire = :annee ");
        verifQuery.bindValue(":annee", annee);
        if (verifQuery.exec() && !verifQuery.next()) {
                QSqlQuery an;
                an.prepare("INSERT INTO ANNEE_SCOLAIRE (anneeScolaire, Debut, Fin, Duree)  VALUES (?, ?, ?, ?)");
                an.bindValue(0, annee);
                an.bindValue(1, debut);
                an.bindValue(2, fin);
                an.bindValue(3, checkboxUsed);
                if(!an.exec()){
                    QMessageBox::critical(this, "Erreur", "L'enregistrement de l'année scolaire a échoué");
                }
        }
        if (verifQuery.exec() && verifQuery.next()) {
                QSqlQuery month;
                month.prepare("UPDATE ANNEE_SCOLAIRE SET Debut = :debut, Fin = :fin, Duree = :duree WHERE anneeScolaire = :annee");
                month.bindValue(":debut", debut);
                month.bindValue(":fin", fin);
                month.bindValue(":annee", annee);
                month.bindValue(":duree", checkboxUsed);
                if(!month.exec()){
                    QMessageBox::critical(this, "Erreur", "La mis à jour des mois a échoué\n" + month.lastError().text());
                }
        }

       /* //Permet d'insérer dans l'historique des revenus les mois existant pour l'année scolaire en cours
        QSqlQuery effaceRevenu;
        effaceRevenu.prepare("DELETE FROM HISTORIQUE_REVENU WHERE anneeScolaire = :annees");
        effaceRevenu.bindValue(":annees", annee);
        if(!effaceRevenu.exec()) QMessageBox::critical(this, "Erreur", "La suppression des revenus a échoué\n" + effaceRevenu.lastError().text());
*/
        //Permet d'insérer les mois dans l'historique de revenu
        QSqlQuery revenus;
        revenus.prepare("SELECT * FROM HISTORIQUE_REVENU WHERE Mois = :Mois AND anneeScolaire = :anneeScolaire");
        revenus.bindValue(":anneeScolaire", annee);
        int commencer = 0, commencement = 0, terminer = 0;
        for(int l = 0; l < 12; ++l){
            if(mois[l] == debut){
                commencer = l; commencement = l;
            }
            if(mois[l] == fin) terminer = l;
        }
        while(true){
            if(commencer == 12) commencer = 0;
            QSqlQuery revenu;
            revenu.prepare("INSERT INTO HISTORIQUE_REVENU (Mois, anneeScolaire) VALUES (:mois, :an)");
            revenu.bindValue(":an", annee);
            if(mois[commencer] == fin){
                revenus.bindValue(":Mois", mois[commencer]);
                if(revenus.exec() && !revenus.first()){
                    revenu.bindValue(":mois", mois[commencer]);
                    if(!revenu.exec()) QMessageBox::critical(this, "Erreur", "L'insertion des mois dans HISTORIQUE_REVENU a échoué\n" + revenu.lastError().text());
                }
                break;
            }
                revenus.bindValue(":Mois", mois[commencer]);
                if(revenus.exec() && !revenus.first()){
                    revenu.bindValue(":mois", mois[commencer]);
                    if(!revenu.exec()) QMessageBox::critical(this, "Erreur", "L'insertion des mois dans HISTORIQUE_REVENU a échoué\n" + revenu.lastError().text());
                }

            commencer++;
        }

        //Permet de supprimer les mois dans l'historique de revenu si il y a une mis à jour et il ne sont plus inclus dans l'Année Scolaire
        for(int t = 0; t < 12; t++){
            if(commencement > terminer){
                if(t > terminer && t < commencement){
                    QSqlQuery effaceRevenu;
                    effaceRevenu.prepare("DELETE FROM HISTORIQUE_REVENU WHERE Mois = :Month AND anneeScolaire = :Year");
                    effaceRevenu.bindValue(":Month", mois[t]);
                    effaceRevenu.bindValue(":Year", annee);
                    if(!effaceRevenu.exec()) QMessageBox::critical(this, "Erreur", "La suppression des revenus a échoué\n" + effaceRevenu.lastError().text());
                }
            }
            if(commencement < terminer && commencement == 0){
                if(t > terminer){
                    QSqlQuery effaceRevenu;
                    effaceRevenu.prepare("DELETE FROM HISTORIQUE_REVENU WHERE Mois = :Month AND anneeScolaire = :Year");
                    effaceRevenu.bindValue(":Month", mois[t]);
                    effaceRevenu.bindValue(":Year", annee);
                    if(!effaceRevenu.exec()) QMessageBox::critical(this, "Erreur", "La suppression des revenus a échoué\n" + effaceRevenu.lastError().text());
                }
            }
            if(commencement < terminer && commencement != 0)
                if(t < commencement ||  t > terminer){
                    QSqlQuery effaceRevenu;
                    effaceRevenu.prepare("DELETE FROM HISTORIQUE_REVENU WHERE Mois = :Month AND anneeScolaire = :Year");
                    effaceRevenu.bindValue(":Month", mois[t]);
                    effaceRevenu.bindValue(":Year", annee);
                    if(!effaceRevenu.exec()) QMessageBox::critical(this, "Erreur", "La suppression des revenus a échoué\n" + effaceRevenu.lastError().text());
                }
        }

        //Permet de soumettre ke  tableau du montant dans la base de données
        QSqlTableModel *saveModel = qobject_cast<QSqlTableModel*>(ui->tableauParametre->model());
        if (saveModel) {
            if (saveModel->submitAll()) {
                QMessageBox::information(this, "Enregistrement", "Modifications enregistrées avec succès");
            } else {
                QMessageBox::warning(this, "Erreur d'enregistrement", "Impossible d'enregistrer les modifications");
            }
        }

        //Recharger de nouveau le tableau des réductions en fonction des nouveaux montant
        ui->tableWidget_2->setRowCount(0);
        afficheReduction();
}

//Permet d'aller à la page de paramètre d'écolage depuis le tableau de vue
void MainWindow::on_paramEco1_clicked()
{
    ui->pushButton_10->click();
}

//Permet d'enregistrer les réductions
void MainWindow::on_pushButton_15_clicked()
{
    QString matricule = ui->matricule_2->text(), classe = ui->classe_2->text(), raison = ui->comboBox_6->currentText();
    QString ecolage = ui->prix->text(), duree = ui->duree->text();
    int idReduction = ui->comboBox_6->currentIndex();
    int ligne = ui->tableWidget_2->rowCount(); // Obtenez le numéro de ligne actuel
    //Vérifier si la matricule ne figure pas déjà dans le tableau
    for(int i = 0; i < ligne; i++){
            if(ui->tableWidget_2->item(i, 0)->text() == matricule){
                QMessageBox::warning(this, "Avertissement", "Impossible d'enregisrter car cette matricule figure déjà"
                                                            " dans le tableau");
                ui->matricule_2->setText("");
                ui->classe_2->setText("");
                ui->taux->setText("");
                ui->duree->setText("");
                ui->comboBox_6->setCurrentIndex(0);
                ui->prix->setText("");
                return;
            }
    }
    //Vérifier si le champs sont complètement remplis
    if(matricule.isEmpty() || classe.isEmpty() || raison == "non mentionnée"){
            QMessageBox::warning(this, "Avertissement", "Veuiller remplir correctement tous les champs");
            ui->matricule_2->setText("");
            ui->classe_2->setText("");
            ui->taux->setText("");
            ui->duree->setText("");
            ui->comboBox_6->setCurrentIndex(0);
            ui->prix->setText("");
            return;
    }

            QSqlQuery envoi;
            envoi.prepare("UPDATE ELEVE SET idREDUCTION = :id WHERE matricule = :matricule");
            envoi.bindValue(":id", idReduction);
            envoi.bindValue(":matricule", matricule);
            if(envoi.exec()){
                ui->tableWidget_2->insertRow(ligne); // Insérez une nouvelle ligne

                // Insérez les données dans les colonnes appropriées
                ui->tableWidget_2->setItem(ligne, 0, new QTableWidgetItem(matricule));
                ui->tableWidget_2->setItem(ligne, 1, new QTableWidgetItem(classe));
                ui->tableWidget_2->setItem(ligne, 2, new QTableWidgetItem(raison));
                ui->tableWidget_2->setItem(ligne, 3, new QTableWidgetItem(ecolage));
                ui->tableWidget_2->setItem(ligne, 4, new QTableWidgetItem(duree));

                //Pour enregistrer le paiment pour ceux qui obtiennent un taux de réduction à 100%
                QSqlQuery tauxReduction;
                tauxReduction.prepare("SELECT Taux FROM REDUCTION WHERE idREDUCTION = :idReduction");
                tauxReduction.bindValue(":idReduction", idReduction);
                if(tauxReduction.exec() && tauxReduction.first()){
                    int taux = tauxReduction.value(0).toInt();
                    if(taux == 100){
                        QSqlQuery payee;
                        QString today = QDate::currentDate().toString("dd/MM/yyyy");
                        QString anneeSco = ui->annee->text();
                        payee.prepare("INSERT INTO HISTORIQUE_PAIEMENT (Matricule, Classe, DatePaiement, MoisPayes, Montant, AnneeScolaire, Reduction, RaisonReduction, TauxReduction) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
                        payee.bindValue(0, matricule);
                        payee.bindValue(1, classe);
                        payee.bindValue(2, today);
                        payee.bindValue(4, ecolage);
                        payee.bindValue(5, anneeSco);
                        payee.bindValue(6, "oui");
                        payee.bindValue(7, raison);
                        payee.bindValue(8, taux);
                        for(int t = 0; t < duree.toInt(); t++){
                            payee.bindValue(3, checkBoxes[t]->text());
                            if(!payee.exec()) QMessageBox::critical(this, "Erreur", "L'enregistrement a échoué ");
                        }
                    }
                }

                QMessageBox::information(this, "Félicitation!", "L'enregistrement s'est bien passé");
                ui->matricule_2->setText(""); ui->classe_2->setText(""); ui->taux->setText(""); ui->prix->setText("");
                ui->duree->setText("");
            }else{
                QMessageBox::critical(this, "Erreur", envoi.lastError().text());
            }

}



//Permet de supprimer une ligne sélectionnée dans la gestion des réductions
void MainWindow::on_pushButton_17_clicked()
{
    // Obtenez la cellule sélectionnée
    QModelIndexList selectedIndexes = ui->tableWidget_2->selectionModel()->selectedIndexes();

    // Assurez-vous qu'une cellule est sélectionnée
    if (!selectedIndexes.isEmpty()) {

        int reponse = QMessageBox::question(this, "Avertissement", "Voulez-vous vraiment supprimer cette ligne?",  QMessageBox::Yes | QMessageBox::No);

        if(reponse == QMessageBox::Yes){
                // Récupérez l'indice de la première cellule sélectionnée (l'indice de la colonne 0)
                QModelIndex selectedCell = selectedIndexes.at(0);

                // Récupérez l'indice de la ligne correspondante à la cellule sélectionnée
                int rowIndex = selectedCell.row();

                // Supprimez la ligne correspondante à l'indice rowIndex
                QString matricule = ui->tableWidget_2->item(rowIndex, 0)->text();
                ui->tableWidget_2->removeRow(rowIndex);

                QSqlQuery updateQuery;
                updateQuery.prepare("UPDATE ELEVE SET idREDUCTION = NULL WHERE Matricule = :matricule");
                updateQuery.bindValue(":matricule", matricule);
                if(!updateQuery.exec()){
                    QMessageBox::critical(this, "Erreur", "La mis à jour de l'idREDUCTION n'est pas effectuée");
                }else{
                    QMessageBox::information(this, "Information", "La matricule " + matricule + " est correctement "
                                                                                                "supprimer");
                }
            }

    }else{
        QMessageBox::warning(this, "Avertissement", "Veuiller sélectionnée une cellule de la ligne que "
                                                    "vous voulez supprimer");
    }

}

//Pour filtrer les mois disponibles dans le comboBox de la fin d'année Scolaire
void MainWindow::on_debut_currentTextChanged(const QString &arg1)
{
    ui->fin->clear();

    //Ajouter tous les mois
    for(int l = 0; l < 12; l++){
        ui->fin->addItem(mois[l]);        //ui->fin->setItemText(l, mois[l]);
    }

    //Supprimer le mois qui est déjà indiquer dans le currentText du comboBox debut
    for(int h = 0; h < 12; h++){
        if(ui->fin->itemText(h) == arg1){
                ui->fin->removeItem(h);
        }
    }
}

//Afficher l'enregistrement de paiement depuis le paramètre d'écolage
void MainWindow::on_pushButton_5_clicked()
{
    afficheCheckbox();
    ui->paiement1->click();
}

//Pour aller dans la page du tableau de vue depuis le paramètre d'écolage
void MainWindow::on_pushButton_6_clicked()
{
    ui->pushButton_11->click();
}

//Pour montrer le calendrier dans le statistique
void MainWindow::on_pushButton_7_clicked()
{
    nbClic++;
    if(nbClic % 2 != 0) ui->calendarWidget->show();
    else ui->calendarWidget->hide();
}

//Permettant de sélectionner la date dans le paramètre
void MainWindow::on_calendarWidget_activated(const QDate &date)
{
    QLocale frenchLocale(QLocale::French);  // Créez une locale française
    QString moisFrancais = frenchLocale.toString(date, "MMMM");
    QString dateEnFrancais = QString("%1 %2 %3")
                                 .arg(date.day())
                                 .arg(moisFrancais)
                                 .arg(date.year());
    ui->dates->setText(dateEnFrancais);
    statistique();
    ui->calendarWidget->hide();
}

//Pour afficher le statisque (nombre élève + payé + non payé)
void MainWindow::statistique(){    
    //Afficher le nombre d'élève dans l'école
    QSqlQuery eleve;
    QDate afficheDate = ui->calendarWidget->selectedDate();
    QString Mois = ui->comboMois->currentText();
    int ecolagePaye = 0, nonPayee;
    QString anneeCourant = ui->anneeCourant->currentText();
    int nbEleve = 0;
    if(!ui->dates->text().isEmpty()){
        eleve.prepare("SELECT DateEntree, DateSortie FROM ELEVE");
        if(eleve.exec()){
            while(eleve.next()){
                QString entree = eleve.value(0).toString(), sortie = eleve.value(1).toString();
                QDate entree1 = QDate::fromString(entree, "dd/MM/yyyy");
                QDate sortie1 = QDate::fromString(sortie, "dd/MM/yyyy");
                if(!sortie.isNull()){
                    if(entree1 <= afficheDate && sortie1 > afficheDate) nbEleve++;
                }else{
                    if(entree1 <= afficheDate) nbEleve++;
                }
            }
        }else QMessageBox::critical(this, "Erreur", eleve.lastError().text());

        //Afficher le nombre des élèves qui ont payé et qui n'ont pas payé l'écolage
        QSqlQuery moisPaye;
        moisPaye.prepare("SELECT DatePaiement FROM HISTORIQUE_PAIEMENT WHERE MoisPayes = :mois AND AnneeScolaire = :anneeSc");
        moisPaye.bindValue(":mois", Mois);
        moisPaye.bindValue(":anneeSc", anneeCourant);
        if(moisPaye.exec()){
            while(moisPaye.next()){
                QString moisPayes = moisPaye.value(0).toString();
                QDate datePaiement = QDate::fromString(moisPayes, "dd/MM/yyyy");
                if(datePaiement <= afficheDate) ecolagePaye++;
            }
        }
    }else{
        int annee1 = anneeCourant.left(4).toInt(), annee2 = anneeCourant.right(4).toInt(); //Récupérer les deux années qui forment l'AnnéeSco
        int indexDecembre = ui->comboMois->findText("Décembre"), indiceMois = 0;
        QDate dernierJour; //Récupérer la date du dernier jour pour le mois mentionner afin de rechercher le nombre d'élève qui était encore présent
        for(int f = 0; f < 12; f++){
            if(mois[f] == Mois) indiceMois = f + 1;
        }

        if(ui->comboMois->currentIndex() < indexDecembre){
            QDate debutJour(annee1, indiceMois, 1);
            dernierJour = debutJour.addMonths(1).addDays(-1);
        }else{
            QDate debutJour(annee2, indiceMois, 1);
            dernierJour = debutJour.addMonths(1).addDays(-1);
        }
        QSqlQuery ancienNbEleve;
        ancienNbEleve.prepare("SELECT DateEntree, DateSortie FROM ELEVE");
        if(ancienNbEleve.exec()){
            while(ancienNbEleve.next()){
                QString entree = ancienNbEleve.value(0).toString(), sortie = ancienNbEleve.value(1).toString();
                QDate entree1 = QDate::fromString(entree, "dd/MM/yyyy");
                QDate sortie1 = QDate::fromString(sortie, "dd/MM/yyyy");
                if(!sortie.isNull()){
                    if(entree1 <= dernierJour && sortie1 > dernierJour) nbEleve++;
                }else{
                    if(entree1 <= dernierJour) nbEleve++;
                }
            }
        }

        QSqlQuery ancienRevenu;
        ancienRevenu.prepare("SELECT COUNT(*) FROM HISTORIQUE_PAIEMENT WHERE MoisPayes = :Months AND anneeScolaire = :AnneeSco");
        ancienRevenu.bindValue(":Months", Mois);
        ancienRevenu.bindValue(":AnneeSco", anneeCourant);
        if(ancienRevenu.exec() && ancienRevenu.next()){
            ecolagePaye = ancienRevenu.value(0).toInt();
        }
    }
    ui->eleve->setText(QString::number(nbEleve)); //Nombre d'élève

    ui->elevePayee->setText(QString::number(ecolagePaye));//Elèves qui ont Payé
    nonPayee = nbEleve - ecolagePaye;
    ui->eleveNonPayee->setText(QString::number(nonPayee));//Elève qui n'ont pas Payé

    ui->moisRevenu->setText(ui->comboMois->currentText()); //Mois concerné pour le revenu mensuel
    ui->anneeRevenu->setText(ui->anneeCourant->currentText()); //Année Scolaire concerné pour le revenu
    ui->statutMois->setText(ui->comboMois->currentText()); //Statut pour le mois concerné

    //Les revenus si la date est affichée
    if(!ui->dates->text().isEmpty()){
        //Remplissage du Revenu mensuel
        QSqlQuery mensuel;
        int revenuMensuel = 0;
        mensuel.prepare("SELECT Montant, DatePaiement FROM HISTORIQUE_PAIEMENT WHERE MoisPayes = :moiS AND AnneeScolaire = :SchoolYear");
        mensuel.bindValue(":moiS", Mois);
        mensuel.bindValue(":SchoolYear", anneeCourant);
        if(mensuel.exec()){
            while(mensuel.next()){
                QString dateString = mensuel.value(1).toString();
                QDate datePaiement = QDate::fromString(dateString, "dd/MM/yyyy");
                int montant = mensuel.value(0).toInt();
                if(datePaiement <= afficheDate) revenuMensuel+=montant;
            }
            ui->revenuMensuel->setText(QString::number(revenuMensuel));
        }else QMessageBox::critical(this, "Erreur", "Erreur de calcul des revenus Mensuels\n" + mensuel.lastError().text());

        //Remplissage du Revenu Annuel
        QSqlQuery annuel;
        int revenuAnnuel = 0;
        annuel.prepare("SELECT Montant, DatePaiement FROM HISTORIQUE_PAIEMENT WHERE AnneeScolaire = :Annee_Scolaire");
        annuel.bindValue(":Annee_Scolaire", anneeCourant);
        if(annuel.exec()){
            while(annuel.next()){
                QString dateString = annuel.value(1).toString();
              //  QMessageBox::information(this, "Date", dateString);
                QDate datePaiement = QDate::fromString(dateString, "dd/MM/yyyy");
                int montant = annuel.value(0).toInt();
                if(datePaiement <= afficheDate) revenuAnnuel+=montant;
            }
            ui->revenuAnnuel->setText(QString::number(revenuAnnuel));
        }
    }

    //Les revenus si la date n'est pas affichée
    else{
        //Remplissage du Revenu mensuel
        QSqlQuery mensuel;
        mensuel.prepare("SELECT Revenu FROM HISTORIQUE_REVENU WHERE Mois = :m AND anneeScolaire = :AS");
        mensuel.bindValue(":m", Mois);
        mensuel.bindValue(":AS", anneeCourant);
        if(mensuel.exec() && mensuel.first()){
            QString revenuMensuel = mensuel.value(0).toString();
            ui->revenuMensuel->setText(revenuMensuel);
        }
        //Remplissage du Revenu Annuel
        QSqlQuery annuel;
        int revenuAnnuel = 0;
        annuel.prepare("SELECT Revenu FROM HISTORIQUE_REVENU WHERE anneeScolaire = :Annee_Scolaire");
        annuel.bindValue(":Annee_Scolaire", anneeCourant);
        if(annuel.exec()){
            while(annuel.next()){
                int revenu = annuel.value(0).toInt();
                revenuAnnuel+=revenu;
            }
            ui->revenuAnnuel->setText(QString::number(revenuAnnuel));
        }
    }

    //Remplissage Statut
    if(ui->eleve->text() == ui->elevePayee->text()){
        ui->statutPaiement->clear();
        InsertImage(ui->Status, ":/Images/check.png");
        ui->statutPaiement->setStyleSheet("font-size: 25px; font-weight: bold; color: green");
        ui->statutPaiement->setText("Complet");
    }else{
        ui->statutPaiement->clear();
        InsertImage(ui->Status, ":/C:/Users/AVOTRA/Downloads/delete.png");
        ui->statutPaiement->setStyleSheet("font-size: 25px; font-weight: bold; color: rgb(253, 243, 0);");
        ui->statutPaiement->setText("Incomplet");
    }


    //Les Graphiques

    if(nbEleve != 0){
        int pourcentagePaye = ecolagePaye * 100 / nbEleve;
        int pourcentageNonPaye = 100 - pourcentagePaye;
        //QMessageBox::information(this, "Pourcentage", "Payé = " + QString::number(pourcentagePaye) + " et Non Payé = " + QString::number(pourcentageNonPaye));
        mettreAJourDonutChart(pourcentagePaye, pourcentageNonPaye);
     }
}


void MainWindow::mettreAJourDonutChart(int pourcentagePaye, int pourcentageNonPaye) {
    series->clear();
    series->append("Impayés " + QString::number(pourcentageNonPaye) + "%", pourcentageNonPaye)->setBrush(QColor("red"));
    series->append("Payés " + QString::number(pourcentagePaye) + "%", pourcentagePaye)->setBrush(QColor("green"));
}


//Pour arranger la date dans le calendrier pour le statistique
void MainWindow::date(){
    QString anneeCourant = ui->anneeCourant->currentText();
    //Remplissage de la date
    QSqlQuery comboMois;
    comboMois.prepare("SELECT Debut, Fin, Duree FROM ANNEE_SCOLAIRE WHERE anneeScolaire = :annees");
    comboMois.bindValue(":annees", anneeCourant);
    if(comboMois.exec() && comboMois.first()){
        QString moisDebut = comboMois.value(0).toString(), moisFin = comboMois.value(1).toString();
        int duree = comboMois.value(2).toInt();
        int annee1 = ui->anneeCourant->currentText().left(4).toInt();
        int annee_2 = ui->anneeCourant->currentText().right(4).toInt();
        int indiceDebut, indiceFin, commencement;
        //Récupérer l'indice du début et de la fin de l'année scolaire
        for(int g = 0; g < 12; g++){
                if(mois[g] == moisDebut){
                    indiceDebut = g + 1;
                    commencement = g;
                }
                if(mois[g] == moisFin) indiceFin = g + 1;
        }
        QDate anneeDebut(annee1, indiceDebut, 1);
        ui->calendarWidget->setMinimumDate(anneeDebut);
        if(anneeCourant != ui->annee->text()){
            int dernierJourDuMois = QDate(annee1, indiceFin, 1).daysInMonth(); //récupérer le dernier jour du mois de fin d'année Scolaire
            QDate anneeFin(annee1, indiceFin, dernierJourDuMois); //dernierJourDuMois);
            //Pour déterminer si le mois de la fin d'année scolaire appartient au première année ou pas
            for(int k = 0; k < duree; k++){
                    if(indiceDebut == 13){
                       dernierJourDuMois = QDate(annee_2, indiceFin, 1).daysInMonth(); //récupérer le dernier jour du mois de fin d'année Scolaire
                        anneeFin = QDate(annee_2, indiceFin, dernierJourDuMois);
                        break;
                    }
                    indiceDebut++;
            }

            ui->calendarWidget->setMaximumDate(anneeFin);
            //Pour afficher la date en français
            QDate dateCourant = QDate::currentDate();
            QLocale frenchLocale(QLocale::French);  // Créez une locale française
            QString moisFrancais = frenchLocale.toString(dateCourant, "MMMM");
            QString dateEnFrancais = QString("%1 %2 %3")
                                         .arg(dateCourant.day())
                                         .arg(moisFrancais)
                                         .arg(dateCourant.year());
            ui->dates->setText(dateEnFrancais);    
        }else{
            QDate dateActuelle = QDate::currentDate();
            ui->calendarWidget->setMaximumDate(dateActuelle);
            //Pour afficher la date en français
            ui->calendarWidget->setSelectedDate(QDate::currentDate());
            QDate dateCourant = ui->calendarWidget->selectedDate();
            QLocale frenchLocale(QLocale::French);  // Créez une locale française
            QString moisFrancais = frenchLocale.toString(dateCourant, "MMMM");
            QString dateEnFrancais = QString("%1 %2 %3")
                                         .arg(dateCourant.day())
                                         .arg(moisFrancais)
                                         .arg(dateCourant.year());
            ui->dates->setText(dateEnFrancais);
        }
        //Remplir le comboBox du mois
        ui->comboMois->clear(); //Vider le comboBox du mois avant l'insertion
        --indiceFin;
        while(true){
            if(commencement == 12) commencement = 0;
            if(mois[commencement] == mois[indiceFin]){
                    ui->comboMois->addItem(mois[indiceFin]);
                    break;
            }else
                    ui->comboMois->addItem(mois[commencement]);
            commencement++;
        }
    }
}

//Quand l'année scolaire courant dans le ComboBox du statistique change
void MainWindow::on_anneeCourant_currentTextChanged()
{
    date();
    if(ui->anneeCourant->currentText() != ui->annee->text()) ui->dates->setText("");
    statistique();
}

//Pour aller dans le choix d'opération depuis le paramètre d'écolage
void MainWindow::on_pushButton_18_clicked()
{
    ui->pushButton_4->click();
}

//Permet de mettre à jour le statistique quand le mois change
void MainWindow::on_comboMois_currentTextChanged()
{
    statistique();
}


void MainWindow::on_annee2_textEdited(const QString &arg1)
{
    if(arg1.length() == 9){
        int annee1 = arg1.left(4).toInt();
        ui->annee2->setText(QString::number(annee1) + "-" + QString::number(annee1 + 1));
    }
}

//Permet de mettre à jour le mois quand l'année scolaire change dans le statut de paiement
void MainWindow::on_Annee_Scolaire_currentTextChanged(const QString &annee)
{
    if(!annee.isEmpty()){
        ui->MoisPaiement->clear();
        historiqueMois();
    }
}

//Lorsque je génère le filtrage dans le dans le statut de paiement
void MainWindow::on_pushButton_clicked()
{
    ui->historique->clearContents();
    ui->historique->setRowCount(0);
    QString niveauMentionne = ui->Niveau->currentText();
    //Triage par niveau
    if(niveauMentionne != "Tous les niveaux"){

        historiqueTableau();
        QSqlQuery niveau;
        niveau.prepare("SELECT Niveau FROM ELEVE WHERE Matricule = :matricule");
        for(int row = 0; row < ui->historique->rowCount(); row++){
            QString matricule = ui->historique->item(row, 0)->text();
            niveau.bindValue(":matricule", matricule);
            if(niveau.exec() && niveau.first()){
                QString niveauEleve = niveau.value(0).toString();
                if(niveauEleve != niveauMentionne)
                    ui->historique->hideRow(row);
            }
        }
    }else{
        historiqueTableau();
    }
    //Filtrage par catégorie
    QSqlQuery payee;
    payee.prepare("SELECT * FROM HISTORIQUE_PAIEMENT WHERE Matricule = :matricule AND MoisPayes = :mois AND AnneeScolaire = :annee ");
    if(ui->categorie->currentText() == "Payés"){
        for(int l = ui->historique->rowCount() - 1; l >= 0; l--){
            QString mat = ui->historique->item(l, 0)->text(), mois = ui->historique->item(l, 5)->text();
            QString annee = ui->Annee_Scolaire->currentText();
            payee.bindValue(":matricule", mat);
            payee.bindValue(":mois", mois);
            payee.bindValue(":annee", annee);
            if(payee.exec() && !payee.first()){
                ui->historique->hideRow(l);
            }
        }
    }else if(ui->categorie->currentText() == "Impayés"){
        for(int l = ui->historique->rowCount() - 1; l >= 0; l--){
            QString mat = ui->historique->item(l, 0)->text(), mois = ui->historique->item(l, 5)->text();
            QString annee = ui->Annee_Scolaire->currentText();
            payee.bindValue(":matricule", mat);
            payee.bindValue(":mois", mois);
            payee.bindValue(":annee", annee);
            if(payee.exec() && payee.first()){
                ui->historique->hideRow(l);
            }
        }
    }
}
/*
//Changer le placeholder de la barre de recherche dans le statut de paiement
void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    ui->recherche_bar_2->setPlaceholderText("Recherche par " + arg1);
}

//Faire des recherches dans le statut de paiement
void MainWindow::on_recherche_bar_2_textEdited(const QString &arg1)
{
    for (int row = 0; row < ui->historique->rowCount(); row++) {

            bool match = false;
            for (int col = 0; col < 2; col++) {
                QTableWidgetItem *item = ui->historique->item(row, col);
                if (item && item->text().contains(arg1, Qt::CaseInsensitive)) {
                    match = true;
                    break;
                }
            }
            ui->historique->setRowHidden(row, !match);

    }
}
*/
/*//Code filtrage
void MainWindow::applyFilters(const QString &niveau, const QString &categorie)
{
    // Réinitialisez l'affichage en supprimant les filtres
    historiqueTableau();

    // Appliquez le filtre par niveau si différent de "Tous les niveaux"
    if (niveau != "Tous les niveaux")
    {
        for (int row = 0; row < ui->historique->rowCount(); row++)
        {
            QString matricule = ui->historique->item(row, 0)->text();
            QSqlQuery niveauQuery;
            niveauQuery.prepare("SELECT Niveau FROM ELEVE WHERE Matricule = :matricule");
            niveauQuery.bindValue(":matricule", matricule);
            if (niveauQuery.exec() && niveauQuery.first())
            {
                QString niveauEleve = niveauQuery.value(0).toString();
                if (niveauEleve != niveau)
                {
                    ui->historique->hideRow(row);
                }
            }
        }
    }

    // Appliquez le filtre par catégorie (Payés ou Impayés)
    QSqlQuery payee;
    payee.prepare("SELECT * FROM HISTORIQUE_PAIEMENT WHERE MoisPayes = :mois AND AnneeScolaire = :annee ");
    for (int row = 0; row < ui->historique->rowCount(); row++)
    {
        QString matricule = ui->historique->item(row, 0)->text();
        QString mois = ui->historique->item(row, 5)->text();
        QString annee = ui->Annee_Scolaire->currentText();
        payee.bindValue(":matricule", matricule);
        payee.bindValue(":mois", mois);
        payee.bindValue(":annee", annee);

        if (categorie == "Payés" && (payee.exec() && !payee.first()))
        {
            ui->historique->hideRow(row);
        }
        else if (categorie == "Impayés" && (payee.exec() && payee.first()))
        {
            ui->historique->hideRow(row);
        }
    }
}


//Lorsque je génère le filtrage dans le dans le statut de paiement
void MainWindow::on_pushButton_clicked()
{
    ui->historique->clearContents();
    ui->historique->setRowCount(0);
    QString niveauMentionne = ui->Niveau->currentText();
    QString categorieMentionnee = ui->categorie->currentText();

    applyFilters(niveauMentionne, categorieMentionnee);

}
*/
//Changer le placeholder de la barre de recherche dans le statut de paiement
void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    ui->recherche_bar_2->setPlaceholderText("Recherche par " + arg1);
    if(ui->comboBox->currentText() == "Matricule"){
        QIntValidator *validator = new QIntValidator(this);
        ui->recherche_bar_2->setValidator(validator);
    }else{
        ui->recherche_bar_2->setValidator(nullptr);
    }
}

void MainWindow::on_recherche_bar_2_textEdited(const QString &arg1)
{
    // Sauvegardez l'état actuel des filtres
    /*QString niveauMentionne = ui->Niveau->currentText();
    QString categorieMentionnee = ui->categorie->currentText();*/

    // Effacez tous les filtres précédemment appliqués
//    historiqueTableau();
    if(arg1.isEmpty())
        on_pushButton_clicked();

    // Réappliquez les filtres de niveau et de catégorie
    //applyFilters(niveauMentionne, categorieMentionnee);


    // Parcourez seulement les lignes visibles
    for (int row = 0; row < ui->historique->rowCount(); row++)
    {
        if (!ui->historique->isRowHidden(row))
        {
            bool match = false;
            for (int col = 0; col < 3; col++)
            {
                QTableWidgetItem *item = ui->historique->item(row, col);
                if (item && item->text().contains(arg1, Qt::CaseInsensitive))
                {
                    match = true;
                    break;
                }
            }
            ui->historique->setRowHidden(row, !match);
        }
    }
}

//Ajout d'utilisateur
void MainWindow::on_pushButton_20_clicked()
{
    QString nom = ui->username->text(), mdp = ui->mdp->text(), confirmation = ui->confirmation->text();
    if(!nom.isEmpty() && !mdp.isEmpty() && !confirmation.isEmpty()){
            if(mdp == confirmation){
                QSqlQuery utilisateur;
                utilisateur.prepare("INSERT INTO UTILISATEUR (NiveauAcces, Password) VALUES (?, ?)");
                utilisateur.bindValue(0, nom);
                utilisateur.bindValue(1, mdp);
                if(!utilisateur.exec()) QMessageBox::critical(this, "Erreur", "Une erreur s'est produite lors de l'enregistrement du nouvel utilisateur");
                else QMessageBox::information(this, "Information", "Enregistrement efféctué avec succès");
            }else{
                QMessageBox::warning(this, "Avertissement", "Veuillez vérifier le mot de passe");
                ui->username->setText(""); ui->mdp->setText(""); ui->confirmation->setText("");
            }
    }else QMessageBox::warning(this, "Avertissement", "Veuillez remplir correctement chaque champ");
}

//Apparition de la page d'ajout d'utilisateur
void MainWindow::on_pushButton_13_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

//Aller au menu de choix depuis l'ajout d'utilisateur
void MainWindow::on_pushButton_23_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}


void MainWindow::on_pushButton_25_clicked()
{
    QString raison = ui->raison->text(), taux = ui->taux2->text(), duree = ui->duree2->text();
    if(!raison.isEmpty() && !taux.isEmpty() && !duree.isEmpty()){
            QSqlQuery nouveauRaison;
            nouveauRaison.prepare("INSERT INTO REDUCTION (Raison, Taux, DureeValiditee) VALUES (?, ?, ?)");
            nouveauRaison.bindValue(0, raison);
            nouveauRaison.bindValue(1, taux);
            nouveauRaison.bindValue(2, duree);
            if(!nouveauRaison.exec()){
                QMessageBox::critical(this, "Erreur", "Echec d'enregistrement");
            }

            //Création du modèle de tableau pour l'affichage de la table
            QSqlTableModel *reduction = new QSqlTableModel(this);
            reduction->setTable("REDUCTION");
            reduction->setEditStrategy(QSqlTableModel::OnFieldChange);

            if (reduction->select()) {
                ui->tableReduction->setModel(reduction);
                ui->tableReduction->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);

                // Supprimer la colonne de numérotation des lignes
                ui->tableReduction->verticalHeader()->setVisible(false);

                // Appliquez le CentreItemDelegate pour centrer le contenu des cellules
                ui->tableReduction->setItemDelegate(new CenteredItemDelegate);

                // Ajuster les colonnes pour qu'elles aient la même taille
                int numCols = reduction->columnCount();
                for (int col = 0; col < numCols; ++col) {
                ui->tableReduction->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
                }

                reduction->setHeaderData(3, Qt::Horizontal, "Durée de Validité");

                ui->tableReduction->hideColumn(0);
            }

            ui->comboBox_6->clear();
            ui->comboBox_6->addItem("non mentionné");
                //actualisation
                //ComboBox raison
                //ComboBox raison
                QSqlQuery existeRaison;
            existeRaison.prepare("SELECT Raison FROM REDUCTION ORDER BY idREDUCTION ASC");
            if(existeRaison.exec()){
                while(existeRaison.next()){
                QString raison = existeRaison.value(0).toString();
                ui->comboBox_6->addItem(raison);
                }
            }
            QMessageBox::information(this, "Information", "Enregistrement effectué");
                ui->raison->setText(""); ui->taux2->setText(""); ui->duree2->setText("");
    }
    else{
            QMessageBox::warning(this, "Avertissement", "Veuillez remplir les champs");
    }
}


void MainWindow::on_pushButton_22_clicked()
{
    if(ui->mdp->echoMode() == QLineEdit::Password){
        QIcon icon(":/C:/Users/AVOTRA/Downloads/password-visible-9-48.png");
        ui->pushButton_22->setIcon(icon);
        ui->mdp->setEchoMode(QLineEdit::Normal);
    }
    else{
        QIcon icon(":/C:/Users/AVOTRA/Downloads/hide (1).png");
        ui->pushButton_22->setIcon(icon);
        ui->mdp->setEchoMode(QLineEdit::Password);
    }
}


void MainWindow::on_pushButton_21_clicked()
{
    if(ui->confirmation->echoMode() == QLineEdit::Password){
        QIcon icon(":/C:/Users/AVOTRA/Downloads/password-visible-9-48.png");
        ui->pushButton_21->setIcon(icon);
        ui->confirmation->setEchoMode(QLineEdit::Normal);
    }
    else{
        QIcon icon(":/C:/Users/AVOTRA/Downloads/hide (1).png");
        ui->pushButton_21->setIcon(icon);
        ui->confirmation->setEchoMode(QLineEdit::Password);
    }
}


void MainWindow::on_pushButton_24_clicked()
{
    ui->stackedWidget->setCurrentIndex(9);
}


void MainWindow::on_pushButton_26_clicked()
{
    ui->stackedWidget->setCurrentIndex(8);
}

void MainWindow::on_BouttonSupprimerReduction_clicked()
{
    // Obtenez la cellule sélectionnée
    QModelIndexList selectedIndexes = ui->tableReduction->selectionModel()->selectedIndexes();

    // Assurez-vous qu'une cellule est sélectionnée
    if (!selectedIndexes.isEmpty()) {

        int reponse = QMessageBox::question(this, "Avertissement", "Voulez-vous vraiment supprimer cette ligne?",  QMessageBox::Yes | QMessageBox::No);

        if(reponse == QMessageBox::Yes){
                // Récupérez l'indice de la première cellule sélectionnée (l'indice de la colonne 0)
                QModelIndex selectedCell = selectedIndexes.at(0);

                // Récupérez l'indice de la ligne correspondante à la cellule sélectionnée
                int rowIndex = selectedCell.row();

                // Récupérez le modèle de données associé au QTableView
                QAbstractItemModel *model = ui->tableReduction->model();

                // Supprimez la ligne correspondante à l'indice rowIndex
                QModelIndex index = ui->tableReduction->model()->index(rowIndex, 1);
                QString raison = ui->tableReduction->model()->data(index).toString();
                model->removeRow(rowIndex);

                // Mettez à jour l'affichage du QTableView
                ui->tableReduction->setModel(model);

                QSqlQuery query;
                query.prepare("DELETE FROM REDUCTION WHERE Raison = :raison");
                query.bindValue(":raison", raison);
                qDebug () << raison ;
                if(!query.exec()){
                    QMessageBox::critical(this, "Erreur", "La suppression de la raison de réduction a échoué.");
                }else{
                    QMessageBox::information(this, "Information", "La réduction ayant comme raison : " + raison + " est correctement "
                                                                                                "supprimé");
                            int index1 = ui->comboBox_6->findText(raison); // Recherche l'index de l'élément correspondant au texte
                            if (index1 != -1) {
                                ui->comboBox_6->removeItem(index1); // Supprime l'élément
                            }

                    }
                query.exec("SELECT * FROM REDUCTION");
                    QSqlQueryModel *Model = new QSqlQueryModel;
                    Model->setQuery(std::move(query));
                    ui->tableReduction->setModel(Model);

            }

    }else{
        QMessageBox::warning(this, "Avertissement", "Veuiller sélectionnée une cellule de la ligne que "
                                                    "vous voulez supprimer");
    }
}


