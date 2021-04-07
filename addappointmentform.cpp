#include "addappointmentform.h"
#include "db.h"

AddAppointmentForm::AddAppointmentForm(QDate date, QWidget *parent) :
    QWidget(parent, Qt::Window | Qt::Tool), original(date)
{
    PGresult* res = DB::ExecCommand("SELECT get_text_default('appointment', 'content')");

    if (res == nullptr)
    {
        delete horizontalButtonLayout;
        delete formLayout;
        throw 0;
    }

    contentEdit = new AutosaveTextEdit(this, "Nova Consulta", PQgetvalue(res, 0, 0));
    PQclear(res);

    setWindowTitle("Agendar Consulta");
    resize(600, 400);
    setAttribute(Qt::WA_DeleteOnClose);

    formLayout->setWidget(0, QFormLayout::LabelRole, dateLabel);

    dateTimeEdit->setDate(date);
    dateTimeEdit->setCurrentSection(QDateTimeEdit::HourSection);
    formLayout->setWidget(0, QFormLayout::FieldRole, dateTimeEdit);

    formLayout->setWidget(1, QFormLayout::LabelRole, patientLabel);
    formLayout->setWidget(1, QFormLayout::FieldRole, lineEdit);
    new PatientLineEdit(lineEdit);

    verticalLayout->addLayout(formLayout);
    verticalLayout->addWidget(contentEdit);

    horizontalButtonLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    horizontalButtonLayout->addWidget(pushButton);
    horizontalButtonLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    verticalLayout->addLayout(horizontalButtonLayout);

    connect(pushButton, &QPushButton::clicked, this, &AddAppointmentForm::save);
}

void AddAppointmentForm::save()
{
    std::vector<QString> values;
    QString name = lineEdit->text();
    PGresult* res;
    std::vector<char*> expectedErrors = {"23503", "23505"};

    if (name.isEmpty())
    {
        QMessageBox::warning(this, "Erro de agendamento", "O nome do paciente está em branco.");
        return;
    }

    contentEdit->save();

    values.push_back(name);
    values.push_back(dateTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm"));
    values.push_back(contentEdit->toPlainText());

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

        return;
    }

    if (res == nullptr)
    {
        saveFailed = true;
        return;
    }
    PQclear(res);

    dateEdited(dateTimeEdit->date(), name);
    contentEdit->ended();
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
            !contentEdit->document()->isModified()
        )
    )
    {
        event->accept();
        return;
    }

    if (saveFailed)
    {
        contentEdit->save();

        QMessageBox::information(this, "Agendamento salvo para recuperação",
            "O agendamento foi falvo para posterior recuperação devido ao erro de salvamento.");

        event->accept();
        return;
    }

    QMessageBox::StandardButton b_ans = QMessageBox::warning(this, "Cancelar Agendamento",
        "Tem certeza que deseja fechar essa janela?\n A consulta não será agendada.",
        QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);

    if (b_ans == QMessageBox::Yes)
    {
        event->accept();
        contentEdit->ended();
    }
    else
        event->ignore();
 }
