#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <taglib/tbytevector.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v2frame.h>
#include <taglib/id3v2header.h>
#include <taglib/attachedpictureframe.h>
#include <iostream>
#include <QDebug>

#include "tagmanager.h"
#include <QFontDatabase>
#include <QString>
#include <QFont>


#include <QMimeData>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include "customdial.h"

#include "playlisttabwidget.h"
#include "playlistview.h"

#include "initdb.h"
#include "dboperate.h"

#include <QtSql>

#include "LAVA/lava.h"


class CBK : public LAVA::AudioCallbackInject
{
public:
    CBK(MainWindow * mainWindow = nullptr);
    virtual void update(uint8_t *stream, int len);
private:
    MainWindow *inst = nullptr;
};

CBK::CBK(MainWindow *mainWindow)
{
    inst = mainWindow;
}

void CBK::update(uint8_t *stream, int len)
{
    qDebug() << stream;
    qDebug() << len;
    if (inst) {
        inst->cbk(stream, len);
    }
}

void MainWindow::cbk(uint8_t *stream, int len)
{
    qDebug() << "haha";
    qDebug() << stream[0];
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setMouseTracking(true);
    // initialize the database
    QSqlError err = initDb();
    if (err.type() != QSqlError::NoError) {
        QMessageBox::critical(this, "Unable to initialize Database",
                    "Error initializing database: " + err.text());
        return;
    }


    ui->setupUi(this);
    ui->cover->setScaledContents(true);


    QPixmap cover = TagManager::instance()->getCover("/Users/marcoqin/marco/1.mp3");
    ui->cover->setPixmap(cover);

    int id = QFontDatabase::addApplicationFont(":/fonts/fonts/digital-7 (mono).ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    std::cout << family.toStdString() << std::endl;
    QFont font(family, 27);
    ui->timePassed->setFont(font);
    ui->timePassed_back->setFont(font);
    ui->timePassedTotal->setFont(font);
    ui->timePassedTotal_back->setFont(font);

    CustomDial *dial = new CustomDial("VOL",this, 0, 100);
    dial->setValue(100);
    dial->setStyleSheet("CustomDial {qproperty-arcColor: #dc5053;qproperty-arcWidth: 3;background-color: transparent;font-size: 18px;font-family: DIN;color: #dc5053; }");
    dial->setGeometry(QRect(780, 535,90, 90));

    PlayListTabWidget *tabWidget = new PlayListTabWidget(this);
    PlayListView * view = tabWidget->createTab();
    connect(this, SIGNAL(onAddSong()), view, SLOT(refreshModel()));
    connect(view, SIGNAL(onSongDoubleClicked(QSqlRecord)), this, SLOT(onDoubleClickSong(QSqlRecord)));

    setAcceptDrops(true);

    titleBar = new UI::TitleBar(this);
    connectSignals();

    CBK *cbk = new CBK(this);
    LAVA::Core::instance()->setAudioCallbackInject(static_cast<LAVA::AudioCallbackInject*>(cbk));
}


void MainWindow::connectSignals()
{
    connect(titleBar, SIGNAL(closeClicked()), this, SLOT(close()));
    connect(titleBar, SIGNAL(minClicked()), this, SLOT(setMinimumWindow()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    foreach (const QUrl &url, e->mimeData()->urls()) {
        QString fileName = url.toLocalFile();
        qDebug() << "Dropped file:" << fileName;
        TagInfo info = TagManager::instance()->getTagInfo(fileName.toStdString().c_str());
        addSong(0, info.path, info.title, info.artist, info.album, info.bitrate, info.samplerate, info.length, info.genre, info.track, info.year);
    }
    emit onAddSong();
}

void MainWindow::onDoubleClickSong(const QSqlRecord &rowInfo)
{
    QString path = rowInfo.value("path").toString();
    qDebug() << path;
    LAVA::Core::instance()->load_file(path.toStdString().c_str());
    const QString title = rowInfo.value("title").toString();
    ui->title->setText(title);
    ui->album->setText(rowInfo.value("album").toString());
    ui->artist->setText(rowInfo.value("artist").toString());
    ui->cover->setPixmap(TagManager::instance()->getCover(path.toStdString().c_str()));
    ui->bit_rate->setText(QString::number(rowInfo.value("bitrate").toInt()) + " Kb/s");
    ui->sample_rate->setText(QString::number(rowInfo.value("samplerate").toInt()) + " Hz");
    std::string fmt = path.toStdString();
    std::size_t found = fmt.rfind(".");
    if (found!=std::string::npos)  {
        std::string s = fmt.substr(found + 1);
        std::transform(s.begin(), s.end(), s.begin(), ::toupper);
        ui->format->setText(s.c_str());
        qDebug() << s.c_str();
    }
}

void MainWindow::setMinimumWindow()
{
    titleBar->hide();
    setWindowState(Qt::WindowMinimized);
}

void MainWindow::showEvent(QShowEvent *event)
{
    QTimer::singleShot(300, this, SLOT(resetTitleBar()));
    event->accept();
}

void MainWindow::resetTitleBar()
{
    titleBar->resetPosition(this);
    titleBar->show();
}

void MainWindow::moveEvent(QMoveEvent *event)
{
    titleBar->resetPosition(this);
}
