/*  Copyright 2021 Bruno Del Monde.
 * Licensed under the EUPL-1.2.
 */

#ifndef PATIENTMODEL_H
#define PATIENTMODEL_H
#include <QAbstractListModel>
#include <QMessageBox>
#include <QDate>
#include "common/qutils.h"
#include "common/db.h"

class PatientModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit PatientModel(char* _patient, QObject* parent = nullptr);

    //Overwrited functions
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    //Other functions
    inline QDate getBirthday() {return QDate::fromString(fieldValues[BIRTHDATE_INDEX].c_str(), Qt::ISODate);}

    //Variables
    std::vector<string> fieldValues;

signals:
    void nameEdited(char* name);
    void birthdayEdited(QDate birthday);
    void notesCellEdited();
};


#endif // PATIENTMODEL_H
