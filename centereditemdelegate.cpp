// centereditemdelegate.cpp
#include "centereditemdelegate.h"
#include <QPainter>
#include <QStyleOptionViewItem>

CenteredItemDelegate::CenteredItemDelegate(QObject *parent)
    : QItemDelegate(parent) {}

void CenteredItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyleOptionViewItem centeredOption = option;
    centeredOption.displayAlignment = Qt::AlignCenter;
    QItemDelegate::paint(painter, centeredOption, index);
}
