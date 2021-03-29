#include "calendar.h"
#include "ui_calendar.h"
#include "addpatientform.h"
#include "addappointmentform.h"
#include "patientlineedit.h"
#include "db.h"
#include <QListView>
#include "edittabmodel.h"

Calendar::Calendar(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Calendar)
{
    DB::setMainWindow(this);
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

    for (int i = 0; i < 4; ++i)
        ui->menuEditar_modelos->addAction(DB::tabNames->at(i), [=] {editModel(i);});

    dayChanged();
}

Calendar::~Calendar()
{
    delete ui;
}

void Calendar::dayChanged()
{
    QDate qdate = ui->calendarWidget->selectedDate();

    PGresult* res = DB::Exec("SELECT patient, to_char(day, 'HH24:MI') FROM appointment\
 WHERE date(day) = $1", qdate.toString("yyyy-MM-dd"));

    if (res == nullptr)
        return;

    ui->label->setText("Consultas de " + qdate.toString("dddd, dd/MM/yyyy"));

    int n = PQntuples(res);
    ui->tableWidget->setUpdatesEnabled(false);
    ui->tableWidget->setRowCount(0);

    for (int i = 0; i < n; ++i) {
        ui->tableWidget->insertRow(i);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(PQgetvalue(res, i, 0)));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(PQgetvalue(res, i, 1)));
    }
    PQclear(res);
    ui->tableWidget->setUpdatesEnabled(true);

    ui->tableWidget->sortItems(1);
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
    Patient* p;

    patient_it it = open_patients.find(qname);
    if (it != open_patients.end()) {
        it->second->activateWindow();
        return false;
    }

    try {
        p = new Patient(qname, this);
    } catch (...) {
        return false;
    }

    connect(p, SIGNAL(closed(char*)), this, SLOT(patient_closed(char*)));
    connect(p->appointmentWidget, SIGNAL(dateEdited(QDate, QDate)), this, SLOT(dateChanged(QDate, QDate)));
    connect(p, SIGNAL(patientEdited(char*)), this, SLOT(nameChanged(char*)));
    p->showMaximized();

    open_patients.insert({qname, p});
    return true;
}

void Calendar::editModel(int k)
{
    try {
        EditTabModel* e = new EditTabModel(k, this);
        e->show();
    } catch (...) {
    }
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

void Calendar::nameChanged(char* name)
{
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i)
        if (ui->tableWidget->item(i, 0)->text() == name)
        {
            dayChanged();
            return;
        }
}

#if defined(__unix__)
    #define FORMAT "hh:mm_dd-MM-yyyy"
#elif defined(_WIN32)
    #define FORMAT "hh-mm_dd-MM-yyyy"
#else
    #error Unknown environment!
#endif

void Calendar::on_actionBackup_triggered()
{
    string path = QFileDialog::getSaveFileName(this, "Selecione o destino do backup",
        QDir::homePath() + '//' + PROGRAM_PREFIX + "backup_" + QDateTime::currentDateTime().toString(FORMAT) + ".bkp", QString(),
        nullptr, QFileDialog::ShowDirsOnly | QFileDialog::HideNameFilterDetails).toStdString();

    if (path.empty())
        return;

    if (DB::backupDB(path))
        QMessageBox::information(this, "Backup Criado", ("O Backup foi criado com sucesso em: " + path + '.').c_str());
    else
        QMessageBox::critical(this, "Erro de Backup", "O backup não foi criado devido a um erro durante o procedimento.");
}
