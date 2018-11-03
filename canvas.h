#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPainter>
#include <memory>
#include <mutex>
#include <random>
#include <set>
#include <functional>
#include "mandelbrot.h"

class Canvas: public QWidget
{
    Q_OBJECT
public:
    Canvas(QWidget* parent = 0);
    void setThreads(uint8_t t);

protected:
    void paintEvent(QPaintEvent* ev) override;
    void resizeEvent(QResizeEvent* ev) override;
    void wheelEvent(QWheelEvent* ev) override;
    void mousePressEvent(QMouseEvent* ev) override;
    void mouseMoveEvent(QMouseEvent* ev) override;
    void mouseReleaseEvent(QMouseEvent* ev) override;
private:
    const uint32_t PREVIEW_PIXCOUNT = 30000;
    uint8_t thread_num;

    m_dimension dim_viewport;
    m_dimension tmp_viewport;
    bool mousePressed;
    int32_t mousePressedX;
    int32_t mousePressedY;

    std::unique_ptr<Mandelbrot> mandelbrot;
    QImage preview_buffer;
    std::vector<QImage> t_draw_buffer;
    std::vector<QImage> t_preview_buffer;

    void resizeBuffer();
signals:
    void positionCoordsChanged(QString real, QString imag);
};

#endif // CANVAS_H
