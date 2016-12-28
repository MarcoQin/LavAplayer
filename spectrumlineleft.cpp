#include "spectrumlineleft.h"
#include <QDebug>
#include <QPainter>

SpectrumLineLeft::SpectrumLineLeft(QWidget *parent) : QWidget(parent)
{
    this->setGeometry(26, 90, 339, 51);
}

void SpectrumLineLeft::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);

    QColor barColor(128, 127, 68);
//    const QColor gridColor = barColor.darker();
    QPen gridPen(barColor);
    painter.setPen(gridPen);

    int numBars = 100;

    const int widgetWidth = rect().width();
    const int barPlusGapWidth = widgetWidth / numBars;
    const int barWidth = 0.8 * barPlusGapWidth;
    const int gapWidth = barPlusGapWidth - barWidth;
    const int paddingWidth = widgetWidth - numBars * (barWidth + gapWidth);
    const int leftPaddingWidth = (paddingWidth + gapWidth) / 2;
    const int barHeight = rect().height() - 2 * gapWidth;

    int x_margin = -20;
    int originY = 0;
    int originX = 0 + x_margin;
    if (m_bars.size() > 0) {
        originY = m_bars[0];
    }
    QPoint origin(originX, originY);

    QLine line(origin, origin);
    // left
    for (auto i = 0u; i < m_bars.size(); ++i) {

        auto bar_height = 0.0;
        auto column_index = i;
//        column_index = static_cast<uint32_t>(m_bars.size()) - i - 1;

        bar_height = m_bars[column_index] / 2.0 + 10;


        bar_height = std::max(0.0, bar_height);

        int y = rect().top() + 13 + (rect().height() - bar_height);

        int x = x_margin + rect().left() + leftPaddingWidth + (i * (gapWidth + barWidth));
        line.setP2(QPoint(x, y));
        painter.drawLine(line);
        line.setP1(line.p2());
    }
}

void SpectrumLineLeft::onBarGenerated(std::vector<double> &bars)
{
    m_bars = bars;
    repaint();
}
