#include "db.h"
#include <QMessageBox>
#include <QPushButton>
#include "qutils.h"


//Boolean config defines
//#define CLOUD_DB
//#define DB_VERBOSE

#define LOGFILE "dblog.txt"

#if defined(__unix__)
    #define DATABASE_CONFIG_FILE "/etc/RaqMed/database.config"
#elif defined(_WIN32)
    #define DATABASE_CONFIG_FILE "database.config"
#else
    #error Unknown environment!
#endif

#ifdef CLOUD_DB
    PGconn* DB::conn = DB::setConn();
#else
    PGconn* DB::conn = PQconnectdb("user=brudel dbname=rqm_pgloader");
#endif

QWidget* DB::mainWindow = nullptr;

QMessageBox* DB::reconnectWindow = nullptr;

bool DB::rb = false;

QStringList* DB::fieldNames = new QStringList({"Nome", "Data de aniversário", "Endereço", "Bairro", "Cidade", "Estado",
"Telefone 1", "Telefone 2", "Telefone 3", "Email", "Nome da mãe", "Profissão da mãe", "Nome do pai",
"Profissão do pai", "Indicação", "Observações"});

std::vector<string> DB::tableFields({"name", "birthdate", "address", "district", "city", "state", "phone1","phone2", "phone3",
"email", "mother", "mother_occupation", "father", "father_occupation", "recommendation", "notes"});

string DB::tableFieldsLine = QUtils::constructLine(DB::tableFields.begin() + 1,
DB::tableFields.end());

QStringList* DB::tabNames = new QStringList({"Motivos", "Antecedentes", "Exames", "Relatórios"});

std::vector<string> DB::tableTabs({"reasons", "antecedents", "exams", "reports"});

string DB::tableTabsLine = QUtils::constructLine(DB::tableTabs);

PGconn* DB::setConn()
{
    char* config;
    FILE* database = fopen(DATABASE_CONFIG_FILE, "r");
    PGconn* conn;

    config = QUtils::readFileLine(database);
    conn = PQconnectdb(config);
    free(config);

    atexit(&freeConn);
    return conn;
}

void DB::freeConn()
{
    PQfinish(conn);
}

void DB::setMainWindow(QWidget* mW)
{
    mainWindow = mW;
}

PGresult* DB::Exec(string command, std::vector<char*> params, std::vector<char *> expectedErros)
{
    return safeExec(command.c_str(), params.size(), params.data(), expectedErros);
}

PGresult* DB::Exec(string command, QString param, std::vector<char *> expectedErros)
{
    string str = param.toStdString();
    const char* buff = str.c_str();
    return safeExec(command.c_str(), 1, &buff, expectedErros);
}

PGresult* DB::Exec(string command, string param, std::vector<char *> expectedErros)
{
    const char* buff = param.c_str();
    return safeExec(command.c_str(), 1, &buff, expectedErros);
}

PGresult* DB::Exec(string command, char* param, std::vector<char *> expectedErros)
{
    return safeExec(command.c_str(), 1, &param, expectedErros);
}


PGresult* DB::ExecCommand(string command, std::vector<char *> expectedErros)
{
    return safeExec(command.c_str(), 0, nullptr, expectedErros);
}

PGresult* DB::safeExec(const char *command, int nParams, const char *const *paramValues,
    std::vector<char*> expectedErros)
{
    PGresult* res;

#ifdef DB_VERBOSE
        printf("intern %s;\n", command); \
        fflush(stdout);
#endif

    if (!IS_CONNECTION_OK)
    {
        reconnectWindow->show();
        reconnectWindow->activateWindow();
        return nullptr;
    }

    if (rb)
    {
        res = PQexec(conn, "ROLLBACK");
        PQclear(res);
        if (!IS_CONNECTION_OK)
        {
            createReconectWindow();
            return nullptr;
        }

        rb = false;
    }

    res = PQexecParams(conn, command, nParams, nullptr, paramValues, nullptr, nullptr, 0);

    if (IS_RESULT_OK(res))
        return res;

    if (IS_CONNECTION_OK)
        return nonconnectionErrorHandler(res, command, nParams, paramValues, expectedErros);

    PQclear(res);
    PQreset(conn);
    if (IS_CONNECTION_OK)
    {
        res = PQexecParams(conn, command, nParams, nullptr, paramValues, nullptr, nullptr, 0);
        if (IS_RESULT_OK(res))
            return res;

        if (IS_CONNECTION_OK)
            return nonconnectionErrorHandler(res, command, nParams, paramValues, expectedErros);

        PQclear(res);
    }

    createReconectWindow();
    return nullptr;
}

PGresult* DB::nonconnectionErrorHandler(PGresult* res, const char *command, int nParams,
    const char *const *paramValues, std::vector<char*> expectedErros)
{
    char* sqlstate = PQresultErrorField(res, PG_DIAG_SQLSTATE);

    for (int i = 0; i < expectedErros.size(); ++i)
        if (!strcmp(expectedErros[i], sqlstate))
        {
			PQclear(res);
            throw i;
        }

    unknownDBError(res, command, nParams, paramValues);
    PQclear(res);
    return nullptr;
}

void DB::createReconectWindow()
{
    if (reconnectWindow != nullptr)
    {
        reconnectWindow->show();
        return;
    }

    reconnectWindow = new QMessageBox(mainWindow);
    reconnectWindow->setText("A conexão com o servidor em nuvem que hospeda o banco de dados foi interrompida."
        "Verifique sua conexão com a internet.");
    reconnectWindow->setIcon(QMessageBox::Critical);
    reconnectWindow->setWindowModality(Qt::NonModal);
    QPushButton* qb = reconnectWindow->addButton("Reconectar", QMessageBox::ApplyRole);
    QObject::connect(qb, &QPushButton::clicked, &DB::tryReconnect);
    reconnectWindow->show();
}

void DB::tryReconnect()
{
    PQreset(conn);
    if (IS_CONNECTION_OK)
        QMessageBox::about(mainWindow, "Reconectado com sucesso",
            "A conexão com o banco de dados foi reestabelecida com sucesso, você pode continuar o trabalho.");
    else
        reconnectWindow->show();
}

void DB::unknownDBError(PGresult* res, const char *command, int nParams, const char *const *paramValues)
{
    FILE* logfile = fopen(LOGFILE, "a");

    if (logfile == nullptr)
    {
        QMessageBox::warning(mainWindow, "Erro do banco de dados",
            "Não foi possível realizar essa operação devido a um erro não identificado.");
        return;
    }

    fprintf(
        logfile,
        "{\n"
        "\ttime: \"%s\"\n"
        "\tError status: \"%s\"\n"
        "\tSQL state: \"%s\"\n"
        "\tError message: \"%s\"\n"
        "\tquery: \"%s\"\n"
        "\tArguments:\n"
        "\t{\n",
        QDateTime::currentDateTime().toString(Qt::ISODate).toStdString().c_str(),
        PQresStatus(PQresultStatus(res)),
        PQresultErrorField(res, PG_DIAG_SQLSTATE),
        PQresultErrorMessage(res),
        command
        );

    for (int i = 0; i < nParams; ++i)
        fprintf(logfile, "\t\t$%d: \"%s\"\n", i + 1, paramValues[i]);

    fprintf(logfile, "\t}\n}\n");

    fclose(logfile);

    QMessageBox::warning(mainWindow, "Erro do banco de dados",
        "Não foi possível realizar essa operação devido a um erro não identificado.\n"
        "O log do erro foi registrado, contate um desenvolvedor para análise.");
}

bool DB::rollBack()
{

    if (!IS_CONNECTION_OK)
    {
        rb = true;
        return false;
    }

    PGresult* res = PQexec(conn, "ROLLBACK");

    PQclear(res);

    if (!IS_CONNECTION_OK)
    {
        createReconectWindow();
        rb = true;
        return false;
    }

    return true;
}

bool DB::backupTable(string table, FILE* file)
{
    char* buf;
    int size;
    PGresult* res;

    //Header
    fprintf(file, "--\n-- Tabela: %s\n--\n\n", table.c_str());

    res = ExecCommand("SELECT * FROM " + table + " WHERE false");

    //Copy statement
    fprintf(file, "COPY public.%s (", table.c_str());
    size = PQnfields(res);
    for (int i = 0; i < size - 1; ++i)
        fprintf(file, "%s, ", PQfname(res, i));
    fprintf(file, "%s) FROM STDIN;\n", PQfname(res, size - 1));
    PQclear(res);

    //Data
    res = ExecCommand("COPY " + table + " TO STDOUT");
    PQclear(res);

    while ((size = PQgetCopyData(conn, &buf, 0)) > 0) //#Verify error -2
    {
        fwrite(buf, size, 1, file);
        PQfreemem(buf);
    }
    if (size == -2)
        return false;
    fprintf(file, "\\.\n\n\n");

    res = PQgetResult(conn);
    size = PQresultStatus(res);
    PQclear(res);

    //Check sucess
    if (size != PGRES_FATAL_ERROR)
        return true;
    else
        return false;
}

bool DB::backupDB(string path)
{
    bool sucess;
    FILE* file = fopen(path.c_str(), "w");

    //Headers
    fprintf(file, "--\n-- PostgreSQL database dump\n--\n\n");
    fprintf(file, "-- Dumped by RaqMed\n");
    fprintf(file, "-- Format based on pg_dump version 11.10 (Debian 11.10-0+deb10u1)\n\n");

    //Config
    fprintf(file, "SET statement_timeout = 0;\n"
        "SET lock_timeout = 0;\n"
        "SET idle_in_transaction_session_timeout = 0;\n"
        "SET client_encoding = 'UTF8';\n"
        "SET standard_conforming_strings = on;\n"
        "SELECT pg_catalog.set_config('search_path', '', false);\n"
        "SET check_function_bodies = false;\n"
        "SET xmloption = content;\n"
        "SET client_min_messages = warning;\n"
        "SET row_security = off;\n\n");

    //Backup tables
    sucess = backupTable("patient", file) && backupTable("appointment", file);

    //Complete or erase file
    if (sucess)
    {
        fprintf(file, "--\n-- PostgreSQL database dump complete\n--\n\n");
        fclose(file);
        return true;
    }
    else
    {
        fclose(file);
        remove(path.c_str());
        return false;
    }
}
