#ifndef GOSOBA_H
#define GOSOBA_H

#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <QColor>
#include <QDebug>
#include <QString>

class GOsoba : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    enum { Type = UserType + 1 };

    enum Stil { MUSKA = 0, ZENSKA=1, KORENA = 2, SELEKTOVANA = 4, NASA = 8, NEPOZNATA=16, ODABRANA = 32};

    GOsoba(short int sifra, QString ime_prezime);
    ~GOsoba();

    QRectF boundingRect() const override;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    int type() const;

    void hoverEnterEvent(QGraphicsSceneHoverEvent * event)override;

    void hoverLeaveEvent(QGraphicsSceneHoverEvent * event)override;

    short int Sifra() const;

    void promeniIme(const QString &novoIme);

    void obavestiRelacije();

    void dodajStil(Stil stil);
    void oduzmiStil(Stil stil);

signals:
    void pomerilaSe(QPointF);

public slots:
    void skloniSeSaScene(short sifra);

private:
    short int _sifra;
    int _sirina, _visina;
    QString _ime;
    QColor _osnovnaBoja = Qt::darkGreen;
    QColor _bojaTeksta = Qt::green;
    unsigned _ukljuceniFlegovi;
};

#endif // GOSOBA_H


