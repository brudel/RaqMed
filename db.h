#ifndef PATIENTBDMODEL_H
#define PATIENTBDMODEL_H

#include <QDate>
#include <QMessageBox>
#include "qutils.h"
extern "C" {
#include <libpq-fe.h>
}

#define FIELDS_NUM 16
#define TABS_NUM 4
#define BIRTHDATE_INDEX 1
#define NOTES_INDEX (FIELDS_NUM - 1)
#define IS_CONNECTION_OK (PQstatus(conn) == CONNECTION_OK)
#define IS_RESULT_OK(res) (PQresultStatus(res) != PGRES_FATAL_ERROR)

class DB
{
public:

    //DB functions
    static PGconn* setConn();
    static void setMainWindow(QWidget* mW);
    static PGresult* Exec(string command, std::vector<char*> params, std::vector<char*> expectedErros = {});
    static PGresult* Exec(string command, QString param, std::vector<char*> expectedErros = {});
    static PGresult* Exec(string command, string param, std::vector<char*> expectedErros = {});
    static PGresult* Exec(string command, char* param, std::vector<char*> expectedErros = {});
    static PGresult* ExecCommand(string command, std::vector<char*> expectedErros = {});
    static PGresult* safeExec(const char *command, int nParams, const char *const *paramValues,
        std::vector<char*> expectedErros = {});
    static PGresult* nonconnectionErrorHandler(PGresult* res, const char *command, int nParams, const char *const *paramValues,
        std::vector<char*> expectedErros);
    static void createReconectWindow();
    static void tryReconnect();
    static void unknownDBError(PGresult* res, const char *command, int nParams, const char *const *paramValues);
    static bool rollBack();

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
};

#endif // PATIENTBDMODEL_H
