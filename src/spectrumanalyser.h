#ifndef SPECTRUMANALYSER_H
#define SPECTRUMANALYSER_H

#include <QObject>
#include <fftw3.h>

struct stereo_sample_frame
{
    int16_t l;
    int16_t r;
};

static const int sample_buffer_size = 44100 / 20;

using pcm_stereo_sample = struct stereo_sample_frame;

class SpectrumAnalyser : public QObject
{
    Q_OBJECT
public:
    explicit SpectrumAnalyser(QObject *parent = 0);

    void execute_mono(pcm_stereo_sample *buffer);
    void execute_stereo(pcm_stereo_sample *buffer);

    enum class ChannelMode
    {
        Left = 0,
        Right,
        Both
    };
signals:
    void spectrumAnalysed(fftw_complex *output_left, fftw_complex *output_right, int results);

private:
    void execute(pcm_stereo_sample *buffer, const bool is_stereo);

    size_t m_fftw_results;

    double *m_fftw_input_left;
    double *m_fftw_input_right;

    fftw_complex *m_fftw_output_left;
    fftw_complex *m_fftw_output_right;

    fftw_plan m_fftw_plan_left;
    fftw_plan m_fftw_plan_right;


    bool prepare_fft_input(pcm_stereo_sample *buffer, uint32_t sample_size,
                           double *fftw_input, ChannelMode channel_mode);


signals:

public slots:
};

#endif // SPECTRUMANALYSER_H
