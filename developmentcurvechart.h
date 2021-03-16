#ifndef DEVELOPMENTCURVECHART_H
#define DEVELOPMENTCURVECHART_H

#include <QWidget>
#include <QtCharts>
#include <QLineSeries>
#include <QChart>
#include <QChartView>
#include <QLayout>
#include <QPushButton>

class DevelopmentCurveChart : public QWidget
{
    Q_OBJECT
public:
    explicit DevelopmentCurveChart(char *_name, QDate _birthday, QWidget *parent = nullptr);
    static bool initMarcondes();
    void loadMarcondes(int offset);
    bool loadPatient();
    void setXAxis();
    void setYAxis();

    //Variables
    static std::array<QSplineSeries, 12> glbMarcondes;
    static bool initClass;
    std::array<QSplineSeries, 12> marcondesArray;
    QSplineSeries* marcondes = marcondesArray.data();
    char* name;
    QDate birthday;
    std::array<QLineSeries, 2> patientSeries;
    QLineSeries* patientSerie = patientSeries.data();

    //Layout
    std::array<QButtonGroup*, 2> bgs;
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QValueAxis* xAxis = new QValueAxis(this);
    QValueAxis* yAxis = new QValueAxis(this);
    QChart* chart = new QChart();
    QChartView* chartView = new QChartView(chart, this);

signals:

public slots:
    //User action
    void setMarcondes();
    void switchPatient();

    //Responsivity (may fail to reload without returning any indicator)
    void birthdayChanged(QDate newBirthday);
    void resetPatient();

private:
    inline void addSeries(QLineSeries* series) {chart->addSeries(series); series->attachAxis(xAxis); series->attachAxis(yAxis);}
};

#endif // DEVELOPMENTCURVECHART_H
