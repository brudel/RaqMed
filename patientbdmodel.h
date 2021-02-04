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
#define TABS_NUM 4
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
    static PGresult* safeBDExec(const char *command, int nParams, const char *const *paramValues,
        std::vector<char*> expectedErros = {});
    static void unknownDBError(PGresult* res, const char *command, int nParams, const char *const *paramValues);
    static PGresult* DBExec(string command, std::vector<char*> params, std::vector<char*> expectedErros = {});
    static PGresult* DBExec(string command, QString param, std::vector<char*> expectedErros = {});
    static PGresult* DBExec(string command, string param, std::vector<char*> expectedErros = {});
    static PGresult* DBExec(string command, char* param, std::vector<char*> expectedErros = {});
    static PGresult* DBExecCommand(string command, std::vector<char*> expectedErros = {});
    static bool rollBack();
    static PGresult* nonconnectionErrorHandler(PGresult* res, const char *command, int nParams, const char *const *paramValues,
        std::vector<char*> expectedErros);

    //DB variables
    static PGconn* conn;
    static QWidget* mainWindow;
    static QMessageBox* reconnectWindow;
    static bool rb;

    //Model fields
    static QStringList* fieldNames;
    static std::vector<string> tableFields; //Does not contain 'name' for select purposes
    static string tableFieldsLine;

    //Tabs
    static QStringList* tabNames;
    static std::vector<string> tableTabs;
    static string tableTabsLine;

signals:
    void nameEdited(char* name);
    void birthdayEdited(QDate birthday);
    void notesCellEdited();
};

#endif // PATIENTBDMODEL_H
