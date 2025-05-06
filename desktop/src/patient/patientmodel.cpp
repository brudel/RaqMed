/*  Copyright 2021 Bruno Del Monde.
 * Licensed under the EUPL-1.2.
 */

#include "patient/patientmodel.h"

PatientModel::PatientModel(char* _patient, QObject* parent) :
    QAbstractListModel(parent)
{
    fieldValues.reserve(DB::tableFields.size());
    fieldValues.push_back(_patient);

    PGresult* res = DB::Exec("SELECT " + DB::tableFieldsLine + " FROM patient WHERE name = $1", _patient);
    if (res == nullptr)
        throw 0;

    for (int i = 0; i < DB::tableFields.size() - 1; ++i)
        fieldValues.push_back(PQgetvalue(res, 0, i));
    PQclear(res);
}

Qt::ItemFlags PatientModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemNeverHasChildren | Qt::ItemIsEditable;
}

QVariant PatientModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant::Invalid;

    if (index.row() == BIRTHDATE_INDEX)
        return QVariant(QDate::fromString(fieldValues[index.row()].c_str(), Qt::ISODate));

    return QVariant(fieldValues[index.row()].c_str());

    /* Code for top align 'Observações'

        if (role == Qt::TextAlignmentRole && index.row() == 14)
            return QVariant(Qt::AlignTop);
    */

}

QVariant PatientModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation == Qt::Horizontal)
        return QVariant::Invalid;

    return QVariant(DB::fieldNames->at(section));
}

int PatientModel::rowCount(const QModelIndex &parent) const
{
    return FIELDS_NUM;
}

bool PatientModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    std::vector<string> pgValues({value.toString().toStdString(), fieldValues.front()});

    PGresult* res = DB::Exec("UPDATE patient SET " + DB::tableFields[index.row()] + " = $1  WHERE name = $2", pgValues);

    if (res == nullptr)
        return false;
    PQclear(res);

    fieldValues[index.row()] = value.toString().toStdString();

    dataChanged(index, index, QVector<int>({Qt::EditRole}));

    if (index.row() == 0)
        nameEdited(QUtils::ToCString(fieldValues.front()));

    else if (index.row() == BIRTHDATE_INDEX)
        birthdayEdited(value.toDate());

    else if (index.row() == NOTES_INDEX)
        notesCellEdited();

    return true;
}
