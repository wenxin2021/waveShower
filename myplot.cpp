#include "myplot.h"
#include "ui_myplot.h"
#include <cmath>
#include <QTime>
#include <QMessageBox>
#include "hw_timer.h"

//#define Point_sum 1000

MyPlot::MyPlot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyPlot)
{
    CurName[0] = "XX0";
    CurName[0] = "XX1";
    CurNum =2;

    MinYLeft = -150;
    MaxYLeft =  300;
    ShowFlag =2;

    ui->setupUi(this);
    ui->plotData->setInteraction(QCP::iRangeDrag, true); //鼠标单击拖动
    ui->plotData->setInteraction(QCP::iRangeZoom, true); //滚轮滑动缩放


}

MyPlot::~MyPlot()
{
    delete ui;
}


void MyPlot::RePlot(bool bInit)
{
    static  bool  repeat = false;
    int     i,n;
    if(repeat)  return;
    repeat = true;
    if(bInit)
    {
        ui->PointsValue->setText(QString("%1").arg(Points));
        plotWidth = ui->plotWidth->value();
        // 清除曲线
        int count = ui->plotData->graphCount();
        for(i=0;i<count;i++)
        {
            ui->plotData->removeGraph(ui->plotData->graph());
        }

        // 添加数据
        QVector <double>  x;
        QVector <double>  y;
        for(n=0;n<Points;n++)
            x.append((float)n);
        for(i=0;i<CurNum;i++)
        {
            QPen pen;
            pen.setColor(QColor(qSin(i*2+1.2)*80+80, qSin(i*0.6+0)*80+80, qSin(i*0.6+1.5)*80+80));
            pen.setWidth(plotWidth);
            pGraph[i] = ui->plotData->addGraph(ui->plotData->xAxis,ui->plotData->yAxis);

            pGraph[i]->setLineStyle((QCPGraph::LineStyle)4);
            pGraph[i]->setPen(pen);
            y.clear();
            for(n=0;n<Points;n++)
            {
                rxStruct Dat;
                Dat = plotVector.at(n);
                y.append(Dat.Rx[i]);
            }
            pGraph[i]->setData(x, y);
        }
    }
    pGraph[0]->setVisible(ui->radioButton_1->isChecked());
    pGraph[1]->setVisible(ui->radioButton_2->isChecked());
    ui->plotData->xAxis->setRange(MinX,MaxX);
    ui->plotData->replot();
    repeat = false;
}

void MyPlot::Refresh()
{
    if(pGraph[0] ==0)  {qDebug()<< "data  is empty:"; return;}

   qDebug()<< "data Len is:"<<pGraph[0]->dataCount();

    pGraph[0]->setVisible(ui->radioButton_1->isChecked());
    pGraph[1]->setVisible(ui->radioButton_2->isChecked());
    ui->plotData->replot();
}


void MyPlot::  WriteDataInBuf(QVector <rxStruct> Dat)
{
    plotVector = Dat;
    int sizeAll = plotVector.size() ;

    Points = sizeAll;    // 点数
    CurNum = 2;          // 曲线条数2

    // Plot
    MinX = 0;
    MaxX = Points;
    RePlot(true);
}



// 显示历史曲线
void MyPlot::on_ShowLast_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("装入"), "C:/Cfg/", tr("Config Files(*.cfg)"));
    QFile   readFile(path);

    if(readFile.open(QIODevice::ReadOnly))
    {
            QDataStream in(&readFile);
            in.readRawData((char *)(&Points), sizeof(int));
            in.readRawData((char *)(&CurDates[0][0]), Points*4);
            in.readRawData((char *)(&CurDates[1][0]), Points*4);
            plotVector.clear();
            for(int i=0;i<Points;i++)
            {
                rxStruct Dat;
                Dat.Rx[0] = CurDates[0][i];
                Dat.Rx[1] = CurDates[1][i];
                plotVector.append(Dat);
            }
            MaxX = Points;
            readFile.close();
            RePlot(1);
            QMessageBox::information(this, "提示", "数据载入成功！");
    }
    else    QMessageBox::information(this, "提示", "数据载入失败！");
}

//
void MyPlot::on_ShowAll_clicked()
{
    MinX = 0;
    MaxX = Points;
    RePlot(1);
}



void MyPlot::closeFile(void)
{
    sFile.close();
}


// 保存数据
void MyPlot::on_ShowSave_clicked()
{
    if(Points ==0)
    {
       QMessageBox::information(this, "提示", "没有需要保存的数据！");
       return;
    }

    QString name = "./";
    QString strBuffer;
    QDateTime time;
    time = QDateTime::currentDateTime();
    strBuffer = time.toString("yyyy-MM-dd_hh-mm-ss");
    name += strBuffer;

    QString path = QFileDialog::getSaveFileName(this, tr("保存为"), name, tr("Config Files(*.cfg)"));
    QFile   writeFile(path);

    if(writeFile.open(QIODevice::WriteOnly))
    {
        QDataStream out(&writeFile);
        out.writeRawData((char *)(&Points), sizeof(int));
        for(int i=0;i<Points;i++)
        {
            rxStruct Dat = plotVector.at(i);
            CurDates[0][i] = Dat.Rx[0];
            CurDates[1][i] = Dat.Rx[1];
        }
        out.writeRawData((char *)(&CurDates[0][0]), Points*4);
        out.writeRawData((char *)(&CurDates[1][0]), Points*4);

        writeFile.close();
        QMessageBox::information(this, "提示", "cfg数据保存成功！");
    }
    else QMessageBox::information(this, "提示", "cfg数据保存失败！");

}

void MyPlot::on_radioButton_1_clicked(bool checked)
{
    Refresh();
}

void MyPlot::on_radioButton_2_clicked(bool checked)
{
   Refresh();
}



void MyPlot::plotClear()
{
    Points =0;
    plotVector.clear();
    MinX = 0;
    MaxX = 0;

}
// 清除波形
void MyPlot::on_pushButton_clicked()
{
    plotClear();
    RePlot(true);
}


void MyPlot::on_Y_Zoom_clicked(bool checked)
{
    if(checked != true) return;
    ui->X_Zoom->setChecked(false);
    ui->plotData->axisRect()->setRangeZoom(Qt::Vertical);
}

void MyPlot::on_X_Zoom_clicked(bool checked)
{
    if(checked != true) return;
    ui->Y_Zoom->setChecked(false);
    ui->plotData->axisRect()->setRangeZoom(Qt::Horizontal);
}

