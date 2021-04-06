#ifndef ADDAPPOINTMENTFORM_H
#define ADDAPPOINTMENTFORM_H

#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QPushButton>
#include <qutils.h>
#include <patientlineedit.h>
#include <autosavetextedit.h>
extern "C" {
#include <libpq-fe.h>
}

class AddAppointmentForm : public QWidget
{
    Q_OBJECT
public:
    explicit AddAppointmentForm(QDate date, QWidget *parent = nullptr);
    void closeEvent(QCloseEvent *event) override;

    //Variables
    QDate original;
    bool saved = false;
    bool saveFailed = false;

    //Layout
    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();
    QLabel* dateLabel = new QLabel("Data:", this);
    QBDateTimeEdit* dateTimeEdit = new QBDateTimeEdit(this);
    QLabel* patientLabel = new QLabel("Paciente:", this);
    QLineEdit* lineEdit = new QLineEdit(this);
    AutosaveTextEdit* contentEdit;
    QPushButton *pushButton = new QPushButton("Salvar", this);
    QHBoxLayout *horizontalButtonLayout = new QHBoxLayout();

public slots:
    void save();

signals:
    void dateEdited(QDate newDate, QString name);
};

#endif // ADDAPPOINTMENTFORM_H
