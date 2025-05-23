/*  Copyright 2021 Bruno Del Monde.
 * Licensed under the EUPL-1.2.
 */

#include "patient/patient.h"
#include "widgets/timebutton.h"

#define CHART_LABEL "Gráficos de desenvolvimento"
#define CHART_INDEX 5

#define CANCEL_CONSTRUCTOR      \
delete horizontalLayout;        \
delete stackedLayout;           \

Patient::Patient(QString qname, QWidget* parent) :
    QMainWindow(parent), name(qname.toStdString())
{
    PGresult* res = DB::Exec("SELECT " + DB::tableTabsLine + " FROM patient WHERE name = $1", name.c_str());
    if (res == nullptr)
    {
        CANCEL_CONSTRUCTOR;
        throw 0;
    }

    tabWidget->addTab(tableView, "Identificação");
    for (int i = 0; i < TABS_NUM; ++i)
    {
        tabs[i] = new AutosaveTextEdit(tabWidget, name + " - " + DB::tabNames->at(i).toStdString(),
            PQgetvalue(res, 0, i));
        tabWidget->addTab(tabs[i], DB::tabNames->at(i));
        connect(this, SIGNAL(closed(char*)), tabs[i], SLOT(ended()));
    }
    PQclear(res);

    try {
        pModel = new PatientModel(name.c_str(), centralWidget);
        appointmentWidget = new AppointmentWidget(name.c_str(), comboBox, menuAppointment, pModel->getBirthday(), centralWidget);
    } catch (...) {
        CANCEL_CONSTRUCTOR;
        throw;
    }

    //Sucess construct
    tabWidget->addTab(new QWidget(), CHART_LABEL);

    setWindowTitle("Ficha de " + qname);
    setCentralWidget(centralWidget);
    setMenuBar(menuBar);
    setStatusBar(statusBar);
    setAttribute(Qt::WA_DeleteOnClose);

    menuOp_es->addAction("Excluir paciente", this, SLOT(deletePatient()));
    menuBar->addAction(menuOp_es->menuAction());
    menuBar->addAction(menuAppointment->menuAction());

    nameLbl->setText(qname);
    horizontalLayout->addWidget(nameLbl);
    horizontalLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    horizontalLayout->addWidget(comboBox);

    //Table View
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->horizontalHeader()->hide();
    tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    tableView->setModel(pModel);
    tableView->setItemDelegate(new PatientDelegate(tableView));
    tableView->verticalHeader()->setStretchLastSection(true);
    resizeNoteCell();

    verticalLayout->addLayout(horizontalLayout);
    tabWidget->setDocumentMode(true);
    stackedLayout->addWidget(tabWidget);
    stackedLayout->addWidget(appointmentWidget);
    verticalLayout->addLayout(stackedLayout);

    //Appointment interface
    connect(comboBox, SIGNAL(activated(int)), this, SLOT(appointmentOpened()));
    connect(appointmentWidget, SIGNAL(exited()), this, SLOT(appointmentClosed()));
    connect(this, SIGNAL(appointmentChanged()), appointmentWidget, SLOT(loadDates()));

    //Patient Model interface
    connect(pModel, SIGNAL(nameEdited(char*)), this, SLOT(nameChanged(char*)));
    connect(pModel, SIGNAL(nameEdited(char*)), appointmentWidget, SLOT(nameChanged(char*)));
    connect(pModel, SIGNAL(birthdayEdited(QDate)), appointmentWidget, SLOT(birthdayChanged(QDate)));
    connect(pModel, SIGNAL(notesCellEdited()), this, SLOT(resizeNoteCell()));

    //Others
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(constructChart(int)));
}

Patient::~Patient()
{
    if (isVisible() && exit())
        closed(name.c_str());
}


void Patient::closeEvent(QCloseEvent *event)
{
    if(exit())
    {
        event->accept();
        closed(name.c_str());
    }
    else
        event->ignore();
}

bool Patient::exit()
{
    PGresult* res;

    if (invalid)
        return true;

    if (stackedLayout->currentIndex() == 1)
    {
        res = DB::ExecCommand("BEGIN");

        if (res == nullptr)
            return false;

        PQclear(res);

        if (!saveTabs() || !appointmentWidget->saveChanges())
        {
            DB::rollBack();
            return false;
        }

        res = DB::ExecCommand("COMMIT");

        if (res == nullptr)
        {
            DB::rollBack();
            appointmentWidget->restoreDate();
            return false;
        }
        PQclear(res);
    }

    else if (!saveTabs())
        return false;

    return true;
}

bool Patient::saveTabs()
{
    int i, j = 2;
    string command = "UPDATE patient SET";
    std::vector<string> parameters = {name};

    for (i = 0; i < TABS_NUM; ++i)
        if (tabs[i]->document()->isModified())
        {
            command += /*(string) */(j == 2 ? " " : ", ") + DB::tableTabs[i] + " = $" + std::to_string(j);
            ++j;
            parameters.push_back(tabs[i]->toPlainText().toStdString());
            tabs[i]->save();
        }

    if (j != 2)
    {
        command += " WHERE name = $1";
        PGresult* res = DB::Exec(command, parameters);

        if (res == nullptr)
            return false;
        PQclear(res);
    }

    return true;
}

void Patient::deletePatient()
{
    TimeButton* yesButton = new TimeButton("Sim");

    QMessageBox messageBox(this);
    messageBox.setWindowTitle("Deletar Paciente");
    messageBox.setIcon(QMessageBox::Warning);
    messageBox.setText("Tem certeza que você deseja deletar permanentemente esse paciente?\nTodos os dados serão perdidos.");
    messageBox.addButton(yesButton, QMessageBox::AcceptRole);
    messageBox.addButton("Cancelar", QMessageBox::RejectRole);
    messageBox.setDefaultButton(QMessageBox::Cancel);
    messageBox.exec();

    if (messageBox.clickedButton() != yesButton)
        return;

    PGresult* res = DB::Exec("DELETE FROM patient WHERE name = $1", name.c_str());
    if (res == nullptr)
        return;
    PQclear(res);

    patientEdited(name.c_str());

    invalid = true;
    close();
}

void Patient::appointmentOpened()
{
    stackedLayout->setCurrentIndex(1);
}

void Patient::constructChart(int tab)
{
    if (tab != CHART_INDEX)
        return;

    try {
        developmentChart = new DevelopmentCurveChart(name.c_str(), pModel->getBirthday(), centralWidget);
    } catch (...) {
        return;
    }

    disconnect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(constructChart(int)));

    tabWidget->setUpdatesEnabled(false);
    tabWidget->removeTab(CHART_INDEX);
    tabWidget->addTab(developmentChart, CHART_LABEL);
    tabWidget->setCurrentIndex(CHART_INDEX);
    tabWidget->setUpdatesEnabled(true);

    connect(appointmentWidget, SIGNAL(dateEdited(QDate, QDate)), developmentChart, SLOT(resetPatient()));
    connect(appointmentWidget, SIGNAL(dataEdited()), developmentChart, SLOT(resetPatient()));
    connect(pModel, SIGNAL(birthdayEdited(QDate)), developmentChart, SLOT(birthdayChanged(QDate)));
}

void Patient::appointmentClosed()
{
    stackedLayout->setCurrentIndex(0);
}

void Patient::nameChanged(char* newName)
{
    patientEdited(name.c_str());
    name = newName;
    nameLbl->setText(newName);

    for (int i = 0; i < TABS_NUM; ++i)
    {
        tabs[i]->setTitle(name + " - " + DB::tabNames->at(i).toStdString());
        if (tabs[i]->wasUsed())
            tabs[i]->save();
    }
}

void Patient::resizeNoteCell()
{
    tableView->resizeRowToContents(NOTES_INDEX);
}
