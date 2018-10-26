#ifndef COMPLEX_H
#define COMPLEX_H

#include <memory>
#include <math.h>
#include <iostream>
#include <utility>
#include <immintrin.h>

class complex;

using ucmp = std::unique_ptr<complex>;
using scmp = std::shared_ptr<complex>;

class complex
{
private:
    double real;
    double imag;
public:
    complex();
    complex(double r, double i);
    complex(const complex& c);

    double getReal() const;
    double getImag() const;

    void add(const complex& r);
    void sub(const complex& r);

    void mul(const complex& r);
    void div(const complex& r);

    void copy(const complex& c);

    double getAbs() const;

    complex operator+(const complex& r) {
        complex f(*this);
        f.add(r);
        return f;
    }

    complex operator-(const complex& r) {
        complex f(*this);
        f.sub(r);
        return f;
    }

    complex operator*(const complex& r) {
        complex f(*this);
        f.mul(r);
        return f;
    }

    complex operator/(const complex& r) {
        complex f(*this);
        f.div(r);
        return f;
    }

    bool operator!=(const complex& r) {
        return real != r.getReal() || imag != r.getImag();
    }

    friend std::ostream& operator<<(std::ostream& s, const complex& c) {
        return s << c.getReal() << " + " << c.getImag() << "i";
    }

    //static __m256d mul_avx(__m256d& v1, __m256d& v2);
};



//void operator+=(complex& l, const complex r) {
//    l.add(r);
//}
//
//void operator-=(complex& l, const complex r) {
//    l.sub(r);
//}
//
//void operator*=(complex& l, const complex r) {
//    l.mul(r);
//}
//
//void operator/=(complex& l, const complex r) {
//    l.div(r);
//}

#endif // COMPLEX_H
