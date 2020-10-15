#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <iostream>
#include "dataprocess.h"

using namespace std;
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    Timer = new QTimer(this);
    InitPort();
    InitPlot();
    QTimer::singleShot(80,this,&Widget::ReadPort);
    Clk = 0;
}

Widget::~Widget()
{
    delete ui;
}

void Widget::InitPort()
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite))
        {
            ui->comboBoxCom->addItem(info.portName());
        }
    }
}

void Widget::InitPlot()
{
    Plot = ui->widget;
    Plot->addGraph();

    Plot->plotLayout()->insertRow(0);
    Plot->plotLayout()->addElement(0,0,new QCPTextElement(Plot,"心电图",QFont("新宋体",12,QFont::Bold)));
    Plot->graph(0)->setPen(QPen(Qt::green));

    Plot->xAxis->setRange(0,10);
    Plot->yAxis->setRange(-50,50);
    Plot->xAxis->setNumberPrecision(10);
    Plot->yAxis->setNumberPrecision(1);

    Plot->xAxis->grid()->setSubGridVisible(true);
    Plot->yAxis->grid()->setSubGridVisible(true);

    Plot->axisRect()->setupFullAxesBox();
    Plot->axisRect()->setBackground(QBrush(Qt::black));

    Plot->xAxis->setTickLabels(false);
    Plot->yAxis->setTickLabels(false);
}

void Widget::ReadPort()
{
    QTimer::singleShot(40,this,&Widget::ReadPort);
    if(SerialPort.isOpen() == false)return;
    QByteArray info = SerialPort.readAll();
    if(info.isEmpty()) {return;}
    QByteArray hexData = info.toHex();
    ui->textEditR->append(hexData);
    if(hexData.isEmpty()){return;}
    int channel;
    unsigned char buff[8];
    channel=DataCheck(info,buff);
    InfoShow(channel,buff);
}

unsigned char Widget::DataCheck(QByteArray val, unsigned char* data)
{
    //unsigned char ch = (unsigned char)val[2];
    int len=val.size();
    for(int x=0;x<len;x++)
    {
        unsigned char ch = (unsigned char)val[x];
        if((ch == 0xaa)&(val[x+1]==0xbb))
        {
             if((val[x+11] == 0xee)&(val[x+12]==0xee))
             { 
                 for(int i=x+3;i<x+11;++i){
                   *data = (unsigned char)val[i];
                   data++;
                 }
                return (unsigned char)val[x+2];
             }
        }
        else
            continue;
    }
}

void Widget::InfoShow(int channel,unsigned char* buff)
{
    switch (channel)
    {
        case 1:
        {
            float rate = heart_rate_process(buff);
            char rate_str[5];
            sprintf(rate_str,"%3.1f",rate);
            ui->lineEdit_6->setText(QString(rate_str));
            EcgShow(buff);
        }
        break;
    case 2:
    {
        float T=temprature_process(buff);
        char str[5];
        sprintf(str,"%3.3f",T);
        ui->lineEdit_5->setText(QString(str));
    } break;
    case 3:
    {
        long Step=step_process(buff);
        ui->lineEdit_4->setText(QString::number(Step));

        float dis= (float)Step*0.9;
        char str_dis[5];
        sprintf(str_dis,"%3.3f",dis);
        ui->lineEdit_3->setText(QString(str_dis)); //距离
    } break;
    default:
            break;
    }
    /*
    if(channel == 1) //心率
    {
        int rate = heart_rate_process(buff);
        ui->lineEdit_6->setText(QString::number(rate));
        EcgShow(buff);
    }else if(channel == 2){  //温度
        float T=temprature_process(buff);
        char str[5];
        sprintf(str,"%3.3f",T);
        ui->lineEdit_5->setText(QString(str));
    }else if(channel == 3){ //步数
        long Step=step_process(buff);
        ui->lineEdit_4->setText(QString::number(Step));

        float dis= (float)Step*0.9;
        char str_dis[5];
        sprintf(str_dis,"%3.3f",dis);
        ui->lineEdit_3->setText(QString(str_dis)); //距离
    }
    */
}


void Widget::EcgShow(unsigned char* buff)
{
    unsigned int adc=(unsigned int)((buff[0]<<24)+(buff[1]<<16)+(buff[2]<<8)+buff[3]);
    unsigned int dc=(unsigned int)((buff[4]<<24)+(buff[5]<<16)+(buff[6]<<8)+buff[7]);

//    qDebug()<<"adc:"<<adc;
//     qDebug()<<"dc:"<<dc;
    int temp;
    temp=adc;
    temp<<=8;
    temp>>=8;
    int temp1;
    temp1=dc;
    temp1<<=8;
    temp1>>=8;
//    qDebug()<<"temp:"<<temp;
//    qDebug()<<"temp1:"<<temp1;
    static QTime time(QTime::currentTime());
    double key = time.elapsed()/1000.0; // 开始到现在的时间，单位秒
    static double lastPointKey = 0;

    double disdata=(double)((temp-temp1)*8000.0)/8388607;
   //  qDebug()<<"disdata"<<disdata;
    if (key-lastPointKey > 0.01) // 大约2ms添加一次数据
    {
      //Plot->graph(0)->addData(key, qSin(key)+qrand()/(double)RAND_MAX*1*qSin(key/0.3843));
      Plot->graph(0)->addData(key,disdata);
      //记录当前时刻
      lastPointKey = key;
    }
    // 曲线能动起来的关键在这里，设定x轴范围为最近20个时刻
    Plot->xAxis->setRange(key, 20, Qt::AlignRight);
    //绘图
    Plot->replot();
}

void Widget::on_pushButtonS_clicked() //串口开启
{
    if(ui->pushButtonS->text() == "开启串口")
    {
        SerialPort.setPortName(ui->comboBoxCom->currentText());
        if(SerialPort.open(QIODevice::ReadOnly))
        {
            SerialPort.setBaudRate(ui->comboBoxBaut->currentText().toInt());
            SerialPort.setDataBits(QSerialPort::Data8);
            SerialPort.setParity(QSerialPort::NoParity);
            SerialPort.setFlowControl(QSerialPort::NoFlowControl);
            SerialPort.setStopBits(QSerialPort::OneStop);
            ui->pushButtonS->setText("关闭串口");
        }
        else{
            qDebug() << "串口没有打开";
            return;
        }
    }else if(ui->pushButtonS->text() == "关闭串口")
        {
        SerialPort.close();
        ui->pushButtonS->setText("开启串口");
    }
    else{
        qDebug() <<"无效操作";
    }
}

void Widget::on_pushButtonCR_clicked()//刷新
{
    ui->comboBoxCom->clear();
    InitPort();
}


void Widget::on_pushButtonCW_clicked() //清除
{
    ui->textEditR->clear();
    Clk = 0;
}























