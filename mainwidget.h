#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPaintEvent>

#include <cmath>

#include <gsl/gsl_rng.h>

#define PI 3.14159265

#define J 1

#define WINDOW_WIDTH 768
#define WINDOW_HEIGHT 512

#define MIN_L 4
#define MAX_L 128

#define GRAPH_POINTS 100

class QGridLayout;
class QLabel;
class QSlider;
class QPushButton;
class QButtonGroup;
class QCheckBox;

class Lattice;
class DisplayMag;
class DisplayEn;

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

    void paintEvent(QPaintEvent *);

    void vortices();

    int right_neighbour(int i, int len);
    int left_neighbour(int i, int len);
    int upper_neighbour(int i, int len);
    int lower_neighbour(int i, int len);

    qreal dist(qreal position1, qreal position2);
    qreal translate(qreal position, qreal dx);

    bool vortex;
    int L, interval;
    int clusterlength, t;
    int clockwiseN, counterClockwiseN;
    int cluster[MAX_L*MAX_L];
    int clockwise[MAX_L*MAX_L], counterClockwise[MAX_L*MAX_L];
    qreal T, dT, tolerance;
    qreal spins[MAX_L*MAX_L];

    gsl_rng *rng;

    QTimer *clock;
    QGridLayout *mainLayout;
    QLabel *tempLabel, *sizeLabel, *intervalLabel, *vortexLabel;
    QSlider *tempSlider, *sizeSlider, *intervalSlider;
    QPushButton *coolButton, *holdButton, *heatButton;
    QPushButton *startButton, *playButton, *nextButton;
    QButtonGroup *tempButtonGroup;
    QCheckBox *vortexCheck;

    Lattice *latticeWidget;
    DisplayMag *magWidget;
    DisplayEn *enWidget;

signals:
    void sendPointer(qreal *p, int *clust, int *c, int *cc);

public slots:
    void initialize();
    void updateLattice();
    int sendL();
    int sendClusterL();
    void Magnetization_Density();
    void Energy_Density();
    void tempChanged(int temperature);
    void dTchanged();
    void sizeChanged(int size);
    void intervalChanged(int dt);
    void vortexChanged();
    void newClicked();
    void playClicked();
};

#endif // WIDGET_H
