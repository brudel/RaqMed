#include "autosavetextedit.h"
#include <filesystem>

namespace fs = std::filesystem;

int AutosaveTextEdit::docNumber = 0;

AutosaveTextEdit::AutosaveTextEdit(QWidget* parent, string title, QString text) :
    QPlainTextEdit(text, parent)
{
    path = AUTOSAVES_DIR + QDateTime::currentDateTime().toString(Qt::ISODate).toStdString() + "-"
        + std::to_string(docNumber++) + ".autosave";
    #ifdef _WIN32
    std::replace(path.begin() + 2, path.end(), ':', '-');
    #endif

    header = title + "\n";

    timer.setTimerType(Qt::VeryCoarseTimer);
    timer.setInterval(20E3);
    timer.setSingleShot(true);

    connect(this, SIGNAL(textChanged()), this, SLOT(setSave()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(save()));
}

void AutosaveTextEdit::setSave()
{
    timer.start();
}

void AutosaveTextEdit::save()
{
    string text;
    FILE* file = fopen(path.c_str(), "w");

    if (!fileExists)
        fs::permissions(path.c_str(), fs::perms::owner_write | fs::perms::owner_read);

    fwrite(header.c_str(), sizeof(char), header.size(), file);

    text = toPlainText().toStdString();

    fwrite(text.c_str(), sizeof(char), text.size(), file);

    fclose(file);

    fileExists = true;
}

void AutosaveTextEdit::ended()
{
    timer.stop();
    if (fileExists)
    {
        remove(path.c_str());
        fileExists = false;
    }
}
