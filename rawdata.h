#ifndef RAWDATA_H
#define RAWDATA_H

#include <QWidget>
#include "hextostring.h"
namespace Ui {
class RawData;
}

class RawData : public QWidget
{
    Q_OBJECT

public:
    explicit RawData(QWidget *parent = nullptr);
    ~RawData();

private:
    Ui::RawData *ui;

    hexToString convert;
    uint64_t    SendInterval =0;
    QTimer      *Qtime;
    uint64_t    TimeCnt ;
    uint64_t timeLast =0;
    uint64_t timeNow  =0;
    

    QByteArray  hexStrToByteArray(const QString &str);
    char        convertHexChar(char ch) ;
    void        GetPulseData(QByteArray *Send);
    void        GetSineData(QByteArray *Send);
    void        GetSawtoothData(QByteArray *Send);
    void        GetRandomData(QByteArray *Send);
    void        SendFlagUpdata(void);

public:
    void        getCommData();
    bool        GetTxFrame(QByteArray *Str);
    void        showData(QByteArray data);
    uint64_t    GetTimeCnt(void);           // 获取当前时间ms单位

    //bool        GetTxFrame(QString *Str);
    int         SendFlag =0;  // 0 不发送  1 发送方波  2 发送正弦  3 发送锯齿 4 发送随机


private slots:

    void on_UartSend_clicked();
    void on_pushButton_send_clear_clicked();
    void on_pushButton_recv_clear_clicked();
    void on_spinBox_valueChanged(const QString &arg1);
    void on_Pulse_clicked(bool checked);
    void on_Sine_clicked(bool checked);
    void on_Sawtooth_clicked(bool checked);
    void on_Random_clicked(bool checked);
    void on_Close_clicked(bool checked);

    void timeoutProcess(void);
    void on_pushButton_clicked();
};

#endif // RAWDATA_H
