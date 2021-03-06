#ifndef CALENDAR_H
#define CALENDAR_H

#include <QMainWindow>
#include <unordered_map>
#include "patient.h"
#include <qutils.h>
extern "C" {
#include <libpq-fe.h>
}

namespace Ui {
class Calendar;
}

#define patient_umap std::unordered_map<QString, Patient*>
#define patient_it std::unordered_map<QString, Patient*>::const_iterator

class Calendar : public QMainWindow
{
    Q_OBJECT

public:
    explicit Calendar(QWidget *parent = nullptr);
    ~Calendar();

    //Functions
    bool openPatient(QString qname);
    void editModel(int k);
        //Edit k indexed model

    //Variables
    patient_umap open_patients;

public slots:

    //User action
    void dayChanged();
    void on_tableWidget_cellClicked(int row);
    void doneQuery(QString qname);

    //Extern action
    void patient_closed(char* name);

    //Add menu
    void on_actionAdicionar_Paciente_triggered();
    void on_actionAdicionar_Consulta_triggered();

    //Template edit menu
    void reasons() {editModel(0);};
    void antecedents() {editModel(1);};
    void exams() {editModel(2);};
    void reports() {editModel(3);};

    //Responsive control
    void dateChanged(QDate newQDate, QString name); //Add
    void dateChanged(QDate oldQDate, QDate newQDate); //Reschedule
    void dateChanged(QDate qDate); //Geral
    void nameChanged(char* name);

private:
    Ui::Calendar *ui;
};

#endif // CALENDAR_H
