//
// File: cw_fft.h
//
// MATLAB Coder version            : 5.0
// C/C++ source code generated on  : 12-Apr-2021 22:18:16
//
#ifndef CW_FFT_H
#define CW_FFT_H

// Include Files
#include <cstddef>
#include <cstdlib>
#include "rtwtypes.h"
#include "cw_fft_types.h"
#include <QMainWindow>

// Function Declarations
void cw_fft_128(const QVector<double> cw_wave, double fs, QVector<double> *f, QVector <creal_T>
            *mag_data, int mag_size[2]);
extern void cw_fft_initialize();
extern void cw_fft_terminate();

#endif

//
// File trailer for cw_fft.h
//
// [EOF]
//
