#include "patient.h"
#include "timebutton.h"

#define CANCEL_CONSTRUCTOR      \
free(name);                     \
delete horizontalLayout;        \
delete stackedLayout;           \

Patient::Patient(QString qname, QWidget *parent) :
    QMainWindow(parent)
{
    name = QUtils::ToCString(qname);
    PGresult* res = DB::Exec("SELECT " + DB::tableTabsLine + " FROM patient WHERE name = $1", name);
    if (res == nullptr)
    {
        CANCEL_CONSTRUCTOR;
        throw 0;
    }

    tabWidget->addTab(tableView, "Identificação");
    for (int i = 0; i < TABS_NUM; ++i)
    {
        tabs[i] = new AutosaveTextEdit(tabWidget, (string) name + " - " + DB::tabNames->at(i).toStdString(),
            PQgetvalue(res, 0, i));
        tabWidget->addTab(tabs[i], DB::tabNames->at(i));
        connect(this, SIGNAL(closed(char*)), tabs[i], SLOT(ended()));
    }
    PQclear(res);

    try {
        pModel = new PatientModel(name, centralWidget);
        appointmentWidget = new AppointmentWidget(name, comboBox, menuAppointment, pModel->getBirthday(), centralWidget);
        developmentChart = new DevelopmentCurveChart(name, pModel->getBirthday(), this);
    } catch (...) {
        CANCEL_CONSTRUCTOR;
        throw;
    }

    //Sucess construct
    tabWidget->addTab(developmentChart, "Gráficos de desenvolvimento");

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
    connect(appointmentWidget, SIGNAL(dateEdited(QDate, QDate)), developmentChart, SLOT(dateChanged()));
    connect(appointmentWidget, SIGNAL(dataEdited()), developmentChart, SLOT(dateChanged()));

    //Patient Model interface
    connect(pModel, SIGNAL(nameEdited(char*)), this, SLOT(nameChanged(char*)));
    connect(pModel, SIGNAL(nameEdited(char*)), appointmentWidget, SLOT(nameChanged(char*))); //#Mover para um deles
    connect(pModel, SIGNAL(birthdayEdited(QDate)), appointmentWidget, SLOT(birthdayChanged(QDate)));
    connect(pModel, SIGNAL(birthdayEdited(QDate)), developmentChart, SLOT(birthdayChanged(QDate)));
    connect(pModel, SIGNAL(notesCellEdited()), this, SLOT(resizeNoteCell()));
}

Patient::~Patient()
{
    if (isVisible() && exit())
        closed(name);

    free(name);
}


void Patient::closeEvent(QCloseEvent *event)
{
    if(exit())
    {
        event->accept();
        closed(name);
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
    std::vector<char*> parameters = {name};

    for (i = 0; i < TABS_NUM; ++i)
        if (tabs[i]->document()->isModified())
        {
            command += /*(string) */(j == 2 ? " " : ", ") + DB::tableTabs[i] + " = $" + std::to_string(j);
            ++j;
            parameters.push_back(QUtils::ToCString(tabs[i]->toPlainText()));
            tabs[i]->save();
        }

    if (j != 2)
    {
        command += " WHERE name = $1";
        PGresult* res = DB::Exec(command, parameters);

        --j;
        while (--j > 0)
            free(parameters[j]);

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
    messageBox.setText("Tem certeza que você deseja deletar permanentemente esse paciente?\nTodos os dados serão perdidos");
    messageBox.addButton(yesButton, QMessageBox::AcceptRole);
    messageBox.addButton("Cancelar", QMessageBox::RejectRole);
    messageBox.setDefaultButton(QMessageBox::Cancel);
    messageBox.exec();

    if (messageBox.clickedButton() != yesButton)
        return;

    PGresult* res = DB::Exec("DELETE FROM patient WHERE name = $1", name);
    if (res == nullptr)
        return;
    PQclear(res);

    patientEdited(name);

    invalid = true;
    close();
}

void Patient::appointmentOpened()
{
    stackedLayout->setCurrentIndex(1);
}

void Patient::appointmentClosed()
{
    stackedLayout->setCurrentIndex(0);
}

void Patient::nameChanged(char* newName)
{
    patientEdited(name);
    free(name);
    name = newName;
    nameLbl->setText(newName);

    for (int i = 0; i < TABS_NUM; ++i)
    {
        tabs[i]->setTitle((string) name + " - " + DB::tabNames->at(i).toStdString());
        if (tabs[i]->wasUsed())
            tabs[i]->save();
    }
}

void Patient::resizeNoteCell()
{
    tableView->resizeRowToContents(NOTES_INDEX);
}
