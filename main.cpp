#include <QApplication>
#include "calendar.h"
#include "autosaverestore.h"

int main(int argc, char *argv[])
{
    int status;
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":icon.png"));

    if (PQstatus(DB::conn) == CONNECTION_BAD)
    {
        QMessageBox::critical(nullptr, "Erro no banco de dados",
"Não foi possível se conectar ao banco de dados, encerrando programa.");
        exit(1);
    }

    Calendar* c = new Calendar();
    c->showMaximized();

    AutosaveRestore ar;

    status = a.exec();

    delete c;

    PQfinish(DB::conn);

    return status;
}
