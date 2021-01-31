#include "qutils.h"

char* QUtils::ToCString(QString qstr) {
    //cppstr length is different from qstr, but the same from cstr
	string cppstr = qstr.toStdString();
	char* cstr = (char*) malloc((cppstr.size() + 1) * sizeof(char));
	memcpy(cstr, cppstr.c_str(), (cppstr.size() + 1) * sizeof(char));

    return cstr;
}

char* QUtils::ToCString(string cppstr) {
    char* cstr = (char*) malloc((cppstr.size() + 1) * sizeof(char));
    memcpy(cstr, cppstr.c_str(), (cppstr.size() + 1) * sizeof(char));

    return cstr;
}

std::string QUtils::constructLine(std::vector<std::string> tableFields)
{
    return constructLine(tableFields.begin(), tableFields.end());
}

std::string QUtils::constructLine(std::vector<std::string>::iterator it, std::vector<std::string>::iterator end)
{
    std::string str = *it;
    while (++it != end)
        str += ", " + *it;

    return str;
}


void QBDateTimeEdit::focusInEvent(QFocusEvent *event){
        Section section = currentSection();
        QDateTimeEdit::focusInEvent(event);
        setSelectedSection(section);
}

#ifdef __unix__
std::size_t std::hash<QString>::operator()(const QString qstr) const
{
    return hash<string>()(qstr.toStdString());
}
#endif

char** cStringArray(int n, ...)
{
    va_list al;
    char** cstra = (char**) malloc(sizeof(*cstra) * n);

    va_start(al, n);
    for (int i = 0; i < n; ++i) {
        cstra[i] = va_arg(al, char*);
    }
    va_end(al);

    return cstra;
}

string QUtils::toBrDate(char* dateTime)
{
    return string("") + dateTime[8] + dateTime[9] + '/' + dateTime[5] + dateTime[6] + '/'
+ dateTime[0] + dateTime[1] + dateTime[2] + dateTime[3];
}


void QUtils::BDdebug(PGresult* res)
{
    qDebug(PQresultErrorMessage(res));
}

char* QUtils::readFile(FILE *file)
{
    char* s = malloc(1);
    size_t n = 1;
    char c;

    c = getc(file);
    while (c != EOF && c != '\n') {
        s[n-1] = c;
        s = realloc(s, ++n);
        c = getc(file);
    }
    s[n-1] = '\0';

    return s;
}

QDateTime QUtils::stringToQDateTime(char* str)
{
    return QDateTime(QDate(atoi(str), atoi(str + 5), atoi(str + 8)), QTime(atoi(str + 11), atoi(str + 14)));
}

QDate QUtils::stringToQDate(char* str)
{
    return QDate(atoi(str), atoi(str + 5), atoi(str + 8));
}

int QUtils::yearsTo(QDate first, QDate second)
{
    int deltaYears = second.year() - first.year();

    if (second.month() > first.month())
        return deltaYears;

    if (second.month() == first.month() && second.day() >= first.day())
        return deltaYears;

    return deltaYears - 1;
}


