#ifndef SPECTROGRAPH_H
#define SPECTROGRAPH_H

#include <QWidget>
#include <fftw3.h>

//static const uint32_t default_low_cutoff_frequency = 100;
static const uint32_t default_low_cutoff_frequency = 1000;
static const uint32_t default_high_cutoff_frequency = 22050;
//static const uint32_t default_high_cutoff_frequency = 44100;
static const uint32_t default_sampling_frequency = 44100;
static const uint32_t default_sgs_smoothing_passes = 2;
static const uint32_t default_sgs_smoothing_points = 3;
static const double default_spectrum_falloff_weight = 0.85;

class Spectrograph : public QWidget
{
    Q_OBJECT
public:
    explicit Spectrograph(QWidget *parent = 0);

    void paintEvent(QPaintEvent *event);

signals:
    void barsGeneratedLeft(std::vector<double> &bars);
    void barsGeneratedRight(std::vector<double> &bars);

public slots:
    void spectrumChanged(fftw_complex *output_left, fftw_complex *output_right, int results);
    void cleanGraph();
private:
    std::vector<double> m_bars_left;
    std::vector<double> m_bars_right;

    std::vector<double> m_bars_falloff_left;
    std::vector<double> m_bars_falloff_right;

    QVector<uint32_t> m_low_cutoff_frequencies;
    QVector<uint32_t> m_high_cutoff_frequencies;
    std::vector<double> m_frequency_constants_per_bin;

    std::vector<double> m_previous_max_heights;



    int32_t m_previous_win_width; // Used to determine if freq cutoffs need to be re-caculated

    void create_spectrum_bars(fftw_complex *fftw_output, const size_t fftw_results,
                              const int32_t win_height, const int32_t win_width,
                              const uint32_t number_of_bars, std::vector<double> &bars,
                              std::vector<double> &bars_falloff);

    void recaculate_cutoff_frequencies(uint32_t number_of_bars, QVector<uint32_t> *low_cutoff_frequencies,
                                       QVector<uint32_t> *high_cutoff_frequencies,
                                       std::vector<double> *freqconst_per_bin);

    void generate_bars(std::vector<double> &bars, const uint32_t number_of_bars,
                       const fftw_complex *fftw_output, const size_t fftw_results,
                       const QVector<uint32_t> &low_cutoff_frequencies,
                       const QVector<uint32_t> &high_cutoff_frequencies) const;

    void smooth_bars(std::vector<double> &bars);
    void sgs_smoothing(std::vector<double> &bars);
    void scale_bars(std::vector<double> &bars, const int32_t height);
    void calculate_moving_average_and_std_dev(const double new_value, const size_t max_number_of_elements,
                                             std::vector<double> &old_values, double *moving_average,
                                             double *std_dev) const;

    void maybe_reset_scaling_window(const double current_max_height, const size_t max_number_of_elements,
                                    std::vector<double> *values, double *moving_average, double *std_dev);

    std::vector<double> apply_falloff(const std::vector<double> &bars, std::vector<double> &falloff_bars) const;
};

#endif // SPECTROGRAPH_H
