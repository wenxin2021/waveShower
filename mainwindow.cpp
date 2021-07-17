#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hw_timer.h"
#include "QCheckBox"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->baud->setCurrentText("115200");
    MaxPointSet = ui->MaxPointSet->value();
    initComm();
    uartRxVector.clear();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//
void MainWindow::closeEvent(QCloseEvent *event)
 {
    serialport->close();
    QTime reachTime = QTime::currentTime().addMSecs(100);
    while(QTime::currentTime() < reachTime);
    event->accept();
 }

// 串口初始化
void MainWindow::initComm()
{
    serialport = new QSerialPort();
    // 创建信号槽，接收串口数据
    connect(serialport, SIGNAL(readyRead()), this, SLOT(getCommData()));
    ComUpdate();
}

void MainWindow:: ComUpdate(void)
{
    // 初始化下拉控件
    ui->comboBox_sp_name->clear();
    foreach(QSerialPortInfo info, QSerialPortInfo::availablePorts())
    {
        if(!info.isBusy())
        {
            ui->comboBox_sp_name->addItem(info.portName());
        }
    }
}

// 0x5A 0xA5 0x08 00 01 00 02 00 03 00 04 xx
int MainWindow::dataAnalysis(void)
{
    rxStruct  rxTemp;
    while(RxArray.length() >7)
    {
        if((uchar)RxArray[0] != 0x5A)//校验帧头第一个字节AA
        {
            RxArray.remove(0, 1);
            continue;
        }
        else if((uchar)RxArray[1] != 0xA5)//校验帧头第二个字节AA
        {
            RxArray.remove(0, 2);
            continue;
        }
        else
        {
            for(int i=0;i<2;i++)
            {
                uint8_t temp[2] ;
                temp[0] = RxArray[2 + i*2] ;
                temp[1] = RxArray[3 + i*2] ;
                int16_t temp16 = *(uint16_t *)(temp);
                rxTemp.Rx[i] = temp16;
            }
            uartRxVector.append(rxTemp);
            RxArray.remove(0, 8);
        }
    }
    int size = uartRxVector.size();
    if( size > MaxPointSet )
    {
        int sizeDel = size - MaxPointSet;
        uartRxVector.remove(0,sizeDel);
    }
    return size;
}

// 接收数据
// 数据缓存时间
void MainWindow::getCommData()
{
    static  unsigned long           timeNow=0,timeLast=0;

    QByteArray recvByteArray = serialport->readAll();
    unsigned long timeInterval = ui->timeInterval->value();
    RawShow = ui->RawShow->isChecked();

    int len = recvByteArray.length();
    if( len <=0) return ;

    RxArray += recvByteArray;
    timeNow = TimerGetMs();

    if( (timeNow - timeLast) > timeInterval )
    {
        timeLast = timeNow;
        if( RawShow == true)     ui->rawData->showData(RxArray);
        dataAnalysis();        
        ui->plot->WriteDataInBuf(uartRxVector);     // output  数据存储，曲线绘制
    }
}

// 大循环
void MainWindow:: Run(uint8_t Status)
{
    while(Status ==1)
    {
        QCoreApplication::processEvents();   //更新界面显示
        static QByteArray SendStr;
        if( ui->rawData->GetTxFrame(&SendStr) )
        {
            serialport->write(SendStr );
        }
    }
}
/***********slot*******************************/
void MainWindow::on_toolButton_sp_open_clicked()
{
    if(ui->toolButton_sp_open->text() == "打开串口")
    {
        // 配置串口，同时查看是否配置成功
        serialport->setPortName(ui->comboBox_sp_name->currentText());
        qDebug() << serialport->portName() << ui->comboBox_sp_name->currentText();
        int baud = ui->baud->currentText().toInt();
        serialport->setBaudRate(baud);
        serialport->setParity(  QSerialPort::NoParity);
        serialport->setDataBits(QSerialPort::Data8);
        serialport->setStopBits(QSerialPort::OneStop);

        if(serialport->open(QSerialPort::ReadWrite))
        {
            ui->toolButton_sp_open->setText("关闭串口");
            Run(1);
        }
        else
        {
            ui->toolButton_sp_open->setChecked(false);
            QMessageBox::critical(this, "错误", "串口打开失败", "确定");
        }
    }
    else
    {
        serialport->close();
        //ui->plot->plotClear();
        ui->toolButton_sp_open->setText("打开串口");
    }
}


void MainWindow::on_UpdataCom_clicked()
{
    ComUpdate();
}

void MainWindow::on_MaxPointSet_valueChanged(int arg1)
{
    MaxPointSet = arg1;
}
