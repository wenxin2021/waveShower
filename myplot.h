#ifndef MYPLOT_H
#define MYPLOT_H

#include <QWidget>
#include <QRadioButton>
#include "qcustomplot.h"
#include <QVector>
#include "com.h"

#define  MaxPoints  20000
#define  MaxCurves  (2)



namespace Ui {
class MyPlot;
}

class MyPlot : public QWidget
{
    Q_OBJECT

public:
    explicit MyPlot(QWidget *parent = nullptr);
    ~MyPlot();

    QVector <rxStruct>  plotVector;
    float   CurDates[MaxCurves][MaxPoints];  //

    bool    CurVisable[MaxCurves];
    QString CurName[MaxCurves];


    int     Points;     // 曲线点数
    int     CurNum;     // 曲线条数
    double   Rx_Y_Max;
    double   Rx_Y_Min;

    int     plotWidth;  // 线条宽度
    int     MaxX, MinX;
    int     MaxYLeft, MinYLeft;
    int     ShowFlag =1;
    int     ShowCnt  =0;

    void   WriteDataInBuf(QVector <rxStruct> Dat);
    void    PositionChanged(void);
    void    Refresh();
signals:
    void saveSignal(void);     //自定信号

private slots:

    void on_ShowSave_clicked();
    void on_ShowLast_clicked();
    void on_radioButton_1_clicked(bool checked);
    void on_radioButton_2_clicked(bool checked);
    void on_ShowAll_clicked();
    void on_pushButton_clicked();
    void on_Y_Zoom_clicked(bool checked);
    void on_X_Zoom_clicked(bool checked);

private:
    Ui::MyPlot *ui;
    QCPGraph   *pGraph[MaxCurves];

    QFile sFile;
    QList<QString> list;
    QCPItemText *TextTip;
    void plotClear();

    void    RePlot(bool bInit);
    void    test(void);

    void    closeFile(void);

};

#endif // MYPLOT_H
