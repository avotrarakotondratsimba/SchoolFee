// alternaterowcolordelegate.cpp
#include "alternaterowcolordelegate.h"
#include <QPainter>

AlternateRowColorDelegate::AlternateRowColorDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

void AlternateRowColorDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    // Alternez les couleurs en fonction du numéro de ligne
    if (index.row() % 2 == 0) {
        // Ligne paire : couleur bleue en hexadécimal (#ABD6EA)
        painter->fillRect(option.rect, Qt::blue);
    } else {
        // Ligne impaire : couleur blanche noircie en hexadécimal (#F5EDE0)
        painter->fillRect(option.rect, Qt::white);
    }

    // Rendu du texte
    QStyledItemDelegate::paint(painter, option, index);
}
