#include "appointmentwidget.h"
#include "patientbdmodel.h"
#include <QMenu>
#include <QMessageBox>

AppointmentWidget::AppointmentWidget(char* name, QComboBox* _comboBox, QMenu* _menu, QWidget *parent) :
QWidget(parent)
{
    ident.push_back(name);
    comboBox = _comboBox;
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

    loadDates();
}

void AppointmentWidget::setDate(int index)
{
    if (ident.size() == 2)
        saveChanges();

    ident.push_back(dateTimes[index]);

    label->setText(comboBox->itemText(index));

    PGresult* ans = PatientBDModel::BDExec("SELECT content FROM appointment WHERE patient = $1 AND day = $2", ident);

    plainTextEdit->setPlainText(PQgetvalue(ans, 0, 0));
    menu->menuAction()->setVisible(true);
}

void AppointmentWidget::exit()
{
    saveChanges();
    menu->menuAction()->setVisible(false);
    comboBox->setCurrentIndex(0);
    exited();
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


void AppointmentWidget::saveChanges()
{
    if (contentChanged && !deleted) {
        ident.push_back(QUtils::ToCString(plainTextEdit->toPlainText()));
        QUtils::BDdebug(PatientBDModel::BDExec("UPDATE appointment SET content = $3 WHERE patient = $1 AND day = $2", ident));
        ident.pop_back();

        contentChanged = false;
    }

    //#UnIFicar

    if (dateTimeChanged  && !deleted) {
        QDateTime newDateTime = dateTimeEdit->dateTime();
        if (oldDateTime != newDateTime) {
            char* newDateTimeStr = QUtils::ToCString(newDateTime.toString("yyyy-MM-dd hh:mm:00"));

            ident.push_back(newDateTimeStr);
            PatientBDModel::BDExec("UPDATE appointment SET day = $3 WHERE patient = $1 AND day = $2", ident);
            ident.pop_back();

            dateEdited(oldDateTime.date(), newDateTime.date()); //Signal
        }

        dateTimeChanged = false;
        stackedLayout->setCurrentIndex(0);
    }


    ident.pop_back();
}

void AppointmentWidget::contentChange(bool mudado)
{
    contentChanged = mudado;
}


void AppointmentWidget::nameChanged(char* newName)
{
    ident[0] = newName;
}

void AppointmentWidget::deleteAppointment()
{
    QMessageBox::StandardButton b_ans = QMessageBox::warning(this, "Apagar Consulta",
"Tem certeza que você deseja apagar essa consulta?\nTodos os dados serão perdidos",
QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

    if (b_ans == QMessageBox::Cancel)
        return;

    PatientBDModel::BDExec("DELETE FROM appointment WHERE patient = $1 AND day = $2", ident);

    deleted = true;
    exit();
    deleted = false;

    oldDateTime.setDate(QDate(atoi(ident[1]), atoi(ident[1] + 5), atoi(ident[1] + 8)));
    dateEdited(oldDateTime.date(), oldDateTime.date());
}

void AppointmentWidget::loadDates()
{
    PGresult* ans = PatientBDModel::BDExec("SELECT day FROM appointment WHERE patient = $1 ORDER BY day DESC", ident.front());

    int n = PQntuples(ans);
    dateTimes.clear();
    dateTimes.reserve(n);
    comboBox->clear();

    for (int i = 0; i < n; ++i) {
        dateTimes.push_back(PQgetvalue(ans, i, 0));
        comboBox->addItem(QUtils::toBrDate(dateTimes.back()).c_str());
    }
}

