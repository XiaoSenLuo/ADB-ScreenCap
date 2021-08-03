#include "widget.h"
#include "ui_widget.h"


#include "adb.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    init_ui();
    init_connect();
}

Widget::~Widget()
{
    delete ui;
    if(m_adb_run){
        ADB::killServer(m_adb_path);
    }
}

void Widget::handle_timer_timeout()
{
    if(m_work) return;
    m_work = 1;
    if(m_bat == nullptr) return;

    dx = this->ui->sbDx->value();
    dy = this->ui->sbDy->value();

    QString p = this->ui->lePDF->text();
    if(p.isEmpty()){
        p = QApplication::applicationDirPath();
        p.append("/pdf");
    }
    p.append(QString::fromUtf8("/assert/%1.png").arg(page_index));


    QString cmd = QApplication::applicationDirPath();
    cmd.append("/platform-tools/sc.bat");

    m_bat->start(cmd, QStringList() << QString::fromUtf8("%1").arg(dx) << QString::fromUtf8("%1").arg(dy) << p);
}

void Widget::init_connect()
{
    QObject::connect(this->ui->tbtADB, &QToolButton::clicked, [=](bool clicked){
        QString dir;
        if(m_adb_path.isEmpty()){
//            dir = QApplication::applicationDirPath();
            dir.append("platform-tools");
        }else{
            dir = m_adb_path;
        }
        const QString str = QFileDialog::getOpenFileName(this, QString::fromUtf8("打开ADB"), dir, QString::fromUtf8("adb.exe"));
        if(str.isEmpty()) return;
        m_adb_path = str;
        this->ui->leADB->setText(m_adb_path);
    });
    QObject::connect(this->ui->leADB, &QLineEdit::returnPressed, [=](){
        const QString str = this->ui->leADB->text();
        if(str.isEmpty()) return;
        m_adb_path = str;
    });
    QObject::connect(this->ui->pbtADBConnect, &QPushButton::clicked, [=](bool clicked){
        QString str = this->ui->leADB->text();
        if(str.isEmpty()){
            str = this->ui->leADB->placeholderText();
        }
        if((str.compare(QLatin1String("adb.exe")) == 0) || (str.compare(QLatin1String("adb")) == 0)){
            m_adb_path = QString::fromUtf8("platform-tools/adb.exe");
        }else{
            m_adb_path = str;
        }

        if(!m_adb_run){
            int e = ADB::startServer(m_adb_path);
            if(e == 0){
                m_adb_run = true;
            }else{
                m_adb_run = false;
            }
            qDebug() << "Exit Code:" << e ;
        }
        if(m_adb_run){
            ADB adb;
            bool ok = false;
            const QString id = adb.devices(m_adb_path, &ok);
            if(ok){
                this->ui->pbtStart->setEnabled(true);
                this->setWindowTitle(QString::fromUtf8("ADB-ScreenCap 设备[%1]").arg(id));
            }else{
                this->ui->pbtStart->setEnabled(0);
                this->setWindowTitle(QString::fromUtf8("ADB-ScreenCap 设备未授权"));
                QMessageBox::critical(this, QString::fromUtf8("ERROR"), QString::fromUtf8("设备未授权!"));
            }
        }
    });

    QObject::connect(this->ui->pbtGetAxis, &QPushButton::clicked, [=](bool c){
//        ADB adb;

//        adb.getevent(m_adb_path, &dx, &dy);

    });

    QObject::connect(this->ui->lePDF, &QLineEdit::returnPressed, [=](){
        const QString str = this->ui->lePDF->text();
        if(str.isEmpty()) return;
        m_pdf_path = str;
    });
    QObject::connect(this->ui->tbtPDF, &QToolButton::clicked, [=](bool clicked){
        const QString str = QFileDialog::getExistingDirectory(this, QString::fromUtf8("选择保存文件夹"), m_pdf_path);
        if(str.isEmpty()) return;
        m_pdf_path = str;
        this->ui->lePDF->setText(str);
    });

    QObject::connect(this->ui->pbtStart, &QPushButton::clicked, [=](bool c){
        if(!m_start){
            if(!m_adb_run) return;
            if(m_timer == nullptr){
                m_timer = new QTimer(this);
                QObject::connect(m_timer, &QTimer::timeout, this, &Widget::handle_timer_timeout);
            }
            dx = this->ui->sbDx->value();
            dy = this->ui->sbDy->value();

            const QString name = this->ui->leName->text();
            if(name.isEmpty()){
                QMessageBox::warning(this, QString::fromUtf8("Warning"), QString::fromUtf8("请输入书名"));
                return;
            }

            const QString autor = this->ui->leAutor->text();
            if(autor.isEmpty()){
                QMessageBox::warning(this, QString::fromUtf8("Warning"), QString::fromUtf8("请输入作者"));
                return;
            }
            QString pdir = this->ui->lePDF->text();
            if(pdir.isEmpty()){
                pdir = QApplication::applicationDirPath();
            }
            QDir dir(pdir);
            if(!dir.cd("pdf")){
                dir.mkdir("pdf");
                dir.cd("pdf");
            }
            const QString sdir = QString::fromUtf8("《%1》 %2").arg(name).arg(autor);
            dir.mkdir(sdir);
            dir.cd(sdir);
            dir.mkdir("assert");

            this->ui->lePDF->setText(dir.path());
            this->ui->pbtStart->setText(QString::fromUtf8("停止"));
            m_start = 1;

            if(m_bat == nullptr){
                m_bat = new QProcess(this);
                QObject::connect(m_bat, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [=](int exitCode, QProcess::ExitStatus exitStatus){
                    qDebug() << "ADB BAT Exit Code:" << exitCode;
                    page_index += 1;
                    m_work = false;
                });
                m_bat->setWorkingDirectory("platform-tools");
            }
            this->ui->pbtReset->setEnabled(0);
            if(!m_timer->isActive())  m_timer->start(100);

        }else{
            if(m_timer && m_timer->isActive()){
                m_timer->stop();
            }
            this->ui->pbtReset->setEnabled(1);
            this->ui->pbtStart->setText(QString::fromUtf8("开始"));
            m_start = 0;
        }
    });
    QObject::connect(this->ui->pbtReset, &QPushButton::clicked, [=](bool c){
         page_index = 0;
    });
}

void Widget::init_ui()
{
//    this->ui->leADB->setText(QString::fromUtf8("adb.exe"));
    this->setWindowTitle(QString::fromUtf8("ADB-ScreenCap"));
    this->ui->lePDF->setPlaceholderText(QString::fromUtf8("pdf"));
}

