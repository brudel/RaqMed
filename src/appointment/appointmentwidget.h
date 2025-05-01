/*  Copyright 2021 Bruno Del Monde.
 * Licensed under the EUPL-1.2.
 */

#ifndef APPOINTMENTWIDGET_H
#define APPOINTMENTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QDateTimeEdit>
#include <QStackedLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QShortcut>
#include "qutils.h"
#include "autosavetextedit.h"
extern "C" {
#include <libpq-fe.h>
}


class AppointmentWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AppointmentWidget(char* name, QComboBox* _comboBox, QMenu* _menu, QDate _birthday, QWidget *parent = nullptr);

    //Functions
    bool saveChanges();
    void restoreDate();

    //Variables
    std::vector<char*> ident;
    bool invalid = false;
    QDateTime currentDateTime;
    std::vector<string> dateTimes;
    QDate birthday;

    //Layout
    QLabel* dateLabel = new QLabel("Agendamento:", this);
    QDateTimeEdit* dateTimeEdit = new QDateTimeEdit(this);
    QLabel* heightLabel = new QLabel("Altura:", this);
    QLineEdit* heightLineEdit = new QLineEdit(this);
    QLabel* weightLabel = new QLabel("Peso:", this);
    QLineEdit* weightLineEdit = new QLineEdit(this);
    QLabel* ageLabel = new QLabel(this);
    QPushButton* exitButton = new QPushButton("Salvar e fechar", this);
    QHBoxLayout* horizontalLayout = new QHBoxLayout();
    AutosaveTextEdit* contentEdit = new AutosaveTextEdit(this);
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    QComboBox* comboBox;
    QMenu* menu;
    QShortcut* exitShortcut = new QShortcut(QKeySequence("Esc"), this);

signals:
    void exited();
    void dateEdited(QDate oldQDate, QDate newQDate);
    void dataEdited();

public slots:
    //User action
    void setDate(int index); //Combobox
    bool exit(); //Button: "Salvar e sair"
    bool deleteAppointment();

    //Basic action
    bool loadDates();

    //Responsivity
    void nameChanged(char* newName);
    void birthdayChanged(QDate newBirthday);
};

#endif // APPOINTMENTWIDGET_H
