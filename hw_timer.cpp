#include "hw_timer.h"

#include    <QTime>
#include    <QCoreApplication>

unsigned long   TimerGetMs(void)
{
    return QTime::currentTime().msecsSinceStartOfDay();
}

void   TimerDelayMs(unsigned int ms)
{
    unsigned long   time0,tmd;
    time0 = TimerGetMs();
    while(1)
    {
        QCoreApplication::processEvents();
        tmd = TimerGetMs()-time0;
        if(tmd >= ms) break;
    }
}
