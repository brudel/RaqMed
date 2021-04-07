#include "edittabmodel.h"
#include "db.h"
#include <QMessageBox>

EditTabModel::EditTabModel(int _tabNumber, QWidget *parent) :
    QWidget(parent, Qt::Window | Qt::Tool), tabNumber(_tabNumber)
{
    PGresult* res;
    QString* name;

    if (tabNumber >= 0)
        name = &DB::tabNames->at(tabNumber);
    else
        name = new QString("Consulta");

    if (tabNumber >= 0)
        res = DB::Exec("SELECT get_text_default('patient', $1)", DB::tableTabs[tabNumber]);
    else
        res = DB::ExecCommand("SELECT get_text_default('appointment', 'content')");

    if (res == nullptr)
    {
        delete horizontalButtonLayout;
        throw 0;
        return;
    }

    templateEdit = new AutosaveTextEdit(this, ("Modelo de " + *name).toStdString(), PQgetvalue(res, 0, 0));
    PQclear(res);

    setWindowTitle("Editar Modelo de " + *name);
    resize(600, 400);
    setAttribute(Qt::WA_DeleteOnClose);

    label->setText(*name);
    verticalLayout->addWidget(label);
    verticalLayout->addWidget(templateEdit);

    horizontalButtonLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    horizontalButtonLayout->addWidget(saveButton);
    horizontalButtonLayout->addWidget(cancelButton);
    horizontalButtonLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    verticalLayout->addLayout(horizontalButtonLayout);

    connect(saveButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));


    if (tabNumber < 0)
        delete name;
}

void EditTabModel::save()
{
    PGresult* res;
    string aux;

    templateEdit->save();

    aux = templateEdit->toPlainText().toStdString();
    char* new_default = PQescapeLiteral(DB::conn, aux.c_str(), aux.size());

    if (tabNumber >= 0)
        res = DB::ExecCommand("ALTER TABLE patient ALTER " + DB::tableTabs[tabNumber] + " SET DEFAULT " + new_default);
    else
        res = DB::ExecCommand((string) "ALTER TABLE appointment ALTER content SET DEFAULT " + new_default);

    PQfreemem(new_default);

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
