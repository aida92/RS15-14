#ifndef STABLO_H
#define STABLO_H

#include <QObject>
#include <QGraphicsView>
#include <QPoint>
#include <QMouseEvent>

class Stablo : public QGraphicsView
{
    Q_OBJECT
public:
    Stablo();
    ~Stablo();
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
signals:
    void kliknut(QPoint pozicija);
    void vucen(QPoint prva, QPoint druga);

private:
    QPoint _prva, _druga;
};

#endif // STABLO_H
