#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include "spectrumanalyser.h"

#include <QVector>
#include <QString>
#include <QThread>

class WatchDog : public QThread
{
    Q_OBJECT
    void run() Q_DECL_OVERRIDE;
signals:
    void aboutToStop();
};


class Player : public QObject
{
    Q_OBJECT
    Q_ENUMS(PlayState)
public:

    enum PlayState {
        NoState,
        Playing,
        Paused,
        Stopped
    };
    ~Player();
    static Player *instance();
    Player::PlayState currentState();
    QVector<QString> getAudioDevices();

public slots:
    void play(QString &filename);
    void play();
    void pause();
    void stop();
    void seekTo(double pos);
    void setVolume(int value);
    void changeAudioDevice(QString device);

    void cbk(pcm_stereo_sample *input_buffer);
private:
    WatchDog *watchDogThread;
    QString m_filename;
    PlayState m_state = NoState;
    static Player *_instance;
    explicit Player(QObject *parent = 0);
    int m_last_position = 0;
    void emitStateSignal();

signals:
    void playStateChange(Player::PlayState state);
    void positionChanged(int pos);
    void aboutToStop();
    void audioBufferReady(pcm_stereo_sample *input_buffer);

public slots:
    void watchDogAboutToStop();
};
Q_DECLARE_METATYPE(Player::PlayState)

#endif // PLAYER_H
