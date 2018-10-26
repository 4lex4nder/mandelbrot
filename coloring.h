#ifndef _COLORING_H
#define _COLORING_H

#include <QPainter>

class Coloring {
    private:

    public:
        Coloring();
        virtual QColor getColor(int32_t iterations, double normal) = 0;
};

#endif
