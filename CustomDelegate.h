// CustomDelegate.h
#ifndef CUSTOMDELEGATE_H
#define CUSTOMDELEGATE_H

#include <QStyledItemDelegate>
#include <QSqlTableModel>

class CustomDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    CustomDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    QStyledItemDelegate *niveauDelegate;
    QStyledItemDelegate *paddingLeftDelegate;
    QSqlTableModel *niveauModel;
};

#endif // CUSTOMDELEGATE_H
