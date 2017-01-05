#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include "spectrumanalyser.h"

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
    static Player *instance();
    Player::PlayState currentState();

public slots:
    void play(QString &filename);
    void play();
    void pause();
    void stop();
    void seekTo(double pos);
    void setVolume(int value);

    void cbk(pcm_stereo_sample *input_buffer);
private:
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
};
Q_DECLARE_METATYPE(Player::PlayState)

#endif // PLAYER_H
