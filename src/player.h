#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include "spectrumanalyser.h"

class Player : public QObject
{
    Q_OBJECT
public:
    enum PlayState {
        NoState = 0,
        Playing,
        Paused,
        Stopped
    };

    static Player *instance();
    void play(QString &filename);
    void play();
    void pause();
    void stop();
    Player::PlayState currentState();

    void cbk(pcm_stereo_sample *input_buffer);
private:
    QString m_filename;
    PlayState m_state = NoState;
    static Player *_instance;
    explicit Player(QObject *parent = 0);
    int m_last_position = 0;
    void emitStateSignal();

signals:
    void playStateChange(PlayState &m_state);
    void positionChanged(int pos);
    void aboutToStop();
    void audioBufferReady(pcm_stereo_sample *input_buffer);

public slots:
};

#endif // PLAYER_H
