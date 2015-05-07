#ifndef PORODICNOSTABLO_H
#define PORODICNOSTABLO_H
#include<vector>
#include<map>
#include<string>
#include<QDate>
#include"engine/osoba.h"


/**
 * @brief The PorodicnoStablo class sluzi za pamcenje podataka o povezanosti clanova stabla, kao i njihovo indeksiranje.
 */
class PorodicnoStablo
{
public:
    PorodicnoStablo();
    PorodicnoStablo(std::string& ime, std::string& prezime,char pol,QDate datumRodjenja,QDate datumsmrti,bool krvniSrodnik);

    ~PorodicnoStablo();//brise apsolutno sve, tako sto prvo raskine sve veze u strukturama, a onda brise redom koristeci vektore sa pokazivacima

    Osoba * KljucnaOsoba();


    //dodaje novu osobu u stablo,ocekuje se da posle poziva sledi i poziv za dodavanje deteta ili braka, da bi stablo bilo povezano u svakom momentu!!!
    short int DodajOsobu(std::string ime, std::string prezime, char pol, QDate datumRodjenja, QDate datum_smrti, bool krvniSrodnik);
    //slicno, samo pravi NN lice
    short int DodajNNLice();

    //dodaje relaciju dete, od braka do osobe
    short int DodajDete(short int sifraBraka,short int sifraOsobe,std::string trivija="",QDate* datumUsvajanja=nullptr);

    //dodaje relaciju brak izmedju dve osobe date siframa
    short int DodajBrak(short int sifraNaseOsobe, short int sifraTudjeOsobe, std::string trivija="", QDate* datumUpoznavanja=nullptr,
                        QDate* datumVeze=nullptr, QDate* datumRaskida=nullptr, QDate* datumVeridbe=nullptr, QDate* datumVencanja=nullptr);


    Osoba* NadjiOsobuSifrom(const short sifra);

    Dete* NadjiDeteSifrom(const short sifra);

    Brak* NadjiBrakSifrom(const short sifra);




private:
    Osoba _kljucnaOsoba;//osoba cije se porodicno stablo kreira
    std::vector<Osoba*> _sveOsobe;//vektor sa pokazivacima na sve osobe u stablu
    std::vector<Dete *> _svaDeca;//vektor sa pokazivacima na sve deca relacije u stablu
    std::vector<Brak*> _sveVeze;//vektor sa pokazivacima na sve brak relacije u stablu
    std::map<std::string, std::vector<Osoba*> > _indeksIme;//mapa koja vezuje parove ime, vektor svih osoba sa tim imenom
    std::map<QDate, std::vector<Osoba*> > _indeksRodjenje;//mapa koja vezuje parove datum rodjenja, vektor svih osoba sa tim datumom rodjenja
    std::map<int, std::vector<Osoba*> > _indeksRodjendan;//mapa koja vezuje dan [1,366] u godini, sa osobom kojoj je tog rednog dana u godini rodjendan
    std::map<short int, Osoba* > _indeksSifraOsobe;//mapa koja vezuje sifru osobe za tu osobu
    std::map<short int, Brak* > _indeksSifraVeza;//mapa koja vezuje sifru braka za taj brak
    std::map<short int, Dete* > _indeksSifraDete;//mapa koja vezuje sifru deteta za konkretan relacioni objekat dete

    void InicijalizujSveStrukture();


};

#endif // PORODICNOSTABLO_H
