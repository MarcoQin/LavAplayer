#include "spectrumanalyser.h"


SpectrumAnalyser::SpectrumAnalyser(QObject *parent) :
    QObject(parent), m_fftw_results(0),
    m_fftw_input_left(nullptr), m_fftw_input_right(nullptr),
    m_fftw_output_left(nullptr), m_fftw_output_right(nullptr),
    m_fftw_plan_left(nullptr), m_fftw_plan_right(nullptr)
{
    m_fftw_results = (static_cast<size_t>(sample_buffer_size) / 2) + 1;

    m_fftw_input_left = static_cast<double *>(fftw_malloc(sizeof(double) * sample_buffer_size));
    m_fftw_input_right = static_cast<double *>(fftw_malloc(sizeof(double) * sample_buffer_size));

    m_fftw_output_left = static_cast<fftw_complex *>(fftw_malloc(sizeof(fftw_complex) * m_fftw_results));
    m_fftw_output_right = static_cast<fftw_complex *>(fftw_malloc(sizeof(fftw_complex) * m_fftw_results));
}

bool SpectrumAnalyser::prepare_fft_input(pcm_stereo_sample *buffer,
                                         uint32_t sample_size,
                                         double *fftw_input,
                                         ChannelMode channel_mode)
{
    bool is_silent = true;

    for (auto i = 0u; i < sample_size; ++i)
    {
        switch (channel_mode) {
        case ChannelMode::Left:
            fftw_input[i] = buffer[i].l;
            break;
        case ChannelMode::Right:
            fftw_input[i] = buffer[i].r;
            break;
        case ChannelMode::Both:
            fftw_input[i] = buffer[i].l + buffer[i].r;
            break;
        default:
            break;
        }

        if (is_silent && fftw_input[i] > 0)
        {
            is_silent = false;
        }
    }

    return is_silent;
}

void SpectrumAnalyser::execute(pcm_stereo_sample *buffer, const bool is_stereo)
{
    if (is_stereo) {
        prepare_fft_input(buffer, sample_buffer_size, m_fftw_input_left, ChannelMode::Left);
        prepare_fft_input(buffer, sample_buffer_size, m_fftw_input_right, ChannelMode::Right);
    } else {
        prepare_fft_input(buffer, sample_buffer_size, m_fftw_input_left, ChannelMode::Both);
    }

    m_fftw_plan_left = fftw_plan_dft_r2c_1d(static_cast<int>(sample_buffer_size), m_fftw_input_left,
                                            m_fftw_output_left, FFTW_ESTIMATE);
    if (is_stereo) {
        m_fftw_plan_right = fftw_plan_dft_r2c_1d(static_cast<int>(sample_buffer_size), m_fftw_input_right,
                                                 m_fftw_output_right, FFTW_ESTIMATE);
    }

    fftw_execute(m_fftw_plan_left);
    if (is_stereo) {
        fftw_execute(m_fftw_plan_right);
    }

    emit spectrumAnalysed(m_fftw_output_left, m_fftw_output_right, static_cast<int>(m_fftw_results));

    fftw_destroy_plan(m_fftw_plan_left);

    if (is_stereo) {
        fftw_destroy_plan(m_fftw_plan_right);
    }
}

void SpectrumAnalyser::execute_mono(pcm_stereo_sample *buffer)
{
    execute(buffer, false);
}

void SpectrumAnalyser::execute_stereo(pcm_stereo_sample *buffer)
{
    execute(buffer, true);
}
