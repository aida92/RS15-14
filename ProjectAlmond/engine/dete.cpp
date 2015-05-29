#include "dete.h"
#include <iostream>
#include <QDataStream>

short int Dete::_sledecaSifra=0;

Dete::Dete()
{}

Dete::Dete(const short sifraOsobe, const short sifraRoditeljskeVeze, const QString &trivija)
    : _sifra(++_sledecaSifra), _sifraOsobe(sifraOsobe), _sifraRoditeljskeVeze(sifraRoditeljskeVeze), _trivija(trivija)
{}

Dete::Dete(const Dete &drugo)
    :_sifra(drugo._sifra),_sifraOsobe(),_sifraRoditeljskeVeze(),_trivija(drugo._trivija)
{}

Dete::~Dete()
{}

short int Dete::Sifra()
{
    return _sifra;
}

short Dete::SifraOsobe()
{
    return _sifraOsobe;
}

short Dete::SifraRoditeljskogOdnosa()
{
    return _sifraRoditeljskeVeze;
}

const QString &Dete::Trivija() const
{
    return _trivija;
}

void Dete::postaviSledecuSifru(int sifra)
{
    _sledecaSifra=sifra;
}

QDataStream& operator<<(QDataStream &out,Dete& dete)
{
    out << qint32(dete._sifra);
    out << qint32(dete._sifraOsobe);
    out << qint32(dete._sifraRoditeljskeVeze);
    out << dete._trivija;
    return out;
}

QDataStream& operator>>(QDataStream &in,Dete& dete)
{
    qint32 a;
    in>>a;
    dete._sifra=(short)a;
    in>>a;
    dete._sifraOsobe=(short)a;
    in>>a;
    dete._sifraRoditeljskeVeze=(short)a;
    in >> dete._trivija;
    return in;
}

