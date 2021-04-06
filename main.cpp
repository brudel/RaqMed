#include <QApplication>
#include <filesystem>
#include <fstream>
#include <QMessageBox>
#include "calendar.h"
#include "autosaverestore.h"
#include "settingsform.h"

namespace fs = std::filesystem;

#define TESTE_DB

#ifdef TESTE_DB
    #define TESTE_CONN free(config); config = strdup("user=brudel dbname=raqmed");
#else
    #define TESTE_CONN
#endif

void setConfigs()
{

    char* config, c;
    FILE* configFile;

    if (!fs::exists(LOCALDIR))
    {
        fs::create_directory(LOCALDIR);
        fs::permissions(LOCALDIR, fs::perms::owner_all);
        fs::create_directory(AUTOBACKUP_DIR, LOCALDIR);
        fs::create_directory(AUTOSAVES_DIR, LOCALDIR);
    }

    if (!fs::exists(CONFIG_FILE))
    {
        QMessageBox wellcome(QMessageBox::NoIcon, "RaqMed", "Bem vindo ao RaqMed!\n"
            "Será aberta uma janela de configuração, para iniciar o uso do programa você inserir o endereço do servidor"
            " de banco de dados, em caso de dúvidas consulte o fornecedor do programa.");
        wellcome.setIconPixmap(QPixmap(":icon.png"));
        wellcome.exec();

        SettingsForm sf(nullptr, SettingsForm::init | SettingsForm::create);
        sf.exec();

        return;
    }

    configFile = fopen(CONFIG_FILE.c_str(), "r");

    config = QUtils::readFileLine(configFile);
    TESTE_CONN;
    DB::conn = PQconnectdb(config);
    free(config);

    atexit(&DB::freeConn);

    fscanf(configFile, "QUtils::autobackup %c", &c);

    if (c == '1')
        QUtils::autobackup = true;
    else if (c == '0')
        QUtils::autobackup = false;
}

void recoverConn()
{
    int r;

    while (1) {
        r = QMessageBox::warning(nullptr, "Erro de conexão", "Não foi possível se conectar ao banco de dados.",
            "Tentar novamente", "Fechar", "Editar configurações");

        if (r != 0)
            break;

        PQreset(DB::conn);
        if (IS_CONNECTION_OK)
            return;
    }

    if (r == 1)
        exit(0);

    if (r == 2)
    {
        SettingsForm sf(nullptr, SettingsForm::init);
        sf.exec();
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":icon.png"));
    setlocale(LC_ALL, "C"); //Unseted by QApplication constructor

    setConfigs();

    if (PQstatus(DB::conn) == CONNECTION_BAD)
        recoverConn();

    if (QUtils::autobackup)
        DB::periodicBackup();

    Calendar c;
    c.showMaximized();

    AutosaveRestore ar;

    return a.exec();
}
