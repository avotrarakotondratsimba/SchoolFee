// centereditemdelegate.h
#ifndef CENTEREDITEMDELEGATE_H
#define CENTEREDITEMDELEGATE_H

#include <QItemDelegate>

class CenteredItemDelegate : public QItemDelegate {
public:
    CenteredItemDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // CENTEREDITEMDELEGATE_H
