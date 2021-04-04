#include "settingsform.h"
#include "db.h"

SettingsForm::SettingsForm(QWidget *parent, bool blank) : QWidget(parent, Qt::Window | Qt::Tool)
{
    setWindowTitle("Configurações");
    resize(600, 400); //#Find elegant answer

    if (!blank)
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
    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    mainLayout->addLayout(buttonsLayout);

    connect(saveButton, SIGNAL(clicked(bool)), SLOT(save()));
    connect(cancelButton, SIGNAL(clicked(bool)), SLOT(close()));
}


void SettingsForm::save()
{
    string connStr = connStrLineEdit->text().toStdString();
    FILE* configFile = fopen(CONFIG_FILE.c_str(), "w");

    fprintf(configFile, "%s\nautobackup %c\n", connStr.c_str(),
            autoBkpChkBtn->isChecked() ? '1': '0');

    fclose(configFile);

    DB::conn = PQconnectdb(connStr.c_str());
    DB::autobackup = autoBkpChkBtn->isChecked();

    close();
}
