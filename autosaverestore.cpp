#include "autosaverestore.h"
#include <filesystem>
#include <QMessageBox>

namespace fs = std::filesystem;

AutosaveRestore::AutosaveRestore(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Recuperação de textos");
    resize(750, 500); //#Find elegant answer

    labelsLayout->addWidget(titleLbl);
    labelsLayout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding));
    labelsLayout->addWidget(indexLbl);
    mainLayout->addLayout(labelsLayout);

    textEdit->setReadOnly(true);
    mainLayout->addWidget(textEdit);

    buttonsLayout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding));
    buttonsLayout->addWidget(previousButton);
    buttonsLayout->addWidget(discartButton);
    buttonsLayout->addWidget(nextButton);
    buttonsLayout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding));
    mainLayout->addLayout(buttonsLayout);

    for (auto entry : fs::directory_iterator(BKPDIR))
        files.push_back(entry.path());

    if (files.size() == 0)
        return;

    QMessageBox::information(this, "Recuperação de dados",
        "Foram detectados textos que não salvos corretamente em uma seção anterior.\n"
        "Será aberto um diálogo onde você pode ver os textos e copia-los para onde desejar.\n"
        "Após ter certeza que salvou tudo que deseja, você pode descartar os textos da ferramenta de recuperação.");

    it_files = files.begin();

    connect(previousButton, SIGNAL(clicked(bool)), this, SLOT(previous()));
    connect(discartButton, SIGNAL(clicked(bool)), this, SLOT(discart()));
    connect(nextButton, SIGNAL(clicked(bool)), this, SLOT(next()));

    load();
    show();
}

void AutosaveRestore::load()
{
    size_t contentStart;
    int contentSize;
    char* content;

    previousButton->setEnabled(index == 1 ? false : true);
    nextButton->setEnabled(index == files.size() ? false : true);

    indexLbl->setText((std::to_string(index) + "/" + std::to_string(files.size())).c_str());

    FILE* currentFile = fopen(it_files->c_str(), "r");

    titleLbl->setText(QUtils::readFileLine(currentFile));

    contentStart = ftell(currentFile);
    fseek(currentFile, 0, SEEK_END);
    contentSize = ftell(currentFile) - contentStart;
    fseek(currentFile, contentStart, SEEK_SET);
    content = malloc(contentSize + sizeof(char));
    content[contentSize] = '\0';
    fread(content, 1, contentSize, currentFile);
    textEdit->setPlainText(content);

    free(content);
    fclose(currentFile);
}

void AutosaveRestore::previous()
{
    --index;
    --it_files;
    load();
}

void AutosaveRestore::discart()
{
    remove(it_files->c_str());
    if (files.size() == 1)
    {
        close();
        return;
    }

    it_files = files.erase(it_files);
    if (it_files == files.end())
    {
        --it_files;
        --index;
    }

    load();
}

void AutosaveRestore::next()
{
    ++index;
    ++it_files;
    load();
}
