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
    void setXAxis();
    void setYAxis(int offset);

    //Variables
    static std::array<QSplineSeries, 12> glbMarcondes;
    std::array<QSplineSeries, 12> marcondes;
    static bool initClass;
    char* name;
    QDate birthday;
    QLineSeries patientSerie;

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
    void setMarcondes();
    bool loadPatient();
};

#endif // DEVELOPMENTCURVECHART_H
