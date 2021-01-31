#ifndef PATIENTBDMODEL_H
#define PATIENTBDMODEL_H

#include <QAbstractListModel>
#include <QMessageBox>
#include <QWidget>
#include <qutils.h>
#include <QDate>
extern "C" {
#include <libpq-fe.h>
}

#define FIELDS_NUM 16
#define BIRTHDATE_INDEX 1
#define NOTES_INDEX (FIELDS_NUM - 1)
#define IS_CONNECTION_OK (PQstatus(conn) == CONNECTION_OK)
#define IS_RESULT_OK(res) (PQresultStatus(res) != PGRES_FATAL_ERROR)

class PatientBDModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit PatientBDModel(char* _patient, QObject* parent = nullptr);

    //Model functions
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QDate getBirthday();

    //Model variables
    bool invalid = false;
    std::vector<string> fieldValues;

    //DB functions
    static PGconn* setConn();
    static void setMainWindow(QWidget* mW);
    static void createReconectWindow();
    static void tryReconnect();
    static PGresult* safeBDExec(const char *command, int nParams, const char *const *paramValues);
    static PGresult* DBExec(string command, std::vector<char*> params);
    static PGresult* DBExec(string command, QString param);
    static PGresult* DBExec(string command, string param);
    static PGresult* DBExec(string command, char* param);
    static PGresult* DBExec(string command);

    //DB variables
    static PGconn* conn;
    static QWidget* mainWindow;
    static QMessageBox* reconnectWindow;

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
    void birthdayEdited(QDate birthday);
    void notesCellEdited();
};

#endif // PATIENTBDMODEL_H
