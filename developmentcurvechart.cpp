#include "developmentcurvechart.h"
#include "db.h"

#define Y_SCALE 10
#define Y_MINOR_TICK_COUNT 4

std::array<QSplineSeries, 12> DevelopmentCurveChart::glbMarcondes;
bool DevelopmentCurveChart::initClass = DevelopmentCurveChart::initMarcondes();

DevelopmentCurveChart::DevelopmentCurveChart(char* _name, QDate _birthday, QWidget *parent) : QWidget(parent)
{
    name = _name;
    birthday = _birthday;

    char* buttonLabels[4] = {"Menino", "Menina", "Peso", "Altura"};

    buttonsLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    for (int i = 0; i < 2; ++i) {
        bgs[i] = new QButtonGroup(this);
        for (int j = 0; j < 2; ++j) {
            QRadioButton* button = new QRadioButton(buttonLabels[2 * i + j]);
            buttonsLayout->addWidget(button);
            bgs[i]->addButton(button);
        }
        bgs[i]->button(-2)->setChecked(true);
    }
    buttonsLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    mainLayout->addLayout(buttonsLayout);

    //DB dependent function
    //Must be before any setYAxis to setYAxis not fail
    //Mus be after button inicialization
    if (!loadPatient())
    {
        delete chart;
        throw 0;
    }

    //Axis
    chart->setAxisX(xAxis);
    chart->setAxisY(yAxis);
    xAxis->setLabelFormat("%d");
    yAxis->setLabelFormat("%d");
    yAxis->setMinorTickCount(Y_MINOR_TICK_COUNT);

    //Series
    setMarcondes();
    for (auto &series : patientSeries)
        series.setPointsVisible();
    addSeries(patientSerie);  //Must be after add marcondes because of colors

    //Chart
    chart->legend()->hide();

    //ChartView
    mainLayout->addWidget(chartView);
    chartView->setRenderHints(QPainter::Antialiasing);

    connect(bgs[0], SIGNAL(buttonClicked(int)), this, SLOT(setMarcondes()));
    connect(bgs[1], SIGNAL(buttonClicked(int)), this, SLOT(switchPatient()));
    connect(bgs[1], SIGNAL(buttonClicked(int)), this, SLOT(setMarcondes()));
}

bool DevelopmentCurveChart::initMarcondes()
{
    int month;
    float sample;

    FILE* file = fopen("curves.csv", "r");

    for (int i = 0; i < 24; ++i)
    {
        fscanf(file, "%d ", &month);
        for (int j = 0; j < 12; ++j)
        {
            fscanf(file, "%f ", &sample);
            glbMarcondes[j].append(month / 12., sample);
        }
    }

    fclose(file);

    return true;
}

void DevelopmentCurveChart::loadMarcondes(int offset)
{
    for (int i = 0; i < 3; ++i)
        marcondes[offset + i] << glbMarcondes[offset + i].points();
}

void DevelopmentCurveChart::setXAxis()
{
    double xmin, xmax;

    xmin = floor(patientSerie->at(0).rx());
    xmax = ceil(patientSerie->at(patientSerie->count() - 1).rx());
    xmax = xmax < 19 ? 19 : xmax;

    xAxis->setMin(xmin);
    xAxis->setMax(xmax);
    xAxis->setTickCount(xmax - xmin + 1);
}

void DevelopmentCurveChart::setYAxis(int offset)
{
    double ymin, ymax;
    int xMinIndex, xmin = xAxis->min();

    xMinIndex = (xmin > 1 ? xmin + 4 : xmin == 1 ? 4 : 0);
    ymin = patientSerie->at(0).ry() < marcondes[offset].at(xMinIndex).ry() ?
        patientSerie->at(xMinIndex).ry() : marcondes[offset].at(xMinIndex).ry();
    ymin = floor(ymin / Y_SCALE) * Y_SCALE;

    ymax = patientSerie->at(patientSerie->count() - 1).ry() > marcondes[offset + 2].at(marcondes[offset].count() - 1).ry() ?
        patientSerie->at(patientSerie->count() - 1).ry() : marcondes[offset + 2].at(marcondes[offset].count() - 1).ry();
    ymax = ceil(ymax / Y_SCALE) * Y_SCALE;

    yAxis->setMin(ymin);
    yAxis->setMax(ymax);
    yAxis->setTickCount((ymax - ymin) / Y_SCALE + 1);
}

void DevelopmentCurveChart::setMarcondes()
{
    int offset = marcondesOffset();
    chartView->setUpdatesEnabled(false);

    if (marcondes[offset].count() == 0)
        loadMarcondes(offset);

    for (auto serie : chart->series())
        if (serie != patientSerie)
            chart->removeSeries(serie);

    for (int i = 0; i < 3; ++i)
        addSeries(&marcondes[i + offset]);

    setYAxis(offset);

    chartView->setUpdatesEnabled(true);
}

bool DevelopmentCurveChart::loadPatient()
{
    int n;
    PGresult* res = DB::Exec("SELECT weight, height, day FROM appointment WHERE patient = $1 ORDER BY DAY", name);

    if (res == nullptr)
        return false;

    n = PQntuples(res);

    for (int i = 0; i < n; ++i)
        if (PQgetvalue(res, i, 0)[0] != '0' && PQgetvalue(res, i, 0)[0] != '\0')
        {
            double years = QUtils::monthsTo(birthday, QUtils::stringToQDate(PQgetvalue(res, i, 2))) / 12.;
            patientSeries[0].append(years, atof(PQgetvalue(res, i, 0)));
            patientSeries[1].append(years, atof(PQgetvalue(res, i, 1)));
        }

    PQclear(res);

    setXAxis();

    return true;
}

void DevelopmentCurveChart::switchPatient()
{
    if (patientSeries[bgs[1]->checkedId() == -2 ? 0 : 1].chart() != nullptr)
        return;

    patientSerie = &patientSeries[bgs[1]->checkedId() == -2 ? 0 : 1];
    addSeries(patientSerie);

    //Must be after add the new to keep diferent colors
    chart->removeSeries(&patientSeries[bgs[1]->checkedId() != -2 ? 0 : 1]);
}

void DevelopmentCurveChart::birthdayChanged(QDate newBirthday)
{
    birthday = newBirthday;
    resetPatient();
}

void DevelopmentCurveChart::resetPatient()
{
    for (auto &series : patientSeries)
        series.clear();
    loadPatient();
    setYAxis(marcondesOffset());
}
