//
// File: cw_fft.cpp
//
// MATLAB Coder version            : 5.0
// C/C++ source code generated on  : 12-Apr-2021 22:18:16
//

// Include Files
#include "cw_fft.h"
#include "rt_nonfinite.h"
#include <cmath>

// Type Definitions
class FFTImplementationCallback
{
  public:
    static void doHalfLengthRadix2(const double x_data[], creal_T y_data[],
        const double costab_data[], const double sintab_data[]);
};

// Variable Definitions
static boolean_T isInitialized_cw_fft = false;

// Function Declarations
static void fft(const double x_data[], creal_T y_data[], int y_size[2]);
static void normalize(const QVector <double> a, double n[128]);
static double rt_hypotd_snf(double u0, double u1);

// Function Definitions

//
// Arguments    : const double x_data[]
//                creal_T y_data[]
//                const double costab_data[]
//                const double sintab_data[]
// Return Type  : void
//
void FFTImplementationCallback::doHalfLengthRadix2(const double x_data[],
    creal_T y_data[], const double costab_data[], const double sintab_data[])
{
    int i;
    int ju;
    int iy;
    double hcostab_data[33];
    double hsintab_data[33];
    int k;
    creal_T reconVar1_data[64];
    int bitrevIndex_data[64];
    creal_T reconVar2_data[64];
    int wrapIndex_data[64];
    double temp_re;
    double temp_im;
    int iheight;
    double y_im;
    double y_re;
    double b_y_im;
    int j;
    double temp2_re;
    double temp2_im;
    for (i = 0; i < 32; i++) {
        iy = ((i + 1) << 1) - 2;
        hcostab_data[i] = costab_data[iy];
        hsintab_data[i] = sintab_data[iy];
    }

    ju = 0;
    iy = 1;
    for (i = 0; i < 64; i++) {
        reconVar1_data[i].re = sintab_data[i] + 1.0;
        reconVar1_data[i].im = -costab_data[i];
        reconVar2_data[i].re = 1.0 - sintab_data[i];
        reconVar2_data[i].im = costab_data[i];
        if (i + 1 != 1) {
            wrapIndex_data[i] = 65 - i;
        } else {
            wrapIndex_data[0] = 1;
        }

        bitrevIndex_data[i] = 0;
    }

    for (k = 0; k < 63; k++) {
        boolean_T tst;
        bitrevIndex_data[k] = iy;
        iy = 64;
        tst = true;
        while (tst) {
            iy >>= 1;
            ju ^= iy;
            tst = ((ju & iy) == 0);
        }

        iy = ju + 1;
    }

    bitrevIndex_data[63] = iy;
    iy = 0;
    for (i = 0; i < 64; i++) {
        y_data[bitrevIndex_data[i] - 1].re = x_data[iy];
        y_data[bitrevIndex_data[i] - 1].im = x_data[iy + 1];
        iy += 2;
    }

    for (i = 0; i <= 62; i += 2) {
        temp_re = y_data[i + 1].re;
        temp_im = y_data[i + 1].im;
        y_data[i + 1].re = y_data[i].re - y_data[i + 1].re;
        y_data[i + 1].im = y_data[i].im - y_data[i + 1].im;
        y_data[i].re += temp_re;
        y_data[i].im += temp_im;
    }

    iy = 2;
    ju = 4;
    k = 16;
    iheight = 61;
    while (k > 0) {
        int istart;
        int temp_re_tmp;
        for (i = 0; i < iheight; i += ju) {
            temp_re_tmp = i + iy;
            temp_re = y_data[temp_re_tmp].re;
            temp_im = y_data[temp_re_tmp].im;
            y_data[temp_re_tmp].re = y_data[i].re - temp_re;
            y_data[temp_re_tmp].im = y_data[i].im - temp_im;
            y_data[i].re += temp_re;
            y_data[i].im += temp_im;
        }

        istart = 1;
        for (j = k; j < 32; j += k) {
            int ihi;
            temp2_re = hcostab_data[j];
            temp2_im = hsintab_data[j];
            i = istart;
            ihi = istart + iheight;
            while (i < ihi) {
                temp_re_tmp = i + iy;
                temp_re = temp2_re * y_data[temp_re_tmp].re - temp2_im *
                    y_data[temp_re_tmp].im;
                temp_im = temp2_re * y_data[temp_re_tmp].im + temp2_im *
                    y_data[temp_re_tmp].re;
                y_data[temp_re_tmp].re = y_data[i].re - temp_re;
                y_data[temp_re_tmp].im = y_data[i].im - temp_im;
                y_data[i].re += temp_re;
                y_data[i].im += temp_im;
                i += ju;
            }

            istart++;
        }

        k /= 2;
        iy = ju;
        ju += ju;
        iheight -= iy;
    }

    temp_re = y_data[0].re;
    temp_im = y_data[0].im;
    y_im = y_data[0].re * reconVar1_data[0].im + y_data[0].im * reconVar1_data[0]
        .re;
    y_re = y_data[0].re;
    b_y_im = -y_data[0].im;
    y_data[0].re = 0.5 * ((y_data[0].re * reconVar1_data[0].re - y_data[0].im *
                           reconVar1_data[0].im) + (y_re * reconVar2_data[0].re
                           - b_y_im * reconVar2_data[0].im));
    y_data[0].im = 0.5 * (y_im + (y_re * reconVar2_data[0].im + b_y_im *
                           reconVar2_data[0].re));
    y_data[64].re = 0.5 * ((temp_re * reconVar2_data[0].re - temp_im *
                            reconVar2_data[0].im) + (temp_re * reconVar1_data[0]
                            .re - -temp_im * reconVar1_data[0].im));
    y_data[64].im = 0.5 * ((temp_re * reconVar2_data[0].im + temp_im *
                            reconVar2_data[0].re) + (temp_re * reconVar1_data[0]
                            .im + -temp_im * reconVar1_data[0].re));
    for (i = 0; i < 31; i++) {
        temp_re = y_data[i + 1].re;
        temp_im = y_data[i + 1].im;
        iy = wrapIndex_data[i + 1];
        ju = iy - 1;
        temp2_re = y_data[ju].re;
        temp2_im = y_data[ju].im;
        y_im = y_data[i + 1].re * reconVar1_data[i + 1].im + y_data[i + 1].im *
            reconVar1_data[i + 1].re;
        y_re = y_data[ju].re;
        b_y_im = -y_data[ju].im;
        y_data[i + 1].re = 0.5 * ((y_data[i + 1].re * reconVar1_data[i + 1].re -
            y_data[i + 1].im * reconVar1_data[i + 1].im) + (y_re *
            reconVar2_data[i + 1].re - b_y_im * reconVar2_data[i + 1].im));
        y_data[i + 1].im = 0.5 * (y_im + (y_re * reconVar2_data[i + 1].im +
            b_y_im * reconVar2_data[i + 1].re));
        y_data[i + 65].re = 0.5 * ((temp_re * reconVar2_data[i + 1].re - temp_im
            * reconVar2_data[i + 1].im) + (temp2_re * reconVar1_data[i + 1].re -
            -temp2_im * reconVar1_data[i + 1].im));
        y_data[i + 65].im = 0.5 * ((temp_re * reconVar2_data[i + 1].im + temp_im
            * reconVar2_data[i + 1].re) + (temp2_re * reconVar1_data[i + 1].im +
            -temp2_im * reconVar1_data[i + 1].re));
        y_data[ju].re = 0.5 * ((temp2_re * reconVar1_data[ju].re - temp2_im *
                                reconVar1_data[ju].im) + (temp_re *
                                reconVar2_data[ju].re - -temp_im *
                                reconVar2_data[ju].im));
        y_data[ju].im = 0.5 * ((temp2_re * reconVar1_data[ju].im + temp2_im *
                                reconVar1_data[ju].re) + (temp_re *
                                reconVar2_data[ju].im + -temp_im *
                                reconVar2_data[ju].re));
        iy += 63;
        y_data[iy].re = 0.5 * ((temp2_re * reconVar2_data[ju].re - temp2_im *
                                reconVar2_data[ju].im) + (temp_re *
                                reconVar1_data[ju].re - -temp_im *
                                reconVar1_data[ju].im));
        y_data[iy].im = 0.5 * ((temp2_re * reconVar2_data[ju].im + temp2_im *
                                reconVar2_data[ju].re) + (temp_re *
                                reconVar1_data[ju].im + -temp_im *
                                reconVar1_data[ju].re));
    }

    temp_re = y_data[32].re;
    temp_im = y_data[32].im;
    y_im = y_data[32].re * reconVar1_data[32].im + y_data[32].im *
        reconVar1_data[32].re;
    y_re = y_data[32].re;
    b_y_im = -y_data[32].im;
    y_data[32].re = 0.5 * ((y_data[32].re * reconVar1_data[32].re - y_data[32].
                            im * reconVar1_data[32].im) + (y_re *
                            reconVar2_data[32].re - b_y_im * reconVar2_data[32].
                            im));
    y_data[32].im = 0.5 * (y_im + (y_re * reconVar2_data[32].im + b_y_im *
                            reconVar2_data[32].re));
    y_data[96].re = 0.5 * ((temp_re * reconVar2_data[32].re - temp_im *
                            reconVar2_data[32].im) + (temp_re * reconVar1_data
                            [32].re - -temp_im * reconVar1_data[32].im));
    y_data[96].im = 0.5 * ((temp_re * reconVar2_data[32].im + temp_im *
                            reconVar2_data[32].re) + (temp_re * reconVar1_data
                            [32].im + -temp_im * reconVar1_data[32].re));
}

//
// Arguments    : const double x_data[]
//                creal_T y_data[]
//                int y_size[2]
// Return Type  : void
//
static void fft(const double x_data[], creal_T y_data[], int y_size[2])
{
    double costab1q_data[33];
    int k;
    double costab_data[65];
    double sintab_data[65];
    costab1q_data[0] = 1.0;
    for (k = 0; k < 16; k++) {
        costab1q_data[k + 1] = std::cos(0.049087385212340517 * (static_cast<
            double>(k) + 1.0));
    }

    for (k = 0; k < 15; k++) {
        costab1q_data[k + 17] = std::sin(0.049087385212340517 * (32.0 - (
            static_cast<double>(k) + 17.0)));
    }

    costab1q_data[32] = 0.0;
    costab_data[0] = 1.0;
    sintab_data[0] = 0.0;
    for (k = 0; k < 32; k++) {
        double costab_tmp;
        double sintab_tmp;
        costab_tmp = costab1q_data[k + 1];
        costab_data[k + 1] = costab_tmp;
        sintab_tmp = -costab1q_data[31 - k];
        sintab_data[k + 1] = sintab_tmp;
        costab_data[k + 33] = sintab_tmp;
        sintab_data[k + 33] = -costab_tmp;
    }

    y_size[0] = 128;
    y_size[1] = 1;
    FFTImplementationCallback::doHalfLengthRadix2((x_data), (y_data),
        (costab_data), (sintab_data));
}

//
// Arguments    : const double a[128]
//                double n[128]
// Return Type  : void
//
static void normalize(QVector <double> a, double n[128])
{
    double y;
    int nn;
    int k;
    double xbar;
    int nnans;
    double mu[128];
    double v[128];
    double absdiff[128];
    if (rtIsNaN(a[0])) {
        y = 0.0;
        nn = 0;
    } else {
        y = a[0];
        nn = 1;
    }

    for (k = 0; k < 127; k++) {
        xbar = a[k + 1];
        if (!rtIsNaN(xbar)) {
            y += xbar;
            nn++;
        }
    }

    xbar = y / static_cast<double>(nn);
    for (k = 0; k < 128; k++) {
        mu[k] = a[k] - xbar;
        v[k] = a[k];
    }

    nnans = 0;
    for (k = 0; k < 128; k++) {
        if (rtIsNaN(v[k])) {
            nnans++;
        } else {
            v[k - nnans] = v[k];
        }
    }

    nn = 128 - nnans;
    if (128 - nnans == 0) {
        xbar = rtNaN;
    } else if (128 - nnans == 1) {
        if ((!rtIsInf(v[0])) && (!rtIsNaN(v[0]))) {
            xbar = 0.0;
        } else {
            xbar = rtNaN;
        }
    } else {
        if (128 - nnans == 0) {
            xbar = 0.0;
        } else {
            xbar = v[0];
            for (k = 2; k <= nn; k++) {
                xbar += v[k - 1];
            }
        }

        xbar /= static_cast<double>(128 - nnans);
        for (k = 0; k < nn; k++) {
            absdiff[k] = std::abs(v[k] - xbar);
        }

        y = 0.0;
        if (128 - nnans >= 1) {
            if (128 - nnans == 1) {
                y = absdiff[0];
            } else {
                xbar = 3.3121686421112381E-170;
                nn = 127 - nnans;
                for (k = 0; k <= nn; k++) {
                    if (absdiff[k] > xbar) {
                        double t;
                        t = xbar / absdiff[k];
                        y = y * t * t + 1.0;
                        xbar = absdiff[k];
                    } else {
                        double t;
                        t = absdiff[k] / xbar;
                        y += t * t;
                    }
                }

                y = xbar * std::sqrt(y);
            }
        }

        xbar = y / std::sqrt(static_cast<double>(127 - nnans));
    }

    for (k = 0; k < 128; k++) {
        n[k] = mu[k] / xbar;
    }
}

//
// Arguments    : double u0
//                double u1
// Return Type  : double
//
static double rt_hypotd_snf(double u0, double u1)
{
    double y;
    double a;
    a = std::abs(u0);
    y = std::abs(u1);
    if (a < y) {
        a /= y;
        y *= std::sqrt(a * a + 1.0);
    } else if (a > y) {
        y /= a;
        y = a * std::sqrt(y * y + 1.0);
    } else {
        if (!rtIsNaN(y)) {
            y = a * 1.4142135623730951;
        }
    }

    return y;
}

//
// Arguments    : const double cw_wave[128]
//                double fs
//                double f[65]
//                creal_T mag_data[]
//                int mag_size[2]
// Return Type  : void
//
void cw_fft_128(const QVector<double> cw_wave, double fs, QVector<double> *f, QVector <creal_T>
            *mag_data, int mag_size[2])
{
    double dv[128];
    creal_T z_data[128];
    int z_size[2];
    coder::array<creal_T, 2U> mag_tmp;
    int k;
    coder::array<creal_T, 2U> b_mag_tmp;
    int i;
    if (!isInitialized_cw_fft) {
        cw_fft_initialize();
    }

    normalize(cw_wave, dv);
    fft(dv, z_data, z_size);
    mag_tmp.set_size(128, 1);
    for (k = 0; k < 128; k++) {
        double re;
        double im;
        if (z_data[k].im == 0.0) {
            re = z_data[k].re / 128.0;
            im = 0.0;
        } else if (z_data[k].re == 0.0) {
            re = 0.0;
            im = z_data[k].im / 128.0;
        } else {
            re = z_data[k].re / 128.0;
            im = z_data[k].im / 128.0;
        }

        z_data[k].re = re;
        z_data[k].im = im;
        mag_tmp[k].re = rt_hypotd_snf(re, im);
        mag_tmp[k].im = 0.0;
    }

    b_mag_tmp.set_size(mag_tmp.size(1), mag_tmp.size(0));
    k = mag_tmp.size(0);
    for (i = 0; i < k; i++) {
        int loop_ub;
        loop_ub = mag_tmp.size(1);
        for (int i1 = 0; i1 < loop_ub; i1++) {
            b_mag_tmp[i1 + b_mag_tmp.size(0) * i] = mag_tmp[i + mag_tmp.size(0) *
                i1];
        }
    }

    mag_tmp.set_size(1, 128);
    for (i = 0; i < 128; i++) {
        mag_tmp[i] = b_mag_tmp[i];
    }

    mag_size[0] = 1;
    mag_size[1] = 65;
    for (i = 0; i < 65; i++) {
        (*mag_data)[i] = mag_tmp[i];
    }

    for (i = 0; i < 63; i++) {
        (*mag_data)[i + 1].re = 2.0 * mag_tmp[i + 1].re;
        (*mag_data)[i + 1].im = 2.0 * mag_tmp[i + 1].im;
    }

    for (i = 0; i < 65; i++) {
       (*f)[i] = fs * static_cast<double>(i) / 128.0;
    }
}

//
// Arguments    : void
// Return Type  : void
//
void cw_fft_initialize()
{
    rt_InitInfAndNaN();
    isInitialized_cw_fft = true;
}

//
// Arguments    : void
// Return Type  : void
//
void cw_fft_terminate()
{
    // (no terminate code required)
    isInitialized_cw_fft = false;
}

//
// File trailer for cw_fft.cpp
//
// [EOF]
//
