QT      += core gui
QT	+= widgets printsupport
QT	+= serialport
QT	+= network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES +=  main.cpp \
    dsp/cw_bpf_1_5.cpp \
    dsp/cw_fft.cpp \
    dsp/cw_lpf.cpp \
    dsp/rtGetInf.cpp \
    dsp/rtGetNaN.cpp \
    dsp/rt_nonfinite.cpp \
    mainwindow.cpp \
    qcustomplot.cpp \
    qipaddress.cpp

HEADERS += mainwindow.h \
    dsp/coder_array.h \
    dsp/complex.h \
    dsp/cw_bpf_1_5.h \
    dsp/cw_fft.h \
    dsp/cw_fft_types.h \
    dsp/cw_lpf.h \
    dsp/rtGetInf.h \
    dsp/rtGetNaN.h \
    dsp/rt_nonfinite.h \
    dsp/rtwtypes.h \
    dsp/tmwtypes.h \
    qcustomplot.h \
    qipaddress.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    darkstyle.qrc \
    logo.qrc \
    qss.qrc \
    qss_custom.qrc
