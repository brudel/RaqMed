#ifndef EDITTABMODEL_H
#define EDITTABMODEL_H

#include <QDialog>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <qutils.h>
extern "C" {
#include <libpq-fe.h>
}

class EditTabModel : public QDialog
{
    Q_OBJECT

public:
    EditTabModel(int _tabNumber, QWidget *parent = nullptr);
    void closeEvent(QCloseEvent *event) override;

    int tabNumber;
    bool saved = false;

    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    QLabel* label = new QLabel("Data:", this);
    QPlainTextEdit *plainTextEdit = new QPlainTextEdit(this);
    QPushButton *saveButton = new QPushButton("Salvar", this);
    QPushButton *cancelButton = new QPushButton("Cancelar", this);
    QHBoxLayout *horizontalButtonLayout = new QHBoxLayout();

public slots:
    void save();

};

#endif // EDITTABMODEL_H
