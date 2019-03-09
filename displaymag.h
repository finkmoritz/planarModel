#ifndef DISPLAYMAG
#define DISPLAYMAG

#include <QWidget>
#include <QPaintEvent>

#include "mainwidget.h"

class DisplayMag : public QWidget
{
    Q_OBJECT

public:
    explicit DisplayMag(QWidget *parent = 0);
    ~DisplayMag();

    void paintEvent(QPaintEvent *);

    void newVal(qreal val);

    int index, pointsN;
    qreal points[GRAPH_POINTS];

signals:

public slots:

};

#endif // DISPLAYMAG

