#ifndef NIVEAUITEMDELEGATE_H
#define NIVEAUITEMDELEGATE_H

#include <QItemDelegate>
#include <QSqlTableModel>
#include <QComboBox>
#include <QStringList>
#include <QAbstractItemDelegate>
#include <QLineEdit>
#include <QAbstractItemView>



class NiveauItemDelegate : public QItemDelegate {
public:
    NiveauItemDelegate(QSqlTableModel *niveauModel, QObject *parent = nullptr)
        : QItemDelegate(parent), niveauModel(niveauModel) {
        // Récupérer les valeurs possibles depuis la table "NIVEAU_ACADEMIQUE"
        for (int row = 0; row < niveauModel->rowCount(); ++row) {
            niveauValues.append(niveauModel->data(niveauModel->index(row, 0)).toString());
        }
    }

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        if (index.column() == colonneNiveau) {
            // Créez un widget d'édition avec les valeurs possibles
            QComboBox *comboBox = new QComboBox(parent);
            comboBox->addItems(niveauValues);
            QAbstractItemView *itemView = comboBox->view();
            if (itemView) {
                itemView->setStyleSheet("QAbstractItemView {"
                                        "   alignment: center;"
                                        "}"
                                        );
            }
            comboBox->setStyleSheet("QComboBox {"
                                    "   padding-left: 57px;"
                                    "}"
                                    );
            return comboBox;
        }

        return QItemDelegate::createEditor(parent, option, index);
    }

    // ... Autres fonctions du délégué

private:
    const int colonneNiveau = 3;
    QStringList niveauValues;
    QSqlTableModel *niveauModel;
};

#endif // NIVEAUITEMDELEGATE_H
