#ifndef APPOINTMENTWIDGET_H
#define APPOINTMENTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QDateTimeEdit>
#include <QStackedLayout>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QLineEdit>
#include "qutils.h"
extern "C" {
#include <libpq-fe.h>
}


class AppointmentWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AppointmentWidget(char* name, QComboBox* _comboBox, QMenu* _menu, QDate _birthday, QWidget *parent = nullptr);
    ~AppointmentWidget();

    //Functions
    bool saveChanges();
    void restoreDate();

    //Variables
    std::vector<char*> ident;
    bool contentChanged = false;
    bool invalid = false;
    QDateTime currentDateTime;
    std::vector<char*> dateTimes;
    QDate birthday;

    //Layout
    QLabel* dateLabel = new QLabel("Agendamento:", this);
    QDateTimeEdit* dateTimeEdit = new QDateTimeEdit(this);
    QLabel* heightLabel = new QLabel("Altura:", this);
    QLineEdit* heightLineEdit = new QLineEdit(this);
    QLabel* weightLabel = new QLabel("Peso:", this);
    QLineEdit* weightLineEdit = new QLineEdit(this);
    QLabel* ageLabel = new QLabel(this);
    QSpacerItem *horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QPushButton* exitButton = new QPushButton("Salvar e fechar", this);
    QHBoxLayout* horizontalLayout = new QHBoxLayout();
    QPlainTextEdit* plainTextEdit = new QPlainTextEdit(this);
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    QComboBox* comboBox;
    QMenu* menu;

signals:
    void exited();
    void dateEdited(QDate oldQDate, QDate newQDate);

public slots:
    //User action
    void setDate(int index); //Combobox
    bool exit(); //Button: "Salvar e sair"
    bool deleteAppointment();

    //Basic action
    bool loadDates();

    //Intern action
    void contentChange(bool modified);

    //Responsivity
    void nameChanged(char* newName);
    void birthdayChanged(QDate newBirthday);
};

#endif // APPOINTMENTWIDGET_H
