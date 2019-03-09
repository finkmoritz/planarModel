#include <QWidget>
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QTimer>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QButtonGroup>
#include <QCheckBox>

#include "mainwidget.h"
#include "lattice.h"
#include "displaymag.h"
#include "displayen.h"

#include <gsl/gsl_rng.h>
#include <time.h>
#include <cmath>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent)
{
    latticeWidget = new Lattice(this);
    connect(this,SIGNAL(sendPointer(qreal*,int*,int*,int*)),latticeWidget,SLOT(receivePointer(qreal*,int*,int*,int*)));
    connect(latticeWidget,SIGNAL(receiveL()),this,SLOT(sendL()));
    connect(latticeWidget,SIGNAL(receiveClusterL()),this,SLOT(sendClusterL()));

    magWidget = new DisplayMag(this);
    enWidget = new DisplayEn(this);

    tempLabel = new QLabel("Temperature = 0.8 (+0)",this);
    tempLabel->setAlignment(Qt::AlignCenter);

    sizeLabel = new QLabel("Lattice size = 64x64",this);
    sizeLabel->setAlignment(Qt::AlignCenter);

    intervalLabel = new QLabel("Update every 500 milliseconds",this);
    intervalLabel->setAlignment(Qt::AlignCenter);

    vortexLabel = new QLabel("Vortex centers = 0",this);
    vortexLabel->setAlignment(Qt::AlignCenter);

    tempSlider = new QSlider(Qt::Horizontal,this);
    tempSlider->setValue(80);
    tempSlider->setMinimum(1);
    tempSlider->setMaximum(200);
    tempSlider->setSingleStep(1);
    tempSlider->setTickPosition(QSlider::TicksAbove);
    tempSlider->setTickInterval(10);
    tempSlider->setPageStep(10);
    connect(tempSlider,SIGNAL(valueChanged(int)),this,SLOT(tempChanged(int)));

    sizeSlider = new QSlider(Qt::Horizontal,this);
    sizeSlider->setValue(6);
    sizeSlider->setMinimum(1);
    sizeSlider->setMaximum(7);
    sizeSlider->setSingleStep(1);
    sizeSlider->setTickPosition(QSlider::TicksAbove);
    sizeSlider->setTickInterval(1);
    sizeSlider->setPageStep(1);
    connect(sizeSlider,SIGNAL(valueChanged(int)),this,SLOT(sizeChanged(int)));

    intervalSlider = new QSlider(Qt::Horizontal,this);
    intervalSlider->setValue(5);
    intervalSlider->setMinimum(1);
    intervalSlider->setMaximum(6);
    intervalSlider->setSingleStep(1);
    intervalSlider->setTickPosition(QSlider::TicksAbove);
    intervalSlider->setTickInterval(1);
    intervalSlider->setPageStep(1);
    connect(intervalSlider,SIGNAL(valueChanged(int)),this,SLOT(intervalChanged(int)));

    vortexCheck = new QCheckBox("on/off");
    connect(vortexCheck,SIGNAL(pressed()),this,SLOT(vortexChanged()));

    coolButton = new QPushButton("Cool",this);
    coolButton->setStyleSheet("QPushButton {color: darkBlue;}");
    coolButton->setCheckable(true);
    connect(coolButton,SIGNAL(clicked()),this,SLOT(dTchanged()));

    holdButton = new QPushButton("Hold",this);
    holdButton->setCheckable(true);
    holdButton->setChecked(true);
    connect(holdButton,SIGNAL(clicked()),this,SLOT(dTchanged()));

    heatButton = new QPushButton("Heat",this);
    heatButton->setStyleSheet("QPushButton {color: darkRed;}");
    heatButton->setCheckable(true);
    connect(heatButton,SIGNAL(clicked()),this,SLOT(dTchanged()));

    tempButtonGroup = new QButtonGroup(this);
    tempButtonGroup->addButton(coolButton);
    tempButtonGroup->addButton(holdButton);
    tempButtonGroup->addButton(heatButton);

    startButton = new QPushButton("New",this);
    connect(startButton,SIGNAL(clicked()),this,SLOT(newClicked()));

    playButton = new QPushButton("Play/Pause",this);
    playButton->setCheckable(true);
    playButton->setChecked(true);
    connect(playButton,SIGNAL(pressed()),this,SLOT(playClicked()));

    nextButton = new QPushButton("Next",this);
    nextButton->setHidden(true);
    connect(nextButton,SIGNAL(clicked()),this,SLOT(updateLattice()));

    clock = new QTimer(this);
    connect(clock,SIGNAL(timeout()),this,SLOT(updateLattice()));

    mainLayout = new QGridLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);

    mainLayout->addWidget(latticeWidget,0,0,19,6);
    mainLayout->addWidget(magWidget,0,6,4,3);
    mainLayout->addWidget(enWidget,4,6,4,3);
    mainLayout->addWidget(tempLabel,9,6,1,3);
    mainLayout->addWidget(tempSlider,10,6,1,3);
    mainLayout->addWidget(coolButton,11,6,1,1);
    mainLayout->addWidget(holdButton,11,7,1,1);
    mainLayout->addWidget(heatButton,11,8,1,1);
    mainLayout->addWidget(sizeLabel,12,6,1,3);
    mainLayout->addWidget(sizeSlider,13,6,1,3);
    mainLayout->addWidget(vortexCheck,14,8,1,1);
    mainLayout->addWidget(vortexLabel,14,6,1,2);
    mainLayout->addWidget(intervalLabel,16,6,1,3);
    mainLayout->addWidget(intervalSlider,17,6,1,3);
    mainLayout->addWidget(startButton,18,6,1,1);
    mainLayout->addWidget(playButton,18,7,1,1);
    mainLayout->addWidget(nextButton,18,8,1,1);

    vortex = false;
    L = 64;
    T = 0.8;
    dT = 0.0;
    interval = 500;
    tolerance = 0.1;
    t = time(NULL);

    emit sendPointer(spins,cluster,clockwise,counterClockwise);

    initialize();

    clock->setInterval(interval);
    clock->start();
}

MainWidget::~MainWidget()
{
    delete latticeWidget;
    delete magWidget;
    delete enWidget;
    delete tempLabel;
    delete tempSlider;
    delete coolButton;
    delete holdButton;
    delete heatButton;
    delete tempButtonGroup;
    delete sizeLabel;
    delete sizeSlider;
    delete intervalLabel;
    delete intervalSlider;
    delete vortexLabel;
    delete vortexCheck;
    delete startButton;
    delete playButton;
    delete nextButton;
    delete mainLayout;
    delete clock;
}

void MainWidget::paintEvent(QPaintEvent *)
{
    QPainter myPainter(this);
    QPoint p;

    myPainter.setPen(QColor(0,0,0,50));

    p = QPoint(magWidget->x(),magWidget->y());
    myPainter.drawRect(p.x()-1,p.y(),tempLabel->width(),enWidget->y()+enWidget->height()+1);

    p = QPoint(tempLabel->x(),tempLabel->y());
    myPainter.drawRect(p.x()-1,p.y(),tempLabel->width(),coolButton->y()+coolButton->height()-tempLabel->y()+1);

    p = QPoint(sizeLabel->x(),sizeLabel->y());
    myPainter.drawRect(p.x()-1,p.y(),sizeLabel->width(),sizeSlider->y()+sizeSlider->height()-sizeLabel->y()+1);

    p = QPoint(intervalLabel->x(),intervalLabel->y());
    myPainter.drawRect(p.x()-1,p.y(),intervalLabel->width(),intervalSlider->y()+intervalSlider->height()-intervalLabel->y()+1);

    p = QPoint(vortexLabel->x(),vortexLabel->y());
    myPainter.drawRect(p.x()-1,p.y(),intervalLabel->width(),vortexLabel->height()+1);
}

void MainWidget::initialize()
{
    int i;

    long seed = time(NULL);

    rng = gsl_rng_alloc(gsl_rng_mt19937);
    gsl_rng_set(rng,seed);

    for(i=0; i<MAX_L*MAX_L; i++) spins[i] = gsl_rng_uniform(rng);

    latticeWidget->cN = 0;
    latticeWidget->ccN = 0;

    clusterlength = 0;
}

int MainWidget::sendL()
{
    return L;
}

int MainWidget::sendClusterL()
{
    return clusterlength;
}

void MainWidget::tempChanged(int temperature)
{
    QString txt, txt2;

    T = (qreal)temperature/100; // Rundungsfehler !

    txt.setNum(T);
    txt2.setNum(dT);
    txt.prepend("Temperature = ");
    txt.append(" (");
    if(dT >= 0) txt.append("+");
    txt.append(txt2);
    txt.append(")");

    tempLabel->setText(txt);
    tempSlider->setValue(temperature);
}

void MainWidget::dTchanged()
{
    if(holdButton->isChecked())
    {
        dT = 0;
    }
    else if(coolButton->isChecked())
    {
        dT = fmin(dT,0)-0.01;
    }
    else
    {
        dT = fmax(dT,0)+0.01;
    }

    tempChanged((int)(T*100));
}

void MainWidget::sizeChanged(int size)
{
    int i;
    QString txt, txt2;

    L = 2;
    for(i=1; i<size; i++) L *= 2;

    txt2.setNum(L);
    txt = "Lattice size = ";
    txt.append(txt2);
    txt.append("x");
    txt.append(txt2);

    sizeLabel->setText(txt);

    initialize();
    latticeWidget->update();
}

void MainWidget::intervalChanged(int dt)
{
    QString txt;

    if(dt == 1) interval = 25;
    else if(dt == 2) interval = 50;
    else if(dt == 3) interval = 100;
    else if(dt == 4) interval = 250;
    else if(dt == 5) interval = 500;
    else if(dt == 6) interval = 1000;

    txt.setNum(interval);
    txt.prepend("Update every ");
    txt.append(" milliseconds");

    intervalLabel->setText(txt);
    clock->setInterval(interval);
}

void MainWidget::vortexChanged()
{
    if(vortexCheck->isChecked())
    {
        vortex = false;
        vortexLabel->setText("Vortex centers = 0");
    }
    else
    {
        vortex = true;
    }

    vortices();

    latticeWidget->cN = clockwiseN;
    latticeWidget->ccN = counterClockwiseN;

    latticeWidget->update();
}

void MainWidget::newClicked()
{
    initialize();
    latticeWidget->update();
}

void MainWidget::playClicked()
{
    if(nextButton->isHidden())
    {
        clock->stop();
        nextButton->setHidden(false);
        playButton->setChecked(true);
    }
    else
    {
        clock->start();
        nextButton->setHidden(true);
        playButton->setChecked(false);
    }
}

//---------------------------------------------------

int MainWidget::right_neighbour(int i, int len)
{
    if ((i+1)%len == 0) return i+1-len;
    else return i+1;
}

int MainWidget::left_neighbour(int i, int len)
{
    if ((i+1)%len == 1) return i+len-1;
    else return i-1;
}

int MainWidget::upper_neighbour(int i, int len)
{
    if (i < len) return i+len*(len-1);
    else return i-len;
}

int MainWidget::lower_neighbour(int i, int len)
{
    if (i+1 > len*(len-1)) return i-len*(len-1);
    else return i+len;
}

qreal MainWidget::dist(qreal position1, qreal position2)
//Returns the shortest angle between position1 and position2
{
    qreal path;

    if(position2 > position1) path = position2-position1;
    else path = position2-position1+1.0;

    if(path > .5) path = path-1.0;

    return path;
}

qreal MainWidget::translate(qreal position, qreal dx)
//Translates the spin counter clockwise by dx
{
    position += dx;
    if (position > 1) position -= 1.0;
    if (position < 0) position += 1.0;
    return position;
}

//------------------------------------------------------------

void MainWidget::updateLattice()
//Creates and flips a cluster and returns the cluster's size
{
    int i, j, k;
    int nr_new, i_max, start, t0;
    int inCluster, same_area;
    int nbr[4];
    qreal normal_vec;
    qreal distance;
    qreal p; //Probability of adding a spin to the cluster
    qreal s_in; //<seed spin * normal vector>
    qreal s_jn; //<spin_j * normal vector>

    T = fmin(2,fmax(0.01,T+dT));
    const qreal beta = 1/T;
    const qreal twoPi = 2*PI;

    if(T == 0.01 || T == 2)
    {
        dT = 0;
        holdButton->setChecked(true);
    }

    if(dT != 0) tempChanged(T*100);

    cluster[0] = (int)(gsl_rng_uniform(rng)*L*L); //Choose seed spin
    nr_new = 1; //Number of newly added spins
    clusterlength = 1;

    normal_vec = gsl_rng_uniform(rng); //Choose normal vector
    distance = fabs(normal_vec-spins[cluster[0]]); //Angle between normal vector and seed spin

    if(distance >= .25 && distance < .75) //Normal vector and seed spin not in the same hemisphere
    {
        same_area = 0;
    }
    else same_area = 1; //Normal vector and seed spin are in the same hemisphere

    do
    {
        start = clusterlength-nr_new; //Smallest index of a newly added spin
        i_max = nr_new;
        nr_new=0;

        for(i=0; i<i_max; i++) //Look at every new spin
        {
            s_in = cos(twoPi * dist(spins[cluster[start+i]],normal_vec));

            nbr[0] = right_neighbour(cluster[start+i],L);
            nbr[1] = left_neighbour(cluster[start+i],L);
            nbr[2] = upper_neighbour(cluster[start+i],L);
            nbr[3] = lower_neighbour(cluster[start+i],L);

            for(j=0; j<4; j++) //Look at the four neighbours
            {
                inCluster = 0;

                for(k=0; k<clusterlength; k++) if(nbr[j]==cluster[k])
                //Is the neighbour already in the cluster?
                {
                    inCluster = 1; //Neighbour already in cluster
                    break;
                }

                if(!inCluster) //Neighbour not in cluster
                {
                    distance = fabs(normal_vec-spins[nbr[j]]);
                    //Distance between normal vector and neighbour

                    if(((distance<.25||distance>=.75)&&same_area) || (distance>=.25 && distance<.75 && !same_area))
                    // Neighbour is in hemisphere of normal vector and seed spin OR
                    // neighbour is in the other hemisphere and hence in the one of the seed spin
                    {
                        s_jn = cos(twoPi * dist(spins[nbr[j]],normal_vec));
                        p = 1 - exp(-2*beta*J*s_in*s_jn);

                        if(gsl_rng_uniform(rng)<=p)
                        {
                            nr_new++;
                            cluster[clusterlength] = nbr[j]; //Add neighbour to cluster
                            clusterlength++;
                        }
                    }
                }
            }
        }
    } while(nr_new != 0); //Repeat as long as spins were added

    //Flip cluster
    for(i=0; i<clusterlength; i++)
    {
        distance = dist(spins[cluster[i]],normal_vec);
        if(distance>=0) spins[cluster[i]] = translate(spins[cluster[i]],2*(distance-.25)); //Flip spin
        else spins[cluster[i]] = translate(spins[cluster[i]],2*(.25+distance)); //Flip spin
    }


    vortices();

    latticeWidget->cN = clockwiseN;
    latticeWidget->ccN = counterClockwiseN;

    latticeWidget->update();

    t0 = time(NULL);
    if(t0 > t)
    {
        t = t0;
        Magnetization_Density();
        Energy_Density();
    }
}

//--------------------------------------------------------

void MainWidget::Magnetization_Density()
//Returns the absolute value of magnetization per spin
{
    int i;
    const qreal twoPi = 2*PI;
    qreal M, dx, dy, average_spin;

    dx = 0;
    dy = 0;

    for(i=0; i<L*L; i++)
    {
        dx += cos(twoPi*spins[i]);
        dy += sin(twoPi*spins[i]);
    }

    average_spin = atan2(dy,dx)/twoPi;

    M=0;
    for(i=0; i<L*L; i++) M += cos(twoPi*dist(spins[i],average_spin));

    magWidget->newVal(fabs(M/(L*L)));
}

void MainWidget::Energy_Density()
//Returns the energy per spin
{
    int i;
    const qreal twoPi = 2*PI;
    qreal E;

    E=0;
    for (i=0; i<L*L; i++)
    {
        E -= J*cos(twoPi*dist(spins[i],spins[right_neighbour(i,L)]));
        E -= J*cos(twoPi*dist(spins[i],spins[upper_neighbour(i,L)]));
    }

    enWidget->newVal(E/(L*L));
}

void MainWidget::vortices()
{
    bool sameDir;
    int i, dir;
    qreal angle, tmp;
    QString txt;

    clockwiseN = 0;
    counterClockwiseN = 0;

    if(!vortex) return;

    for(i=0; i<L*L; i++)
    {
        angle = 0;

        angle += dist(spins[i], spins[lower_neighbour(i,L)]);
        if(angle >= 0) dir = 1;
        else dir = -1;

        tmp = dist(spins[lower_neighbour(i,L)], spins[right_neighbour(lower_neighbour(i,L),L)]);
        if(dir*tmp >= 0) sameDir = true;
        else sameDir = false;
        angle += tmp;

        tmp = dist(spins[right_neighbour(lower_neighbour(i,L),L)], spins[right_neighbour(i,L)]);
        if(dir*tmp >= 0 && sameDir) sameDir = true;
        else sameDir = false;
        angle += tmp;

        tmp = dist(spins[right_neighbour(i,L)], spins[i]);
        if(dir*tmp >= 0 && sameDir) sameDir = true;
        else sameDir = false;
        angle += tmp;

        if(sameDir)
        {
            if(angle <= 1+tolerance && angle > 1-tolerance)
            {
                counterClockwise[counterClockwiseN++] = i;
            }
            else if(-angle <= 1+tolerance && -angle > 1-tolerance)
            {
                clockwise[clockwiseN++] = i;
            }
        }
    }

    txt.setNum(counterClockwiseN+clockwiseN);
    txt.prepend("Vortex centers = ");

    vortexLabel->setText(txt);
}
