// alternaterowcolordelegate.h
#ifndef ALTERNATEROWCOLORDELEGATE_H
#define ALTERNATEROWCOLORDELEGATE_H

#include <QStyledItemDelegate>

class AlternateRowColorDelegate : public QStyledItemDelegate {
public:
    AlternateRowColorDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // ALTERNATEROWCOLORDELEGATE_H
