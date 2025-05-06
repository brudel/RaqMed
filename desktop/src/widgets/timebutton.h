/*  Copyright 2021 Bruno Del Monde.
 * Licensed under the EUPL-1.2.
 */

#ifndef TIMEBUTTON_H
#define TIMEBUTTON_H

#include <QPushButton>
#include <QTimer>
#include "common/qutils.h"

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
