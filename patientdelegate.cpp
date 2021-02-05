#include "patientdelegate.h"
#include "db.h"

PatientDelegate::PatientDelegate(QWidget *parent) : QStyledItemDelegate(parent)
{
}

QWidget *PatientDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & option, const QModelIndex &index) const
{
    if (index.row() == NOTES_INDEX)
        return new QPlainTextEdit(parent);

    return QStyledItemDelegate::createEditor(parent, option, index);
}

/*
void PatientDelegate::setEditorData(QWidget *editor,
                                 const QModelIndex &index) const
{
    if (index.row() == 14)
    {
        QPlainTextEdit *pteditor = qobject_cast<QPlainTextEdit *>(editor);
        pteditor->setPlainText(index.data().toString());
    }

    else
    {
        QLineEdit *leditor = qobject_cast<QLineEdit *>(editor);
        leditor->setText(index.data().toString());
    }
}

void PatientDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    if (index.row() == 14)
    {
        QPlainTextEdit *pteditor = qobject_cast<QPlainTextEdit *>(editor);
        model->setData(index, pteditor->toPlainText());
    }

    else
    {
        QLineEdit *leditor = qobject_cast<QLineEdit *>(editor);
        model->setData(index, leditor->text());
    }
}
*/
