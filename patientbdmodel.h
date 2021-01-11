#ifndef PATIENTBDMODEL_H
#define PATIENTBDMODEL_H

#include <QAbstractListModel>
#include <qutils.h>
extern "C" {
#include <libpq-fe.h>
}

#define FIELDS_NUM 16
#define BIRTHDATE_INDEX 1
#define NOTES_INDEX FIELDS_NUM - 1
#define RESULT_OK(res) PQresultStatus(res) != PGRES_FATAL_ERROR

class PatientBDModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit PatientBDModel(char* _patient, QObject* parent = nullptr);

    //Model overwrite functions
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    //Model variables
    std::vector<string> fieldValues;

    //DB functions
    static PGconn* setConn();
    static PGresult* safeBDExec(const char *command, int nParams, const char *const *paramValues);
    static PGresult* BDExec(string command, std::vector<char*> params);
    static PGresult* BDExec(string command, QString param);
    static PGresult* BDExec(string command, string param);
    static PGresult* BDExec(string command, char* param);
    static PGresult* BDExec(string command);

    //DB
    static PGconn* conn;

    //DB human names
    static QStringList* fieldNames;
    static std::vector<string> tableFields; //Does not contain 'name' for select purposes
    static string tableFieldsLine;

    //Column names
    static QStringList* tabNames;
    static std::vector<string> tableTabs; //Does not contain 'name' for select purposes
    static string tableTabsLine;

signals:
    void nameEdited(char* name);
    void notesCellEdited();
};

#endif // PATIENTBDMODEL_H
