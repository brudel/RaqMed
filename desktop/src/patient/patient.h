/*  Copyright 2021 Bruno Del Monde.
 * Licensed under the EUPL-1.2.
 */

#ifndef PATIENT_H
#define PATIENT_H

#include <QMainWindow>
#include <QTableView>
#include <QBoxLayout>
#include <QComboBox>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QStackedLayout>
#include <QHeaderView>
#include <QMessageBox>
#include "common/qutils.h"
#include "appointment/appointmentwidget.h"
#include "common/db.h"
#include "patient/patientdelegate.h"
#include "patient/patientmodel.h"
#include "autosave/autosavetextedit.h"
#include "patient/developmentcurvechart.h"
extern "C" {
#include <libpq-fe.h>
}

namespace Ui {
class Patient;
}

class Patient : public QMainWindow
{
    Q_OBJECT

public:
    explicit Patient(QString name, QWidget *parent = nullptr);
    ~Patient();
    void closeEvent(QCloseEvent *event) override;
    bool exit();
    bool saveTabs();

    //Variables
    string name;
    bool invalid = false;
    std::array<AutosaveTextEdit*, TABS_NUM> tabs;

    //Layout
    QWidget *centralWidget = new QWidget(this);
    QMenuBar *menuBar = new QMenuBar(this);
    QMenu *menuOp_es = new QMenu("Opções", menuBar);
    QMenu *menuAppointment = new QMenu("Consulta", menuBar);
    QStatusBar *statusBar = new QStatusBar(this);
    QVBoxLayout *verticalLayout = new QVBoxLayout(centralWidget);
    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    QLabel *nameLbl = new QLabel(centralWidget);
    QComboBox *comboBox = new QComboBox(centralWidget);
    QStackedLayout* stackedLayout = new QStackedLayout();
    QTableView* tableView = new QTableView(centralWidget);
    QTabWidget *tabWidget = new QTabWidget(centralWidget);
    AppointmentWidget* appointmentWidget;
    PatientModel* pModel;
    DevelopmentCurveChart* developmentChart;

signals:
    void closed(char*);

    //Responsive
    void patientEdited(char* name);
    void appointmentChanged();

public slots:
    //User action
    void deletePatient();
    void appointmentOpened();
    void constructChart(int tab);

    //Extern action
    void appointmentClosed();
    void nameChanged(char* newName);
    void resizeNoteCell();
};

#endif // PATIENT_H
