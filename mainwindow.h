#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QListWidget>
#include <QMap>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QFile>
#include <QTableView>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QIntValidator>
#include <QTableWidget>
#include <QDebug>
#include <QShortcut>
#include <QSqlError>
#include <QSettings>
#include <QtCharts>
#include <QChartView>
#include <QPieSeries>
#include <QPieSlice>
#include <QLineSeries>
#include <QString>
#include <QDate>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void InsertImage(QLabel *label, const QString &imagePath);
    void InsertNav(QListWidget *etiquette, const QString &ficPath, const QString menu);
    // Déclaration de la fonction eventFilter
    QString mois[12];
    QCheckBox *checkBoxes[12];
    void RefreshTable();
    int row;
    QString level; //Pour avoir le montant correspondant au niveau dans la gestion des réductions
    void afficheCheckbox(); //Pour l'arrangement des checkbox de Mois dans l'enregistrement de paiement
    void afficheTableau(int m, int indiceColonne, int stoppeur, QTableWidget *tableWidget); //Pour afficher les titres du tableau de suivi en fonction des mois de l'année scolaire
    void afficheReduction();
    QString renvoiMatricule; //Pour récupérer la matricule de l'élève renvoyé
    void gestionEleve(); //Pour l'affichage du tableau dans la gestion des élèves
    int nbClic; //Pour afficher et masquer le calendrier dans le statistique
    void statistique();
    void date();
    void historiqueAnneeScolaire();
    void historiqueMois();
    void historiqueTableau();


private:
    Ui::MainWindow *ui;
    QSqlDatabase bd;
    //int ligne;
    QPieSeries *series;
    QChart *chart;
    void mettreAJourDonutChart(int pourcentagePaye, int pourcentageNonPaye);
    QCursor handCursor; //Curseur indiquant les boutons cliquables

public slots:
    void CliquementItem(QListWidgetItem *item);

private slots:
    void on_checkBox_stateChanged(int arg1);
    void on_NomAcces_cursorPositionChanged();
    void on_password_cursorPositionChanged();
    void on_pushButton_2_clicked();
    void on_Bouton_2_clicked();
    void on_Enregistrer_clicked();
    void on_ajoutLigne_clicked();
    void on_supprimer_clicked();
    void on_comboBox_5_currentIndexChanged(int index);
    void on_pushButton_4_clicked();
    void on_matricule_textEdited(const QString &arg1);
    void on_mois_textChanged(const QString &arg1);
    void on_pushButton_9_clicked();
    void on_Bouton_clicked();
    void on_pushButton_12_clicked();
    void on_pushButton_11_clicked();
    void on_paiement1_clicked();
    void on_recherche_bar2_textChanged(const QString &arg1);
    void on_recherche_bar_textChanged();
    void on_pushButton_10_clicked();
    void on_pushButton_8_clicked();
    void on_matricule_2_textChanged(const QString &arg1);
    void on_comboBox_6_currentTextChanged(const QString &arg1);
    void on_pushButton_16_clicked();
    void on_paramEco1_clicked();
    void on_pushButton_15_clicked();
    void on_pushButton_17_clicked();
    void on_debut_currentTextChanged(const QString &arg1);
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
    void on_calendarWidget_activated(const QDate &date);
    void on_anneeCourant_currentTextChanged();
    void on_pushButton_18_clicked();
    void on_comboMois_currentTextChanged();

    void on_annee2_textEdited(const QString &arg1);
    void on_Annee_Scolaire_currentTextChanged(const QString &annee);
    void on_pushButton_clicked();
    void on_comboBox_currentTextChanged(const QString &arg1);
    void on_recherche_bar_2_textEdited(const QString &arg1);
    void on_pushButton_20_clicked();
    void on_pushButton_13_clicked();
    void on_pushButton_23_clicked();
    void on_pushButton_25_clicked();
    void on_pushButton_22_clicked();
    void on_pushButton_21_clicked();
  //  void applyFilters(const QString &niveau, const QString &categorie);
    void on_pushButton_24_clicked();
    void on_pushButton_26_clicked();
    void on_BouttonSupprimerReduction_clicked();
};
#endif // MAINWINDOW_H
