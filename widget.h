#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include "qcustomplot.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
    void InitPort();
    void InitPlot();
    void ReadPort();

    unsigned char DataCheck(QByteArray val, unsigned char* data);
    void InfoShow(int channel,unsigned char* buff);
    void EcgShow(unsigned char* buff);

private slots:
    void on_pushButtonS_clicked();
    void on_pushButtonCW_clicked();
    void on_pushButtonCR_clicked();
private:
    Ui::Widget *ui;
    QTimer *Timer;
    QSerialPort SerialPort;
    QSerialPortInfo SerialPortInfo;
    int Clk ;
    QCustomPlot *Plot;//画笔
};


#endif // WIDGET_H










