#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QProcess>
#include <QTimer>
#include <QDialog>
#include <QMessageBox>
#include <QDir>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();


    void handle_timer_timeout(void);



private:
    Ui::Widget *ui;

    QTimer *m_timer = nullptr;

    QString m_adb_path;
    QString m_pdf_path;

    bool m_adb_connect = false;
    bool m_adb_run = false;
    bool m_start = false;
    bool m_work = false;

    int dx = 0, dy = 0;
    int page_index = 0;

    QProcess *m_adb = nullptr;
    QProcess *m_bat = nullptr;

    void init_connect(void);
    void init_ui(void);
};
#endif // WIDGET_H
