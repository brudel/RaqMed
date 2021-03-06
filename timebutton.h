#ifndef TIMEBUTTON_H
#define TIMEBUTTON_H

#include <QPushButton>
#include <QTimer>
#include "qutils.h"

class TimeButton : public QPushButton
{
    Q_OBJECT

public:
    TimeButton(string _baseText, int _time = 5, QWidget* parent = nullptr);
    inline QString makeText(string text, int ltime);

    string baseText;
    int time;
    QTimer timer;

public slots:
    void timepass();
};

#endif // TIMEBUTTON_H
