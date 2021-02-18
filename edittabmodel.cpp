#include "edittabmodel.h"
#include "db.h"
#include <QMessageBox>

EditTabModel::EditTabModel(int _tabNumber, QWidget *parent) :
    QDialog(parent)
{
    PGresult* res = DB::Exec("SELECT get_text_default('patient', $1)", DB::tableTabs[_tabNumber]);

    if (res == nullptr)
{
        close();
        return;
}

    plainTextEdit = new QPlainTextEdit(PQgetvalue(res, 0, 0), this);
    PQclear(res);

    tabNumber = _tabNumber;

    setWindowTitle("Editar Modelo de " + DB::tabNames->at(tabNumber));
    resize(600, 400); //#Find elegant answer

    label->setText(DB::tabNames->at(tabNumber));
    verticalLayout->addWidget(label);
    verticalLayout->addWidget(plainTextEdit);

    horizontalButtonLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    horizontalButtonLayout->addWidget(saveButton);
    horizontalButtonLayout->addWidget(cancelButton);
    horizontalButtonLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    verticalLayout->addLayout(horizontalButtonLayout);

    connect(saveButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

void EditTabModel::save()
{
    std::vector<char*> values;

    values.push_back((char*) DB::tableTabs[tabNumber].c_str());
    values.push_back(QUtils::ToCString(plainTextEdit->toPlainText()));

    PGresult* res = DB::Exec("SELECT define_default('patient', $1, $2)", values);

    free(values[1]);

    if (res == nullptr)
        return;
    PQclear(res);

    saved = true;
    this->close();
}

void EditTabModel::closeEvent(QCloseEvent *event)
{
    if (saved || !plainTextEdit->document()->isModified())
    {
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
