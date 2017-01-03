#include "customdial.h"

#include <QPainter>
#include <QColor>
#include <QLabel>
#include <QRectf>
#include <QPen>
#include <QResizeEvent>
#include <QImage>

CustomDial::CustomDial(QWidget* parent)
: QDial(parent)
{ }

CustomDial::CustomDial(QWidget* parent,
                       int minimum,
                       int maximum)
: QDial(parent),
  arcRect_(new QRectF)
{

    bg = new QImage(":/pic/images/_knob.png");
//    bg = new QImage(":/pic/images/__knob2.png");

    dot = new QImage(":/pic/images/voldot4g.png");
    dot1 = new QImage(":/pic/images/voldot4s.png");

    mute0 = new QImage(":/pic/images/_mute0.png");

    QDial::setRange(minimum, maximum);

//    QDial::setCursor(Qt::PointingHandCursor);


}

void CustomDial::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    // So that we can use the background color
    // Otherwise the background is transparent
    painter.setBackgroundMode(Qt::OpaqueMode);

    // Smooth out the circle
    painter.setRenderHint(QPainter::Antialiasing);

    painter.drawImage(*arcRect_, *bg);
    painter.translate(arcRect_->center());
    painter.drawImage(-mute0->width() / 2, -mute0->height()/2, *mute0);


    double value = QDial::value();

    // Get ratio between current value and maximum to calculate angle
    double ratio = value / QDial::maximum();
    ratio *= 270;
    if (ratio <= 0) {
        ratio += 5;
    } else {
        ratio += 7;
    }
    painter.rotate(ratio);
    int base = 26;
    painter.drawImage(-base + 1, base - 1, *dot1);
    painter.drawImage(-base, base, *dot);

}

void CustomDial::resizeEvent(QResizeEvent* event)
{
    QDial::setMinimumSize(event->size());

    *arcRect_ = QRectF(arcWidth_ / 2,
                       arcWidth_ / 2,
                       QDial::width() - arcWidth_,
                       QDial::height() - arcWidth_);
}

