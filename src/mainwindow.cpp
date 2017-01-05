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

#include "spectrograph.h"
#include "spectrumanalyser.h"


class CBK : public LAVA::AudioCallbackInject
{
public:
    CBK(MainWindow * mainWindow = nullptr);
    virtual void update(uint8_t *stream, int len);
private:
    MainWindow *inst = nullptr;
    pcm_stereo_sample *buf;
};

CBK::CBK(MainWindow *mainWindow)
{
    inst = mainWindow;
    buf = static_cast<pcm_stereo_sample *>(calloc(sample_buffer_size, sizeof(pcm_stereo_sample)));
}

void CBK::update(uint8_t *stream, int len)
{
    memset(buf, 0, sample_buffer_size);
    memcpy(buf, (int16_t *)stream,  len);
    if (inst) {
        inst->cbk(buf);
    }
}

void MainWindow::cbk(pcm_stereo_sample *input_buffer)
{
    analyser->execute_stereo(input_buffer);
    int cur_pos = static_cast<int>(LAVA::Core::instance()->time_position());
    if (cur_pos != m_last_position) {
        m_last_position = cur_pos;
        emit posChanged(cur_pos);
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    analyser = new SpectrumAnalyser(this);

    setWindowFlags(Qt::FramelessWindowHint);
    setMouseTracking(true);


    // initialize the database
    QSqlError err = initDb();
    if (err.type() != QSqlError::NoError) {
        QMessageBox::critical(this, "Unable to initialize Database",
                    "Error initializing database---> " + err.text());
        return;
    }


    ui->setupUi(this);
    ui->cover->setScaledContents(true);

    ui->pauseBtn->hide();

    spectrumGraph = new Spectrograph(this);
    spectrumLineLeft = new SpectrumLineLeft(this);
    spectrumLineRight = new SpectrumLineRight(this);

    int id = QFontDatabase::addApplicationFont(":/fonts/fonts/digital-7 (mono).ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    std::cout << family.toStdString() << std::endl;
    QFont font(family, 27);
    ui->timePassed->setFont(font);
    ui->timePassed_back->setFont(font);
    ui->timePassedTotal->setFont(font);
    ui->timePassedTotal_back->setFont(font);

    CustomDial *dial = new CustomDial(this, 0, 100);
    dial->setValue(100);
    dial->setGeometry(QRect(780 - 8, 535 - 8,90 + 26, 90 + 26));

    PlayListTabWidget *tabWidget = new PlayListTabWidget(this);
    PlayListView * view = tabWidget->createTab();
    connect(this, SIGNAL(onAddSong()), view, SLOT(refreshModel()));
    connect(this, SIGNAL(songStartPlay(int)), view, SLOT(onSongStartPlay(int)));
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

    connect(analyser, SIGNAL(spectrumAnalysed(fftw_complex*,fftw_complex*,int)), spectrumGraph, SLOT(spectrumChanged(fftw_complex*,fftw_complex*,int)));
    connect(spectrumGraph, SIGNAL(barsGeneratedLeft(std::vector<double>&)), spectrumLineLeft, SLOT(onBarGenerated(std::vector<double>&)));
    connect(spectrumGraph, SIGNAL(barsGeneratedRight(std::vector<double>&)), spectrumLineRight, SLOT(onBarGenerated(std::vector<double>&)));

    connect(this, SIGNAL(posChanged(int)), this, SLOT(onPosChanged(int)));

    connect(ui->process_bar, SIGNAL(sliderReleased()), this, SLOT(onSliderMoved()));

    connect(this, SIGNAL(songStartPlay()), this, SLOT(changeBtnToPause()));

    connect(ui->pauseBtn, SIGNAL(clicked()), this, SLOT(pauseSong()));
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

    int length = rowInfo.value("length").toInt();
    ui->timePassedTotal->setText(QTime(0, 0, 0).addSecs(length).toString("HH:mm:ss"));
    ui->timePassed->setText(QTime(0, 0, 0).toString("HH:mm:ss"));
    ui->percent->setText("0%");
    m_last_position = 0;
    m_length = length;
    ui->process_bar->setMaximum(length);

    int id = rowInfo.value("id").toInt();
    setPlaying(0, id);
    emit songStartPlay(id);
    emit songStartPlay();
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

void MainWindow::onPosChanged(int pos)
{
    ui->process_bar->setValue(pos);
    ui->timePassed->setText(QTime(0, 0, 0).addSecs(pos).toString("HH:mm:ss"));
    ui->percent->setText(QString::number(pos * 100 / m_length) + "%");
}

void MainWindow::onSliderMoved()
{
    LAVA::Core::instance()->seek_by_absolute_pos(static_cast<double>(ui->process_bar->value()));
}

void MainWindow::changeBtnToPause()
{
    ui->pauseBtn->show();
    ui->playBtn->hide();
}

void MainWindow::changeBtnToPlay()
{
    ui->playBtn->show();
    ui->pauseBtn->hide();
}

void MainWindow::pauseSong()
{
    LAVA::Core::instance()->pause();
}
