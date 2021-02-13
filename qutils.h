#ifndef QUTILS_H
#define QUTILS_H

#include <QString>
#include <QDateTimeEdit>
#include <functional>
#include <QDate>
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
    static char* readFileLine(FILE* file);
    static QDateTime stringToQDateTime(char* str);
    static QDate stringToQDate(char* str);
    static int yearsTo(QDate first, QDate second);
    static bool stringGreaterThan(char* a, char* b);
};

class QBDateTimeEdit: public QDateTimeEdit {
public:
    QBDateTimeEdit(QWidget *p): QDateTimeEdit(p) {}
protected:
    void focusInEvent(QFocusEvent *event);
};


#if QT_VERSION <= QT_VERSION_CHECK(5, 11, 3)
template <>
struct std::hash<QString>
{
    std::size_t operator()(const QString qstr) const;
};
#endif

#endif // QUTILS_H
