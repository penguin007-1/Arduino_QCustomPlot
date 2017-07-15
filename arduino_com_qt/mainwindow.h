#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QtSerialPort>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    int timerId;
    QSerialPort *arduino;
    static const uint16_t arduino_uno_vendor_id = 6790;
    static const uint16_t arduino_uno_product_id = 29987;
    QByteArray serialData;
    QString serialBuffer;
    QString parsed_data;
    double value;

protected:
    void timerEvent(QTimerEvent *event);

private slots:
    void readSerial();
};

#endif // MAINWINDOW_H
