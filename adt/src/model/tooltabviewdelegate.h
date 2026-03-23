#ifndef TOOLTABVIEWDELEGATE_H
#define TOOLTABVIEWDELEGATE_H

#include "adttool.h"

#include <QPainter>
#include <QStyledItemDelegate>

#include <gui/toolwidget.h>

class ToolTabViewDelegate : public QStyledItemDelegate
{
public:
    ToolTabViewDelegate(ADTTool *tool, int vPadding = 10, QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;

    void setWidget(ToolWidget *widget);

private slots:
    void commit();

private:
    ADTTool *m_tool;

    ToolWidget *m_widget;

    /*
     *  NOTE: QSpinBox does not provide ability of modifying value
     *  without emitting a signal. We need to avoid emitting commitData()
     *  while initial values are filled to editors form a model.
     */
    mutable bool may_commit{true};

private:
    ToolTabViewDelegate(const ToolTabViewDelegate &)            = delete;
    ToolTabViewDelegate(ToolTabViewDelegate &&)                 = delete;
    ToolTabViewDelegate &operator=(const ToolTabViewDelegate &) = delete;
    ToolTabViewDelegate &operator=(ToolTabViewDelegate &&)      = delete;
};

#endif // TOOLTABVIEWDELEGATE_H
