/*  Copyright 2021 Bruno Del Monde.
 * Licensed under the EUPL-1.2.
 */

#ifndef TEXTRESTORE_H
#define TEXTRESTORE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include "common/qutils.h"
#include "autosave/autosavetextedit.h"

class AutosaveRestore : public QWidget
{
    Q_OBJECT
public:
    explicit AutosaveRestore(QWidget *parent = nullptr);

    void load();

    std::list<string> files;
    std::list<string>::iterator it_files;
    int index = 1;

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *labelsLayout = new QHBoxLayout();
    QLabel* titleLbl = new QLabel(this);
    QLabel* indexLbl = new QLabel(this);
    QPlainTextEdit* textEdit = new QPlainTextEdit(this);
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QPushButton* previousButton = new QPushButton("Anterior", this);
    QPushButton* discartButton = new QPushButton("Descartar", this);
    QPushButton* nextButton = new QPushButton("Próximo", this);

signals:

public slots:
    void previous();
    void discart();
    void next();
};

#endif // TEXTRESTORE_H
