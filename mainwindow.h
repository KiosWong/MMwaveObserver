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
    void refreshPlot(QCustomPlot *customPlot, int key, double num);
    void plotWaveform();
    void readWaveform(QString waveformFileName);
    void saveWaveform(QString waveformFileName, QVector<unsigned int> waveformAdValue, bool saveFileInCsvFormat);
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

private:
    Ui::MainWindow *ui;
    QButtonGroup *mRadioButtonGroupInput;
    QButtonGroup *mRadioButtonGroupZoom;
    QSerialPort *dataSerialPortHandle;
    QSerialPort *configSerialPortHandle;
    QCPGraph *mmwaveAdValueCurve;
    QCPGraph *mmwaveProcessedValueCurve;
    QString dataComName;
    QString configComName;
    int dataComBaudrate;
    int configComBaudrate;
    QString udp_server_ipaddr;
    int udp_server_port;
    QUdpSocket *udpRecvSocket;
    QTimer *plotTimer;
    QTimer *uartTimer;
    QTimer *ethTimer;
    QTimer *readWaveformTimer;
    bool automaticRescaleAxis;
    int plotPoints;
    QFile *waveformFileHandle;
    int waveformLength;
    QByteArray waveformData;
    QVector<unsigned int> rawWaveformIssueQueue;
    QVector<unsigned int> processingWaveformDataQueue;
    QVector<unsigned int> processedWaveformIssueQueue;
};
#endif // MAINWINDOW_H
