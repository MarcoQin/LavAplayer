#ifndef SPECTRUMLINELEFT_H
#define SPECTRUMLINELEFT_H

#include <QWidget>

class SpectrumLineLeft : public QWidget
{
    Q_OBJECT
public:
    explicit SpectrumLineLeft(QWidget *parent = 0);

    void paintEvent(QPaintEvent *event);

signals:

public slots:
    void onBarGenerated(std::vector<double> &bars);
private:
    std::vector<double> m_bars;
};

#endif // SPECTRUMLINELEFT_H
