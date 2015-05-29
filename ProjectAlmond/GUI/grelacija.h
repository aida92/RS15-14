#ifndef GRELACIJA_H
#define GRELACIJA_H

#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <QColor>
#include <QDebug>
#include <QString>
#include <QPointF>

class GRelacija : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    enum { Type = UserType + 2 };
    /** konstruktor prihvata sifru relacije, i pozicije onoga sto spaja (dve osobe, ili brak i dete) **/
    GRelacija(short sifra, QPointF prva, QPointF druga, bool brak);
    ~GRelacija();

    QPainterPath shape() const;

    QRectF boundingRect() const override;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    int type() const;

    short int Sifra() const;
    bool BrakJe() const;

public Q_SLOTS:
    void pomeriPrvu(QPointF prva);
    void pomeriDrugu(QPointF druga);
    void ukloniSeSaScene(short sifra);

signals:
    void pomerilaSe(QPointF);
    void obrisiMe(short);

private:
    short _sifra;
    QPointF _prva, _druga;
    bool _brak;

    void postaviNaSredinu();
};

#endif // GRELACIJA_H
