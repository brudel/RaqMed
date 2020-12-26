#include "addpatientform.h"
#include "patientbdmodel.h"
#include <QMessageBox>

AddPatientForm::AddPatientForm(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle("Cadastrar Paciente");

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

void AddPatientForm::save() {
    std::vector<char*> values;

    values.push_back(QUtils::ToCString(lineEdits[0]->text()));
    values.push_back(QUtils::ToCString(birthDateEdit->date().toString("yyyy-MM-dd")));
    for (int i = 1; i < FIELDS_NUM - 2; ++i)
        values.push_back(QUtils::ToCString(lineEdits[i]->text()));
    values.push_back(QUtils::ToCString(plainTextEdit->toPlainText()));

    PatientBDModel::BDExec("INSERT INTO patient (name, " + PatientBDModel::tableFieldsLine +
") VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14, $15, $16)", values);

    saved = true;
    this->close();
}

void AddPatientForm::closeEvent(QCloseEvent *event)
{
    if (saved) {
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
