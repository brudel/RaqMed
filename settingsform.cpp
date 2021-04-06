#include "settingsform.h"
#include <filesystem>
#include "db.h"

namespace fs = std::filesystem;

SettingsForm::SettingsForm(QWidget *parent, formMode _mode) : QDialog(parent, Qt::Window | Qt::Tool), mode(_mode)
{
    setWindowTitle("Configurações");
    resize(600, 400); //#Find elegant answer

    if (!(mode & create))
    {
        char* connStr, c;
        FILE* configFile = fopen(CONFIG_FILE.c_str(), "r");

        connStr = QUtils::readFileLine(configFile);
        connStrLineEdit->setText(connStr);
        free(connStr);

        fscanf(configFile, "autobackup %c", &c);
        autoBkpChkBtn->setChecked(c == '1');

        fclose(configFile);
    }

    connStrLayout->addWidget(connStrLabel);
    connStrLayout->addWidget(connStrLineEdit);
    mainLayout->addLayout(connStrLayout);

    mainLayout->addWidget(autoBkpChkBtn);

    buttonsLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    buttonsLayout->addWidget(saveButton);
    cancelButton =  new QPushButton(init ? "Fechar" : "Cancelar", this);
    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    mainLayout->addLayout(buttonsLayout);

    connect(saveButton, SIGNAL(clicked(bool)), SLOT(save()));
    connect(cancelButton, SIGNAL(clicked(bool)), SLOT(cancel()));
}


void SettingsForm::save()
{
    int r;
    string connStr = connStrLineEdit->text().toStdString();

    FILE* configFile = fopen(CONFIG_FILE.c_str(), "w");

    fprintf(configFile, "%s\nautobackup %c\n", connStr.c_str(),
            autoBkpChkBtn->isChecked() ? '1': '0');

    fclose(configFile);

    if (mode & create)
        fs::permissions(CONFIG_FILE, fs::perms::owner_read | fs::perms::owner_write);

    QUtils::autobackup = autoBkpChkBtn->isChecked();

    if (mode & init)
    {
        DB::conn = PQconnectdb(connStr.c_str());
        if (IS_CONNECTION_OK)
        {
            close();
            return;
        }

        while (1) {
            r = QMessageBox::warning(this, "Erro de conexão", "Não foi possível se conectar ao banco de dados,"
                "verifique o endereço fornececido e a sua conexão de internet e tente novamente.",
                "Editar", "Tentar conectar novamente", "Fechar");

            if (r != 1)
                break;

            PQreset(DB::conn);
            if (IS_CONNECTION_OK)
                close();
        };

        if (r == 0)
            return;

        if (r == 2)
            exit(0);
    }
    else
        if (connStrLineEdit->isModified())
            exit(0);

    close();
}

void SettingsForm::cancel()
{
    if (mode & init)
        exit(0);
    else
        close();
}
