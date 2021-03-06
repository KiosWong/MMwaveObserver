//
// File: cw_bpf_1_5.cpp
//
// MATLAB Coder version            : 5.0
// C/C++ source code generated on  : 14-Apr-2021 22:42:59
//

// Include Files
#include "cw_bpf_1_5.h"
#include "rt_nonfinite.h"
#include <cmath>

// Variable Definitions
static boolean_T isInitialized_cw_bpf_1_5 = false;

// Function Definitions

//
// Arguments    : const double x[128]
//                double y[128]
// Return Type  : void
//
void cw_bpf_1_5(const QVector<double> x, QVector<double> *y)
{
    double b_y;
    int nn;
    int k;
    double xbar;
    int nnans;
    double mu[128];
    double v[128];
    int naxpy;
    double absdiff[128];
    static const double dv[100] = { -0.00270242276325328, 0.0050803082616177886,
        0.0058361575879473642, -0.015223303408204387, 0.0010954267020509174,
        0.013246999451597945, -0.0051091723352467491, 0.0018041986925962589,
        -0.0079726302350255421, -0.0012989386862738081, 0.0088215043781914582,
        -0.0027943819720111489, 0.0078047997555611591, -0.010913205277151323,
        -0.0026201642395188348, 0.0042362536341789374, -0.0024420603913929435,
        0.015719916073979663, -0.012246704370342873, -0.00072848512620388509,
        -0.004418194421733506, -0.0047068303903756575, 0.021968962828599291,
        -0.010523827684944113, 0.0075148005164173207, -0.015023904392577399,
        -0.009533924959540481, 0.021191827661376447, -0.0076576383309120821,
        0.023242718634925615, -0.022146235208467278, -0.013690742581881792,
        0.0096213706397625075, -0.0093708133664636914, 0.043573713221988865,
        -0.020363945852573357, -0.010118891321204922, -0.011395406335636481,
        -0.024279332588543735, 0.062361409042931491, -0.00918994492995661,
        0.013561953935426446, -0.034542995943514961, -0.0687566004847324,
        0.0767615858082592, -0.00024983538468379557, 0.10843483387272673,
        -0.049733141964871608, -0.33879957025378815, 0.27720435737658033,
        0.27720435737658033, -0.33879957025378815, -0.049733141964871608,
        0.10843483387272673, -0.00024983538468379557, 0.0767615858082592,
        -0.0687566004847324, -0.034542995943514961, 0.013561953935426446,
        -0.00918994492995661, 0.062361409042931491, -0.024279332588543735,
        -0.011395406335636481, -0.010118891321204922, -0.020363945852573357,
        0.043573713221988865, -0.0093708133664636914, 0.0096213706397625075,
        -0.013690742581881792, -0.022146235208467278, 0.023242718634925615,
        -0.0076576383309120821, 0.021191827661376447, -0.009533924959540481,
        -0.015023904392577399, 0.0075148005164173207, -0.010523827684944113,
        0.021968962828599291, -0.0047068303903756575, -0.004418194421733506,
        -0.00072848512620388509, -0.012246704370342873, 0.015719916073979663,
        -0.0024420603913929435, 0.0042362536341789374, -0.0026201642395188348,
        -0.010913205277151323, 0.0078047997555611591, -0.0027943819720111489,
        0.0088215043781914582, -0.0012989386862738081, -0.0079726302350255421,
        0.0018041986925962589, -0.0051091723352467491, 0.013246999451597945,
        0.0010954267020509174, -0.015223303408204387, 0.0058361575879473642,
        0.0050803082616177886, -0.00270242276325328 };

    if (!isInitialized_cw_bpf_1_5) {
        cw_bpf_1_5_initialize();
    }

    if (rtIsNaN(x[0])) {
        b_y = 0.0;
        nn = 0;
    } else {
        b_y = x[0];
        nn = 1;
    }

    for (k = 0; k < 127; k++) {
        xbar = x[k + 1];
        if (!rtIsNaN(xbar)) {
            b_y += xbar;
            nn++;
        }
    }

    xbar = b_y / static_cast<double>(nn);
    for (k = 0; k < 128; k++) {
        mu[k] = x[k] - xbar;
        v[k] = x[k];
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

        b_y = 0.0;
        if (128 - nnans >= 1) {
            if (128 - nnans == 1) {
                b_y = absdiff[0];
            } else {
                xbar = 3.3121686421112381E-170;
                nn = 127 - nnans;
                for (k = 0; k <= nn; k++) {
                    if (absdiff[k] > xbar) {
                        double t;
                        t = xbar / absdiff[k];
                        b_y = b_y * t * t + 1.0;
                        xbar = absdiff[k];
                    } else {
                        double t;
                        t = absdiff[k] / xbar;
                        b_y += t * t;
                    }
                }

                b_y = xbar * std::sqrt(b_y);
            }
        }

        xbar = b_y / std::sqrt(static_cast<double>(127 - nnans));
    }

    for (k = 0; k < 128; k++) {
        v[k] = mu[k] / xbar;
        (*y)[k] = 0.0;
    }

    for (k = 0; k < 28; k++) {
        for (nn = 0; nn < 100; nn++) {
            nnans = k + nn;
            (*y)[nnans] += v[k] * dv[nn];
        }
    }

    naxpy = 99;
    for (k = 0; k < 100; k++) {
        for (nn = 0; nn <= naxpy; nn++) {
            nnans = (k + nn) + 28;
            (*y)[nnans] += v[k + 28] * dv[nn];
        }

        naxpy--;
    }
}

//
// Arguments    : void
// Return Type  : void
//
void cw_bpf_1_5_initialize()
{
    rt_InitInfAndNaN();
    isInitialized_cw_bpf_1_5 = true;
}

//
// Arguments    : void
// Return Type  : void
//
void cw_bpf_1_5_terminate()
{
    // (no terminate code required)
    isInitialized_cw_bpf_1_5 = false;
}

//
// File trailer for cw_bpf_1_5.cpp
//
// [EOF]
//
