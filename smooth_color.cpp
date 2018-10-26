#include "smooth_color.h"

SmoothColoring::SmoothColoring(): SmoothColoring(3) {
}

SmoothColoring::SmoothColoring(uint32_t num_colors): 
    SmoothColoring(num_colors, 50) {
}

SmoothColoring::SmoothColoring(uint32_t num_colors, uint32_t num_gradient):
    n_colors(num_colors), n_gradient(num_gradient) {
    
    // Initialize base colors randomly.
    std::srand(std::time(nullptr));
    base_colors.resize(n_colors);
    for(uint32_t i=0; i<base_colors.size(); i++) {
        double r = (static_cast<double>(std::rand()) / RAND_MAX) * 255;
        double g = (static_cast<double>(std::rand()) / RAND_MAX) * 255;
        double b = (static_cast<double>(std::rand()) / RAND_MAX) * 255;
        base_colors.at(i) = QColor(r, g, b);
    }

    // Create gradient of base colors over a larger LUT.
    gradient_colors.resize(n_gradient);
    double dist = static_cast<double>(gradient_colors.size() - 1)
            / (base_colors.size() - 1);

    for(uint32_t i=0; i<gradient_colors.size(); i++) {
        QColor& curc = base_colors.at(i / dist);
        QColor& nexc = i / dist + 1 < base_colors.size() ?
                    base_colors.at((i / dist) + 1):
                    base_colors.at(i / dist);
        double distance = static_cast<double>(i) / (dist * (i / dist + 1));

        gradient_colors.at(i) = interpolateColor(curc, nexc, distance);
    }

    log_2 = std::log(2.0);
}

QColor SmoothColoring::interpolateColor(const QColor& c1, const QColor& c2, 
        double r) {
    double red = c1.redF() + static_cast<double>(c2.redF() - c1.redF()) * r;
    double green = c1.greenF() + static_cast<double>(c2.greenF() 
            - c1.greenF()) * r;
    double blue = c1.blueF() + static_cast<double>(c2.blueF() - c1.blueF()) * r;

    QColor ret;
    ret.setRgbF(red, green, blue);

    return ret;
}

double SmoothColoring::nu(int32_t it, double norm) {
    double log_zn = std::log(norm) / 2;
    double nu = std::log(log_zn / log_2) / log_2;
    return static_cast<double>(it) + 1 - nu;
}

QColor SmoothColoring::getColor(int32_t iterations, double normal) {
    if(iterations == INT32_MIN)
        return QColor(0, 0, 0);

    double itnorm = nu(iterations, normal);
    int32_t fcval = static_cast<int32_t>(std::floor(itnorm));

    return interpolateColor(gradient_colors.at(fcval
                                          % gradient_colors.size()),
                         gradient_colors.at((fcval + 1)
                                          % gradient_colors.size()),
                         itnorm - (long)itnorm);
}

