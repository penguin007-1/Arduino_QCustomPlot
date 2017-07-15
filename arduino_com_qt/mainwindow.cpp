/*
    This project demonstrates:
    1. how to scan COM part and configure COM port.
    2. how to set up GUI for plotting real time data.

    Note:
    1. add  "serialport" and "printsupport" in *.pro project file.
    Because the serial port module and a third party module called
    "QCustomPlot" is used. More information about QCustomPlot can be
    found at: http://www.qcustomplot.com/
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*****************************************************************************************
                            Scan COM Port and Setup UART Communication
    *****************************************************************************************/
    //Use the code below to find vendor ID and product ID of the arduino board
    /*qDebug() << "Number of available ports: " << QSerialPortInfo::availablePorts().length();

    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        qDebug() << "Has vendor ID: " << serialPortInfo.hasVendorIdentifier();
        if(serialPortInfo.hasVendorIdentifier()){
            qDebug() << "Vendor ID: " << serialPortInfo.vendorIdentifier();
        }
        qDebug() << "Has Product ID: " << serialPortInfo.hasProductIdentifier();
        if(serialPortInfo.hasProductIdentifier()){
            qDebug() << "Product ID: " << serialPortInfo.productIdentifier();
        }
    }*/

    arduino = new QSerialPort;
    bool arduino_is_available = false;
    QString arduino_port_name = "";

    foreach(const QSerialPortInfo &serialPortInfo,QSerialPortInfo::availablePorts()){
        if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()){
            if(serialPortInfo.vendorIdentifier() == arduino_uno_vendor_id){
                if(serialPortInfo.productIdentifier() == arduino_uno_product_id){
                    arduino_port_name = serialPortInfo.portName();
                    arduino_is_available = true;
                }
            }
        }
    }

    if(!arduino_is_available)
        QMessageBox ::warning(this, "Port error", "couldn't find arduino" );

    if(arduino_is_available){
        //open and configure the serialport
        arduino->setPortName(arduino_port_name);
        arduino->open(QSerialPort::ReadWrite);
        arduino->setBaudRate(QSerialPort::Baud115200);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        arduino->setFlowControl(QSerialPort::NoFlowControl);

        QObject::connect(arduino, SIGNAL(readyRead()), this, SLOT(readSerial()));
    }

    /****************************************************************************************
                             Setup GUI For Real Time Plotting
   *****************************************************************************************/
    ui->plot1->addGraph(); // blue line
    ui->plot1->graph(0)->setPen(QPen(Qt::blue));
    ui->plot1->graph(0)->setName("value a");
    ui->plot1->xAxis->setLabel("time");
    ui->plot1->yAxis->setLabel("value");

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->plot1->xAxis->setTicker(timeTicker);

    ui->plot1->yAxis->setRange(-150, 150);
    ui->plot1->legend->setVisible(true); //show legend

    timerId = startTimer(10);
    /*if(!timerId)
        qDebug() << "Timer could not start" << endl;
    else
        qDebug() << "Timer starts now" << endl;*/
}

MainWindow::~MainWindow()
{
    if(arduino->isOpen()){
        arduino->close();
    }

    killTimer(timerId);
    delete ui;
}

void MainWindow::readSerial(){
    /*
         * readyRead() doesn't guarantee that the entire message will be received all at once.
         * The message can arrive split into parts.  Need to buffer the serial data and then parse for the temperature value.
         *
         */
    QStringList buffer_split = serialBuffer.split(","); //  split the serialBuffer string, parsing with ',' as the separator

    //arduino->setReadBufferSize(1);

    //  Check to see if there less than 2 tokens in buffer_split.
    //  If there are at least 2 then this means there were 1 commas,
    if(buffer_split.length() < 2){
        // no parsed value yet so continue accumulating bytes from serial in the buffer.
        serialData = arduino->read(1); //read only one byte each time
        serialBuffer = serialBuffer + QString::fromStdString(serialData.toStdString());
        serialData.clear();
    }else{
        serialBuffer = "";
        parsed_data = buffer_split[0]; //the first element
        value = parsed_data.toDouble();
        //qDebug() << "Get angle: " << angleGet << "\n";
    }
}

void MainWindow::timerEvent(QTimerEvent *event){

    static QTime time(QTime::currentTime());
    double key = time.elapsed() / 100.0; // time elapsed since start of demo, in seconds

    static double lastPointKey = 0;
    if (key - lastPointKey > 0.001) // at most add point every 1 ms
    {
        // add data to lines:
        ui->plot1->graph(0)->addData(key, value);
        //qDebug() << "x: " << key << "y: " << value << endl;

        lastPointKey = key;
    }
    // make key axis range scroll with the data (at a constant range size of 8):
    ui->plot1->xAxis->setRange(key, 8, Qt::AlignRight);
    ui->plot1->replot();
}











