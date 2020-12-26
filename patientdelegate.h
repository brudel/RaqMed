#ifndef PATIENTDELEGATE_H
#define PATIENTDELEGATE_H

#include <QStyledItemDelegate>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QTableView>

/*  Personalized Delegate to patient

Differ of QStyledItemDelegate because notes cell is multilined,
so demands a aproprieted editor (QPlainTextEdit)
*/

class PatientDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit PatientDelegate(QWidget *parent = 0);
    //explicit PatientDelegate(QTableView *view, QWidget *parent = 0);

    //Delegate overwrite functions
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    //virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    //virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    //Variables
    //QTableView *view;
};

#endif // PATIENTDELEGATE_H
