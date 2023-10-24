#include "mywidget.h"
#include "ui_mywidget.h"
#include <QPainter>
#include <QMouseEvent>
#include "math2.h"

MyWidget::MyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyWidget)
{
    ui->setupUi(this);

    drag_fi = -1;
    drag_pi = -1;
    cfig = -1;

    start = QPoint(10,10);
    end = QPoint(300, 300);

    memset( map, 0, sizeof(map) );
    map_size = 0;
    path_l = 0;
}

MyWidget::~MyWidget()
{
    delete ui;
}

void MyWidget::mousePressEvent( QMouseEvent *event )
{
    int fi, pi;

    if (event->button() == Qt::LeftButton)
    {
        if (mode == 0)
        {
            if (findPoint( event->x(), event->y(), fi, pi ))
            {
                drag_fi = fi;
                drag_pi = pi;

                return;;
            }

            if (cfig < 0)
            {
                Figure *f = new Figure();

                cfig = figs.size();
                figs.append(f);
            }

            figs[cfig]->points.append( QPoint( event->x(), event->y() ) );
        } else if (mode == 1)
        {
            start = QPoint( event->x(), event->y() );
            mode = 2;
        } else if (mode == 2)
        {
            end = QPoint( event->x(), event->y() );
            mode = 0;
        }




        update();
    }
    else if (event->button() == Qt::RightButton)
    {
        cfig = -1;
    }
}

void MyWidget::mouseReleaseEvent(QMouseEvent *event)
{
    drag_fi = -1;
    drag_pi = -1;
}

void MyWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (drag_fi >= 0)
    {
        figs[drag_fi]->points[drag_pi].setX( event->x() );
        figs[drag_fi]->points[drag_pi].setY( event->y() );

        update();
    }
}

void MyWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    if (path_l > 0)
    {
        p.setPen( QPen( QColor(255,0,0), 3 ) );
        for( int i = 0; i < path_l-1; i++ )
        {
            p.drawLine( *points[path[i]], *points[path[i+1]] );
        }
    }

    for( int i = 0; i < map_size; i++ )
    {
        p.setPen( QColor( 192, 192, 192 ) );
        for( int j  = 0; j < map_size; j++ )
            if (map[i][j] > 0)
                p.drawLine( *points[i], *points[j] );
    }

    p.setPen( QColor(0,0,0) );
    for( int i = 0; i < figs.size(); i++ )
    {
        QPoint points[100];

        for( int j = 0; j < figs[i]->points.size(); j++ )
            points[j] = figs[i]->points[j];

        p.drawPolygon( points, figs[i]->points.size() );

        for( int j = 0; j < figs[i]->points.size(); j++ )
        {
            p.drawEllipse( points[j].x()-5, points[j].y()-5, 10, 10 );
        }
    }

    if (mode == 0)
        p.drawText( 10, height() - 15, QString( "Рисуйте многоугольники" ) );
    if (mode == 1)
        p.drawText( 10, height() - 15, QString( "Выберите старт" ) );
    if (mode == 2)
        p.drawText( 10, height() - 15, QString( "Выберите финиш" ) );

    p.setPen( QColor(0,255,0) );
    p.drawEllipse( start.x()-5, start.y()-5, 10, 10 );
    p.drawText( start.x() + 5, start.y() - 5, QString( "Старт" ) );

    p.setPen( QColor(0,0,255) );
    p.drawEllipse( end.x()-5, end.y()-5, 10, 10 );
    p.drawText( end.x() + 5, end.y() - 5, QString( "Финиш" ) );
}

bool MyWidget::findPoint( int x, int y, int &fi, int &pi )
{
    QPoint coord(x,y);

    for( int i = 0; i < figs.size(); i++ )
    {
        for( int j = 0; j < figs[i]->points.size(); j++ )
            if ((figs[i]->points[j] - coord).manhattanLength() <= 5)
            {
                fi = i;
                pi = j;
                return true;
            }
    }

    return false;
}

void MyWidget::calc()
{
    int fig_i[100], fig_pi[100];

    // Подготовка всех точек и карты
    map_size = 1;

    points[0] = &start;
    fig_i[0] = -1;

    for( int i = 0; i < figs.size(); i++ )
    {
        for( int j = 0; j < figs[i]->points.size(); j++ )
        {
            points[map_size] = &figs[i]->points[j];
            fig_i[map_size] = i;
            fig_pi[map_size] = j;

            map_size++;
        }
    }

    points[map_size] = &end;
    fig_i[map_size] = -2;
    map_size++;

    // Вид от старта и финиша
    memset( map, 0, sizeof(map) );
    for( int i = 0; i < map_size; i++ )
    {
        if (i > 0)
        {
            // Пересекаем линию 0-i со всеми полигонами
            if (!checkIntersect( *points[0], *points[i] ))
                addLine( i, 0 );
        }

        if (i < map_size-1)
        {
            // Пересекаем линию 0-i со всеми полигонами
            if (!checkIntersect( *points[map_size-1], *points[i] ))
                addLine( i, map_size-1 );
        }
    }

    // Остальные точки
    for( int i = 1; i < map_size-1; i++ )
        for( int j = i+1; j < map_size-1; j++ )
        {
            // Проверка - линия внутрь фигуры уходит или нет
            Figure *f = figs[ fig_i[i] ];

            int pi2 = (fig_pi[i]+1) % f->points.size();
            QPoint *p2 = &f->points[pi2];

            if (cw( *points[i], *p2, *points[j] ))
                continue;

            if (checkIntersect( *points[i], *points[j] ))
                continue;

            addLine( i, j );
        }

    doWave(0);
    if (!findPath())
        path_l = 0;

}

int sqr( int v )
{
    return v*v;
}

void MyWidget::addLine( int i, int j )
{
    int dist = sqr( points[i]->x() - points[j]->x() ) +
            sqr( points[i]->y() - points[j]->y() );

    map[i][j] = dist;
    map[j][i] = dist;
}

bool MyWidget::checkIntersect( const QPoint &p1, const QPoint &p2 )
{
    // Перебираем все многоугольники
    for( int i = 0; i < figs.size(); i++ )
    {
        // Все грани
        for( int j = 0; j < figs[i]->points.size(); j++ )
        {
            int j2 = j+1;
            if (j2 == figs[i]->points.size())
                j2 = 0;

            float u, v;

            if (!LineLineIntersection( p1, p2, figs[i]->points[j], figs[i]->points[j2], &u, &v ))
                continue;

            if ((0 < u) & (u < 1) & (0 < v) & (v < 1))
                return true;
        }
    }

    return false;
}

bool cw( const QPoint &p, const QPoint &p1, const QPoint &p2 )
{
    QPoint d1, d2;

    d1 = p1 - p;
    d2 = p2 - p;

    return (d1.x() * d2.y() - d1.y() * d2.x()) > 0;
}

// Провести волну
void MyWidget::doWave( int start )
{
    int x, y, c;

    for( int i = 0; i < map_size; i++ )
        d[i] = -1;
    d[ start ] = 0;

    // Пока делаем шаги
    for( c = 1; c > 0; )
    {
        c = 0;

        for( y = 0; y < map_size; y++ )
        { // Если тут были
            if (d[y] < 0) continue;

            // Обойти соседей
            for( x = 0; x < map_size; x++ )
            {
                if (map[x][y] == 0) continue;


                if ( (d[x] < 0) | // У соседа не были
                    (d[y] + map[x][y] < d[x]) ) // Сосед менее оптимален
                { // Сделать шаг
                    d[x] = d[y] + map[x][y];
                    c++;
                }
            }
        }
    }
}

bool MyWidget::findPath()
{
    // Начинаем построение пути по волне
    int i, x2, y2, j, min, minj;
    int pt;

    // Начинаем построение пути по волне

    path[0] = pt = map_size-1;
    path_l = 1;

    for( i = 1; ;i++ )
    {
        if (pt == 0)
            return true;

        // Ищем минимального соседа
        minj = -1;
        min = 0;
        for( j = 0; j < map_size; j++ )
        {
            if (map[j][pt] == 0)
                continue;

            int v = d[j] + map[j][pt];

            if ( (minj < 0) | (min > v) )
            {
                minj = j;
                min = v;
            }
        }

        // Не должно быть, но все же
        if (minj < 0) return false;

        path[path_l++] = minj;

        pt = minj;
    }
}
