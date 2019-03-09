#include <QWidget>
#include <QPainter>
#include <QDebug>

#include "mainwidget.h"
#include "displaymag.h"

DisplayMag::DisplayMag(QWidget *parent) :
    QWidget(parent)
{
    index = 0;
    pointsN = 0;
}

DisplayMag::~DisplayMag()
{

}

void DisplayMag::paintEvent(QPaintEvent *)
{
    int i;
    qreal w, h;
    QPainter stift(this);
    QString s;

    w = WINDOW_WIDTH/3-10;
    h = WINDOW_HEIGHT*2/9-40;

    stift.setPen(QColor(0,0,0,50));
    for(i=0; i<10; i++) stift.drawLine(10,i*h/10,10+w,i*h/10);

    stift.setPen(Qt::black);
    stift.drawRect(10,0,w,h);

    stift.setPen(Qt::darkGreen);
    s.setNum(points[index-1]);
    s.prepend("Magnetization Density = ");
    stift.drawText(0,h,w+10,20,Qt::AlignCenter,s);

    stift.setPen(Qt::black);
    stift.drawText(0,0,10,20,Qt::AlignTop,"1");
    stift.drawText(0,h-20,10,20,Qt::AlignBottom,"0");

    for(i=0; i<pointsN; i++)
    {
        stift.fillRect(w-i*w/GRAPH_POINTS+10-1,(1-points[(index-1-i+GRAPH_POINTS)%GRAPH_POINTS])*h-1,2,2,Qt::darkGreen);
    }
}

void DisplayMag::newVal(qreal val)
{
    points[index] = val;

    index = (index+1)%GRAPH_POINTS;
    pointsN = fmin(100,pointsN+1);

    update();
}
