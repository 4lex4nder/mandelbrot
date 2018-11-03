#include "canvas.h"

Canvas::Canvas(QWidget* parent): QWidget(parent) {
    dim_viewport.m_height = 2;
    dim_viewport.m_offset_y = 1;
    dim_viewport.m_offset_x = -2;
    dim_viewport.m_width = 3;

    thread_num = 4;

    mandelbrot = std::unique_ptr<Mandelbrot>(new Mandelbrot());
    t_draw_buffer.resize(thread_num);
    t_preview_buffer.resize(thread_num);
    resizeBuffer();

    mandelbrot->updateComplexDimensions(dim_viewport);

    mousePressed = false;
    this->setMouseTracking(true);
}


void Canvas::resizeBuffer() {
    uint32_t seg_height = this->height() / thread_num;
    for(uint32_t i=0; i<t_draw_buffer.size(); i++) {
        if(i < t_draw_buffer.size() - 1)
            t_draw_buffer.at(i) = QImage(this->width(), seg_height,
                    QImage::Format_ARGB32);
        else
            t_draw_buffer.at(i) = QImage(this->width(), this->height()
                    - (thread_num - 1) * seg_height,
                    QImage::Format_ARGB32);

    }

    double ratio = static_cast<double>(this->width()) / this->height();
    uint32_t nw = std::sqrt(PREVIEW_PIXCOUNT);
    uint32_t nh = std::sqrt(PREVIEW_PIXCOUNT/(ratio * ratio));
    uint32_t pseg_height = nh / thread_num;
    for(uint32_t i=0; i<t_preview_buffer.size(); i++) {
        if(i < t_preview_buffer.size() - 1)
            t_preview_buffer.at(i) = QImage(nw, pseg_height,
                    QImage::Format_ARGB32);
        else
            t_preview_buffer.at(i) = QImage(nw, nh
                    - (thread_num - 1) * pseg_height,
                    QImage::Format_ARGB32);

    }
    preview_buffer = QImage(nw, nh, QImage::Format_ARGB32);
}

void Canvas::paintEvent(QPaintEvent* ev) {
    QPainter p(this);
    if(!mousePressed) {
        // Directly draw tiles onto main draw buffer
        mandelbrot->refreshMandelbrotTiled(t_draw_buffer);
        uint32_t cur_hstart = 0;
        uint32_t cur_hend = t_draw_buffer.at(0).height();
        for(uint32_t i=0; i<t_draw_buffer.size(); i++) {
            if(i > 0) {
                cur_hstart = cur_hend;
                cur_hend = cur_hstart + t_draw_buffer.at(i).height();
            }
            p.drawImage(0, cur_hstart, t_draw_buffer.at(i));
        }

    } else {
        // First draw preview tile onto preview buffer
        mandelbrot->refreshMandelbrotTiled(t_preview_buffer);
        QPainter pp(&preview_buffer);

        uint32_t cur_hstart = 0;
        uint32_t cur_hend = t_preview_buffer.at(0).height();
        for(uint32_t i=0; i<t_draw_buffer.size(); i++) {
            if(i > 0) {
                cur_hstart = cur_hend;
                cur_hend = cur_hstart + t_preview_buffer.at(i).height();
            }
            pp.drawImage(0, cur_hstart, t_preview_buffer.at(i));
        }

        // Draw (scaled) preview buffer to main draw buffer
        p.drawImage(0, 0, preview_buffer.scaled(this->width(), this->height()));
    }
}

void Canvas::resizeEvent(QResizeEvent* ev) {
    resizeBuffer();

    QWidget::resizeEvent(ev);
}

void Canvas::mousePressEvent(QMouseEvent* ev) {
    mousePressed = true;
    mousePressedX = ev->x();
    mousePressedY = ev->y();

    this->setCursor(Qt::OpenHandCursor);

    QWidget::mousePressEvent(ev);
}

void Canvas::mouseMoveEvent(QMouseEvent* ev) {
    double posX = static_cast<double>(ev->x())
            / this->width();
    double posY = static_cast<double>(ev->y())
            / this->height();
    double real = dim_viewport.m_offset_x + posX * dim_viewport.m_width;
    double imag = dim_viewport.m_offset_y - posY * dim_viewport.m_height;

    emit positionCoordsChanged(QString::fromStdString(std::to_string(real)),
                               QString::fromStdString(std::to_string(imag)));

    if(!mousePressed)
        return;

    tmp_viewport = dim_viewport;

    double xratio = static_cast<double>(mousePressedX - ev->x())
            / this->width();
    double yratio = static_cast<double>(ev->y() - mousePressedY)
            / this->height();

    tmp_viewport.m_offset_x += xratio * tmp_viewport.m_width;
    tmp_viewport.m_offset_y += yratio * tmp_viewport.m_height;

    mandelbrot->updateComplexDimensions(tmp_viewport);
    repaint();
}

void Canvas::mouseReleaseEvent(QMouseEvent* ev) {
    if(!mousePressed)
        return;

    mousePressed = false;
    dim_viewport = tmp_viewport;

    this->setCursor(Qt::ArrowCursor);

    this->repaint();

    QWidget::mouseReleaseEvent(ev);
}

void Canvas::wheelEvent(QWheelEvent* ev) {
    double focal_real = dim_viewport.m_offset_x
            + (static_cast<double>(ev->x()) / this->width())
            * dim_viewport.m_width;
    double focal_imag = dim_viewport.m_offset_y
            - (static_cast<double>(ev->y()) / this->height())
            * dim_viewport.m_height;
    double real_ratio = static_cast<double>(ev->x()) / this->width();
    double imag_ratio = static_cast<double>(ev->y()) / this->height();

    double zoom = static_cast<double>(ev->delta()) / 100;
    if(zoom > 0) {
        dim_viewport.m_width /= zoom;
        dim_viewport.m_height /= zoom;
    } else {
        dim_viewport.m_width *= -zoom;
        dim_viewport.m_height *= -zoom;
    }

    dim_viewport.m_offset_x = focal_real - real_ratio * dim_viewport.m_width;
    dim_viewport.m_offset_y = focal_imag + imag_ratio * dim_viewport.m_height;

    mandelbrot->updateComplexDimensions(dim_viewport);
    repaint();
}

void Canvas::setThreads(uint8_t t) {
    thread_num = t;
    t_draw_buffer.resize(t);
    t_preview_buffer.resize(t);
    resizeBuffer();
}
