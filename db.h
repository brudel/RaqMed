#ifndef DB_H
#define DB_H

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
    static PGconn* configDB();
    static void freeConn();
    static PGresult* safeExec(const char *command, int nParams, const char *const *paramValues,
        std::vector<char*> expectedErros = {});
    static PGresult* nonconnectionErrorHandler(PGresult* res, const char *command, int nParams, const char *const *paramValues,
        std::vector<char*> expectedErros);
    static void createReconectWindow();
    static void tryReconnect();
    static void unknownDBError(PGresult* res, const char *command, int nParams, const char *const *paramValues);
    static bool rollBack();
    static bool backupTable(string table, FILE* file);
    static bool backupDB(string path);
    static void periodicBackup();

    //Inline functions
    static inline void setMainWindow(QWidget* mW) {mainWindow = mW;}

    static inline PGresult* Exec(string command, std::vector<QString> params, std::vector<char*> expectedErros = {})
        {return safeExec(command.c_str(), params.size(), QUtils::sVecToCVec(QUtils::qVecToSVec(params)).data(), expectedErros);}

    static inline PGresult* Exec(string command, std::vector<string> params, std::vector<char*> expectedErros = {})
        {return safeExec(command.c_str(), params.size(), QUtils::sVecToCVec(params).data(), expectedErros);}

    static inline PGresult* Exec(string command, std::vector<char*> params, std::vector<char*> expectedErros = {})
        {return safeExec(command.c_str(), params.size(), params.data(), expectedErros);}

    static PGresult* Exec(string command, QString param, std::vector<char*> expectedErros = {})
        {string str = param.toStdString();
        const char* buff = str.c_str();
        return safeExec(command.c_str(), 1, &buff, expectedErros);}

    static PGresult* Exec(string command, string param, std::vector<char*> expectedErros = {})
        {const char* buff = param.c_str();
        return safeExec(command.c_str(), 1, &buff, expectedErros);}

    static PGresult* Exec(string command, const char* param, std::vector<char*> expectedErros = {})
        {return safeExec(command.c_str(), 1, &param, expectedErros);}

    static PGresult* Exec(string command, char* param, std::vector<char*> expectedErros = {})
        {return safeExec(command.c_str(), 1, &param, expectedErros);}

    static PGresult* ExecCommand(string command, std::vector<char*> expectedErros = {})
        {return safeExec(command.c_str(), 0, nullptr, expectedErros);}

    //DB variables
    static PGconn* conn;
    static QWidget* mainWindow;
    static QMessageBox* reconnectWindow;
    static bool rb;
    static bool autobackup;

    //Model fields
    static QStringList* fieldNames;
    static std::vector<string> tableFields; //Does not contain 'name' for select purposes
    static string tableFieldsLine;

    //Tabs
    static QStringList* tabNames;
    static std::vector<string> tableTabs;
    static string tableTabsLine;
};

#endif // DB_H
