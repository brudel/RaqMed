/*  Copyright 2021 Bruno Del Monde.
 * Licensed under the EUPL-1.2.
 */

#include "widgets/timebutton.h"
#include <string>

TimeButton::TimeButton(string _baseText, int _time, QWidget *parent) :
    QPushButton(makeText(_baseText, _time), parent), baseText(_baseText), time(_time)
{
    setEnabled(false);

    timer.setInterval(1000);
    timer.start();

    connect(&timer, SIGNAL(timeout()), SLOT(timepass()));
}

inline QString TimeButton::makeText(string text, int ltime)
{
    return QString((text + " (" + std::to_string(ltime) + ")").c_str());
}

void TimeButton::timepass()
{
    if (--time)
    {
        setText(makeText(baseText, time));
        return;
    }

    timer.stop();
    setText(baseText.c_str());
    setEnabled(true);
}
