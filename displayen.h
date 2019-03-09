#ifndef DISPLAYEN
#define DISPLAYEN

#include <QWidget>
#include <QPaintEvent>

#include "mainwidget.h"

class DisplayEn : public QWidget
{
    Q_OBJECT

public:
    explicit DisplayEn(QWidget *parent = 0);
    ~DisplayEn();

    void paintEvent(QPaintEvent *);

    void newVal(qreal val);

    int index, pointsN;
    qreal points[GRAPH_POINTS];

signals:

public slots:

};

#endif // DISPLAYEN

