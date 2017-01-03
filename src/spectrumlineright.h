#ifndef SPECTRUMLINERIGHT_H
#define SPECTRUMLINERIGHT_H

#include <QWidget>

class SpectrumLineRight : public QWidget
{
    Q_OBJECT
public:
    explicit SpectrumLineRight(QWidget *parent = 0);

    void paintEvent(QPaintEvent *event);

signals:

public slots:
    void onBarGenerated(std::vector<double> &bars);
private:
    std::vector<double> m_bars;
};

#endif // SPECTRUMLINERIGHT_H
