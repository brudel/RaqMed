#include "addappointmentform.h"
#include "db.h"

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
    PGresult* res;
    std::vector<char*> expectedErrors = {"23503", "23505"};

    if (name.isEmpty())
    {
        QMessageBox::warning(this, "Erro de agendamento", "O nome do paciente está em branco.");
        return;
    }

    values.push_back(QUtils::ToCString(name));
    values.push_back(QUtils::ToCString(dateTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm")));
    values.push_back(QUtils::ToCString(plainTextEdit->toPlainText()));

    try
    {
        res = DB::Exec("INSERT INTO appointment (patient, day, content) VALUES ($1, $2, $3)", values, expectedErrors);
    }

    catch (int sqlstate)
    {
        switch (sqlstate) {
        case 0:
            QMessageBox::warning(this, "Erro de agendamento", "Não existe paciente cadastrado com esse nome.");
            break;

        case 1:
            QMessageBox::warning(this, "Erro de agendamento",
                "Esse paciente já possui uma consulta agendada para esse dia e horário.");
            break;
        }

        res = nullptr;
    }

    for (auto cstr : values)
        free(cstr);

    if (res == nullptr)
        return;

    PQclear(res);

    dateEdited(dateTimeEdit->date(), name);
    saved = true;
    this->close();
}

void AddAppointmentForm::closeEvent(QCloseEvent *event)
 {
    if
    (
        saved ||
        (
            lineEdit->text().isEmpty() &&
            dateTimeEdit->dateTime() == QDateTime(original) &&
            plainTextEdit->toPlainText().isEmpty()
        )
    )
    {
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
