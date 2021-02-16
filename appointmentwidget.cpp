#include "appointmentwidget.h"
#include "db.h"
#include <QMenu>
#include <QMessageBox>
#include <cstring>

AppointmentWidget::AppointmentWidget(char* name, QComboBox* _comboBox, QMenu* _menu, QDate _birthday, QWidget *parent) :
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
    birthday = _birthday;

    menu->addAction("Apagar Consulta", this, SLOT(deleteAppointment()));
    menu->menuAction()->setVisible(false);

    horizontalLayout->addWidget(dateLabel);
    horizontalLayout->addWidget(dateTimeEdit);
    horizontalLayout->addWidget(heightLabel);
    horizontalLayout->addWidget(heightLineEdit);
    horizontalLayout->addWidget(weightLabel);
    horizontalLayout->addWidget(weightLineEdit);
    horizontalLayout->addWidget(ageLabel);
    horizontalLayout->addItem(horizontalSpacer);
    horizontalLayout->addWidget(exitButton);

    verticalLayout->addLayout(horizontalLayout);
    verticalLayout->addWidget(contentEdit, 1);

    connect(comboBox, SIGNAL(activated(int)), this, SLOT(setDate(int)));
    connect(exitButton, SIGNAL(clicked()), this, SLOT(exit()));
    QObject::connect(exitShortcut, SIGNAL(activated()), this, SLOT(exit()));
    connect(this, SIGNAL(dateEdited(QDate,QDate)), this, SLOT(loadDates()));
}

AppointmentWidget::~AppointmentWidget()
{
    for (auto cstr : dateTimes)
        free(cstr);
}

void AppointmentWidget::setDate(int index)
{
    char* old = ident.back();

    if (ident.size() == 2)
        if (!saveChanges())
            return;

    ident.push_back(dateTimes[index]);

    PGresult* res = DB::Exec("SELECT content, height, weight FROM appointment WHERE patient = $1 AND day = $2", ident);
    if (res == nullptr)
    {
        ident[1] = old;
        return;
    }

    contentEdit->setPlainText(PQgetvalue(res, 0, 0));
    heightLineEdit->setText(PQgetvalue(res, 0, 1));
    weightLineEdit->setText(PQgetvalue(res, 0, 2));
    PQclear(res);

    currentDateTime = QUtils::stringToQDateTime(dateTimes[index]);
    dateTimeEdit->setDateTime(currentDateTime);

    contentEdit->setTitle((string)ident[0] + " - Consulta de " + currentDateTime.toString("dd/MM/yyyy hh:mm.").toStdString());

    ageLabel->setText(("Idade: " + std::to_string(QUtils::yearsTo(birthday, currentDateTime.date())) + " anos").c_str());

    menu->menuAction()->setVisible(true);
}

bool AppointmentWidget::exit()
{
    if (!saveChanges())
        return false;

    menu->menuAction()->setVisible(false);
    comboBox->setCurrentIndex(0);
    exited();
    return true;
}

bool AppointmentWidget::saveChanges()
{
    int count = 3;
    string query = "UPDATE appointment SET";
    QDateTime newDateTime = dateTimeEdit->dateTime();
    bool dateChanged = false;

    if (contentEdit->document()->isModified())
    {
        query += " content = $" + std::to_string(count++);
        ident.push_back(QUtils::ToCString(contentEdit->toPlainText()));
        contentEdit->save();
    }

    if (newDateTime != currentDateTime)
    {
        query += (string)(count == 3 ? "" : ",") + " day = $" + std::to_string(count);
        ++count;
        ident.push_back(QUtils::ToCString(newDateTime.toString("yyyy-MM-dd hh:mm:00")));
        dateChanged = true;
    }

    if (heightLineEdit->isModified())
    {
        query += (string)(count == 3 ? "" : ",") + " height = $" + std::to_string(count);
        ++count;
        ident.push_back(QUtils::ToCString(heightLineEdit->text()));
    }

    if (weightLineEdit->isModified())
    {
        query += (string)(count == 3 ? "" : ",") + " weight = $" + std::to_string(count);
        ++count;
        ident.push_back(QUtils::ToCString(weightLineEdit->text()));
    }

    if (count != 3)
    {
        query += " WHERE patient = $1 AND day = $2";
        PGresult* res = DB::Exec(query, ident);

        while (--count > 2)
        {
            free(ident.back());
            ident.pop_back();
        }

        if (res == nullptr)
            return false;
        PQclear(res);

        if (dateChanged)
            dateEdited(currentDateTime.date(), newDateTime.date());
    }

    ident.pop_back();
    contentEdit->ended();
    return true;
}

void AppointmentWidget::restoreDate()
{
    dateTimes.push_back(QUtils::ToCString(currentDateTime.toString(Qt::ISODate)));
    ident.push_back(dateTimes.back());

    if (contentEdit->document()->isModified())
        contentEdit->save();
}

void AppointmentWidget::nameChanged(char* newName)
{
    ident[0] = newName;
    contentEdit->setTitle((string)ident[0] + " - Consulta de " + currentDateTime.toString("dd/MM/yyyy hh:mm.").toStdString());
}

bool AppointmentWidget::deleteAppointment()
{
    QMessageBox::StandardButton b_ans = QMessageBox::warning(this, "Apagar Consulta",
"Tem certeza que você deseja apagar essa consulta?\nTodos os dados serão perdidos",
QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

    if (b_ans == QMessageBox::Cancel)
        return false;

    PGresult* res = DB::Exec("DELETE FROM appointment WHERE patient = $1 AND day = $2", ident);
    if (res == nullptr)
        return false;
    PQclear(res);

    invalid = true;
    exit();
    invalid = false;

    dateEdited(currentDateTime.date(), currentDateTime.date());
    return true;
}

bool AppointmentWidget::loadDates()
{
    PGresult* res = DB::Exec("SELECT day FROM appointment WHERE patient = $1", ident.front());
    if (res == nullptr)
        return false;

    for (auto cstr : dateTimes)
        free(cstr);
    dateTimes.clear();

    int n = PQntuples(res);
    dateTimes.reserve(n);
    comboBox->clear();

    for (int i = 0; i < n; ++i)
        dateTimes.push_back(strdup(PQgetvalue(res, i, 0)));

    std::sort(dateTimes.begin(), dateTimes.end(), &QUtils::stringGreaterThan);

    for (int i = 0; i < n; ++i)
        comboBox->addItem(QUtils::toBrDate(dateTimes[i]).c_str());

    PQclear(res);
    return true;
}

void AppointmentWidget::birthdayChanged(QDate newBirthday)
{
    birthday = newBirthday;
}

