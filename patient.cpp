#include "patient.h"

#define DELETE_CONFIRMATION

Patient::Patient(QString qname, QWidget *parent) :
    QMainWindow(parent)
{
    name = QUtils::ToCString(qname);
    PGresult* res = PatientBDModel::DBExec("SELECT " + PatientBDModel::tableTabsLine + " FROM patient WHERE name = $1", name);
    if (res == nullptr)
    {
        invalid = true;
        return;
    }

    patientModel = new PatientBDModel(name, centralWidget);
    if (patientModel->invalid == true)
    {
        invalid = true;
        return;
    }

    appointmentWidget = new AppointmentWidget(name, comboBox, menuAppointment, patientModel->getBirthday(), centralWidget);
    if (appointmentWidget->invalid == true)
    {
        invalid = true;
        return;
    }

    tabWidget->addTab(tableView, "Identificação");
    for (int i = 0; i < TABS_NUM; ++i) {
        tabs[i] = new QPlainTextEdit(PQgetvalue(res, 0, i), tabWidget);
        tabWidget->addTab(tabs[i], PatientBDModel::tabNames->at(i));
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
    tableView->setModel(patientModel);
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
    connect(patientModel, SIGNAL(nameEdited(char*)), this, SLOT(nameChanged(char*)));
    connect(patientModel, SIGNAL(nameEdited(char*)), appointmentWidget, SLOT(nameChanged(char*))); //#Mover para um deles
    connect(patientModel, SIGNAL(birthdayEdited(QDate)), appointmentWidget, SLOT(birthdayChanged(QDate)));
    connect(patientModel, SIGNAL(notesCellEdited()), this, SLOT(resizeNoteCell()));
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
        res = PatientBDModel::DBExecCommand("BEGIN");

        if (res == nullptr)
        {
            event->ignore();
            return;
        }

        PQclear(res);

        if (!saveTabs() || !appointmentWidget->saveChanges())
        {
            PatientBDModel::rollBack();
            event->ignore();
            return;
        }

        res = PatientBDModel::DBExecCommand("COMMIT");

        if (res == nullptr)
        {
            PatientBDModel::rollBack();
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
            command += /*(string) */(j == 2 ? " " : ", ") + PatientBDModel::tableTabs[i] + " = $" + std::to_string(j);
            ++j;
            parameters.push_back(QUtils::ToCString(tabs[i]->toPlainText()));
        }

    if (j != 2)
    {
        command += " WHERE name = $1";
        PGresult* res = PatientBDModel::DBExec(command, parameters);

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
    QMessageBox::StandardButton b_ans = QMessageBox::warning(this, "Deletar Paciente",
"Tem certeza que você deseja deletar permanentemente esse paciente?\nTodos os dados serão perdidos",
QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

    if (b_ans == QMessageBox::Cancel)
        return;

#ifdef DELETE_CONFIRMATION
    if (comboBox->count() > 0)
    {
        b_ans = QMessageBox::warning(this, "Deletar Paciente",
"Você tem certeza MESMO que quer deletar esse paciente???\n\
Perceba que você não está apagando uma consulta só, está apagando todos os dados do paciente para sempre e inrecuperavelmente",
QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

        if (b_ans == QMessageBox::Cancel)
            return;
    }
#endif

    PGresult* res = PatientBDModel::DBExec("DELETE FROM patient WHERE name = $1", name);
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
