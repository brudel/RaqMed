#ifndef APPOINTMENTWIDGET_H
#define APPOINTMENTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QDateTimeEdit>
#include <QStackedLayout>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QComboBox>
#include "qutils.h"
extern "C" {
#include <libpq-fe.h>
}


class AppointmentWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AppointmentWidget(char* name, QComboBox* _comboBox, QMenu* _menu, QWidget *parent = nullptr);

    //Functions
    void saveChanges();

    //Variables
    std::vector<char*> ident;
    bool contentChanged = false;
    bool dateTimeChanged = false;
    bool deleted = false;
    QDateTime oldDateTime;
    std::vector<char*> dateTimes;

    //Layout
    QLabel* label = new QLabel(this);
    QDateTimeEdit* dateTimeEdit = new QDateTimeEdit(this);
    QStackedLayout* stackedLayout = new QStackedLayout();
    QSpacerItem *horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    //QPushButton* editButton = new QPushButton("Editar data/hora", this);
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
    void exit(); //Salvar e sair
    void dateEdit();
    void deleteAppointment();

    //Basic action
    void loadDates();

    //Intern action
    void contentChange(bool original);

    //Extern action
    void nameChanged(char* newName);
};

#endif // APPOINTMENTWIDGET_H
