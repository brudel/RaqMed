/*  Copyright 2021 Bruno Del Monde.
 * Licensed under the EUPL-1.2.
 */

#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>

class SettingsForm : public QDialog
{
    Q_OBJECT
public:
    enum formMode {
        normal = 0,
        init = 1,
        create = 2
    };
    Q_DECLARE_FLAGS(formModes, formMode)

    explicit SettingsForm(QWidget* parent = nullptr, formModes _mode = normal);

    formModes mode;

    QLabel* connStrLabel = new QLabel("URL do banco de dados", this);
    QLineEdit* connStrLineEdit = new QLineEdit(this);
    QHBoxLayout* connStrLayout = new QHBoxLayout();
    QCheckBox* autoBkpChkBtn = new QCheckBox("Backup local automático", this);
    QPushButton* saveButton = new QPushButton("Salvar", this);
    QPushButton* cancelButton;
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

signals:

public slots:
    void save();
    void cancel();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(SettingsForm::formModes)

#endif // SETTINGSFORM_H
