#ifndef PORODICNOSTABLO_H
#define PORODICNOSTABLO_H
#include <QObject>
#include <vector>
#include <map>
#include <string>
#include <QDate>
#include "engine/osoba.h"
#include "engine/brak.h"
#include "engine/dete.h"
#include <QString>
#include <QFile>
#include <iostream>
#include <QPointF>

/**
 * @brief The PorodicnoStablo class sluzi za pamcenje podataka o povezanosti clanova stabla, kao i njihovo indeksiranje.
 */
class PorodicnoStablo : public QObject
{
    Q_OBJECT
public:
    PorodicnoStablo();
    PorodicnoStablo(const QString &ime, const QString &prezime, const QString &pol, const QDate &rodjenje, const QDate &smrt, bool krvniSrodnik=true);

    ~PorodicnoStablo();//brise apsolutno sve, tako sto prvo raskine sve veze u strukturama, a onda brise redom koristeci vektore sa pokazivacima

    Osoba * KljucnaOsoba();

    //dodaje novu osobu u stablo,ocekuje se da posle poziva sledi i poziv za dodavanje deteta ili braka, da bi stablo bilo povezano u svakom momentu!!!
    short int DodajOsobu(const QString &ime, const QString &prezime, const QChar &pol, const QDate rodjenje, const QDate smrt, bool krvniSrodnik);
    //slicno, samo pravi NN lice
    short int DodajNNLice(bool krvniSrodnik);


    //dodaje relaciju dete, od braka do osobe
    short int DodajDete(const short sifraBraka, const short sifraOsobe, const QString &trivija="");

    //dodaje relaciju brak izmedju dve osobe date siframa
    short int DodajBrak(const short int sifraNaseOsobe, const short int sifraTudjeOsobe, const QString &trivija="");


    Osoba* NadjiOsobuSifrom(const short sifra);

    Dete* NadjiDeteSifrom(const short sifra);

    Brak* NadjiBrakSifrom(const short sifra);


    void UkloniOsobuSifrom(const short sifra);

    void UkloniBrakSifrom(const short sifra);

    void UkloniDeteSifrom(const short sifra);

    //metod koji vraca broj supruznika osobe sa tom sifrom, trebace u GUI-ju
    int osobaImaBrakova(const short sifra);
    //vraca listu sve dece date osobe
    std::vector<short>* ListaDece(const short sifra);
    //vraca max broj supruznika koje neka osoba u stablu ima
    int maxBrakova();
    //vraca listu sifara supruznika
    std::vector<short> *ListaSupruznika(const short sifra);


    std::vector<short>* KomeJeSveRodjendan(const QDate& datum);

    //----GETTERI ZA INDEKSE----//
    std::map<short, Osoba*> &Osobe();
    std::map<short, Brak *> &Brakovi();
    std::map<short, Dete*> &Deca();
    std::multimap<short, short>& OsobaBrak();
    std::multimap<short,short>& BrakDeca();
    std::vector<int> Nivoi();
    //----GETTERI ZA INDEKSE----//
    bool ProcitajFajl(const QString &imeFajla);//citanje fajla
    bool IspisiFajl(const QString &imeFajla);//upisivanje u fajl, tj. cuvanje

    //----GETERI ZA PRETRAGU----//
    std::vector<short>* PretragaPoImenu(const QString& Ime,int tip);
    std::vector<short>* PretragaPoPrezimenu(const QString& Prezime,int tip);
    std::vector<short>* PretragaPoDatumuRodjenja(const QDate& DatRodj,int tip);
    std::vector<short>* PretragaPoDatumuSmrti(const QDate& DatSmrti,int tip);
    std::vector<short>* PretragaPoPolu(const QChar& Pol,int tip);
    //----GETERI ZA PRETRAGU----//
    std::vector<short>* NisuKrvniSrodnici();

    bool jeDeteOd(short sifraPrve, short sifraDruge);

    bool jeRoditeljOd(short sifraPrve,short sifraDruge);

    bool jeSupruznikOd(short sifraPrve, short sifraDruge);

    bool jeBratSestraOd(short sifraPrve,short sifraDruge);

    //nesto za ucitavanje//
    void zapamtiPozicijeOsoba(std::map<short, QPointF> &mapa);
    void zapamtiPozicijeBrakova(std::map<short, QPointF> &mapa);
    std::map<short, QPointF>& vratiPozicijeOsoba();
    std::map<short, QPointF>& vratiPozicijeBrakova();



private:
    std::map<short, QPointF> mapaOsobe;//za ucitavnje
    std::map<short, QPointF> mapaBrakovi;// i iscitavanje

    Osoba *_kljucnaOsoba;//osoba cije se porodicno stablo kreira

    //-------------------INDEKSI------------------------//
    std::multimap<QString, Osoba*> _indeksIme;//OVAJ SE NE AZURIRA< PAZLJIVO KORISTITI

    //-----OVE STALNO AZURIRAMO------//
    //-----SAMO NJIH PISEMO/CITAMO---//
    std::map<short int, Osoba* > _indeksSifraOsobe;//mapa koja vezuje sifru osobe za tu osobu
    std::map<short int, Brak* > _indeksSifraVeza;//mapa koja vezuje sifru braka za taj brak
    std::map<short int, Dete* > _indeksSifraDete;//mapa koja vezuje sifru deteta za konkretan relacioni objekat dete
    //-----OVE STALNO AZURIRAMO------//
    //-----OVE DOLE NE!!!! zato uvek konsultovati prva tri------//
    std::multimap<short int, short int> _indeksOsobaBrak;//mapa koja vezuje sifru osobe sa siframa njenih brakova
    std::multimap<short int, short int> _indeksBrakDeca;//mapa koja vezuje sifru braka sa siframa njegove dece(ali osoba!)
    std::multimap<int, short> _indeksRodjendan;//mapa vezuje dan [1,366] sa sifrom osobe rodjenom tog dana


    //--------------------INDEKSI------------------------//

    void InicijalizujSveStrukture();
    void SpaliCeloStablo();//kao destruktor, unistava sve iz stabla ali ne i njega
    void ObrisiBrakove(short sifra, bool iSupruznike);//ako je krvni srodnik, brise mu supruznika; ako nije, brise bas brak
    void ObrisiDecu(short sifra);//poziva brisanje svakog deteta date osobe


    std::vector<int> _nivoi;

signals:
    void obrisanaOsoba(short sifra);
    void obrisanaVezaDete(short sifra);
    void obrisanaVezaBrak(short sifra);

public Q_SLOTS:
    void azurirajIndeksRodj(const QDate &stari, const QDate &novi, const short sifra);
    std::vector<short> sifreNaPutuOdOsobeDoOsobe(int sifraPocetne,int sifraTrazene);


};

#endif // PORODICNOSTABLO_H
