#ifndef ADDPATIENTFORM_H
#define ADDPATIENTFORM_H

#include <QDialog>
#include <QStringList>
#include <QtCore/QVariant>
#include <QApplication>
#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDateEdit>
#include "qutils.h"
#include "db.h"
extern "C" {
#include <libpq-fe.h>
}

class AddPatientForm : public QDialog
{
    Q_OBJECT

public:
    explicit AddPatientForm(QWidget *parent = nullptr);
    void closeEvent(QCloseEvent *event) override;
    bool noModified();

    bool saved = false;
    QDate defaultDate;

    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();
    QLabel* labels[FIELDS_NUM];
    QLineEdit* lineEdits[FIELDS_NUM - 2];
    QDateEdit* birthDateEdit = new QDateEdit(this);
    QPlainTextEdit *plainTextEdit = new QPlainTextEdit(this);
    QPushButton *pushButton = new QPushButton("Salvar", this);
    QHBoxLayout *horizontalButtonLayout = new QHBoxLayout();

public slots:
    void save();
};

#endif // ADDPATIENTFORM_H
