#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QtWidgets>
#include <QUrl>
#include <QtSql>
#include <fftw3.h>

#include "titlebar.h"
#include "spectrumanalyser.h"
#include "spectrograph.h"
#include "spectrumlineleft.h"
#include "spectrumlineright.h"


#include "customdial.h"
#include "player.h"
#include "playlisttabwidget.h"
#include "playlistview.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
signals:
    void onAddSong();
    void songStartPlay();
    void songStartPlay(int id);
    void songPaused();
    void songStopped();
    void posChanged(int pos);
public slots:
    void onAudioBufferReady(pcm_stereo_sample *input_buffer);
    void onDoubleClickSong(const QSqlRecord &rowInfo);
    void setMinimumWindow();
    void showEvent(QShowEvent *event);
    void resetTitleBar();
    void moveEvent(QMoveEvent *event);
    void onPosChanged(int pos);
    void onSliderMoved();
    void changeBtnToPause();
    void changeBtnToPlay();
    void onPlayStateChanged(Player::PlayState state);
    void tryPlaySong();
protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private:

    Player *m_player;
    int m_last_position = 0;
    int m_length = 0;
    void connectSignals();
    Ui::MainWindow *ui;
    UI::TitleBar *titleBar;


    SpectrumAnalyser *analyser;
    Spectrograph *spectrumGraph;
    SpectrumLineLeft *spectrumLineLeft;
    SpectrumLineRight *spectrumLineRight;

    PlayListTabWidget *playListTabWidget;
    CustomDial *volumeDial;
private slots:
    void playSong();
};

#endif // MAINWINDOW_H
