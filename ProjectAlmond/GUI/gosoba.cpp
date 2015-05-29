#include "gosoba.h"
#include <QGraphicsScene>

GOsoba::GOsoba(short sifra, QString ime_prezime)
    :_sifra(sifra), _sirina(100), _visina(40), _ime(ime_prezime),_ukljuceniFlegovi(0)
{
    setAcceptHoverEvents(true);
}

GOsoba::~GOsoba()
{

}

QRectF GOsoba::boundingRect() const
{
    qreal d = 1;
    return QRectF(-_sirina/2-d, -_visina/2-d, _sirina+d, _visina+d);
}


void GOsoba::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

   if(!(_ukljuceniFlegovi & SELEKTOVANA))//nije selektovana
   {
       _osnovnaBoja=Qt::darkGreen;

       if(_ukljuceniFlegovi& NEPOZNATA)
       {
           _bojaTeksta=Qt::green;
       }
       else if(_ukljuceniFlegovi& ZENSKA)
       {
           _bojaTeksta=QColor(220,50,50);
       }
       else//muska
       {
            _bojaTeksta=QColor(50,50,220);
       }

       if(_ukljuceniFlegovi& KORENA)
       {
           _osnovnaBoja=QColor(186,122,69);
           _bojaTeksta=(_ukljuceniFlegovi& ZENSKA)?QColor(240,200,200):QColor(200,200,240);
       }



   }
   else// selektovana
   {
       _osnovnaBoja=Qt::green;
       if(_ukljuceniFlegovi& NEPOZNATA)
       {
           _bojaTeksta=Qt::darkGreen;
       }
       else if(_ukljuceniFlegovi& ZENSKA)
       {
           _bojaTeksta=QColor(220,50,50);
       }

       else//muska
       {
            _bojaTeksta=QColor(50,50,220);
       }

       if(_ukljuceniFlegovi& KORENA)
       {
           _osnovnaBoja=QColor(128,76,34);
           _bojaTeksta=(_ukljuceniFlegovi& ZENSKA)?QColor(220,180,180):QColor(180,180,220);
       }
   }
   if(_ukljuceniFlegovi& ODABRANA)
   {
    _osnovnaBoja=Qt::yellow;
   }

    painter->setBrush(_osnovnaBoja);
    QRectF rect(-_sirina/2, -_visina/2, _sirina, _visina);
    painter->setRenderHint(QPainter::Antialiasing,false);
    painter->drawRoundedRect( rect.translated(0.5,0.5), 3.0, 3.0 );
    painter->setPen(_bojaTeksta);
    painter->drawText(rect, Qt::AlignCenter | Qt::TextWordWrap, _ime);
}

int GOsoba::type() const
{
    return Type;
}

void GOsoba::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event)
    dodajStil(SELEKTOVANA);
    update(this->boundingRect());
}

void GOsoba::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event)
    oduzmiStil(SELEKTOVANA);
    update(this->boundingRect());
}

short GOsoba::Sifra() const
{
    return _sifra;
}

void GOsoba::promeniIme(const QString &novoIme)
{
    _ime = novoIme;
}

void GOsoba::obavestiRelacije()
{
    emit pomerilaSe(pos());
}

void GOsoba::dodajStil(Stil stil)
{
    _ukljuceniFlegovi|= stil;
    update(this->boundingRect());

}
void GOsoba::oduzmiStil(Stil stil)
{
    _ukljuceniFlegovi &= ~stil;
    update(this->boundingRect());

}

void GOsoba::skloniSeSaScene(short sifra)
{
    if (_sifra != sifra)
        return;
    if (scene() != 0)
        scene()->removeItem(this);
    deleteLater();
}
