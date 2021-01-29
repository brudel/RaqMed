#include "appointmentwidget.h"
#include "patientbdmodel.h"
#include <QMenu>
#include <QMessageBox>

AppointmentWidget::AppointmentWidget(char* name, QComboBox* _comboBox, QMenu* _menu, QWidget *parent) :
QWidget(parent)
{
    ident.push_back(name);
    comboBox = _comboBox;

    if (!loadDates())
    {
        invalid = true;
        return;
    }
    menu = _menu;

    menu->addAction("Remarcar Data/Hora", this, SLOT(dateEdit()));
    menu->addAction("Apagar Consulta", this, SLOT(deleteAppointment()));
    menu->menuAction()->setVisible(false);

    stackedLayout->addWidget(label);
    stackedLayout->addWidget(dateTimeEdit);
    horizontalLayout->addLayout(stackedLayout);
    horizontalLayout->addItem(horizontalSpacer);
    horizontalLayout->addWidget(exitButton);

    verticalLayout->addLayout(horizontalLayout);
    verticalLayout->addWidget(plainTextEdit, 1);

    connect(comboBox, SIGNAL(activated(int)), this, SLOT(setDate(int)));
    connect(exitButton, SIGNAL(clicked()), this, SLOT(exit()));
    connect(this, SIGNAL(dateEdited(QDate,QDate)), this, SLOT(loadDates()));
    connect(plainTextEdit, SIGNAL(undoAvailable(bool)), this, SLOT(contentChange(bool)));
}

void AppointmentWidget::setDate(int index)
{
    char* old = ident.back();

    if (ident.size() == 2)
        if (!saveChanges())
            return;

    ident.push_back(dateTimes[index]);

    PGresult* res = PatientBDModel::DBExec("SELECT content FROM appointment WHERE patient = $1 AND day = $2", ident);
    if (res == nullptr)
    {
        ident[1] = old;
        return;
    }

    plainTextEdit->setPlainText(PQgetvalue(res, 0, 0));

    label->setText(comboBox->itemText(index));
    menu->menuAction()->setVisible(true);
}

bool AppointmentWidget::exit()
{
    if (!saveChanges())
        return;

    menu->menuAction()->setVisible(false);
    comboBox->setCurrentIndex(0);
    exited();
    return true;
}

void AppointmentWidget::dateEdit()
{
    oldDateTime.setDate(QDate(atoi(ident[1]), atoi(ident[1] + 5), atoi(ident[1] + 8)));
    oldDateTime.setTime(QTime(atoi(ident[1] + 11), atoi(ident[1] + 14)));
    dateTimeEdit->setDateTime(oldDateTime);

    stackedLayout->setCurrentIndex(1);
    dateTimeEdit->setFocus();
    dateTimeChanged = true;
}


bool AppointmentWidget::saveChanges()
{
    if (contentChanged && !invalid) {
        ident.push_back(QUtils::ToCString(plainTextEdit->toPlainText()));
        PGresult* res = PatientBDModel::DBExec("UPDATE appointment SET content = $3 WHERE patient = $1 AND day = $2", ident);
        ident.pop_back();

        if (res == nullptr)
            return false;

        contentChanged = false;
    }

    //#UnIFicar

    if (dateTimeChanged  && !invalid) {
        QDateTime newDateTime = dateTimeEdit->dateTime();
        if (oldDateTime != newDateTime) {
            char* newDateTimeStr = QUtils::ToCString(newDateTime.toString("yyyy-MM-dd hh:mm:00"));

            ident.push_back(newDateTimeStr);
            PGresult* res = PatientBDModel::DBExec("UPDATE appointment SET day = $3 WHERE patient = $1 AND day = $2", ident);
            ident.pop_back();

            if (res == nullptr)
                return false;

            dateEdited(oldDateTime.date(), newDateTime.date()); //Signal
        }

        dateTimeChanged = false;
        stackedLayout->setCurrentIndex(0);
    }

    ident.pop_back();
    return true;
}

void AppointmentWidget::contentChange(bool mudado)
{
    contentChanged = mudado;
}


void AppointmentWidget::nameChanged(char* newName)
{
    ident[0] = newName;
}

bool AppointmentWidget::deleteAppointment()
{
    QMessageBox::StandardButton b_ans = QMessageBox::warning(this, "Apagar Consulta",
"Tem certeza que você deseja apagar essa consulta?\nTodos os dados serão perdidos",
QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

    if (b_ans == QMessageBox::Cancel)
        return false;

    PGresult* res = PatientBDModel::DBExec("DELETE FROM appointment WHERE patient = $1 AND day = $2", ident);
    if (res == nullptr)
        return;

    invalid = true;
    exit();
    invalid = false;

    oldDateTime.setDate(QDate(atoi(ident[1]), atoi(ident[1] + 5), atoi(ident[1] + 8)));
    dateEdited(oldDateTime.date(), oldDateTime.date());
    return true;
}

bool AppointmentWidget::loadDates()
{
    PGresult* res = PatientBDModel::DBExec("SELECT day FROM appointment WHERE patient = $1 ORDER BY day DESC", ident.front());
    if (res == nullptr)
        return false;

    int n = PQntuples(res);
    dateTimes.clear();
    dateTimes.reserve(n);
    comboBox->clear();

    for (int i = 0; i < n; ++i) {
        dateTimes.push_back(PQgetvalue(res, i, 0));
        comboBox->addItem(QUtils::toBrDate(dateTimes.back()).c_str());
    }
    return true;
}

