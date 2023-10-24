#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>

namespace Ui {
class MyWidget;
}

class Figure
{
public:
    QList<QPoint> points;
};

class MyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyWidget(QWidget *parent = nullptr);
    ~MyWidget();

    void mousePressEvent( QMouseEvent *event ) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

    bool findPoint( int x, int y, int &fi, int &pi );

    int drag_fi, drag_pi;
    int cfig;
    QPoint start, end;

    int mode;

    QList<Figure*> figs;
    int map[100][100];
    int map_size;
    QPoint* points[100];
    int d[100];
    int path[101];
    int path_l;

    void calc();

    bool checkIntersect( const QPoint &p1, const QPoint &p2 );
    void addLine( int i, int j );
    void doWave( int start );
    bool findPath();
private:
    Ui::MyWidget *ui;
};

bool cw( const QPoint &p, const QPoint &p1, const QPoint &p2 );

#endif // MYWIDGET_H
