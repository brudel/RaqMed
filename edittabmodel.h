#ifndef EDITTABMODEL_H
#define EDITTABMODEL_H

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <autosavetextedit.h>
#include <qutils.h>
extern "C" {
#include <libpq-fe.h>
}

class EditTabModel : public QWidget
{
    Q_OBJECT

public:
    EditTabModel(int _tabNumber, QWidget *parent = nullptr);
    void closeEvent(QCloseEvent *event) override;

    //Variables
    int tabNumber;
    bool saved = false;
    bool saveFailed = false;

    //Layout
    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    QLabel* label = new QLabel("Data:", this);
    AutosaveTextEdit* templateEdit;
    QPushButton *saveButton = new QPushButton("Salvar", this);
    QPushButton *cancelButton = new QPushButton("Cancelar", this);
    QHBoxLayout *horizontalButtonLayout = new QHBoxLayout();

public slots:
    void save();

};

#endif // EDITTABMODEL_H
