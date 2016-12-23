#ifndef CUSTOMDIAL_H
#define CUSTOMDIAL_H

class QColor;
class QRectF;
class QPen;
#include <QDial>

class CustomDial : public QDial
{
    Q_OBJECT

    Q_PROPERTY(QString arcColor READ getArcColor WRITE setArcColor)

    Q_PROPERTY(double arcWidth READ getArcWidth WRITE setArcWidth)

public:

    explicit CustomDial(QWidget* parent = nullptr);

    CustomDial(const QString& text,
               QWidget * parent = nullptr,
               int minimum = 0,
               int maximum = 999);


    void setArcColor(const QString& color);

    QString getArcColor() const;


    void setStartAngle(double angle);

    double getStartAngle() const;


    void setMaximumAngle(double angle);

    double getMaximumAngle() const;


    void setArcWidth(double px);

    double getArcWidth() const;


    void setText(const QString& text);

    QString getText() const;


private slots:

    void updateValue();

private:

    virtual void paintEvent(QPaintEvent*) override;

    virtual void resizeEvent(QResizeEvent* event) override;

    double maximumAngleSpan_;

    double startAngle_;

    double arcWidth_;

    double angleSpan_;

    QString valueString_;

    QString text_;

    QSharedPointer<QRectF> arcRect_;

    QSharedPointer<QRectF> valueRect_;

    QSharedPointer<QRectF> textRect_;

    QSharedPointer<QColor> arcColor_;

    QSharedPointer<QPen> arcPen_;
};

#endif // CUSTOMDIAL_H
