// CustomDelegate.cpp
#include "CustomDelegate.h"
#include "NiveauItemDelegate.h"
#include "PaddingLeftDelegate.h" // Assurez-vous d'inclure le bon fichier d'en-tête pour PaddingLeftDelegate

CustomDelegate::CustomDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {
    niveauDelegate = new NiveauItemDelegate(niveauModel, this);
    paddingLeftDelegate = new PaddingLeftDelegate(this);
}

void CustomDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (index.column() == 3) {
        // Appliquez la logique du PaddingLeftDelegate ici
        paddingLeftDelegate->paint(painter, option, index);
    } else {
        // Appliquez la logique du NiveauItemDelegate pour les autres colonnes
        niveauDelegate->paint(painter, option, index);
    }
}

QWidget *CustomDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (index.column() == 3) {
        // Appliquez la logique du PaddingLeftDelegate ici
        return paddingLeftDelegate->createEditor(parent, option, index);
    } else {
        // Appliquez la logique du NiveauItemDelegate pour les autres colonnes
        return niveauDelegate->createEditor(parent, option, index);
    }
}

// Implémentez les autres méthodes virtuelles de QStyledItemDelegate de manière similaire.
