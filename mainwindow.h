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


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void cbk(pcm_stereo_sample *input_buffer);
signals:
    void onAddSong();
    void onSongStartPlay();
    void posChanged(int pos);
public slots:
    void onDoubleClickSong(const QSqlRecord &rowInfo);
    void setMinimumWindow();
    void showEvent(QShowEvent *event);
    void resetTitleBar();
    void moveEvent(QMoveEvent *event);
    void onPosChanged(int pos);
    void onSliderMoved();
    void changeBtnToPause();
    void changeBtnToPlay();
protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private:
    enum PlayState {
        Play = 0,
        Pause,
        Stop
    };

    int m_last_position = 0;
    int m_length = 0;
    void connectSignals();
    Ui::MainWindow *ui;
    UI::TitleBar *titleBar;
    fftw_plan m_fftw_plan_left;
    fftw_complex *m_fftw_output_left;
    SpectrumAnalyser *analyser;
    Spectrograph *spectrumGraph;
    SpectrumLineLeft *spectrumLineLeft;
    SpectrumLineRight *spectrumLineRight;
};

#endif // MAINWINDOW_H
