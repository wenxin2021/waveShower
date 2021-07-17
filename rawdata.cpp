#include "rawdata.h"
#include "ui_rawdata.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include "QDebug"

#include    <QTime>
#include    <QCoreApplication>
#include    "math.h"
#include    "QTimer"
#include    "QFile"
#include    "QFileDialog"
#include    "QMessageBox"

extern QSerialPort *serialport;

RawData::RawData(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RawData)
{
    ui->setupUi(this);
    TimeCnt = 0;
    QTimer *Qtime = new QTimer(this);
    connect(Qtime,SIGNAL(timeout() ), this , SLOT(timeoutProcess() ));
    Qtime->start(1);
}

RawData::~RawData()
{
    delete ui;
}

void RawData::showData(QByteArray data)
{
    QString RxStr;
    if(ui->radioButton_number->isChecked() )
    {
        RxStr = convert.byteArrayToHexStr(data);
    }
    else
    {
        RxStr = data;
    }
    RxStr = RxStr.toUpper();
    //   把光标移动到末尾，显示接收的数据
    ui->textEdit_recv->moveCursor(QTextCursor::End);
    ui->textEdit_recv->insertPlainText(RxStr);
}

char RawData::convertHexChar(char ch)
{
    if ((ch >= '0') && (ch <= '9')) {
        return ch - 0x30;
    } else if ((ch >= 'A') && (ch <= 'F')) {
        return ch - 'A' + 10;
    } else if ((ch >= 'a') && (ch <= 'f')) {
        return ch - 'a' + 10;
    } else {
        return (-1);
    }
}


QByteArray RawData::hexStrToByteArray(const QString &str)
{
    QByteArray senddata;
    int hexdata, lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len / 2);
    char lstr, hstr;

    for (int i = 0; i < len;) {
        hstr = str.at(i).toLatin1();
        if (hstr == ' ') {
            i++;
            continue;
        }

        i++;
        if (i >= len) {
            break;
        }

        lstr = str.at(i).toLatin1();
        hexdata =    convertHexChar(hstr);
        lowhexdata = convertHexChar(lstr);

        if ((hexdata == 16) || (lowhexdata == 16)) {
            break;
        } else {
            hexdata = hexdata * 16 + lowhexdata;
        }

        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
    return senddata;
}


uint64_t RawData::GetTimeCnt(void)
{
    return TimeCnt;
}

void RawData::on_UartSend_clicked()
{
    SendFlag =1;
}

bool RawData::GetTxFrame(QByteArray *Str)
{

    if( SendFlag ==1)   // 发送区数据
    {
        SendFlag =0;
        if( ui->SendHexMode->isChecked() )
        {
            const QString Str1 = ui->textEdit_send->toPlainText();
            *Str = convert.hexStrToByteArray(Str1);
            return true;
        }
        else
        {
            QString QStr=  ui->textEdit_send->toPlainText();
            *Str = QStr.toLatin1();
            return true;
        }
    }
    else if( SendFlag ==2)   // 发送方波数据
    {
        timeNow = GetTimeCnt();
        QByteArray Send;
        if( (timeNow - timeLast) > SendInterval)
        {
            timeLast = timeNow;
            GetPulseData(Str);
            return true;
        }
    }
    else if( SendFlag ==3 )  // 发送正弦波数据
    {
        timeNow = GetTimeCnt();
        QByteArray Send;
        if( (timeNow - timeLast) > SendInterval)
        {
            timeLast = timeNow;
            GetSineData(Str);
            return true;
        }
    }
    else if( SendFlag ==4 )   // 发送锯齿波数据
    {
        timeNow = GetTimeCnt();
        QByteArray Send;
        if( (timeNow - timeLast) > SendInterval)
        {
            timeLast = timeNow;
            GetSawtoothData(Str);
            return true;
        }
    }
    else if( SendFlag ==5 )   //
    {
        timeNow = GetTimeCnt();
        QByteArray Send;
        if( (timeNow - timeLast) > SendInterval)
        {
            timeLast = timeNow;
            GetRandomData(Str);
            return true;
        }
    }
    return false;
}

void RawData::GetPulseData(QByteArray *Send)
{
    static int Cnt =0;
    uint16_t Dat = 0;

    Send->resize(8);
    (*Send)[0]=  0x5A;
    (*Send)[1]=  0xA5;
    (*Send)[2]=  Dat/256;
    (*Send)[3]=  Dat%256;
    (*Send)[4]= (Dat+50)/256;
    (*Send)[5]= (Dat+50)%256;
    (*Send)[6]=  0x88;
    (*Send)[7]=  0x88;

    if(Cnt <=50)
    {
        Dat =0;
        (*Send)[2]=  Dat/256;
        (*Send)[3]=  Dat%256;
        (*Send)[4]= (Dat+50)/256;
        (*Send)[5]= (Dat+50)%256;

    }
    else
    {
        Dat =200;
        (*Send)[2]=  Dat/256;
        (*Send)[3]=  Dat%256;
        (*Send)[4]= (Dat+50)/256;
        (*Send)[5]= (Dat+50)%256;
    }
    Cnt++;
    if( Cnt>=100)  Cnt =0;
}

#define Pi  3.1415926
void RawData:: GetSineData(QByteArray *Send)
{
    static int  Cnt =0;
    int         PulseData[100];

    for(int i=0;i<100;i++)
        PulseData[i] = ( sin(Pi*2*i/100.0) )*100 + 100;

   // QByteArray Send;
    Send->resize(8);
    (*Send)[0]= 0x5A;
    (*Send)[1]= 0xA5;
    (*Send)[2]= PulseData[Cnt]/256;
    (*Send)[3]= PulseData[Cnt]%256;
    (*Send)[4]= (PulseData[Cnt]+50)/256;
    (*Send)[5]= (PulseData[Cnt]+50)%256;
    (*Send)[6]= 0x88;
    (*Send)[7]= 0x88;
    Cnt++;
    if( Cnt>=100)  Cnt =0;
}

void RawData:: GetSawtoothData(QByteArray *Send)
{
    static int  Cnt =0;
    int         PulseData[100];

    for(int i=0;i<100;i++)
        PulseData[i] = i*2;

    Send->resize(8);
    (*Send)[0]=0x5A;
    (*Send)[1]=0xA5;
    (*Send)[2]= PulseData[Cnt]/256;
    (*Send)[3]= PulseData[Cnt]%256;
    (*Send)[4]= (PulseData[Cnt]+50)/256;
    (*Send)[5]= (PulseData[Cnt]+50)%256;
    (*Send)[6]=0x88;
    (*Send)[7]=0x88;

    Cnt++;
    if( Cnt>=100)  Cnt =0;
}

void RawData:: GetRandomData(QByteArray *Send)
{
    static int  Cnt =0;
    int         RandomData;


    qsrand( QTime(0,0,0,0).msecsTo(QTime::currentTime()));
    for(int i=0; i<10; i++)
    {
        RandomData = qrand()%200;
    }

    Send->resize(8);
    (*Send)[0]=0x5A;
    (*Send)[1]=0xA5;
    (*Send)[2]= RandomData/256;
    (*Send)[3]= RandomData%256;
    (*Send)[4]= (RandomData+50)/256;
    (*Send)[5]= (RandomData+50)%256;
    (*Send)[6]=0x88;
    (*Send)[7]=0x88;

    Cnt++;
    if( Cnt>=100)  Cnt =0;
}

// 清空发送窗口
void RawData::on_pushButton_send_clear_clicked()
{
    ui->textEdit_send->setText(" ");
}

// 清空接收窗口
void RawData::on_pushButton_recv_clear_clicked()
{
    ui->textEdit_recv->setText(" ");
}

void RawData::SendFlagUpdata(void)
{
    SendInterval = ui->spinBox->value();
    if(SendInterval < 0)
    {
        SendFlag =0;    return;
    }
    if      ( ui->Pulse->isChecked()        )    SendFlag =2;
    else if ( ui->Sine->isChecked()         )    SendFlag =3;
    else if ( ui->Sawtooth->isChecked()     )    SendFlag =4;
    else if ( ui->Random->isChecked()       )    SendFlag =5;
    else                                         SendFlag =0;
}

// 发送周期改变
void RawData::on_spinBox_valueChanged(const QString &arg1)
{
    SendFlagUpdata();
}

void RawData::on_Pulse_clicked(bool checked)
{
    SendFlagUpdata();
}

void RawData::on_Sine_clicked(bool checked)
{
    SendFlagUpdata();
}

void RawData::on_Sawtooth_clicked(bool checked)
{
    SendFlagUpdata();
}

void RawData::on_Random_clicked(bool checked)
{
    SendFlagUpdata();
}

void RawData::on_Close_clicked(bool checked)
{
    SendFlagUpdata();
}

void RawData::timeoutProcess()
{
    TimeCnt++;
}

void RawData::on_pushButton_clicked()
{


    QString name = "./";
    name += QTime::currentTime().toString("hh_mm_ss_zzz");
    name += ".txt";
    QString path = QFileDialog::getSaveFileName(this, tr("保存为"), name, tr("Config Files(*.txt)"));
    QFile   writeFile(path);


    if(writeFile.open(QIODevice::WriteOnly))
    {
        QString str = ui->textEdit_recv->toPlainText();
        writeFile.write(str.toUtf8());
        writeFile.close();
        QMessageBox::information(this,"提示","文件保存成功！");
    }
    else QMessageBox::warning(this,"提示","文件保存失败！");
}
