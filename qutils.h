#ifndef QUTILS_H
#define QUTILS_H

#include <QString>
#include <QDateTimeEdit>
#include <functional>
extern "C" {
#include <libpq-fe.h>
}

typedef std::string string;

class QUtils {
public:
    static char* ToCString(QString qstr);
    static char* ToCString(string cppstr);
    static string constructLine(std::vector<string> tableFields);
    static string constructLine(std::vector<string>::iterator begin, std::vector<string>::iterator end);
    static string toBrDate(char* dateTime);
    static void BDdebug(PGresult* ans);
    static char* readFile(FILE* file);
};

class QBDateTimeEdit: public QDateTimeEdit {
public:
    QBDateTimeEdit(QWidget *p): QDateTimeEdit(p) {}
protected:
    void focusInEvent(QFocusEvent *event);
};


#ifdef __unix__
template <>
struct std::hash<QString>
{
    std::size_t operator()(const QString qstr) const;
};
#endif

#endif // QUTILS_H
