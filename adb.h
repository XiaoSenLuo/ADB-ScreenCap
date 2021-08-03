#ifndef ADB_H
#define ADB_H

#include <QObject>
#include <QProcess>
#include <QApplication>
#include <QDebug>
#include <QFile>



class ADB : public QObject
{
    Q_OBJECT
public:
    explicit ADB(QObject *parent = nullptr);


    static int startServer(const QString &p){
        int e = QProcess::execute(p, QStringList() << "start-server");

        return e;
    }

    static int killServer(const QString &adb){
        int e = QProcess::execute(adb, QStringList() << "kill-server");
        return e;
    }

    void setADBPath(const QString& p){
        adb_path = QString(p);
    }

    const QString devices(bool *ok){
        QProcess adb;

        int pos = adb_path.lastIndexOf('/');
        QString dir = adb_path.mid(0, pos);
        dir.append('/');
        adb.setWorkingDirectory(dir);
        qDebug() << dir;
        adb.start(adb_path, QStringList() << "devices", QIODevice::ReadWrite);

        adb.waitForFinished();
        const QString rstr = adb.readAll();
        qDebug() << rstr;

        int pos1 = -1;
        pos = rstr.indexOf("\r\n");
        pos1 = rstr.indexOf('\t');

        if(rstr.contains(QLatin1String("unauthorized"))){
            *ok = false;
        }else{
            *ok = true;
        }

        return rstr.mid(pos + 2, pos1 - pos - 2);
    }

    const QString devices(const QString& p, bool* ok){
        adb_path = QString(p);
        return devices(ok);
    }

    void keyevent(int keycode){

    }

    void tap(int dx = -1, int dy = -1){
        QProcess adb;
        int pos = adb_path.lastIndexOf('/');
        QString dir = adb_path.mid(0, pos);
        adb.setWorkingDirectory(dir);

        adb.start(adb_path, QStringList() << "shell" << "input tap" << QString::fromUtf8("%1").arg(dx) << QString::fromUtf8("%1").arg(dy), QIODevice::ReadWrite);
        adb.waitForFinished();
    }

    void tap(const QString& p, int dx = -1, int dy = -1){
        adb_path = QString(p);
        tap(dx, dy);
    }

    int screencap(const QString& filename){
        QProcess adb;

        int pos = adb_path.lastIndexOf('/');
        QString dir = adb_path.mid(0, pos);
        adb.setWorkingDirectory(dir);

        adb.start(adb_path, QStringList() << "exec-out screencap -p >" << filename, QIODevice::ReadWrite);
        adb.waitForFinished();

        QString rstr = adb.readAll();
        qDebug() << rstr;
        return 0;
    }

    int screencap(const QString& p, const QString& filename){
        adb_path = QString(p);
        return screencap(filename);
    }

    void getevent(const QString& p, int *dx, int *dy){
        QProcess adb;
        QObject::connect(&adb, &QProcess::readyRead, [=](){
            QProcess *adb = qobject_cast<QProcess*>(sender());
             const QString str = adb->readAll();
//            adb->kill();
//            adb->deleteLater();
            qDebug() << str;
        });

        adb.start(p, QStringList() << "shell" << "getevent -p" << "|" << "grep -e \"0035\" -e \"0036\"", QIODevice::ReadWrite);
//        adb.waitForFinished();
//        QString rstr = adb.readAll();
//        qDebug() << rstr;
//        QStringList list = rstr.split("\r\n");

        adb.start(p, QStringList() << "shell" << "getevent | grep -e \"0035\" -e \"0036\"", QIODevice::ReadWrite);
        adb.waitForFinished();
    }

    ~ADB();

signals:


private:
//    static QProcess *q_cmd;
//    static QProcess *q_adb;
      QString adb_path;
      float w_rate, h_rate;
      QProcess *adb = nullptr;

//    static bool q_adb_run;
};

#endif // ADB_H
