#include "addappointmentform.h"
#include "patientbdmodel.h"

AddAppointmentForm::AddAppointmentForm(QDate date, QWidget *parent) :
    QDialog(parent)
{
    original = date;

    setWindowTitle("Agendar Consulta");
    resize(600, 400); //#Find elegant answer

    formLayout->setWidget(0, QFormLayout::LabelRole, dateLabel);

    dateTimeEdit->setDate(date);
    dateTimeEdit->setCurrentSection(QDateTimeEdit::HourSection);
    formLayout->setWidget(0, QFormLayout::FieldRole, dateTimeEdit);

    formLayout->setWidget(1, QFormLayout::LabelRole, patientLabel);
    formLayout->setWidget(1, QFormLayout::FieldRole, lineEdit);
    new PatientLineEdit(lineEdit);

    verticalLayout->addLayout(formLayout);
    verticalLayout->addWidget(plainTextEdit);

    horizontalButtonLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    horizontalButtonLayout->addWidget(pushButton);
    horizontalButtonLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    verticalLayout->addLayout(horizontalButtonLayout);

    connect(pushButton, &QPushButton::clicked, this, &AddAppointmentForm::save);
}

void AddAppointmentForm::save() {
    std::vector<char*> values;
    QString name = lineEdit->text();

    values.push_back(QUtils::ToCString(name));
    values.push_back(QUtils::ToCString(dateTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm")));
    values.push_back(QUtils::ToCString(plainTextEdit->toPlainText()));

    PGresult* res = PatientBDModel::BDExec("INSERT INTO appointment (patient, day, content) VALUES ($1, $2, $3)", values);

    if (res == nullptr)
        return;

    dateEdited(dateTimeEdit->date(), name);
    saved = true;
    this->close();
}

void AddAppointmentForm::closeEvent(QCloseEvent *event)
 {
    if (saved) {
        event->accept();
        return;
    }

    QMessageBox::StandardButton b_ans = QMessageBox::warning(this, "Cancelar Agendamento",
"Tem certeza que deseja fechar essa janela?\n A consulta não será agendada",
QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);

    if (b_ans == QMessageBox::Yes)
         event->accept();
        else
         event->ignore();
 }
