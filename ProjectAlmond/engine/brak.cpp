#include "brak.h"
#include <iostream>
#include <QDataStream>

short int Brak::_sledecaSifra=0;

Brak::Brak()
{}

Brak::Brak(const short sifraNase, const short sifraTudje, const QString &trivija)
    : _sifra(++_sledecaSifra), _sifraNase(sifraNase), _sifraTudje(sifraTudje), _trivija(trivija)
{}

Brak::Brak(const Brak &drugi)
    :_sifra(drugi._sifra),_sifraNase(),_sifraTudje(),_trivija(drugi._trivija)
{
}

Brak::~Brak()
{

}

short int Brak::Sifra()
{
    return _sifra;
}

short Brak::SifraNase()
{
    return _sifraNase;
}
short Brak::SifraTudje()
{
    return _sifraTudje;
}

const QString &Brak::Trivija() const
{
    return _trivija;
}


void Brak::postaviSledecuSifru(int sifra)
{
    _sledecaSifra=sifra;
}

short Brak::Nivo() const
{
    return _nivo;
}

void Brak::Nivo(short nivo)
{
    _nivo = nivo;
}

QDataStream& operator<<(QDataStream &out,Brak& brak)
{
    out << qint32(brak._sifra);
    out << qint32(brak.SifraNase());
    out << qint32(brak.SifraTudje());
    out << brak._trivija;
    out << qint32(brak._nivo);
    return out;
}


QDataStream& operator>>(QDataStream &in,Brak& brak)
{
    qint32 broj;
    in >> broj;
    brak._sifra=(short)broj;
    in >> broj;
    brak._sifraNase=(short)broj;
    in >> broj;
    brak._sifraTudje=(short)broj;
    in >> brak._trivija;
    in >> broj;
    brak._nivo = short(broj);
    return in;
}

