#include "edittabmodel.h"
#include "db.h"
#include <QMessageBox>

EditTabModel::EditTabModel(int _tabNumber, QWidget *parent) :
    QWidget(parent, Qt::Window | Qt::Tool)
{
    PGresult* res = DB::Exec("SELECT get_text_default('patient', $1)", DB::tableTabs[_tabNumber]);

    if (res == nullptr)
    {
        delete horizontalButtonLayout;
        throw 0;
        return;
    }

    templateEdit = new AutosaveTextEdit(this, ("Modelo de " + DB::tabNames->at(_tabNumber)).toStdString(), PQgetvalue(res, 0, 0));
    PQclear(res);

    tabNumber = _tabNumber;

    setWindowTitle("Editar Modelo de " + DB::tabNames->at(tabNumber));
    resize(600, 400); //#Find elegant answer
    setAttribute(Qt::WA_DeleteOnClose);

    label->setText(DB::tabNames->at(tabNumber));
    verticalLayout->addWidget(label);
    verticalLayout->addWidget(templateEdit);

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

    templateEdit->save();

    values.push_back((char*) DB::tableTabs[tabNumber].c_str());
    values.push_back(QUtils::ToCString(templateEdit->toPlainText()));

    PGresult* res = DB::Exec("SELECT define_default('patient', $1, $2)", values);

    free(values[1]);

    if (res == nullptr)
    {
        saveFailed = true;
        return;
    }
    PQclear(res);

    templateEdit->ended();
    saved = true;
    this->close();
}

void EditTabModel::closeEvent(QCloseEvent *event)
{
    if (saved || !templateEdit->document()->isModified())
    {
        event->accept();
        return;
    }

    if (saveFailed)
    {
        templateEdit->save();

        QMessageBox::information(this, "Modelo salvo para recuperação",
            "O modelo foi falvo para posterior recuperação devido ao erro de salvamento.");

        event->accept();
        return;
    }

    QMessageBox::StandardButton b_ans = QMessageBox::warning(this, "Cancelar Edição de Modelo",
        "Tem certeza que deseja fechar essa janela?\n O novo modelo não será salvo.",
        QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);

    if (b_ans == QMessageBox::Yes)
    {
        event->accept();
        templateEdit->ended();
    }
        else
         event->ignore();
}
