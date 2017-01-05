#include "spectrograph.h"
#include "spectrumanalyser.h"

#include <QDebug>
#include <QPainter>

#include <algorithm>
#include <climits>
#include <cmath>
#include <iostream>
#include <numeric>
#include <thread>

namespace {
static const size_t k_secs_for_autoscaling = 30;
static const double k_autoscaling_reset_window_percent = 0.10;
static const double k_autoscaling_erase_percent_on_reset = 0.75;
static const double k_deviation_amount_to_reset =
    1.0; // amount of deviation needed between short term and long term moving
         // max height averages to trigger an auto scaling reset

static const double k_minimum_bar_height = 0.125;
}

Spectrograph::Spectrograph(QWidget *parent) : QWidget(parent)
{
    this->setGeometry(9, 530 + 4, 741, 121 - 8);
}


void Spectrograph::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);

    QColor barColor(128, 127, 68, 200);

    int numBars = m_bars_left.size() * 2;
    if (numBars < 10) {
        return;
    }

    const int widgetWidth = rect().width();
    const int barPlusGapWidth = widgetWidth / numBars;
    const int barWidth = 0.8 * barPlusGapWidth;
    const int gapWidth = barPlusGapWidth - barWidth;
    const int paddingWidth = widgetWidth - numBars * (barWidth + gapWidth);
    const int leftPaddingWidth = (paddingWidth + gapWidth) / 2;

    QGradient g;
    g = QLinearGradient(QPointF(rect().width()/2, 0), QPointF(rect().width() /2, rect().height()));
    g.setColorAt(0.8, barColor);
    g.setColorAt(0.3, barColor.darker());
    g.setSpread(QGradient::PadSpread);

    // left
    for (auto i = 0u; i < m_bars_left.size(); ++i) {

        auto bar_height = 0.0;
        auto column_index = i;
        column_index = static_cast<uint32_t>(m_bars_left.size()) - i - 1;

        bar_height = m_bars_falloff_left[column_index];


        bar_height = std::max(0.0, bar_height);

        QRect bar = rect();
        bar.setLeft(rect().left() + leftPaddingWidth + (i * (gapWidth + barWidth)));
        bar.setWidth(barWidth);
        bar.setTop(rect().top() + gapWidth + (rect().height() - bar_height));
        bar.setBottom(rect().bottom() - gapWidth);
        painter.fillRect(bar, g);
    }

    // right
    for (auto i = 0u; i < m_bars_right.size(); ++i) {

        auto bar_height = 0.0;
        auto column_index = i;

        bar_height = m_bars_falloff_right[column_index];


        bar_height = std::max(0.0, bar_height);

        QRect bar = rect();
        bar.setLeft(rect().left() + leftPaddingWidth + ((i + m_bars_left.size()) * (gapWidth + barWidth)));
        bar.setWidth(barWidth);
        bar.setTop(rect().top() + gapWidth + (rect().height() - bar_height));
        bar.setBottom(rect().bottom() - gapWidth);
        painter.fillRect(bar, g);
    }

}

void Spectrograph::spectrumChanged(fftw_complex *output_left, fftw_complex *output_right, int results)
{
//    qDebug() << "spectrumChanged!!";
    create_spectrum_bars(output_left, results, rect().height() - 10, rect().width(),
                         120, m_bars_left, m_bars_falloff_left);
    create_spectrum_bars(output_right, results, rect().height() - 10, rect().width(),
                         120, m_bars_right, m_bars_falloff_right);
    emit barsGeneratedLeft(m_bars_falloff_left);
    emit barsGeneratedRight(m_bars_falloff_right);
    repaint();
}

void Spectrograph::cleanGraph()
{
    for (auto i = 0; i < m_bars_falloff_left.size(); ++i) {
        m_bars_falloff_left[i] = 0;
        m_bars_falloff_right[i] = 0;
    }
    emit barsGeneratedLeft(m_bars_falloff_left);
    emit barsGeneratedRight(m_bars_falloff_right);
    repaint();
}

void Spectrograph::create_spectrum_bars(fftw_complex *fftw_output, const size_t fftw_results,
                                        const int32_t win_height, const int32_t win_width,
                                        const uint32_t number_of_bars, std::vector<double> &bars,
                                        std::vector<double> &bars_falloff)
{
    if (m_previous_win_width != win_width) {
        recaculate_cutoff_frequencies(number_of_bars, &m_low_cutoff_frequencies,
                                      &m_high_cutoff_frequencies, &m_frequency_constants_per_bin);
        m_previous_win_width = win_width;
    }

    // Separate the frequency spectrum into bars, the number of bars is based on screen width
    generate_bars(bars, number_of_bars, fftw_output, fftw_results,
                  m_low_cutoff_frequencies, m_high_cutoff_frequencies);

    smooth_bars(bars);

    scale_bars(bars, win_height);

    // falloff, save values for next falloff run
    bars_falloff = apply_falloff(bars, bars_falloff);

}

void Spectrograph::recaculate_cutoff_frequencies(uint32_t number_of_bars,
                                                 QVector<uint32_t> *low_cutoff_frequencies,
                                                 QVector<uint32_t> *high_cutoff_frequencies,
                                                 std::vector<double> *freqconst_per_bin)
{
    auto freqconst = std::log10(static_cast<double>(default_low_cutoff_frequency) / static_cast<double>(default_high_cutoff_frequency)) /
                ((1.0 / (static_cast<double>(number_of_bars) + 1.0)) - 1.0);
    (*low_cutoff_frequencies) = QVector<uint32_t>(number_of_bars + 1);
    (*high_cutoff_frequencies) = QVector<uint32_t>(number_of_bars + 1);
    (*freqconst_per_bin) = std::vector<double>(number_of_bars + 1);

    for (auto i = 0u; i < number_of_bars; ++i) {
        (*freqconst_per_bin)[i] = static_cast<double>(default_high_cutoff_frequency) *
                std::pow(10.0, (freqconst * -1) + (((i + 1.0) / (number_of_bars + 1.0)) * freqconst));

        auto frequency = (*freqconst_per_bin)[i] / (default_sampling_frequency / 2.0);

        (*low_cutoff_frequencies)[i] = static_cast<uint32_t>(std::floor(frequency * (static_cast<double>(sample_buffer_size) / 4.0)));

        if (i > 0) {
            if ((*low_cutoff_frequencies)[i] <= (*low_cutoff_frequencies)[i - 1]) {
                (*low_cutoff_frequencies)[i] = (*low_cutoff_frequencies)[i - 1] + 1;
            }
            (*high_cutoff_frequencies)[i - 1] = (*low_cutoff_frequencies)[i - 1];
        }
    }
}

void Spectrograph::generate_bars(std::vector<double> &bars, const uint32_t number_of_bars,
                                 const fftw_complex *fftw_output, const size_t fftw_results,
                                 const QVector<uint32_t> &low_cutoff_frequencies,
                                 const QVector<uint32_t> &high_cutoff_frequencies) const
{
    if (bars.size() != number_of_bars) {
        bars.resize(number_of_bars);
    }

    for (auto i = 0u; i < number_of_bars; ++i) {
        double freq_magnitude = 0.0;
        for (auto cutoff_freq = low_cutoff_frequencies[i];
             cutoff_freq <= high_cutoff_frequencies[i] && cutoff_freq < fftw_results;
             ++cutoff_freq) {
            freq_magnitude += std::sqrt ((fftw_output[cutoff_freq][0] * fftw_output[cutoff_freq][0]) +
                                         (fftw_output[cutoff_freq][1] * fftw_output[cutoff_freq][1]));
        }

        bars[i] = freq_magnitude / (high_cutoff_frequencies[i] - low_cutoff_frequencies[i] + 1);

        // boost high frequencies
        bars[i] *= (std::log2(2 + i) * (100.0 / number_of_bars));
        bars[i] = std::pow(bars[i], 0.5);
    }
}

void Spectrograph::smooth_bars(std::vector<double> &bars)
{
    const char * mode = "none";
    if (mode == "none") {
        return;
    } else if (mode == "mostercat") {

    } else if (mode == "sgs") {
        sgs_smoothing(bars);
    }
}

void Spectrograph::sgs_smoothing(std::vector<double> &bars)
{
    auto original_bars = bars;

    auto smoothing_passes = default_sgs_smoothing_passes;
    auto smoothing_points = default_sgs_smoothing_points;

    for (auto pass = 0u; pass < smoothing_passes; ++pass) {
        auto pivot = static_cast<uint32_t>(std::floor(smoothing_points / 2.0));

        for (auto i = 0u; i < pivot; ++i) {
            bars[i] = original_bars[i];
            bars[original_bars.size() - i - 1] = original_bars[original_bars.size() - i - 1];
        }

        auto smoothing_constant = 1.0 / (2.0 * pivot + 1.0);
        for (auto i = pivot; i < (original_bars.size() - pivot); ++i) {
            auto sum = 0.0;
            for (auto j = 0u; j <= (2 * pivot + 1.0); ++j) {
                sum += (smoothing_constant * original_bars[i + j - pivot]) + j - pivot;
            }
            bars[i] = sum;
        }

        // prepare for next pass
        if (pass < (smoothing_passes - 1)) {
            original_bars = bars;
        }
    }
}

void Spectrograph::scale_bars(std::vector<double> &bars, const int32_t height)
{
    if (bars.empty()) {
        return;
    }

    const auto max_height_iter = std::max_element(bars.begin(), bars.end());

    // max number of elements to caculate for moving average
    const auto max_number_of_elements = static_cast<size_t>(
                ((k_secs_for_autoscaling * default_sampling_frequency) /
                 (static_cast<double>(sample_buffer_size))) *
                2.0);

    double std_dev = 0.0;
    double moving_average = 0.0;
    calculate_moving_average_and_std_dev(*max_height_iter, max_number_of_elements,
                                        m_previous_max_heights, &moving_average, &std_dev);

    maybe_reset_scaling_window(*max_height_iter, max_number_of_elements,
                                   &m_previous_max_heights, &moving_average,
                                   &std_dev);

    auto max_height = moving_average + (2 * std_dev);

    for (auto i = 0u; i < bars.size(); ++i)
    {
        bars[i] = std::min(static_cast<double>(height - 1),
                           ((bars[i] / max_height) * height) - 1);
    }

}

void Spectrograph::calculate_moving_average_and_std_dev(const double new_value,
                                                       const size_t max_number_of_elements,
                                                       std::vector<double> &old_values,
                                                       double *moving_average, double *std_dev) const
{
    if (old_values.size() > max_number_of_elements) {
        old_values.erase(old_values.begin());
    }

    old_values.push_back(new_value);

    auto sum = std::accumulate(old_values.begin(), old_values.end(), 0.0);
    *moving_average = sum / old_values.size();

    auto squared_sumation = std::inner_product(old_values.begin(), old_values.end(), old_values.begin(), 0.0);
    *std_dev = std::sqrt((squared_sumation / old_values.size()) - std::pow(*moving_average, 2));
}

void Spectrograph::maybe_reset_scaling_window(const double current_max_height,
                                              const size_t max_number_of_elements,
                                              std::vector<double> *values,
                                              double *moving_average, double *std_dev)
{
    const auto reset_window_size =
        (k_autoscaling_reset_window_percent * max_number_of_elements);
    // Current max height is much larger than moving average, so throw away most
    // values re-calculate
    if (static_cast<double>(values->size()) > reset_window_size)
    {
        // get average over scaling window
        auto average_over_reset_window =
            std::accumulate(values->begin(),
                            values->begin() +
                                static_cast<int64_t>(reset_window_size),
                            0.0) /
            reset_window_size;

        // if short term average very different from long term moving average,
        // reset window and re-calculate
        if (std::abs(average_over_reset_window - *moving_average) >
            (k_deviation_amount_to_reset * (*std_dev)))
        {
            values->erase(values->begin(),
                          values->begin() +
                              static_cast<int64_t>(
                                  (static_cast<double>(values->size()) *
                                   k_autoscaling_erase_percent_on_reset)));

            calculate_moving_average_and_std_dev(
                current_max_height, max_number_of_elements, *values,
                moving_average, std_dev);
        }
    }
}

std::vector<double> Spectrograph::apply_falloff(const std::vector<double> &bars, std::vector<double> &falloff_bars) const
{
    // Screen size has change which means previous falloff values are not valid
    if (falloff_bars.size() != bars.size())
    {
        return bars;
    }

    for (auto i = 0u; i < bars.size(); ++i)
    {
        // falloff should always by at least one
        auto falloff_value = std::min(
            falloff_bars[i] * default_spectrum_falloff_weight,
            falloff_bars[i] - 1);

        falloff_bars[i] = std::max(falloff_value, bars[i]);
    }
    return falloff_bars;
}
