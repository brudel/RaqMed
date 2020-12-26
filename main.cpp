#include "calendar.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":icon.png"));

    if (PQstatus(PatientBDModel::conn) == CONNECTION_BAD)
    {
        QMessageBox::critical(nullptr, "Erro no banco de dados",
"Não foi possível se conectar ao banco de dados, encerrando programa.");
    exit(1);
    }

    Calendar c;
    c.showMaximized();

    return a.exec();
}

