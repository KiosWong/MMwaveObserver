#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "./dsp/cw_fft.h"
#include "./dsp/cw_lpf.h"
#include "./dsp/cw_bpf_1_5.h"

static int findMaxIndex(QVector<creal_T> vec)
{
    double max_val = 0;
    int max_val_index = 0;
    for(int i = 0; i < vec.length(); i++) {
        if(vec.at(i).re > max_val) {
            max_val = vec.at(i).re;
            max_val_index = i;
        }
    }
    return max_val_index;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initializePlot();
    initStyle();

    mRadioButtonGroupInput = new QButtonGroup(this);
    mRadioButtonGroupInput->addButton(ui->inputSelectFileRadioButton);
    mRadioButtonGroupInput->addButton(ui->inputSelectUartRadioButton);
    mRadioButtonGroupInput->addButton(ui->inputSelectEthRadioButton);

    mRadioButtonGroupZoom = new QButtonGroup(this);
    mRadioButtonGroupZoom->addButton(ui->zoomXRadioButton);
    mRadioButtonGroupZoom->addButton(ui->zoomYRadioButton);
    mRadioButtonGroupZoom->addButton(ui->zoomBothRadioButton);
    dataSerialPortHandle = new QSerialPort();
    configSerialPortHandle = new QSerialPort();
    plotTimer = new QTimer(this);
    uartTimer = new QTimer(this);
    frameCountTimer = new QTimer(this);

    ui->startPlotPushButton->setStyleSheet("color: #191970");
    ui->pausePlotPushButton->setStyleSheet("color: #191970");
    ui->replotPushButton->setStyleSheet("color: #191970");
    ui->stopPlotPushButton->setStyleSheet("color: #191970");

    ui->hduLogoLabel->setPixmap( QPixmap(":/images/hduLogo.png"));
    ui->inputSelectFileRadioButton->click();
    ui->automaticShiftCheckBox->click();
    ui->automaticZoomCheckBox->click();
    ui->zoomBothRadioButton->click();
    ui->serialSettingPushButton->setEnabled(false);
    ui->ethSettingPushButton->setEnabled(false);
    ui->openPlotPushButton->setEnabled(true);
    ui->displaySpeedDial->setMinimum(5);
    ui->displaySpeedDial->setMaximum(200);
    ui->displaySpeedDial->setWrapping(false);
    ui->displaySpeedDial->setValue(5);
    ui->displaySpeedSpinBox->setMinimum(5);
    ui->displaySpeedSpinBox->setMaximum(200);
    ui->displaySpeedSpinBox->setValue(5);
    ui->displayProgressBar->setValue(0);

    ui->breathRateLcdNumber->display("--");
    ui->heartRateLcdNumber->display("--");
//    ui->hduLogoLabel->hide();

    connect(plotTimer,SIGNAL(timeout()),this,SLOT(on_plotTimer_updated()));
    connect(uartTimer,SIGNAL(timeout()),this,SLOT(on_uartTimer_updated()));
    connect(frameCountTimer,SIGNAL(timeout()),this,SLOT(on_frameCountTimer_updated()));


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initStyle()
{
    //加载样式表
    QString qss;
//    QFile file(":/qss/psblack.css");
      QFile file(":/qss/flatwhite.css");
//    QFile file(":/qss/lightblue.css");
//    QFile file(":/darkstyle/darkstyle.qss");
    if (file.open(QFile::ReadOnly)) {
#if 1
        //用QTextStream读取样式文件不用区分文件编码 带bom也行
        QStringList list;
        QTextStream in(&file);
        //in.setCodec("utf-8");
        while (!in.atEnd()) {
            QString line;
            in >> line;
            list << line;
        }

        qss = list.join("\n");
#else
        //用readAll读取默认支持的是ANSI格式,如果不小心用creator打开编辑过了很可能打不开
        qss = QLatin1String(file.readAll());
#endif
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(QColor(paletteColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }
}


void MainWindow::resetPlotStatus()
{
    mmwaveAdValueCurve->clearData();
    bmd101AdValueCurve->clearData();
    mmwaveProcessedValueCurve->clearData();
    plotPoints = 0;
    processingMMwaveWaveformDataQueue.clear();
    processingBmd101WaveformDataQueue.clear();
    rawMMwaveWaveformIssueQueue.clear();
    rawBmd101WaveformIssueQueue.clear();
    for(int i = 0; i < 128; i++) {
        processingMMwaveWaveformDataQueue.append(0);
        processingMMwaveWaveformDataQueue.append(0);
    }
    ui->mmwaveAdValuePlot->replot();
    ui->mmwaveProcessedPlot->replot();
    plotStarted = false;
}

void MainWindow::resetWaveformData()
{
    waveformData.clear();
    waveformLength = 0;
}


void MainWindow::initializePlot()
{
    plotPoints = 0;
    ui->mmwaveAdValuePlot->setInteractions( QCP::iRangeDrag|QCP::iRangeZoom| QCP::iSelectAxes |
                                            QCP::iSelectLegend | QCP::iSelectPlottables);

    ui->mmwaveAdValuePlot->xAxis->setRange(0, 500);//x轴范围
    ui->mmwaveAdValuePlot->yAxis->setRange(0, 4096);//y轴范围
    ui->mmwaveAdValuePlot->xAxis->setLabel("采样点");//设置x轴名称
    ui->mmwaveAdValuePlot->yAxis->setLabel("AD数据");//设置y轴名称
    ui->mmwaveAdValuePlot->axisRect()->setRangeZoomFactor(0.5);

    mmwaveAdValueCurve = ui->mmwaveAdValuePlot->addGraph();//画曲线
    mmwaveAdValueCurve->setSmooth(true);
    bmd101AdValueCurve = ui->mmwaveAdValuePlot->addGraph();
    mmwaveAdValueCurve->setSmooth(true);
    bmd101AdValueCurve->setPen(QPen(Qt::red));

    ui->mmwaveProcessedPlot->setInteractions( QCP::iRangeDrag|QCP::iRangeZoom| QCP::iSelectAxes |
                                            QCP::iSelectLegend | QCP::iSelectPlottables);

    ui->mmwaveProcessedPlot->xAxis->setRange(0, 1);//x轴范围
    ui->mmwaveProcessedPlot->yAxis->setRange(0, 100);//y轴范围
    ui->mmwaveProcessedPlot->xAxis->setLabel("频点");//设置x轴名称
    ui->mmwaveProcessedPlot->yAxis->setLabel("幅度");//设置y轴名称
    ui->mmwaveProcessedPlot->axisRect()->setRangeZoomFactor(0.5);

    mmwaveProcessedValueCurve = ui->mmwaveProcessedPlot->addGraph();//画曲线
    mmwaveProcessedValueCurve->setSmooth(true);
    resetPlotStatus();
    resetWaveformData();
    rawMMwaveWaveformIssueQueue.clear();
    rawBmd101WaveformIssueQueue.clear();

    ui->mmwaveProcessedPlot->replot();
    plotStarted = false;

}

void MainWindow::refreshPlot(QCustomPlot *customPlot, int key, int value1, int value2)
{
    // 给曲线添加数据

    customPlot->graph(0)->addData(key, value1);
    customPlot->graph(1)->addData(key, value2);
    if(ui->automaticZoomCheckBox->isChecked()) {
        customPlot->graph(0)->rescaleValueAxis();
        customPlot->graph(1)->rescaleValueAxis();
    }

    if(ui->automaticShiftCheckBox->isChecked())
        customPlot->xAxis->setRange(key, 500, Qt::AlignRight);

    customPlot->replot();
}

void MainWindow::plotWaveform()
{
    QVector<double> fft_wave_data(128, 0);
    QVector<double> fft_wave_data_lfp(128, 0);
    QVector<double> fft_wave_data_bfp(128, 0);
    QVector<double> fft_freq(65);
    QVector<creal_T> fft_mag(65);
    int mag_size[2] = {0};
    int br_max_mag_index = 0;
    int hr_max_mag_index = 0;

    QVector<double> fft_freq_vec;
    QVector<double> fft_mag_vec;

    QString breathrate_lcd_value;
    QString heartrate_lcd_value;
    int breath_rate = 0;
    int heart_rate = 0;
    QDateTime current_date_time =QDateTime::currentDateTime();

    if(ui->inputSelectFileRadioButton->isChecked() && plotPoints == waveformLength)
        plotTimer->stop();
    else {
        refreshPlot(ui->mmwaveAdValuePlot, plotPoints, rawMMwaveWaveformIssueQueue.at(plotPoints), rawBmd101WaveformIssueQueue.at(plotPoints));
        processingMMwaveWaveformDataQueue.append(rawMMwaveWaveformIssueQueue.at(plotPoints));
        processingMMwaveWaveformDataQueue.remove(0);
//        for(int i = 0; i < 128; i++) {
//            fft_wave_data[i]= static_cast<double>(processingMMwaveWaveformDataQueue.at(i));
//        }

//        cw_fft_128(fft_wave_data, 5, &fft_freq, &fft_mag, mag_size);

//        for(int i = 0; i < 64; i++) {
//            fft_freq_vec.append(fft_freq[i]);
//            fft_mag_vec.append(fft_mag[i].re);
//        }
//        ui->mmwaveProcessedPlot->graph(0)->setData(fft_freq_vec, fft_mag_vec);
//        ui->mmwaveProcessedPlot->rescaleAxes();
//        ui->mmwaveProcessedPlot->replot();
//        fft_freq_vec.clear();
//        fft_mag_vec.clear();

//        cw_lpf(fft_wave_data, &fft_wave_data_lfp);
//        cw_bpf_1_5(fft_wave_data, &fft_wave_data_bfp);

//        cw_fft_128(fft_wave_data_lfp, 5, &fft_freq, &fft_mag, mag_size);
//        br_max_mag_index = findMaxIndex(fft_mag);
//        breath_rate = static_cast<int>((fft_freq.at(br_max_mag_index) * 60));

//        cw_fft_128(fft_wave_data_bfp, 5, &fft_freq, &fft_mag, mag_size);
//        hr_max_mag_index = findMaxIndex(fft_mag);
//        heart_rate = static_cast<int>((fft_freq.at(hr_max_mag_index) * 60));

//        breathrate_lcd_value.setNum(breath_rate);
//        QPalette lcd_palette;
//        if(breath_rate <= 4 || breath_rate >= 30)
//            lcd_palette.setColor(QPalette::Normal,QPalette::WindowText,Qt::red);
//        else
//            lcd_palette.setColor(QPalette::Normal,QPalette::WindowText,0x878991);
//        ui->breathRateLcdNumber->setPalette(lcd_palette);
//        ui->breathRateLcdNumber->display(breathrate_lcd_value);

//        heartrate_lcd_value.setNum(heart_rate);
//        if(heart_rate <= 40 || heart_rate >= 120)
//            lcd_palette.setColor(QPalette::Normal,QPalette::WindowText,Qt::red);
//        else
//            lcd_palette.setColor(QPalette::Normal,QPalette::WindowText,0x878991);
//        ui->breathRateLcdNumber->setPalette(lcd_palette);
//        ui->heartRateLcdNumber->display(heartrate_lcd_value);

//        if(ui->inputSelectFileRadioButton->isChecked())
//            ui->displayProgressBar->setValue(plotPoints * 100 / waveformLength);
//        else
//            ui->displayProgressBar->setValue(100);
        plotPoints ++;
        frameCounter ++;
    }
}


void MainWindow::on_plotTimer_updated()
{
    plotWaveform();
}

void MainWindow::on_serialOpenPushButton_clicked()
{

}

void MainWindow::readWaveform(QString waveformFileName)
{
    waveformFileHandle = new QFile(waveformFileName);
    waveformLength = 0;
    if (waveformFileHandle->open(QIODevice::ReadOnly | QIODevice::Text)) {
        resetPlotStatus();
        resetWaveformData();
        rawMMwaveWaveformIssueQueue.clear();
        waveformData = waveformFileHandle->readAll();
        qDebug() << waveformData;
        if(waveformData.length() > 0) {
            if(ui->openFileFormatComboBox->currentText() == "raw")
                waveformLength = parseWaveform(waveformData, raw);
            else
                waveformLength = parseWaveform(waveformData, csv);
        }
        waveformFileHandle->close();
    }
    else
        qDebug() << "error opening file " << waveformFileName << endl;
    waveformFileHandle->destroyed();

}

int MainWindow::parseRawWaveform(QByteArray waveformData)
{
    int waveformParseStatus = 0;
    int waveformLength = 0;
    quint8 wavefromAdValueTmp;
    int wavefromBmd101AdValue = 0;
    int wavefromMmwaveAdValue = 0;

    for(int i = 0; i < waveformData.length(); i++) {
        switch(waveformParseStatus) {
        case 0:
            if(static_cast<unsigned char>(waveformData.at(i)) == 0xAA)
                waveformParseStatus = 1;
            else
                waveformParseStatus = 0;
        break;

        case 1:
            if(static_cast<unsigned char>(waveformData.at(i)) == 0xAA)
                waveformParseStatus = 2;
            else
                waveformParseStatus = 0;
        break;

        case 2:
            waveformParseStatus = 3;
            wavefromAdValueTmp = static_cast<unsigned char>(waveformData.at(i));
            wavefromBmd101AdValue |= wavefromAdValueTmp;
            wavefromBmd101AdValue <<= 8;
        break;

        case 3:
                waveformParseStatus = 4;
                wavefromAdValueTmp = static_cast<unsigned char>(waveformData.at(i));
                wavefromBmd101AdValue |= wavefromAdValueTmp;
                if(wavefromBmd101AdValue > 32768)
                    wavefromBmd101AdValue = wavefromBmd101AdValue - 65536;
        break;
        case 4:
            waveformParseStatus = 5;
            wavefromAdValueTmp = static_cast<unsigned char>(waveformData.at(i));
            wavefromMmwaveAdValue |= wavefromAdValueTmp;
            wavefromMmwaveAdValue <<= 8;
        break;
        case 5:
            waveformParseStatus = 6;
            wavefromAdValueTmp = static_cast<unsigned char>(waveformData.at(i));
            wavefromMmwaveAdValue |= wavefromAdValueTmp;
            if(wavefromMmwaveAdValue > 32768)
                wavefromMmwaveAdValue = wavefromMmwaveAdValue - 65536;

        break;
        case 6:
            rawBmd101WaveformIssueQueue.append(wavefromBmd101AdValue);
            rawMMwaveWaveformIssueQueue.append(wavefromMmwaveAdValue);
            waveformLength++;
            wavefromBmd101AdValue = 0;
            wavefromMmwaveAdValue = 0;
            waveformParseStatus = 0;
            break;
        default:
            waveformParseStatus = 0;
        break;

        }
    }
    return waveformLength;
}

int MainWindow::parseCsvWaveform(QByteArray waveformData)
{
    QString string("");
    string.append(waveformData);
    QStringList adValueStrList = string.split('\n');
    foreach(QString adValueStr, adValueStrList)
        rawMMwaveWaveformIssueQueue.append(adValueStr.toInt());
    return adValueStrList.length();
}

int MainWindow::parseWaveform(QByteArray waveformData, enum FileType waveformFileType)
{

    if(waveformFileType == raw)
        return parseRawWaveform(waveformData);
    else
        return parseCsvWaveform(waveformData);
}

void MainWindow::saveWaveform(QString waveformFileName, QVector<int> mmwaveWaveformAdValue, QVector<int> bmd101WaveformAdValue, bool saveFileInCsvFormat)
{
    QByteArray saveFileContent;
    waveformFileHandle = new QFile(waveformFileName);

    saveFileContent.clear();

    for(int i = 0; i < mmwaveWaveformAdValue.length(); i++) {

        if(saveFileInCsvFormat == true) {
            saveFileContent.append(QString::number(mmwaveWaveformAdValue.at(i)));
            saveFileContent.append(",");
            saveFileContent.append(QString::number(bmd101WaveformAdValue.at(i)));
            saveFileContent.append("\n");
        }
        else {
            saveFileContent.append(static_cast<char>('0xAA'));
            saveFileContent.append(static_cast<char>('0xAA'));
            saveFileContent.append(static_cast<qint16>(mmwaveWaveformAdValue.at(i)));
            saveFileContent.append(static_cast<qint16>(bmd101WaveformAdValue.at(i)));
            saveFileContent.append(static_cast<char>('0xFF'));
        }
    }

    if (waveformFileHandle->open(QIODevice::WriteOnly | QIODevice::Text)) {
        waveformFileHandle->write(saveFileContent);
        waveformFileHandle->close();
    }
}


void MainWindow::on_readWaveformTimer_updated()
{

}

void MainWindow::open_uart(QSerialPort *serialPortHandle, QString pornName, int baudrate)
{

    // 设置串口号
    serialPortHandle->setPortName(pornName);
    // 打开串口
    if(serialPortHandle->open(QIODevice::ReadWrite)) {
        // 设置波特率
        serialPortHandle->setBaudRate(baudrate);
        //设置数据位数
        serialPortHandle->setDataBits(QSerialPort::Data8);
        //设置奇偶校验
        serialPortHandle->setParity(QSerialPort::NoParity);
        // 设置流控制
        serialPortHandle->setFlowControl(QSerialPort::NoFlowControl);
        //设置停止位
        serialPortHandle->setStopBits(QSerialPort::OneStop);
    }
    //打开串口
    else {
        QMessageBox::about(NULL, "提示", "串口无法打开\r\n不存在或已被占用");
        return;
    }
}

int MainWindow::read_uart()
{
    int waveform_length = 0;
    //从缓冲区中读取数据
    QByteArray uart_buffer = dataSerialPortHandle->readAll();
    if(!uart_buffer.isEmpty()) //如果非空说明有数据接收
        waveform_length = parseWaveform(uart_buffer, raw);
    uart_buffer.clear();
    return waveform_length;
}

void MainWindow::on_uartTimer_updated()
{
    waveformLength = read_uart();
    for(int i = 0; i < waveformLength; i++)
        plotWaveform();
}

void MainWindow::establishUdpConnection(int port)
{
    udpRecvSocket = new QUdpSocket(this);
    udpRecvSocket->bind(port);  //端口8080接收
    connect(udpRecvSocket, SIGNAL(readyRead()),this,SLOT(on_udpServer_received()));
}

void MainWindow::on_udpServer_received()
{
    QHostAddress *sourceIP = new QHostAddress();
    quint16 sourcePort;

    while(udpRecvSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        QString dataStr;

        QTextCodec *tutf=QTextCodec::codecForName("UTF-8");  //显示中文
        datagram.resize(udpRecvSocket->pendingDatagramSize());
        udpRecvSocket->readDatagram(datagram.data(),datagram.size(),sourceIP,&sourcePort);    //接收
        dataStr = tutf->toUnicode(datagram);  //接收数据格式化以便于中文显示
//        qDebug() << "receive udp data: " << dataStr;
        int waveform_length = parseWaveform(datagram, raw);
        for(int i = 0; i < waveform_length; i++)
            plotWaveform();
    }
}

void MainWindow::on_startPlotPushButton_clicked()
{
    if(ui->inputSelectFileRadioButton->isChecked()) {
        if(waveformLength == 0) {
            QMessageBox::critical(this, tr("错误"),  tr("未打开波形文件或波形文件格式错误"),
                                  QMessageBox::Accepted,  QMessageBox::Rejected);
            return;
        }
        else
            plotTimer->start(ui->displaySpeedSpinBox->text().toInt());
    }
    else {
        if(ui->inputSelectUartRadioButton->isChecked()) {
            open_uart(dataSerialPortHandle, dataComName, dataComBaudrate);
            if(!dataSerialPortHandle->isOpen()) {
                QMessageBox::critical(this, tr("错误"),  tr("未打开串口"),
                                      QMessageBox::Accepted,  QMessageBox::Rejected);
                return;
            }
            else
                uartTimer->start(1);
        }
        else if(ui->inputSelectEthRadioButton->isChecked()) {
            establishUdpConnection(udp_server_port);
        }
        frameCounter = 0;
        frameCountTimer->start(1000);
    }
    ui->inputSelectFileRadioButton->setEnabled(false);
    ui->inputSelectUartRadioButton->setEnabled(false);
    ui->inputSelectEthRadioButton->setEnabled(false);
    plotStarted = true;
}

void MainWindow::on_pausePlotPushButton_clicked()
{
    if(ui->inputSelectFileRadioButton->isChecked())
        plotTimer->stop();
    else if(ui->inputSelectUartRadioButton->isChecked()) {
        dataSerialPortHandle->close();
        uartTimer->stop();
    }
    else if(ui->inputSelectEthRadioButton->isChecked()) {
        udpRecvSocket->close();
        frameCountTimer->stop();
    }
}

void MainWindow::on_stopPlotPushButton_clicked()
{
    if(plotStarted == true) {
        if(ui->inputSelectFileRadioButton->isChecked())
            plotTimer->stop();
        else if(ui->inputSelectUartRadioButton->isChecked())
            uartTimer->stop();
        else if(ui->inputSelectEthRadioButton->isChecked()) {
            frameCountTimer->stop();
            udpRecvSocket->close();
            udpRecvSocket->deleteLater();
        }
        resetPlotStatus();
        ui->inputSelectFileRadioButton->setEnabled(true);
        ui->inputSelectUartRadioButton->setEnabled(true);
        ui->inputSelectEthRadioButton->setEnabled(true);
        plotStarted = false;
    }
}

void MainWindow::on_openPlotPushButton_clicked()
{
    QString curPath = QDir::currentPath();//获取系统当前目录
    QString dlgTitle="open waveform"; //对话框标题
    QString filter;
    if(ui->openFileFormatComboBox->currentText() == "raw")
        filter ="waveform(*.mwav)";
    else
        filter="waveform(*.csv)";
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this, dlgTitle, curPath, filter);
    if (!fileName.isEmpty()) {
        qDebug() << "open file " <<fileName << endl;
        readWaveform(fileName);
    }
    else
        qDebug() << "error opening file " <<fileName << endl;
}

void MainWindow::on_automaticZoomCheckBox_stateChanged(int arg1)
{
    if(arg1 == 2)
        ui->mmwaveAdValuePlot->xAxis->setRange(plotPoints, 2000, Qt::AlignRight);
}

void MainWindow::on_savePlotPushButton_clicked()
{
    bool saveFileInCsvFormat;
    QString curPath = QDir::currentPath();//获取系统当前目录
    QString dlgTitle="save waveform"; //对话框标题
    QString filter;
    if(ui->saveFileFormatComboBox->currentText() == "raw") {
        filter ="waveform(*.mwav)";
        saveFileInCsvFormat = false;
    }
    else {
        filter="waveform(*.csv)";
        saveFileInCsvFormat = true;
    }
    //打印所有选择的文件的路径
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this, dlgTitle, curPath, filter);
    if (!fileName.isEmpty()) {
        qDebug() << "save file " <<fileName << endl;
        saveWaveform(fileName, rawMMwaveWaveformIssueQueue, rawBmd101WaveformIssueQueue, saveFileInCsvFormat);
    }
    else
        qDebug() << "error saving file " <<fileName << endl;
}

void MainWindow::on_displaySpeedDial_sliderMoved(int position)
{
    plotTimer->setInterval(position);
    ui->displaySpeedSpinBox->setValue(position);
}

void MainWindow::on_zoomXRadioButton_clicked()
{
    ui->mmwaveAdValuePlot->axisRect()->setRangeZoom(Qt::Horizontal);
}

void MainWindow::on_zoomYRadioButton_clicked()
{
    ui->mmwaveAdValuePlot->axisRect()->setRangeZoom(Qt::Vertical);
}

void MainWindow::on_zoomBothRadioButton_clicked()
{
    ui->mmwaveAdValuePlot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
}


void MainWindow::on_displaySpeedSpinBox_valueChanged(const QString &arg1)
{
    ui->displaySpeedDial->setValue(arg1.toInt());
    plotTimer->setInterval(arg1.toInt());
}


void MainWindow::on_inputSelectFileRadioButton_clicked()
{

    ui->serialSettingPushButton->setEnabled(false);
    ui->ethSettingPushButton->setEnabled(false);
    ui->openPlotPushButton->setEnabled(true);
    if(dataSerialPortHandle->isOpen()) {
        if(uartTimer->isActive())
            uartTimer->stop();
        dataSerialPortHandle->close();
    }
    ui->serialSettingPushButton->setEnabled(false);
    ui->displaySpeedDial->setEnabled(true);
    ui->displaySpeedSpinBox->setEnabled(true);
    resetPlotStatus();
    resetWaveformData();
}

void MainWindow::on_inputSelectUartRadioButton_clicked()
{
    ui->serialSettingPushButton->setEnabled(true);
    ui->ethSettingPushButton->setEnabled(false);
    ui->openPlotPushButton->setEnabled(false);
    if(plotTimer->isActive())
        plotTimer->stop();
    ui->displaySpeedDial->setEnabled(false);
    ui->displaySpeedSpinBox->setEnabled(false);
    resetPlotStatus();
    resetWaveformData();
}


void MainWindow::on_inputSelectEthRadioButton_clicked()
{
    ui->serialSettingPushButton->setEnabled(true);
    ui->ethSettingPushButton->setEnabled(true);
    ui->openPlotPushButton->setEnabled(false);
    if(plotTimer->isActive())
        plotTimer->stop();
    ui->displaySpeedDial->setEnabled(false);
    ui->displaySpeedSpinBox->setEnabled(false);
    resetPlotStatus();
    resetWaveformData();
}

void MainWindow::on_ethSettingPushButton_clicked()
{
    QDialog dialog(this);
    QFormLayout form(&dialog);
    dialog.setWindowTitle("网络设置");

    QString ipValue1 = QString("本地主机地址: ");
    QIPAddress *ipWidget1 = new QIPAddress(&dialog);
    form.addRow(ipValue1, ipWidget1);

    // Value2
    QString portValue1 = QString("本地主机端口: ");
    QLineEdit *portLineEdit1 = new QLineEdit(&dialog);

    form.addRow(portValue1, portLineEdit1);

    QString ipValue2 = QString("远程主机地址: ");
    QIPAddress *ipWidget2 = new QIPAddress(&dialog);
    form.addRow(ipValue1, ipWidget2);

    // Value2
    QString portValue2 = QString("远程主机端口: ");
    QLineEdit *portLineEdit2 = new QLineEdit(&dialog);

    form.addRow(portValue1, portLineEdit2);

    // Add Cancel and OK button
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal, &dialog);

    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Process when OK button is clicked
    if (dialog.exec() == QDialog::Accepted) {
        udp_server_port = portLineEdit1->text().toInt();
        if(udp_server_port < 0 || udp_server_port > 65535) {
            QMessageBox::critical(this, tr("错误"),  tr("端口数据错误"),
                                  QMessageBox::Accepted,  QMessageBox::Rejected);
            udp_server_port = 0;
        }
        udp_server_port = portLineEdit2->text().toInt();
        if(udp_server_port < 0 || udp_server_port > 65535) {
            QMessageBox::critical(this, tr("错误"),  tr("端口数据错误"),
                                  QMessageBox::Accepted,  QMessageBox::Rejected);
            udp_server_port = 0;
        }
        udp_server_ipaddr = ipWidget1->getIP();
        udp_server_port = portLineEdit1->text().toInt();
        udp_client_ipaddr = ipWidget2->getIP();
        udp_client_port = portLineEdit2->text().toInt();
        qDebug() << "server ip addr: " << udp_server_ipaddr;
        qDebug() << "server port: " << udp_server_port;
        qDebug() << "client ip addr: " << udp_client_ipaddr;
        qDebug() << "client port: " << udp_client_port;
    }
}

void MainWindow::on_serialSettingPushButton_clicked()
{
    QDialog dialog(this);
    QFormLayout form(&dialog);
    dialog.setWindowTitle("串口设置");

    QString value1 = QString("配置串口: ");
    QComboBox *comboBox1 = new QComboBox(&dialog);
    comboBox1->clear();
    //通过QSerialPortInfo查找可用串口
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
       comboBox1->addItem(info.portName());
    form.addRow(value1, comboBox1);
    // Value2
    QString value2 = QString("波特率: ");
    QComboBox *comboBox2 = new QComboBox(&dialog);
    QStringList baudrateList1;
    baudrateList1<<"9600"<<"19200"<<"38400"<<"57600"
               <<"115200"<<"230400"<<"460800"<<"921600";
    comboBox2->addItems(baudrateList1);
    form.addRow(value2, comboBox2);

    QString value3 = QString("数据串口: ");
    QComboBox *comboBox3 = new QComboBox(&dialog);
    comboBox3->clear();
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
       comboBox3->addItem(info.portName());
    form.addRow(value3, comboBox3);

    QString value4 = QString("波特率: ");
    QComboBox *comboBox4 = new QComboBox(&dialog);
    QStringList baudrateList2;
    baudrateList2<<"9600"<<"19200"<<"38400"<<"57600"
               <<"115200"<<"230400"<<"460800"<<"921600";
    comboBox4->addItems(baudrateList2);
    form.addRow(value4, comboBox4);
    if(ui->inputSelectUartRadioButton->isChecked()) {
        comboBox3->setEnabled(true);
        comboBox4->setEnabled(true);
    }
    else {
        comboBox3->setEnabled(false);
        comboBox4->setEnabled(false);
    }

    // Add Cancel and OK button
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Process when OK button is clicked
    if (dialog.exec() == QDialog::Accepted) {
        if(ui->inputSelectUartRadioButton->isChecked()) {
            if(comboBox1->currentText() == comboBox3->currentText()) {
//                QMessageBox::critical(this, tr("错误"),  tr("配置串口与数据串口冲突"),
//                                      QMessageBox::Accepted,  QMessageBox::Rejected);
//                return;
            }
            dataComName = comboBox3->currentText();
            dataComBaudrate = comboBox4->currentText().toInt();
        }
        configComName = comboBox1->currentText();
        configComBaudrate = comboBox2->currentText().toInt();
    }
}

void MainWindow::on_frameCountTimer_updated()
{
    qDebug() << "frame: " << frameCounter << endl;
    frameCounter = 0;
}

