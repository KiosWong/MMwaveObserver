#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <queue>
#include <QMainWindow>
#include <QTimer>
#include <qcustomplot.h>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QUdpSocket>
#include "qipaddress.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum FileType {
    raw,
    csv
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initStyle();
    void resetPlotStatus();
    void resetWaveformData();
    void initializePlot();
    void refreshPlot(QCustomPlot *customPlot, int key, int value1, int value2);
    void plotWaveform();
    void readWaveform(QString waveformFileName);
    void saveWaveform(QString waveformFileName, QVector<int> mmwaveWaveformAdValue, QVector<int> bmd101WaveformAdValue, bool saveFileInCsvFormat);
    int  parseRawWaveform(QByteArray waveformData);
    int  parseCsvWaveform(QByteArray waveformData);
    int  parseWaveform(QByteArray waveformData, enum FileType waveformFileType);
    void initialize_uart();
    void open_uart(QSerialPort *serialPortHandle, QString pornName, int baudrate);
    int  read_uart();
    void establishUdpConnection(int port);


private slots:
    void on_plotTimer_updated();

    void on_serialOpenPushButton_clicked();

    void on_startPlotPushButton_clicked();

    void on_stopPlotPushButton_clicked();

    void on_savePlotPushButton_clicked();

    void on_automaticZoomCheckBox_stateChanged(int arg1);

    void on_uartTimer_updated();

    void on_udpServer_received();

    void on_readWaveformTimer_updated();

    void on_openPlotPushButton_clicked();

    void on_displaySpeedDial_sliderMoved(int position);

    void on_zoomXRadioButton_clicked();

    void on_zoomYRadioButton_clicked();

    void on_zoomBothRadioButton_clicked();

    void on_displaySpeedSpinBox_valueChanged(const QString &arg1);

    void on_pausePlotPushButton_clicked();

    void on_inputSelectFileRadioButton_clicked();

    void on_inputSelectUartRadioButton_clicked();

    void on_ethSettingPushButton_clicked();

    void on_serialSettingPushButton_clicked();

    void on_inputSelectEthRadioButton_clicked();

    void on_frameCountTimer_updated();

private:
    Ui::MainWindow *ui;
    QButtonGroup *mRadioButtonGroupInput;
    QButtonGroup *mRadioButtonGroupZoom;
    QSerialPort *dataSerialPortHandle;
    QSerialPort *configSerialPortHandle;
    QCPGraph *mmwaveAdValueCurve;
    QCPGraph *bmd101AdValueCurve;
    QCPGraph *mmwaveProcessedValueCurve;
    QString dataComName;
    QString configComName;
    int dataComBaudrate;
    int configComBaudrate;
    QString udp_server_ipaddr;
    int udp_server_port;
    QString udp_client_ipaddr;
    int udp_client_port;
    QUdpSocket *udpRecvSocket;
    bool plotStarted;
    QTimer *plotTimer;
    QTimer *uartTimer;
    QTimer *ethTimer;
    QTimer *frameCountTimer;
    QTimer *readWaveformTimer;
    bool automaticRescaleAxis;
    int plotPoints;
    int frameCounter;
    QFile *waveformFileHandle;
    int waveformLength;
    QByteArray waveformData;
    QVector<int> rawBmd101WaveformIssueQueue;
    QVector<int> rawMMwaveWaveformIssueQueue;
    QVector<int> processingBmd101WaveformDataQueue;
    QVector<int> processingMMwaveWaveformDataQueue;
    QVector<int> processedBmd101WaveformIssueQueue;
    QVector<int> processedMMwaveWaveformIssueQueue;

};
#endif // MAINWINDOW_H
