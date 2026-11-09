/*  Copyright 2021 Bruno Del Monde.
 * Licensed under the EUPL-1.2.
 */

#ifndef QUTILS_H
#define QUTILS_H

#include <QString>
#include <QDateTimeEdit>
#include <functional>
#include <list>
#include <string>
#include <vector>
#include <QDate>
#include <QDebug>
#include <QDir>
extern "C" {
#include <libpq-fe.h>
}

#define HOMEDIR (QDir::homePath() + '/').toStdString()

#if defined(__unix__)
    #ifndef DATADIR
        #define DATADIR "/usr/share/raqmed"
    #endif
#elif defined(_WIN32)
    #ifndef DATADIR
        #define DATADIR "."
    #endif
#else
    #error Unknown environment!
#endif

#define CURVES_FILE (DATADIR "/curves.csv")

#define PROGRAM_PREFIX "raqmed_"

typedef std::string string;

// Per-user paths. On Unix they follow the XDG base directory spec.
extern const string AUTOBACKUP_DIR;
extern const string AUTOSAVES_DIR;
extern const string CONFIG_FILE;
extern const string LOG_FILE;

class QUtils {
public:
    static char* ToCString(QString qstr);
    static char* ToCString(string cppstr);
    static string constructLine(std::vector<string>::iterator begin, std::vector<string>::iterator end);
    static string toBrDate(char* dateTime);
    static void BDdebug(PGresult* ans);
    static char* readFileLine(FILE* file);
    static QDateTime stringToQDateTime(char* str);
    static QDate stringToQDate(char* str);
    static int yearsTo(QDate first, QDate second);
    static std::vector<char*> sVecToCVec(std::vector<string> svec);
    static std::vector<string> qVecToSVec(std::vector<QString> qvec);

    // Creates the directories holding the per-user files declared above.
    static void prepareDirs();

    //Inline functions
    static inline string constructLine(std::vector<std::string> tableFields)
        {return constructLine(tableFields.begin(), tableFields.end());}

    static bool autobackup;
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
