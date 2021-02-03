#include "addpatientform.h"
#include "patientbdmodel.h"
#include <QMessageBox>

AddPatientForm::AddPatientForm(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle("Cadastrar Paciente");

    defaultDate = birthDateEdit->date();

    //Name line
    labels[0] = new QLabel(PatientBDModel::fieldNames->at(0), this);
    lineEdits[0] = new QLineEdit(this);
    formLayout->setWidget(0, QFormLayout::LabelRole, labels[0]);
    formLayout->setWidget(0, QFormLayout::FieldRole, lineEdits[0]);

    //Birthdate line
    labels[BIRTHDATE_INDEX] = new QLabel(PatientBDModel::fieldNames->at(BIRTHDATE_INDEX), this);
    formLayout->setWidget(BIRTHDATE_INDEX, QFormLayout::LabelRole, labels[BIRTHDATE_INDEX]);
    formLayout->setWidget(BIRTHDATE_INDEX, QFormLayout::FieldRole, birthDateEdit);

    //
    for (int i = 2; i < FIELDS_NUM - 1; ++i) {
        labels[i] = new QLabel(PatientBDModel::fieldNames->at(i), this);
        lineEdits[i - 1] = new QLineEdit(this);
        formLayout->setWidget(i, QFormLayout::LabelRole, labels[i]);
        formLayout->setWidget(i, QFormLayout::FieldRole, lineEdits[i - 1]);
    }

    labels[NOTES_INDEX] = new QLabel(PatientBDModel::fieldNames->at(NOTES_INDEX), this);
    formLayout->setWidget(NOTES_INDEX, QFormLayout::LabelRole, labels[NOTES_INDEX]);
    formLayout->setWidget(NOTES_INDEX, QFormLayout::FieldRole, plainTextEdit);

    verticalLayout->addLayout(formLayout);

    horizontalButtonLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    horizontalButtonLayout->addWidget(pushButton);
    horizontalButtonLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    verticalLayout->addLayout(horizontalButtonLayout);

    connect(pushButton, &QPushButton::clicked, this, &AddPatientForm::save);
    lineEdits[0]->setFocus();
}

void AddPatientForm::save()
{
    std::vector<char*> values;
    PGresult* res;
    std::vector<char*> expectedErrors = {"23505"};

    values.push_back(QUtils::ToCString(lineEdits[0]->text().simplified()));
    if (values.front()[0] == '\0')
    {
        QMessageBox::warning(this, "Erro de cadastro", "O nome do paciente está em branco.");
        free(values.front());
        return;
    }

    values.push_back(QUtils::ToCString(birthDateEdit->date().toString("yyyy-MM-dd")));
    for (int i = 1; i < FIELDS_NUM - 2; ++i) // FIELDS_NUM - birthday - notes
        values.push_back(QUtils::ToCString(lineEdits[i]->text()));
    values.push_back(QUtils::ToCString(plainTextEdit->toPlainText()));

    try
    {
        res = PatientBDModel::DBExec("INSERT INTO patient (name, " + PatientBDModel::tableFieldsLine +
            ") VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14, $15, $16)", values, expectedErrors);
    }

    catch (...)
    {
        QMessageBox::warning(this, "Erro de cadastro", "Já existe um paciente cadastrado com esse nome");
        res = nullptr;
    }

    for (auto cstr : values)
        free(cstr);

    if (res == nullptr)
        return;

    PQclear(res);

    saved = true;
    this->close();
}

void AddPatientForm::closeEvent(QCloseEvent *event)
{
    if (saved || noModified())
    {
        event->accept();
        return;
    }

    QMessageBox::StandardButton b_ans = QMessageBox::warning(this, "Cancelar Cadastro",
"Tem certeza que deseja fechar essa janela?\n O paciente não será cadastrado",
QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);

    if (b_ans == QMessageBox::Yes)
         event->accept();
        else
         event->ignore();
}

bool AddPatientForm::noModified()
{
    int i;

    if (birthDateEdit->date() != defaultDate || !plainTextEdit->toPlainText().isEmpty())
        return false;

    for (i = 0; i < FIELDS_NUM - 2; ++i) // FIELDS_NUM - birthday - notes
        if (!lineEdits[i]->text().isEmpty())
            break;

    return i == FIELDS_NUM - 2;
}
