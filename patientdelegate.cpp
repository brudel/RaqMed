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
