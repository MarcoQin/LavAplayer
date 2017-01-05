#include "player.h"

#include "LAVA/lava.h"

class LavaCBK : public LAVA::AudioCallbackInject
{
public:
    LavaCBK(Player *player);
    virtual void update(uint8_t *stream, int len);
private:
    Player *inst = nullptr;
    pcm_stereo_sample *buf;
};

LavaCBK::LavaCBK(Player *player)
{
    inst = player;
    buf = static_cast<pcm_stereo_sample *>(calloc(sample_buffer_size, sizeof(pcm_stereo_sample)));
}

void LavaCBK::update(uint8_t *stream, int len)
{
    memset(buf, 0, sample_buffer_size);
    memcpy(buf, (int16_t *)stream,  len);
    if (inst) {
        inst->cbk(buf);
    }
}


Player *Player::_instance = nullptr;

Player *Player::instance()
{
    return _instance ? _instance : new Player();
}

Player::Player(QObject *parent) : QObject(parent)
{
    LavaCBK *cbk = new LavaCBK(this);
    LAVA::Core::instance()->setAudioCallbackInject(static_cast<LAVA::AudioCallbackInject*>(cbk));
//    qRegisterMetaType<PlayState>("Player::PlayState");
//    qRegisterMetaType<PlayState>("PlayState");
//    qRegisterMetaType<Player::PlayState>("Player::PlayState");
//    qRegisterMetaType<Player::PlayState>("PlayState");
}

void Player::cbk(pcm_stereo_sample *input_buffer)
{
    int cur_pos = static_cast<int>(LAVA::Core::instance()->time_position());
    if (cur_pos != m_last_position) {
        m_last_position = cur_pos;
        emit positionChanged(cur_pos);
    }
    emit audioBufferReady(input_buffer);
    if (LAVA::Core::instance()->is_stopping()) {
        emit aboutToStop();
        m_state = Stopped;
        emitStateSignal();
    }
}

void Player::play(QString &filename)
{
    m_filename = filename;
    LAVA::Core::instance()->load_file(filename.toStdString().c_str());
    m_state = Playing;
    emitStateSignal();
}

void Player::play()
{
    if (m_state == Paused) {
        LAVA::Core::instance()->pause();
        m_state = Playing;
        emitStateSignal();
    } else if (m_state == Stopped && !m_filename.isNull() && !m_filename.isEmpty()) {
        play(m_filename);
    }
}

void Player::pause()
{
    if (m_state == Playing) {
        LAVA::Core::instance()->pause();
        m_state = Paused;
        emitStateSignal();
    }
}

void Player::stop()
{
    LAVA::Core::instance()->stop();
    m_state = Stopped;
    emitStateSignal();
}

void Player::emitStateSignal()
{
    emit playStateChange(m_state);
}

Player::PlayState Player::currentState()
{
    return m_state;
}

void Player::seekTo(double pos)
{
    if (m_state == Playing || m_state == Paused) {
        LAVA::Core::instance()->seek_by_absolute_pos(pos);
    }
}

void Player::setVolume(int value)
{
    LAVA::Core::instance()->set_volume(value);
}
