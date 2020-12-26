#include "calendar.h"
#include "ui_calendar.h"
#include "addpatientform.h"
#include "addappointmentform.h"
#include "patientlineedit.h"
#include "patientbdmodel.h"
#include <QListView>
#include "edittabmodel.h"

Calendar::Calendar(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Calendar)
{
    ui->setupUi(this);

    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList({"Nome", "Horário"}));
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->hide();
    PatientLineEdit* patientQuery = new PatientLineEdit(ui->lineEdit);

    QLabel* label = new QLabel(ui->centralwidget);
    label->setPixmap(QPixmap(":capa.png"));
    ui->verticalLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    ui->verticalLayout->addWidget(label);
    label->hide();

    connect(ui->calendarWidget, SIGNAL(selectionChanged()), this, SLOT(dayChanged()));
    connect(patientQuery, SIGNAL(completationDone(QString)), this, SLOT(doneQuery(QString)));
    ui->menuEditar_modelos->addAction(PatientBDModel::tabNames->at(0), this, SLOT(reasons()));
    ui->menuEditar_modelos->addAction(PatientBDModel::tabNames->at(1), this, SLOT(antecedents()));
    ui->menuEditar_modelos->addAction(PatientBDModel::tabNames->at(2), this, SLOT(exams()));
    ui->menuEditar_modelos->addAction(PatientBDModel::tabNames->at(3), this, SLOT(reports()));

    dayChanged();
}

Calendar::~Calendar()
{
    delete ui;
    PQfinish(PatientBDModel::conn);
}

void Calendar::dayChanged()
{
    QDate qdate = ui->calendarWidget->selectedDate();
    ui->label->setText("Consultas de " + qdate.toString("dddd, dd/MM/yyyy"));

    PGresult* ans = PatientBDModel::BDExec("SELECT patient, to_char(day, 'HH24:MI') FROM appointment\
 WHERE date(day) = $1 ORDER BY day", qdate.toString("yyyy-MM-dd"));

    int n = PQntuples(ans);
    ui->tableWidget->setRowCount(0);

    for (int i = 0; i < n; ++i) {
        ui->tableWidget->insertRow(i);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(PQgetvalue(ans, i, 0)));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(PQgetvalue(ans, i, 1)));
    }
}

void Calendar::on_tableWidget_cellClicked(int row)
{
    QString qname = ui->tableWidget->item(row, 0)->text();
    openPatient(qname);
}

void Calendar::patient_closed(char* name)
{
    open_patients.erase(name);
}

void Calendar::on_actionAdicionar_Paciente_triggered()
{
    AddPatientForm* form = new AddPatientForm(this);
    form->show();
}

void Calendar::on_actionAdicionar_Consulta_triggered()
{
    AddAppointmentForm* form = new AddAppointmentForm(ui->calendarWidget->selectedDate(), this);
    connect(form, SIGNAL(dateEdited(QDate,QString)), this, SLOT(dateChanged(QDate, QString)));
    form->show();
}

void Calendar::doneQuery(QString qname) {
    ui->lineEdit->clear();
    openPatient(qname);
}

bool Calendar::openPatient(QString qname)
{
    patient_it it = open_patients.find(qname);
    if (it != open_patients.end()) {
        it->second->activateWindow();
        return false;
    }

    Patient* p = new Patient(qname, this);
    connect(p, SIGNAL(closed(char*)), this, SLOT(patient_closed(char*)));
    connect(p->appointmentWidget, SIGNAL(dateEdited(QDate, QDate)), this, SLOT(dateChanged(QDate, QDate)));
    connect(p, SIGNAL(patientEdited(char*)), this, SLOT(nameChanged()));
    p->showMaximized();

    open_patients.insert({qname, p});
    return true;
}

void Calendar::editModel(int k)
{
    EditTabModel* e = new EditTabModel(k, this);
    e->show();
}

void Calendar::dateChanged(QDate newQDate, QString name)
{
    dateChanged(newQDate);

    patient_it it = open_patients.find(name);
    if (it == open_patients.end())
        return;

    it->second->appointmentChanged();
}

void Calendar::dateChanged(QDate oldQDate, QDate newQDate)
{
    QDate current = ui->calendarWidget->selectedDate();
    if (current == oldQDate || current == newQDate)
        dayChanged();
}

void Calendar::dateChanged(QDate qDate)
{
    QDate current = ui->calendarWidget->selectedDate();
    if (current == qDate)
        dayChanged();
}

void Calendar::nameChanged()
{
    dayChanged();
    //#Detalhezinho comparar consultas com data atual antes de emitir
}
