#ifndef LATTICE
#define LATTICE

#include <QWidget>
#include <QPaintEvent>

class Lattice : public QWidget
{
    Q_OBJECT

public:
    explicit Lattice(QWidget *parent = 0);
    ~Lattice();

    void paintEvent(QPaintEvent *);

    int cN, ccN;
    int *clusterPointer, *clockwisePointer, *counterPointer;
    qreal *spin;

signals:
    int receiveL();
    int receiveClusterL();

public slots:
    void receivePointer(qreal *p, int* clust, int* c, int* cc);
};

#endif // LATTICE

