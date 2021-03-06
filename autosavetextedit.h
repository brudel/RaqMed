#ifndef AUTOSAVETEXTEDIT_H
#define AUTOSAVETEXTEDIT_H

#include <QPlainTextEdit>
#include <QTimer>
#include "qutils.h"

#define BKPDIR "autosaves/"

class AutosaveTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    AutosaveTextEdit(QWidget *parent =  nullptr, string title = "", QString text = "");

    //Functions
    void setTitle(string title);

    //Inline functions
    inline void abort() {timer.stop();};
    inline bool wasUsed() {return document()->isModified() || document()->isRedoAvailable();};
    inline void setPlainText(const QString &text) { QPlainTextEdit::setPlainText(text); abort();};

    string path;
    QTimer timer;
    string header;

    static int docNumber;

public slots:
    void setSave();
    void save();
    void ended();
};

#endif // AUTOSAVETEXTEDIT_H
