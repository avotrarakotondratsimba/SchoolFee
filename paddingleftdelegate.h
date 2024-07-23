// paddingleftdelegate.h
#pragma once

#include <QStyledItemDelegate>

class PaddingLeftDelegate : public QStyledItemDelegate {
public:
    PaddingLeftDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};
