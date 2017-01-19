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

#include <QFontDatabase>
#include <QString>
#include <QFont>


#include <QMimeData>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>


#include "initdb.h"
#include "dboperate.h"

#include <QtSql>

#include "LAVA/lava.h"

#include "spectrograph.h"
#include "spectrumanalyser.h"

#include "qtconcurrentrun.h"


void MainWindow::onAudioBufferReady(pcm_stereo_sample *input_buffer)
{
    analyser->execute_stereo(input_buffer);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    analyser = new SpectrumAnalyser(this);

    setWindowFlags(Qt::FramelessWindowHint);
    setMouseTracking(true);


    m_tagManager = TagManager::instance();

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

    volumeDial = new CustomDial(this, 0, 100);
    volumeDial->setValue(100);
    volumeDial->setGeometry(QRect(780 - 8, 535 - 8,90 + 26, 90 + 26));

    playListTabWidget = new PlayListTabWidget(this);
    PlayListView * view = playListTabWidget->createTab();
    connect(this, SIGNAL(onAddSong()), view, SLOT(refreshModel()));
    connect(this, SIGNAL(songStartPlay(int)), view, SLOT(onSongStartPlay(int)));
    connect(this, SIGNAL(songStartPlay()), view, SLOT(onSongStartPlay()));
    connect(this, SIGNAL(songPaused()), view, SLOT(onSongPaused()));
    connect(this, SIGNAL(songStopped()), view, SLOT(onSongStopped()));
    connect(view, SIGNAL(onSongDoubleClicked(QSqlRecord)), this, SLOT(onDoubleClickSong(QSqlRecord)));

    connect(ui->prevBtn, SIGNAL(clicked()), view, SLOT(playPrevious()));
    connect(ui->nextBtn, SIGNAL(clicked()), view, SLOT(playNext()));
    connect(ui->noteBtn, SIGNAL(clicked()), view, SLOT(scrollToPlayingSong()));


    setAcceptDrops(true);

    titleBar = new UI::TitleBar(this);
    m_player = Player::instance();
    connect(m_player, SIGNAL(aboutToStop()), view, SLOT(onSongAboutToStop()));


    m_settings = new Settings(this);
    connectSignals();
    connect(ui->settingsBtn, SIGNAL(clicked()), this, SLOT(showSettingDialog()));

}


void MainWindow::connectSignals()
{
    connect(titleBar, SIGNAL(closeClicked()), this, SLOT(close()));
    connect(titleBar, SIGNAL(minClicked()), this, SLOT(setMinimumWindow()));

    connect(analyser, SIGNAL(spectrumAnalysed(fftw_complex*,fftw_complex*,int)), spectrumGraph, SLOT(spectrumChanged(fftw_complex*,fftw_complex*,int)));
    connect(spectrumGraph, SIGNAL(barsGeneratedLeft(std::vector<double>&)), spectrumLineLeft, SLOT(onBarGenerated(std::vector<double>&)));
    connect(spectrumGraph, SIGNAL(barsGeneratedRight(std::vector<double>&)), spectrumLineRight, SLOT(onBarGenerated(std::vector<double>&)));


    connect(ui->process_bar, SIGNAL(sliderReleased()), this, SLOT(onSliderMoved()));

    connect(m_player, SIGNAL(audioBufferReady(pcm_stereo_sample*)), this, SLOT(onAudioBufferReady(pcm_stereo_sample*)));
    connect(m_player, SIGNAL(positionChanged(int)), this, SLOT(onPosChanged(int)));
    connect(ui->pauseBtn, SIGNAL(clicked()), m_player, SLOT(pause()));
    connect(ui->stopBtn, SIGNAL(clicked()), m_player, SLOT(stop()));
    connect(ui->playBtn, SIGNAL(clicked()), this, SLOT(tryPlaySong()));
    connect(m_player, SIGNAL(playStateChange(Player::PlayState)), this, SLOT(onPlayStateChanged(Player::PlayState)));
    connect(volumeDial, SIGNAL(valueChanged(int)), m_player, SLOT(setVolume(int)));

    connect(m_settings, SIGNAL(deviceChanged(QString)), m_player, SLOT(changeAudioDevice(QString)));

    connect(m_tagManager, SIGNAL(gotCover(QPixmap)), this, SLOT(handleCover(QPixmap)));
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

void tryToGetCover(TagManager *inst, QString path)
{
    inst->getCover(path.toStdString().c_str());
}

void MainWindow::onDoubleClickSong(const QSqlRecord &rowInfo)
{
    QString path = rowInfo.value("path").toString();
    qDebug() << path;
//    LAVA::Core::instance()->load_file(path.toStdString().c_str());

    m_player->play(path);


    const QString title = rowInfo.value("title").toString();
    ui->title->setText(title);
    ui->album->setText(rowInfo.value("album").toString());
    ui->artist->setText(rowInfo.value("artist").toString());
    QtConcurrent::run(tryToGetCover, m_tagManager, path);  //  directly getting cover image will block UI thread
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

void MainWindow::handleCover(QPixmap cover)
{
    ui->cover->setPixmap(cover);
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
//    LAVA::Core::instance()->seek_by_absolute_pos(static_cast<double>(ui->process_bar->value()));
    m_player->seekTo(static_cast<double>(ui->process_bar->value()));
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

void MainWindow::playSong()
{
    if (m_player->currentState() != Player::NoState) {
        m_player->play();
    }
}

void MainWindow::onPlayStateChanged(Player::PlayState state)
{
    switch (state) {
    case Player::Playing:
    {
        ui->pauseBtn->show();
        ui->playBtn->hide();
        emit songStartPlay();
        break;
    }
    case Player::Paused:
    {
        ui->playBtn->show();
        ui->pauseBtn->hide();
        emit songPaused();
        break;
    }
    case Player::Stopped:
    {
        ui->playBtn->show();
        ui->pauseBtn->hide();
        spectrumGraph->cleanGraph();
        emit songStopped();
        break;
    }
    default:
        break;
    }
}

void MainWindow::tryPlaySong()
{
    if (m_player->currentState() == Player::NoState) {
        qDebug() << m_player->currentState();
        static_cast<PlayListView *>(playListTabWidget->currentWidget())->tryStartPlay();
    } else {
        m_player->play();
    }
}

void MainWindow::showSettingDialog()
{
    m_settings->setAudioDevices(m_player->getAudioDevices());
    m_settings->show();
}
