#include "complex.h"

complex::complex(): real(0), imag(0) {}

complex::complex(double r, double i): real(r), imag(i) {}

complex::complex(const complex& c): real(c.getReal()), imag(c.getImag()) {}

void complex::add(const complex& c) {
    real = real + c.getReal();
    imag = imag + c.getImag();
}

void complex::sub(const complex& c) {
    real = real - c.getReal();
    imag = imag - c.getImag();
}

void complex::mul(const complex& c) {
    double o_real = real;

    real = real * c.getReal() - imag * c.getImag();
    imag = imag * c.getReal() + o_real * c.getImag();
}

void complex::div(const complex& c) {
    double o_real = real;
    double div1 = c.getReal() * c.getReal() + c.getImag() * c.getImag();

    real = (real * c.getReal() + imag * c.getImag()) / div1;
    imag = (imag * c.getReal() - o_real * c.getImag()) / div1;
}

void complex::copy(const complex& c) {
    real = c.getReal();
    imag = c.getImag();
}

double complex::getImag() const {
    return imag;
}

double complex::getReal() const {
    return real;
}

double complex::getAbs() const {
    return real * real + imag * imag;
}
