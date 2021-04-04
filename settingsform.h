#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>
#include "qutils.h"

class SettingsForm : public QWidget
{
    Q_OBJECT
public:
    explicit SettingsForm(QWidget* parent = nullptr, bool blank = false);

    QLabel* connStrLabel = new QLabel("URL do banco de dados", this);
    QLineEdit* connStrLineEdit = new QLineEdit(this);
    QHBoxLayout* connStrLayout = new QHBoxLayout();
    QCheckBox* autoBkpChkBtn = new QCheckBox("Backup local automático", this);
    QPushButton* saveButton = new QPushButton("Salvar", this);
    QPushButton* cancelButton = new QPushButton("Cancelar", this);
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

signals:

public slots:
    void save();
};

#endif // SETTINGSFORM_H
