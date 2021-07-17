#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QMessageBox>
#include "com.h"

namespace Ui {
class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);

private slots:
    void on_toolButton_sp_open_clicked();
    void getCommData();

    void on_UpdataCom_clicked();

    void on_MaxPointSet_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;    
    QByteArray RxArray;
    QVector <rxStruct>  uartRxVector;
    bool    RawShow = false;
    int     MaxPointSet;    //最大点数设定

    void initComm();
    bool isOpen();
    void showData(QString data);
    void Run(uint8_t Status);
    void ComUpdate(void);
    int  dataAnalysis(void);
public:
    QSerialPort *serialport;
};

#endif // MAINWINDOW_H
