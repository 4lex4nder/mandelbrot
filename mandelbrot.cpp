#include "mandelbrot.h"

Mandelbrot::Mandelbrot() {
    max_iter = 100;

    dimensions.m_height = 2;
    dimensions.m_offset_y = 1;
    dimensions.m_width = 3;
    dimensions.m_offset_x = -2;

    coloring = std::unique_ptr<Coloring>(new SmoothColoring(4, 50));
}

void Mandelbrot::refreshMandelbrotTiled(std::vector<QImage>& tiles) {
    if(tiles.size() == 0)
        return;

    auto start = timer::now();

    dim_mutex.lock();

    // Determine global width and height (sum of all tiles)
    uint32_t width = tiles.at(0).width();
    uint32_t height = 0;

    for(uint32_t i=0; i<tiles.size(); i++) {
        height += tiles.at(i).height();
    }

    // Run each tile in a separate thread
    std::vector<std::thread> workers;
    uint32_t cur_hstart = 0;
    uint32_t cur_hend = tiles.at(0).height();
    for(uint32_t i=0; i<tiles.size(); i++) {
        if(i > 0) {
            cur_hstart = cur_hend;
            cur_hend = cur_hstart + tiles.at(i).height();
        }

        if(__builtin_cpu_supports("avx") || __builtin_cpu_supports("avx2"))
            workers.push_back(std::thread(
                        &Mandelbrot::calcMandelbrotWorkerTiled_avx, 
                        this, cur_hstart, cur_hend, width, height, 
                        std::ref(tiles.at(i))));
        else
            workers.push_back(std::thread(
                        &Mandelbrot::calcMandelbrotWorkerTiled, 
                        this, cur_hstart, cur_hend, width, height, 
                        std::ref(tiles.at(i))));
    }

    for(auto& t: workers)
        t.join();

    dim_mutex.unlock();

    auto end = timer::now();

    std::chrono::duration<double> diff = end - start;
    std::cout << "mandelbrot calculation time: " << diff.count() << std::endl;
}

void Mandelbrot::calcMandelbrotWorkerTiled_avx(uint32_t hstart, uint32_t hend, 
        uint32_t width, uint32_t height, QImage& buf) {

    QPainter p(&buf);
    for(uint32_t y = hstart; y < hend; y++) {
        double imag = dimensions.m_offset_y
                - (static_cast<double>(y) / (height - 1))
                * dimensions.m_height;

        for(uint32_t x = 0; x < width; x += 2) {
            double real1 = dimensions.m_offset_x
                    + (static_cast<double>(x) / (width - 1))
                    * dimensions.m_width;
            double real2 = dimensions.m_offset_x
                    + (static_cast<double>(x + 1) / (width - 1))
                    * dimensions.m_width;

            auto mb = calcMandelbrot_avx(imag, real1, imag, real2);
            uint32_t yy = (y - hstart);
            p.setPen(coloring->getColor(mb.it1, mb.abs1));
            p.drawPoint(QPoint(x, yy));

            if(x + 1 < width) {
                p.setPen(coloring->getColor(mb.it2, mb.abs2));
                p.drawPoint(QPoint(x + 1, yy));
            }
        }
    }
}

void Mandelbrot::calcMandelbrotWorkerTiled(uint32_t hstart, uint32_t hend, 
            uint32_t width, uint32_t height, QImage& buf) {

    QPainter p(&buf);
    for(uint32_t y = hstart; y < hend; y++) {
        double imag = dimensions.m_offset_y
                - (static_cast<double>(y) / (height - 1))
                * dimensions.m_height;

        for(uint32_t x = 0; x < width; x++) {
            double real = dimensions.m_offset_x
                    + (static_cast<double>(x) / (width - 1))
                    * dimensions.m_width;

            auto mb = calcMandelbrot(complex(real, imag));
            uint32_t yy = (y - hstart);
            p.setPen(coloring->getColor(mb.second, mb.first));
            p.drawPoint(QPoint(x, yy));
        }
    }
}

std::pair<double, int32_t> Mandelbrot::calcMandelbrot(const complex& c) const {
    complex z0(0, 0);
    complex z1(c);
    int32_t cur_it = INT32_MIN;

    for(uint32_t i=0; i<max_iter && z1.getAbs() < BAIL_OUT; i++) {
        z0.copy(z1);
        z1.mul(z0);
        z1.add(c);

        if(cur_it == INT32_MIN && z1.getAbs() >= BAIL_OUT) {
            cur_it = i;
            break;
        }
    }

    return std::make_pair(z1.getAbs(), cur_it);
}

mcalc_result_avx Mandelbrot::calcMandelbrot_avx(double imag1,
                                                double real1,
                                                double imag2,
                                                double real2) const {
    double zreal1 = real1;
    double zimag1 = imag1;
    double zreal2 = real2;
    double zimag2 = imag2;
    double znorm1 = 0.0;
    double znorm2 = 0.0;
    int32_t it1 = INT32_MIN;
    int32_t it2 = INT32_MIN;

    __m256d zc = _mm256_setr_pd(real1, imag1, real2, imag2);
    __m256d neg = _mm256_setr_pd(1.0, -1.0, 1.0, -1.0);
    __m256d zero = _mm256_setzero_pd();

    for(uint32_t i=0; i<max_iter && (znorm1 < BAIL_OUT || znorm2 < BAIL_OUT);
        i++) {
        __m256d zx = _mm256_setr_pd(zreal1, zimag1, zreal2, zimag2);
        __m256d zy = zx;

        // z_n = z_(n-1)^2
        __m256d zz = _mm256_mul_pd(zx, zy);
        zy = _mm256_permute_pd(zy, 0x5);
        zy = _mm256_mul_pd(zy, neg);
        __m256d zw = _mm256_mul_pd(zx, zy);
        zx = _mm256_hsub_pd(zz, zw);

        // z_n = z_(n-1)^2 + c
        zx = _mm256_add_pd(zx, zc);

        // |z_n|
        __m256d zd = _mm256_mul_pd(zx, zx);
        zd = _mm256_hadd_pd(zd, zero);

        if(znorm1 < BAIL_OUT) {
            zreal1 = zx[0];
            zimag1 = zx[1];
            znorm1 = zd[0];
        }

        if(znorm2 < BAIL_OUT) {
            zreal2 = zx[2];
            zimag2 = zx[3];
            znorm2 = zd[2];
        }

        if(znorm1 >= BAIL_OUT && it1 == INT32_MIN)
            it1 = i;
        if(znorm2 >= BAIL_OUT && it2 == INT32_MIN)
            it2 = i;
    }

    return mcalc_result_avx{znorm1, znorm2, it1, it2};
}

void Mandelbrot::updateComplexDimensions(const m_dimension& d) {
    dim_mutex.lock();

    dimensions.m_height = d.m_height;
    dimensions.m_width = d.m_width;
    dimensions.m_offset_x = d.m_offset_x;
    dimensions.m_offset_y = d.m_offset_y;

    dim_mutex.unlock();
}
