#ifndef _OSOBA_H_
#define _OSOBA_H_ 1

#include <vector>
#include <string>
#include <QString>
#include <QDate>
#include<QDataStream>

class Osoba
{
public:

    /*!
     * \brief Osoba kreira osobu bez ičega, radi učitavanja
     */
    Osoba();

    /*!
     * \brief Osoba kreira N.N. lice
     */
    Osoba(bool krvniSrodnik);

    Osoba(const QString &ime, const QString &prezime, const QChar &pol, const QDate &rodjenje, const QDate &smrt, bool krvniSrodnik);

    /*!
     * \brief Osoba kopira samo podatke, ne i veze
     * \param druga
     */
    Osoba(const Osoba& druga);

    ~Osoba();


    //----geteri i seteri------//
    short int Sifra() const;//vraća šifru

    const QString &Ime() const;//vraća ime

    const QString& Prezime() const;//vraća prezime

    const QString ImePrezime() const;//vraća ime i prezime spojeno

    const QChar& Pol() const;//vraća pol osobe

    const QDate& DatumRodjenja() const; //vraća datum rođenja

    const QDate& DatumSmrti() const; //vraća datum smrti

    bool JeKrvniSrodnik() const; //vraća da li je osoba u krvnom srodstvu sa ključnom osobom

    short SifraRoditeljskeVeze() const;//vraća šifru braka ove osobe

    /*!
     * \brief postaviSledecuSifru postavlja sledeću šifru koja će biti upotrebljavana
     * \param sifra
     */
    static void postaviSledecuSifru(int sifra);

    void PromeniIme(const QString &ime);
    void PromeniPrezime(const QString &prezime);
    void PromeniPol(const QChar &pol);
    void PromeniDatumRodjenja(const QDate &datum);
    void PromeniDatumSmrti(const QDate &datum);
    void PostaviRoditeljskuSifru(const short sifra);
    bool JeNepoznata();
    //----geteri i seteri------//

    //----metodi potrebni za brisanje----//
    /*!
     * \brief VecSeBrisem proverava da li je već pozvan destruktor, da ne bi dva puta oslobađali
     * \return da li je sve prošlo uspešno
     */
    bool VecSeBrisem();

    //----metodi potrebni za brisanje----//

    //----metodi za čitanje i pisanje ----//

    friend QDataStream& operator<<(QDataStream& out,Osoba& osoba);
    friend QDataStream& operator>>(QDataStream& out,Osoba& osoba);

    //----metodi za čitanje i pisanje ----//

    //----raspoređivanje----//
    short Nivo() const;
    void Nivo(short nivo);

private:

    //----osnovni podaci----//
    static short int _sledecaSifra;
    int _sifra;
    bool _nepoznata;
    QString _ime;
    QString _prezime;
    QChar _pol;
    QDate _datumRodjenja;
    QDate _datumSmrti;
    bool _krvniSrodnik;
    short _sifraRoditeljskeVeze;
    bool _vecSeBrisem=false;
    bool _preskociRazvezivanje=false;
    //----osnovni podaci----//



    short _nivo = -1;

};

#endif
