#include "adb.h"

ADB::ADB(QObject *parent) : QObject(parent)
{
    adb = new QProcess(this);
}

ADB::~ADB()
{
    if(adb && ((adb->state() == QProcess::Running) || (adb->state() == QProcess::Starting))){
        adb->kill();
        adb->deleteLater();
    }
}
