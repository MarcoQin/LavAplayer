#ifndef SPECTRUMANALYSER_H
#define SPECTRUMANALYSER_H

#include <QObject>

class SpectrumAnalyser : public QObject
{
    Q_OBJECT
public:
    explicit SpectrumAnalyser(QObject *parent = 0);

signals:

public slots:
};

#endif // SPECTRUMANALYSER_H