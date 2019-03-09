#include <QWidget>
#include <QPainter>
#include <QDebug>

#include "mainwidget.h"
#include "lattice.h"

Lattice::Lattice(QWidget *parent) :
    QWidget(parent)
{
    cN = 0;
    ccN = 0;
}

Lattice::~Lattice()
{

}

void Lattice::paintEvent(QPaintEvent *)
{
    int i, j, length, clustL;
    qreal len, angle;
    QPainter stift(this);
    QPointF p;

    length = receiveL();
    clustL = receiveClusterL();

    len = WINDOW_HEIGHT/length;

    for(i=0; i<clustL; i++)
    {
        stift.fillRect(clusterPointer[i]%length*len,(int)(clusterPointer[i]/length)*len,len,len,QColor(0,255,0,50));
    }

    for(i=0; i<length; i++)
    {
        for(j=0; j<length; j++)
        {
            angle = spin[i*length+j]*2*PI;
            stift.setPen(QColor(sin(angle)*127.5+127.5,0,sin(angle+PI)*127.5+127.5));

            stift.drawLine(j*len+len/2,i*len+len/2,j*len+cos(angle)*len/2+len/2,i*len+sin(angle)*len/2+len/2);
        }
    }

    stift.setPen(Qt::cyan);
    for(i=0; i<cN; i++)
    {
        p = QPointF(clockwisePointer[i]%length*len+len,(int)(clockwisePointer[i]/length)*len+len);
        stift.drawEllipse(p,len/sqrt(2),len/sqrt(2));
    }
    stift.setPen(Qt::magenta);
    for(i=0; i<ccN; i++)
    {
        p = QPointF(counterPointer[i]%length*len+len,(int)(counterPointer[i]/length)*len+len);
        stift.drawEllipse(p,len/sqrt(2),len/sqrt(2));
    }
}

void Lattice::receivePointer(qreal *p, int *clust, int *c, int *cc)
{
    spin = p;
    clusterPointer = clust;
    clockwisePointer = c;
    counterPointer = cc;
}
