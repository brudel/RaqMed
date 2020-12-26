#include "edittabmodel.h"
#include "patientbdmodel.h"
#include <QMessageBox>

EditTabModel::EditTabModel(int _tabNumber, QWidget *parent) :
    QDialog(parent)
{
    tabNumber = _tabNumber;

    setWindowTitle("Editar Modelo de " + PatientBDModel::tabNames->at(tabNumber));
    resize(600, 400); //#Find elegant answer

    label->setText(PatientBDModel::tabNames->at(tabNumber));
    verticalLayout->addWidget(label);
    verticalLayout->addWidget(plainTextEdit);

    horizontalButtonLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    horizontalButtonLayout->addWidget(saveButton);
    horizontalButtonLayout->addWidget(cancelButton);
    horizontalButtonLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    verticalLayout->addLayout(horizontalButtonLayout);

    connect(saveButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

    PGresult* ans = PatientBDModel::BDExec
("SELECT col.column_default \
FROM information_schema.columns col \
WHERE col.table_name = 'patient' \
AND col.column_name = $1", PatientBDModel::tableTabs[tabNumber]);

    QString old = PQgetvalue(ans, 0, 0) + 1;
    plainTextEdit->setPlainText(old.chopped(7));
}

void EditTabModel::save() {
    char* newDefault = QUtils::ToCString(plainTextEdit->toPlainText());

    PatientBDModel::BDExec("ALTER TABLE patient ALTER " + PatientBDModel::tableTabs[tabNumber]
+ " SET DEFAULT '" + newDefault + '\'');

    saved = true;
    this->close();
}

void EditTabModel::closeEvent(QCloseEvent *event)
{
    if (saved) {
        event->accept();
        return;
    }

    QMessageBox::StandardButton b_ans = QMessageBox::warning(this, "Cancelar Edição de Modelo",
"Tem certeza que deseja fechar essa janela?\n O novo modelo não será salvo",
QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);

    if (b_ans == QMessageBox::Yes)
         event->accept();
        else
         event->ignore();
}
