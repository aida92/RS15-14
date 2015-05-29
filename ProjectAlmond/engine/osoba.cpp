#include"engine/osoba.h"
#include <vector>
#include <string>
#include <QDate>
#include<QDataStream>
#include <QString>
#include <iostream>
short int Osoba::_sledecaSifra=0;

Osoba::Osoba()
    :_sifra(),_nepoznata(false),_ime(),_prezime(),
    _pol(),_datumRodjenja(),_datumSmrti(),_krvniSrodnik(true)
{
    _sifraRoditeljskeVeze = -1;
}

Osoba::Osoba(bool krvniSrodnik)
    :_sifra(_sledecaSifra++),_nepoznata(true),_ime("N."),_prezime("N."),
      _pol('?'),_datumRodjenja(),_datumSmrti(),_krvniSrodnik(krvniSrodnik)
{
    _sifraRoditeljskeVeze = -1;
}

Osoba::Osoba(const QString &ime, const QString &prezime, const QChar &pol, const QDate &rodjenje, const QDate &smrt, bool krvniSrodnik)
    :_sifra(_sledecaSifra++),_nepoznata(false),_ime(ime),_prezime(prezime),
      _pol(pol), _datumRodjenja(), _datumSmrti(), _krvniSrodnik(krvniSrodnik)
{
    if (rodjenje.isValid())
        _datumRodjenja.setDate(rodjenje.year(), rodjenje.month(), rodjenje.day());
    if (smrt.isValid())
        _datumSmrti.setDate(smrt.year(), smrt.month(), smrt.day());
    _sifraRoditeljskeVeze = -1;
}

Osoba::Osoba(const Osoba& druga)
    :_sifra(druga._sifra),_nepoznata(druga._nepoznata),_ime(druga._ime),_prezime(druga._prezime),
      _pol(druga._pol),_krvniSrodnik(druga._krvniSrodnik), _sifraRoditeljskeVeze(druga.SifraRoditeljskeVeze())
{}

Osoba::~Osoba()
{
    //std::cout << "brise se osoba " << _ime << std::endl;
    _vecSeBrisem=true;
}


short int Osoba::Sifra()const
{
    return _sifra;
}

const QString& Osoba::Ime() const
{
    return _ime;
}

const QString &Osoba::Prezime() const
{
    return _prezime;
}


const QString Osoba::ImePrezime() const
{
    QString novi(_ime);
    novi.append(" ");
    novi.append(_prezime);
    return novi;
}

const QChar& Osoba::Pol() const
{
    return _pol;
}

const QDate& Osoba::DatumRodjenja() const
{
    return _datumRodjenja;
}

const QDate& Osoba::DatumSmrti() const
{
    return _datumSmrti;
}

bool Osoba::JeKrvniSrodnik() const
{
    return _krvniSrodnik;
}

short Osoba::SifraRoditeljskeVeze() const
{
    return _sifraRoditeljskeVeze;
}

void Osoba::PromeniIme(const QString &ime)
{
    _ime = ime;
}

void Osoba::PromeniPrezime(const QString &prezime)
{
    _prezime = prezime;
}
void Osoba::PromeniPol(const QChar &pol)
{
    _pol = pol;
}

void Osoba::PromeniDatumRodjenja(const QDate &datum)
{
    if (datum.isValid())
        _datumRodjenja.setDate(datum.year(), datum.month(), datum.day());
}

void Osoba::PromeniDatumSmrti(const QDate &datum)
{
    if (datum.isValid())
        _datumSmrti.setDate(datum.year(), datum.month(), datum.day());
}

void Osoba::PostaviRoditeljskuSifru(const short sifra)
{

    _sifraRoditeljskeVeze = sifra;
}

bool Osoba::JeNepoznata()
{
    return _nepoznata;
}

void Osoba::postaviSledecuSifru(int sifra)
{
    _sledecaSifra=sifra;
}

bool Osoba::VecSeBrisem()
{
    return _vecSeBrisem;
}

short Osoba::Nivo() const
{
    return _nivo;
}

void Osoba::Nivo(short nivo)
{
    _nivo = nivo;
}

QDataStream& operator<<(QDataStream &out,Osoba& osoba)
{

    out << (qint32)osoba.Sifra();
    out << osoba._nepoznata;
    out <<  osoba.Ime();
    out << osoba.Prezime();
    out << osoba.Pol();
    out << osoba._datumRodjenja.toString("dd.MM.yyyy.");
    out << osoba._datumSmrti.toString("dd.MM.yyyy.");
    out << osoba._krvniSrodnik;
    out << (qint32)osoba._nivo;
    return out;
}

QDataStream& operator>>(QDataStream &out,Osoba& osoba)
{

    qint32 sif;
    QString datum;
    out >> sif;
    osoba._sifra = (int)sif;
    out >> osoba._nepoznata;
    out >> osoba._ime;
    out >>osoba._prezime;
    out >> osoba._pol;
    out >> datum;
    osoba._datumRodjenja = QDate::fromString(datum, "dd.MM.yyyy.");
    out >> datum;
    osoba._datumSmrti = QDate::fromString(datum, "dd.MM.yyyy.");
    out >> osoba._krvniSrodnik;
    out >> sif;
    osoba._nivo = (short)sif;
    return out;
}

