#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <cstdint>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <math.h>
#include <iostream>
#include <immintrin.h>
#include <utility>
#include <QPainter>
#include "complex.h"
#include "smooth_color.h"

using timer = std::chrono::high_resolution_clock;

struct m_dimension {
    double m_offset_x;
    double m_offset_y;
    double m_width;
    double m_height;
};

struct mcalc_result_avx {
    double abs1;
    double abs2;
    int32_t it1;
    int32_t it2;
};

class Mandelbrot
{
private:
    std::unique_ptr<Coloring> coloring;
    uint32_t max_iter;
    m_dimension dimensions;

public:
    const uint32_t BAIL_OUT = 32;

    Mandelbrot();

    void refreshMandelbrotTiled(std::vector<QImage>& tiles);
    mcalc_result_avx calcMandelbrotTiled_avx(double real1,
            double imag1, double real2, double imag2) const;
    void calcMandelbrotWorkerTiled_avx(uint32_t hstart, uint32_t hend, 
            uint32_t width, uint32_t height, QImage& time);

    mcalc_result_avx calcMandelbrot_avx(double real1,
                                        double imag1,
                                        double real2,
                                        double imag2) const;

    void calcMandelbrotWorkerTiled(uint32_t hstart, uint32_t hend, 
            uint32_t width, uint32_t height, QImage& buf);

    std::pair<double, int32_t> calcMandelbrot(const complex& c) const;

    void updateComplexDimensions(const m_dimension& d);
};


#endif // MANDELBROT_H
