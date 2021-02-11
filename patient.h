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
#include "qutils.h"
#include "appointmentwidget.h"
#include "db.h"
#include "patientdelegate.h"
#include "patientmodel.h"
#include "autosavetextedit.h"
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
    char* name;
    bool invalid = false;
    std::array<AutosaveTextEdit*, 4> tabs;

    //Layout
    QWidget *centralWidget = new QWidget(this);
    QMenuBar *menuBar = new QMenuBar(this);
    QMenu *menuOp_es = new QMenu("Opções", menuBar);
    QMenu *menuAppointment = new QMenu("Consulta", menuBar);
    QStatusBar *statusBar = new QStatusBar(this);
    QVBoxLayout *verticalLayout = new QVBoxLayout(centralWidget);
    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    QLabel *nameLbl = new QLabel(centralWidget);
    QSpacerItem *horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QComboBox *comboBox = new QComboBox(centralWidget);
    QStackedLayout* stackedLayout = new QStackedLayout();
    QTableView* tableView = new QTableView();
    QTabWidget *tabWidget = new QTabWidget(centralWidget);
    AppointmentWidget* appointmentWidget;
    PatientModel* pModel;

signals:
    void closed(char*);

    //Responsive
    void patientEdited(char* name);
    void appointmentChanged();

public slots:
    //User action
    void deletePatient();
    void appointmentOpened();

    //Extern action
    void appointmentClosed();
    void nameChanged(char* newName);
    void resizeNoteCell();
};

#endif // PATIENT_H
