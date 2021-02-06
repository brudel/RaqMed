#include "patient.h"
#include "timebutton.h"

Patient::Patient(QString qname, QWidget *parent) :
    QMainWindow(parent)
{
    name = QUtils::ToCString(qname);
    PGresult* res = DB::Exec("SELECT " + DB::tableTabsLine + " FROM patient WHERE name = $1", name);
    if (res == nullptr)
    {
        invalid = true;
        return;
    }

    pModel = new PatientModel(name, centralWidget);
    if (pModel->invalid == true)
    {
        invalid = true;
        return;
    }

    appointmentWidget = new AppointmentWidget(name, comboBox, menuAppointment, pModel->getBirthday(), centralWidget);
    if (appointmentWidget->invalid == true)
    {
        invalid = true;
        return;
    }

    tabWidget->addTab(tableView, "Identificação");
    for (int i = 0; i < TABS_NUM; ++i) {
        tabs[i] = new QPlainTextEdit(PQgetvalue(res, 0, i), tabWidget);
        tabWidget->addTab(tabs[i], DB::tabNames->at(i));
    }
    PQclear(res);

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
    horizontalLayout->addItem(horizontalSpacer);
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
    connect(pModel, SIGNAL(nameEdited(char*)), appointmentWidget, SLOT(nameChanged(char*))); //#Mover para um deles
    connect(pModel, SIGNAL(birthdayEdited(QDate)), appointmentWidget, SLOT(birthdayChanged(QDate)));
    connect(pModel, SIGNAL(notesCellEdited()), this, SLOT(resizeNoteCell()));
}

Patient::~Patient()
{
    free(name);
}


void Patient::closeEvent(QCloseEvent *event)
{
    PGresult* res;

    if (invalid)
    {
        closed(name);
        event->accept();
    }

    if (stackedLayout->currentIndex() == 1)
    {
        res = DB::ExecCommand("BEGIN");

        if (res == nullptr)
        {
            event->ignore();
            return;
        }

        PQclear(res);

        if (!saveTabs() || !appointmentWidget->saveChanges())
        {
            DB::rollBack();
            event->ignore();
            return;
        }

        res = DB::ExecCommand("COMMIT");

        if (res == nullptr)
        {
            DB::rollBack();
            appointmentWidget->restoreDate();
            event->ignore();
            return;
        }
        PQclear(res);
    }

    else if (!saveTabs())
    {
        event->ignore();
        return;
    }

    closed(name);
    event->accept();
}

bool Patient::saveTabs()
{
    int i, j = 2;
    string command = "UPDATE patient SET";
    std::vector<char*> parameters = {name};

    for (i = 0; i < 4; ++i)
        if (tabs[i]->document()->isModified())
        {
            command += /*(string) */(j == 2 ? " " : ", ") + DB::tableTabs[i] + " = $" + std::to_string(j);
            ++j;
            parameters.push_back(QUtils::ToCString(tabs[i]->toPlainText()));
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
}

void Patient::resizeNoteCell()
{
    tableView->resizeRowToContents(NOTES_INDEX);
}
