#include "autosavetextedit.h"

int AutosaveTextEdit::docNumber = 0;

AutosaveTextEdit::AutosaveTextEdit(QWidget* parent, string title, QString text) :
    QPlainTextEdit(text, parent)
{
    path = BKPDIR + QDateTime::currentDateTime().toString(Qt::ISODate).toStdString() + "-"
        + std::to_string(docNumber++) + ".autosave";

    header = title + "\n";

    timer.setTimerType(Qt::VeryCoarseTimer);
    timer.setInterval(20E3);
    timer.setSingleShot(true);

    connect(this, SIGNAL(textChanged()), this, SLOT(setSave()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(save()));
}

void AutosaveTextEdit::setTitle(string title)
{
    header = title + "\n";
}

void AutosaveTextEdit::setSave()
{
    timer.start();
}

void AutosaveTextEdit::save()
{
    string text;
    FILE* file = fopen(path.c_str(), "w");

    fwrite(header.c_str(), sizeof(char), header.size(), file);

    text = toPlainText().toStdString();

    fwrite(text.c_str(), sizeof(char), text.size(), file);

    fclose(file);
}

void AutosaveTextEdit::ended()
{
    timer.stop();
    if (document()->isModified() || document()->isRedoAvailable())
        remove(path.c_str());
}
