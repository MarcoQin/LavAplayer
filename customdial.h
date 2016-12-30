#ifndef CUSTOMDIAL_H
#define CUSTOMDIAL_H

class QColor;
class QRectF;
class QPen;
class QImage;
#include <QDial>

class CustomDial : public QDial
{
    Q_OBJECT

public:

    explicit CustomDial(QWidget* parent = nullptr);

    CustomDial(QWidget * parent = nullptr,
               int minimum = 0,
               int maximum = 999);

private slots:

private:

    QImage *bg;
    QImage *dot;
    QImage *dot1;
    QImage *mute0;

    virtual void paintEvent(QPaintEvent*) override;

    virtual void resizeEvent(QResizeEvent* event) override;

    double arcWidth_ = 3;

    QSharedPointer<QRectF> arcRect_;
};

#endif // CUSTOMDIAL_H
