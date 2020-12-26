#include "patient.h"

#define DELETE_CONFIRMATION

Patient::Patient(QString qname, QWidget *parent) :
    QMainWindow(parent)
{
    name = QUtils::ToCString(qname);
    appointmentWidget = new AppointmentWidget(name, comboBox, menuAppointment, centralWidget);

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
    patientModel = new PatientBDModel(name, centralWidget);
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
    connect(patientModel, SIGNAL(notesCellEdited()), this, SLOT(resizeNoteCell()));

    PGresult* ans = PatientBDModel::BDExec("SELECT reasons, antecedents, exams, reports FROM patient WHERE name = $1", name);

    tabWidget->addTab(tableView, "Identificação");
    for (int i = 0; i < 4; ++i){
        tabs[i] = new QPlainTextEdit(PQgetvalue(ans, 0, i), tabWidget);
        tabWidget->addTab(tabs[i], PatientBDModel::tabNames->at(i));
    }
}

Patient::~Patient()
{
    if (!deleted) {
        std::vector<char*> tabTexts;
        for (int i = 0; i < 4; ++i)
            tabTexts.push_back(QUtils::ToCString(tabs[i]->toPlainText()));
        tabTexts.push_back(name);

        PatientBDModel::BDExec("UPDATE patient SET (reasons, antecedents, exams, reports) = ($1, $2, $3, $4)\
 WHERE name = $5", tabTexts);

        if (stackedLayout->currentIndex() == 1)
            appointmentWidget->saveChanges();
    }

    closed(name);
}

void Patient::deletePatient()
{
    QMessageBox::StandardButton b_ans = QMessageBox::warning(this, "Deletar Paciente",
"Tem certeza que você deseja deletar permanentemente esse paciente?\nTodos os dados serão perdidos",
QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

    if (b_ans == QMessageBox::Cancel)
        return;

#ifdef DELETE_CONFIRMATION
    if (comboBox->count() > 0){
        b_ans = QMessageBox::warning(this, "Deletar Paciente",
"Você tem certeza MESMO que quer deletar esse paciente???\n\
Perceba que você não está apagando uma consulta só, está apagando todos os dados do paciente para sempre e inrecuperavelmente",
QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

        if (b_ans == QMessageBox::Cancel)
            return;
    }
#endif

    PatientBDModel::BDExec("DELETE FROM patient WHERE name = $1", name);
    patientEdited(name);

    deleted = true;
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
    name = newName;
    nameLbl->setText(newName);
}

void Patient::resizeNoteCell()
{
    tableView->resizeRowToContents(NOTES_INDEX);
}
