#ifndef OKVIRSTABLA_H
#define OKVIRSTABLA_H

#include <QWidget>

namespace Ui {
class okvirStabla;
}

class okvirStabla : public QWidget
{
    Q_OBJECT

public:
    explicit okvirStabla(QWidget *parent = 0);
    ~okvirStabla();
    int X1();
    int X2();
    int Y1();
    int Y2();

signals:
    void kliknut();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    Ui::okvirStabla *ui;
    int _prviX,_prviY,_drugiX,_drugiY;
};

#endif // OKVIRSTABLA_H
