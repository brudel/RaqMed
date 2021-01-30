#include "patientbdmodel.h"
#include <QMessageBox>
#include <QPushButton>


//Boolean config defines
//#define CLOUD_DB
//#define DB_VERBOSE

#if defined(__unix__)
    #define DATABASE_CONFIG_FILE "/etc/RaqMed/database.config"
#elif defined(_WIN32)
    #define DATABASE_CONFIG_FILE "database.config"
#else
    #error Unknown environment!
#endif

#ifdef CLOUD_DB
    PGconn* PatientBDModel::conn = PatientBDModel::setConn();
#else
    PGconn* PatientBDModel::conn = PQconnectdb("user=brudel dbname=rqm_pgloader");
#endif

QWidget* PatientBDModel::mainWindow = nullptr;

QMessageBox* PatientBDModel::reconnectWindow = nullptr;

QStringList* PatientBDModel::fieldNames = new QStringList({"Nome", "Data de aniversário", "Endereço", "Bairro", "Cidade", "Estado",
"Telefone 1", "Telefone 2", "Telefone 3", "Email", "Nome da mãe", "Profissão da mãe", "Nome do pai",
"Profissão do pai", "Indicação", "Observações"});

std::vector<string> PatientBDModel::tableFields({"name", "birthdate", "address", "district", "city", "state", "phone1","phone2", "phone3",
"email", "mother", "mother_occupation", "father", "father_occupation", "recommendation", "notes"});

string PatientBDModel::tableFieldsLine = QUtils::constructLine(PatientBDModel::tableFields.begin() + 1,
PatientBDModel::tableFields.end());

QStringList* PatientBDModel::tabNames = new QStringList({"Motivos", "Antecedentes", "Exames", "Relatórios"});

std::vector<string> PatientBDModel::tableTabs({"reasons", "antecedents", "exams", "reports"});

string PatientBDModel::tableTabsLine = QUtils::constructLine(PatientBDModel::tableTabs);

PatientBDModel::PatientBDModel(char* _patient, QObject* parent) :
    QAbstractListModel(parent)
{
    fieldValues.reserve(tableFields.size());
    fieldValues.push_back(_patient);

    PGresult* res = DBExec("SELECT " + tableFieldsLine + " FROM patient WHERE name = $1", _patient);
    if (res == nullptr)
    {
        invalid = true;
        return;
    }

    for (int i = 0; i < tableFields.size() - 1; ++i)
        fieldValues.push_back(PQgetvalue(res, 0, i));
    PQclear(res);
}

Qt::ItemFlags PatientBDModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemNeverHasChildren | Qt::ItemIsEditable;
}

QVariant PatientBDModel::data(const QModelIndex &index, int role) const
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

QVariant PatientBDModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation == Qt::Horizontal)
        return QVariant::Invalid;

    return QVariant(fieldNames->at(section));
}

int PatientBDModel::rowCount(const QModelIndex &parent) const
{
    return fieldNames->size();
}

bool PatientBDModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    std::vector<char*> pgValues({QUtils::ToCString(value.toString()), (char*)fieldValues.front().c_str()});

    PGresult* res = DBExec("UPDATE patient SET " + tableFields[index.row()] + " = $1  WHERE name = $2", pgValues);

    free(pgValues.front());

    if (res == nullptr)
        return false;
    PQclear(res);

    fieldValues[index.row()] = value.toString().toStdString();

    dataChanged(index, index, QVector<int>({Qt::EditRole}));
    if (index.row() == 0)
        nameEdited(QUtils::ToCString(fieldValues.front()));

    if (index.row() == NOTES_INDEX)
        notesCellEdited();

    return true;
}

PGconn* PatientBDModel::setConn()
{
    char* config;
    FILE* database = fopen(DATABASE_CONFIG_FILE, "r");
    PGconn* conn;

    config = QUtils::readFile(database);
    conn = PQconnectdb(config);
    free(config);

    return conn;
}

void PatientBDModel::setMainWindow(QWidget* mW)
{
    mainWindow = mW;
}

void PatientBDModel::createReconectWindow()
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
    connect(qb, &QPushButton::clicked, &PatientBDModel::tryReconnect);
    reconnectWindow->show();
}

void PatientBDModel::tryReconnect()
{
    PQreset(conn);
    if (IS_CONNECTION_OK)
        QMessageBox::about(mainWindow, "Reconectado com sucesso",
            "A conexão com o banco de dados foi reestabelecida com sucesso, você pode continuar o trabalho.");
    else
        reconnectWindow->show();
}

PGresult* PatientBDModel::safeBDExec(const char *command, int nParams, const char *const *paramValues)
{

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

    PGresult* res = PQexecParams(conn, command, nParams, nullptr, paramValues, nullptr, nullptr, 0);

    if (IS_RESULT_OK(res))
        return res;

    PQclear(res);
    PQreset(conn);
    if (IS_CONNECTION_OK)
    {
        res = PQexecParams(conn, command, nParams, nullptr, paramValues, nullptr, nullptr, 0);
        if (IS_RESULT_OK(res))
            return res;

        PQclear(res);
    }

    createReconectWindow();
    return nullptr;
}

PGresult* PatientBDModel::DBExec(string command, std::vector<char*> params)
{
    return safeBDExec(command.c_str(), params.size(), params.data());
}

PGresult* PatientBDModel::DBExec(string command, QString param)
{
    string str = param.toStdString();
    const char* buff = str.c_str();
    return safeBDExec(command.c_str(), 1, &buff);
}

PGresult* PatientBDModel::DBExec(string command, string param)
{
    const char* buff = param.c_str();
    return safeBDExec(command.c_str(), 1, &buff);
}

PGresult* PatientBDModel::DBExec(string command, char* param)
{
    return safeBDExec(command.c_str(), 1, &param);
}


PGresult* PatientBDModel::DBExec(string command)
{
    return safeBDExec(command.c_str(), 0, nullptr);
}
