#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QString>
#include <QVector>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT
signals:
    void deviceChanged(QString device);
public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();
    QString getAudioDevice();
    void setAudioDevices(QVector<QString> devices);
protected:
    void showEvent(QShowEvent *);
private:
    Ui::Settings *ui;
    QString m_audioDevice;
    QVector<QString> m_devices;
private slots:
    void boxActivate(int index);
};

#endif // SETTINGS_H
