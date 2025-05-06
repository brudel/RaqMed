/*  Copyright 2021 Bruno Del Monde.
 * Licensed under the EUPL-1.2.
 */

#include "patient/patientdelegate.h"
#include "common/db.h"

PatientDelegate::PatientDelegate(QWidget *parent) : QStyledItemDelegate(parent)
{
}

QWidget *PatientDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & option, const QModelIndex &index) const
{
    if (index.row() == NOTES_INDEX)
        return new QPlainTextEdit(parent);

    return QStyledItemDelegate::createEditor(parent, option, index);
}
