// paddingleftdelegate.cpp
#include "paddingleftdelegate.h"
#include <QPainter>
#include <QRect>

PaddingLeftDelegate::PaddingLeftDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {}

void PaddingLeftDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    if (index.column() == 3) {  // Assurez-vous que vous traitez la quatrième colonne (indice 3)
        // Créez une copie modifiable de l'objet QRect
        QRect adjustedRect = option.rect;
        adjustedRect.adjust(25, 0, 0, 0);  // Ajustez la copie modifiable

        QStyleOptionViewItem adjustedOption = option;
        adjustedOption.rect = adjustedRect;

        QStyledItemDelegate::paint(painter, adjustedOption, index);
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}
