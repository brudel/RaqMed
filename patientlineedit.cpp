/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "patientlineedit.h"
#include "qutils.h"
#include "db.h"

PatientLineEdit::PatientLineEdit(QLineEdit *parent): QObject(parent), editor(parent)
{
    popup = new QListWidget;
    popup->setWindowFlags(Qt::Popup);
    popup->setFocusPolicy(Qt::NoFocus);
    popup->setFocusProxy(parent);
    popup->setMouseTracking(true);

    popup->setEditTriggers(QTreeWidget::NoEditTriggers);
    popup->setFrameStyle(QFrame::Box | QFrame::Plain);

    popup->installEventFilter(this);
    popup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    popup->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(popup, SIGNAL(itemClicked(QListWidgetItem*)),
            SLOT(doneCompletion()));

    timer.setSingleShot(true);
    timer.setInterval(500);
    connect(&timer, SIGNAL(timeout()), SLOT(autoSuggest()));
    connect(editor, SIGNAL(textEdited(QString)), &timer, SLOT(start()));

}

PatientLineEdit::~PatientLineEdit()
{
    delete popup;
}

bool PatientLineEdit::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj != popup)
        return false;

    if (ev->type() == QEvent::MouseButtonPress) {
        popup->hide();
        editor->setFocus();
        return true;
    }

    if (ev->type() == QEvent::KeyPress) {
        bool consumed = false;
        int key = static_cast<QKeyEvent*>(ev)->key();
        switch (key) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
            doneCompletion();
            consumed = true;
            break;

        case Qt::Key_Escape:
            editor->setFocus();
            popup->hide();
            consumed = true;
            break;

        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Home:
        case Qt::Key_End:
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            break;

        default:
            editor->setFocus();
            editor->event(ev);
            popup->hide();
            break;
        }

        return consumed;
    }

    return false;
}

void PatientLineEdit::showCompletion(const QVector<QString> &choices)
{
    popup->setUpdatesEnabled(false);
    popup->clear();

    for (const auto &choice : choices)
        auto item  = new QListWidgetItem(choice, popup);

    int height = popup->sizeHintForRow(0) * popup->model()->rowCount()+ 2 * popup->frameWidth();
    int width = popup->sizeHintForColumn(0) + 2 * popup->frameWidth(); //#
    popup->resize(width, height);
    popup->setCurrentItem(popup->item(0));
    popup->setUpdatesEnabled(true);

    popup->move(editor->mapToGlobal(QPoint(0, editor->height())));
    popup->setFocus();
    popup->show();
}

void PatientLineEdit::doneCompletion()
{
    timer.stop();
    popup->hide();
    editor->setFocus();
    QListWidgetItem *item = popup->currentItem();
    if (item) {
        editor->setText(item->text());
        QMetaObject::invokeMethod(editor, "returnPressed");
    }

    completationDone(item->text());
}

void PatientLineEdit::autoSuggest()
{
    if (editor->text() == "")
        return;

    PGresult* res = DB::Exec("SELECT name FROM patient WHERE name ~* $1 LIMIT 10", editor->text());
    if (res == nullptr)
        return;

    int rows = PQntuples(res);

    if (rows == 0)
    {
        PQclear(res);
        return;
    }

    QVector<QString> choices;

    for (int i = 0; i < rows; ++i) {
        choices << PQgetvalue(res, i, 0);
    }
    PQclear(res);

    showCompletion(choices);
}

void PatientLineEdit::preventSuggest()
{
    timer.stop();
}
