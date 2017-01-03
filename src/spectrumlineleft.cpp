#include "spectrumlineleft.h"
#include <QDebug>
#include <QPainter>
#include <cmath>

SpectrumLineLeft::SpectrumLineLeft(QWidget *parent) : QWidget(parent)
{
    this->setGeometry(26, 90, 339, 51);
}

void SpectrumLineLeft::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    double sum = 0.0;

    for (auto i = 0u; i < m_bars.size(); ++i) {
        auto value = m_bars[i] / 40.0;
        sum += value * value;
    }

    double rmsLevel = std::sqrt(sum / m_bars.size());

    QPainter painter(this);
    painter.setPen(QColor(210, 220, 230, 200).darker());
    painter.drawText(QRectF(rect().width() - 14, 2, 30, 30), "L");

    QColor barColor(128, 127, 68, 200);
    QColor gridColor(155, 155, 163, 180);

    painter.setPen(gridColor.darker());
    double delta_y = 0.5;
    double y0 = rect().height() / 3.0;
    double y1 = y0 * 2;
    y0 -= delta_y;
    y1 += delta_y;
    double x0 = rect().width() - 20;

    QLineF gridLine(QPointF(0, y0), QPointF(x0, y0));
    painter.drawLine(gridLine);
    gridLine.setP1(QPointF(0, y1));
    gridLine.setP2(QPointF(x0, y1));
    painter.drawLine(gridLine);


    painter.setPen(barColor);

    int numBars = m_bars.size();
    if (numBars < 10) {
        return;
    }

    numBars = 110;

    const int widgetWidth = rect().width();
    const int barPlusGapWidth = widgetWidth / numBars;
    const int barWidth = 0.8 * barPlusGapWidth;
    const int gapWidth = barPlusGapWidth - barWidth;
    const int paddingWidth = widgetWidth - numBars * (barWidth + gapWidth);
    const int leftPaddingWidth = (paddingWidth + gapWidth) / 2;

    int x_margin = -20 * 2 - 5;
    int originY = 0;
    int originX = 0;
    QPoint origin(originX, originY);

    QLine line(origin, origin);
    // left
    for (auto i = 0u; i < m_bars.size(); ++i) {

        auto bar_height = 0.0;
        auto column_index = i;

        bar_height = m_bars[column_index] / 2.0 + 10;


        bar_height = std::max(0.0, bar_height);

        int y = rect().top() + 13 + (rect().height() - bar_height);

        int x = x_margin + rect().left() + leftPaddingWidth + (i * (gapWidth + barWidth));
        if (i == 0) {
            line.setP1(QPoint(x, y));
        }
        line.setP2(QPoint(x, y));
        painter.drawLine(line);
        line.setP1(line.p2());
    }

    QGradient g;
    g = QLinearGradient(QPointF(rect().width()/2, 0), QPointF(rect().width() /2, rect().height()));
    int alpha = 100;
    g.setColorAt(1.0, QColor(115, 16, 7, alpha));
    g.setColorAt(0.9, QColor(131,42, 11, alpha));
    g.setColorAt(0.7, QColor(147, 70, 16, alpha));
    g.setColorAt(0.5, QColor(180, 126, 27, alpha));
    g.setSpread(QGradient::PadSpread);


    QRect lvBar1(rect().width() - 16, ((rect().height() - 10) *(1- rmsLevel) + 10), 12, (rect().height()) * rmsLevel);
    painter.fillRect(lvBar1, g);


}

void SpectrumLineLeft::onBarGenerated(std::vector<double> &bars)
{
    m_bars = bars;
    repaint();
}
