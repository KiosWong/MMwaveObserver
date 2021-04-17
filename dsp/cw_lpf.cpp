//
// File: cw_lpf.cpp
//
// MATLAB Coder version            : 5.0
// C/C++ source code generated on  : 14-Apr-2021 09:43:56
//

// Include Files
#include "cw_lpf.h"
#include "rt_nonfinite.h"
#include <cmath>

// Variable Definitions
static boolean_T isInitialized_cw_lpf = false;

// Function Definitions

//
// Arguments    : const double x[128]
//                double y[128]
// Return Type  : void
//
void cw_lpf(const QVector<double> x, QVector<double> *y)
{
    double b_y;
    int nn;
    int k;
    double xbar;
    int nnans;
    double mu[128];
    double v[128];
    double absdiff[128];
    static const double dv[32] = { 7.7210618934326331E-5, 0.00057227346966233353,
        0.0014262371915007618, 0.0030525931687665968, 0.0056741000214483132,
        0.0095580901862676766, 0.01488990809316444, 0.021736287230900403,
        0.029997839883315197, 0.039383826757210222, 0.049412692193360093,
        0.059443443355239253, 0.068737011412684179, 0.076538325617538344,
        0.082170736158368118, 0.08512397533896239, 0.08512397533896239,
        0.082170736158368118, 0.076538325617538344, 0.068737011412684179,
        0.059443443355239253, 0.049412692193360093, 0.039383826757210222,
        0.029997839883315197, 0.021736287230900403, 0.01488990809316444,
        0.0095580901862676766, 0.0056741000214483132, 0.0030525931687665968,
        0.0014262371915007618, 0.00057227346966233353, 7.7210618934326331E-5 };

    if (!isInitialized_cw_lpf) {
        cw_lpf_initialize();
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

    for (k = 0; k < 32; k++) {
        nn = k + 1;
        for (nnans = nn; nnans < 129; nnans++) {
            (*y)[nnans - 1] += dv[k] * v[(nnans - k) - 1];
        }
    }
}

//
// Arguments    : void
// Return Type  : void
//
void cw_lpf_initialize()
{
    rt_InitInfAndNaN();
    isInitialized_cw_lpf = true;
}

//
// Arguments    : void
// Return Type  : void
//
void cw_lpf_terminate()
{
    // (no terminate code required)
    isInitialized_cw_lpf = false;
}

//
// File trailer for cw_lpf.cpp
//
// [EOF]
//
