#include "settings.h"
#include "ui_settings.h"
#include <QDebug>

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    connect(ui->deviceBox, SIGNAL(activated(int)), this, SLOT(boxActivate(int)));
}

Settings::~Settings()
{
    delete ui;
}

QString Settings::getAudioDevice()
{
    return m_audioDevice;
}

void Settings::showEvent(QShowEvent *)
{
    int i = 0;
    int index = 0;
    ui->deviceBox->clear();
    foreach (QString device, m_devices) {
        ui->deviceBox->addItem(device, device);
        if (device == m_audioDevice) {
            index = i;
        }
        i++;
    }
    ui->deviceBox->setCurrentIndex(index);
}

void Settings::boxActivate(int index)
{
    if (ui->deviceBox->count() == 0)
        return;

    QString device = ui->deviceBox->itemData(index).value<QString>();
    qDebug() << device;
    if (device != m_audioDevice) {
        m_audioDevice = device;
        emit deviceChanged(device);
    }
}

void Settings::setAudioDevices(QVector<QString> devices)
{
    m_devices = devices;
}
