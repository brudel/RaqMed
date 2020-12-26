#include "patientbdmodel.h"
#include <QMessageBox>


//Boolean config defines
//#define CLOUD_DB
//#define DB_VERBOSE

#ifdef DB_VERBOSE
    #define DB_VERBOSE_CODE \
        printf("intern %s;\n", command.c_str()); \
        fflush(stdout);
#else
    #define DB_VERBOSE_CODE
#endif

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

    PGresult* ans = BDExec("SELECT " + tableFieldsLine + " FROM patient WHERE name = $1", _patient);

    for (int i = 0; i < tableFields.size() - 1; ++i)
        fieldValues.push_back(PQgetvalue(ans, 0, i));
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

    PGresult* ans = BDExec("UPDATE patient SET " + tableFields[index.row()] + " = $1  WHERE name = $2", pgValues);

    fieldValues[index.row()] = value.toString().toStdString();

    dataChanged(index, index, QVector<int>({Qt::EditRole}));
    if (index.row() == 0)
        nameEdited(fieldValues.front().c_str());

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

PGresult* PatientBDModel::BDExec(string command, std::vector<char*> params)
{
    DB_VERBOSE_CODE

    return PQexecParams(conn, command.c_str(), params.size(), nullptr, params.data(), nullptr, nullptr, 0);
}

PGresult* PatientBDModel::BDExec(string command, QString param)
{
    DB_VERBOSE_CODE

    string str = param.toStdString();
    const char* buff = str.c_str();
    return PQexecParams(conn, command.c_str(), 1, nullptr, &buff, nullptr, nullptr, 0);
}

PGresult* PatientBDModel::BDExec(string command, string param)
{
    DB_VERBOSE_CODE

    const char* buff = param.c_str();
    return PQexecParams(conn, command.c_str(), 1, nullptr, &buff, nullptr, nullptr, 0);
}

PGresult* PatientBDModel::BDExec(string command, char* param)
{
    DB_VERBOSE_CODE

    return PQexecParams(conn, command.c_str(), 1, nullptr, &param, nullptr, nullptr, 0);
}


PGresult* PatientBDModel::BDExec(string command)
{
    DB_VERBOSE_CODE

    return PQexec(conn, command.c_str());
}
