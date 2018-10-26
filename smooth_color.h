#ifndef _SMOOTH_COLOR_H
#define _SMOOTH_COLOR_H

#include <QPainter>
#include <cmath>
#include "coloring.h"

class SmoothColoring: public Coloring {
    private:
        std::vector<QColor> base_colors;
        std::vector<QColor> gradient_colors;
        uint32_t n_colors;
        uint32_t n_gradient;
        double log_2;

        QColor interpolateColor(const QColor& c1, 
                const QColor& c2, double r);
    public:
        SmoothColoring();
        SmoothColoring(uint32_t num_colors);
        SmoothColoring(uint32_t num_colors, uint32_t num_gradient);

        inline double nu(int32_t it, double norm);
        virtual QColor getColor(int32_t iterations, double normal);
};

#endif //_SMOOTH_COLOR_H
