#include "porodicnostablo.h"
#include <QDataStream>
#include <iostream>
#include "alati/trazenjeputa.h"
#include <QMessageBox>

PorodicnoStablo::PorodicnoStablo()
{
    _kljucnaOsoba = nullptr;
    InicijalizujSveStrukture();

}

PorodicnoStablo::PorodicnoStablo(const QString &ime, const QString &prezime, const QString &pol,
                                 const QDate &rodjenje, const QDate &smrt, bool krvniSrodnik)
{
    _kljucnaOsoba = new Osoba(ime, prezime, pol.at(0), rodjenje, smrt, krvniSrodnik);
    _kljucnaOsoba->Nivo(0);

    InicijalizujSveStrukture();
    _nivoi.push_back(1);
    _indeksIme.insert(std::make_pair(ime,_kljucnaOsoba));
    if (rodjenje.isValid())
    {

        _indeksRodjendan.insert(std::make_pair(rodjenje.dayOfYear(), _kljucnaOsoba->Sifra()));
    }
    _indeksSifraOsobe[_kljucnaOsoba->Sifra()]=_kljucnaOsoba;
}

PorodicnoStablo::~PorodicnoStablo()
{
    if (_kljucnaOsoba != nullptr)
        UkloniOsobuSifrom(_kljucnaOsoba->Sifra());
    InicijalizujSveStrukture();

}

Osoba * PorodicnoStablo::KljucnaOsoba()
{
    return _kljucnaOsoba;
}

//dodaje novu osobu u stablo,ocekuje se da posle poziva sledi i poziv za dodavanje deteta ili braka, da bi stablo bilo povezano u svakom momentu!!!
short int PorodicnoStablo::DodajNNLice(bool krvniSrodnik)
{
    Osoba *nova=new Osoba(krvniSrodnik);
    _indeksSifraOsobe[nova->Sifra()]=nova;
    return nova->Sifra();
}
short int PorodicnoStablo::DodajOsobu(const QString &ime, const QString &prezime, const QChar &pol,
                                      const QDate rodjenje, const QDate smrt, bool krvniSrodnik)
{
    Osoba* nova=new Osoba(ime, prezime, pol, rodjenje, smrt, krvniSrodnik);
    _indeksIme.insert(std::make_pair(ime, nova));
    if (rodjenje.isValid())
    {

        _indeksRodjendan.insert(std::make_pair(rodjenje.dayOfYear(), nova->Sifra()));
    }
    _indeksSifraOsobe[nova->Sifra()]=nova;
    return nova->Sifra();
}



//dodaje relaciju dete, od braka do osobe
short int PorodicnoStablo::DodajDete(const short int sifraBraka, const short int sifraOsobe, const QString &trivija)
{
    //osoba je dete iz ovog braka, vec kreirana sa vrednoscu -1 za sifru roditeljskog odnosa, pa joj to postavljamo
    _indeksSifraOsobe[sifraOsobe]->PostaviRoditeljskuSifru(sifraBraka);

    //osobi, tj detetu dodeljujemo nivo_roditelja + 1
    short noviNivo = _indeksSifraVeza[sifraBraka]->Nivo()+1;
    _indeksSifraOsobe[sifraOsobe]->Nivo(noviNivo);
    //i ubacujemo u brojac
    if (_nivoi.size() <= (unsigned)noviNivo)
        _nivoi.push_back(1);
    else
        _nivoi[noviNivo]++;
    Dete* novo=new Dete(sifraOsobe, sifraBraka, trivija);
    _indeksSifraDete[novo->Sifra()]=novo;
    _indeksBrakDeca.insert(std::make_pair(sifraBraka, sifraOsobe));
    return novo->Sifra();
}

//dodaje relaciju brak izmedju dve osobe date siframa
short int PorodicnoStablo::DodajBrak(const short sifraNaseOsobe, const short sifraTudjeOsobe, const QString &trivija)
{
    Brak *novi = new Brak(sifraNaseOsobe, sifraTudjeOsobe, trivija);
    novi->Nivo(_indeksSifraOsobe[sifraNaseOsobe]->Nivo());//treba nam zbog racunanja nivoa deteta
    _indeksSifraVeza[novi->Sifra()]=novi;
    _indeksOsobaBrak.insert(std::make_pair(sifraNaseOsobe, novi->Sifra()));
    _indeksOsobaBrak.insert(std::make_pair(sifraTudjeOsobe, novi->Sifra()));
    return novi->Sifra();
}


Osoba* PorodicnoStablo::NadjiOsobuSifrom(const short sifra)
{
    if(_indeksSifraOsobe.find(sifra)!=_indeksSifraOsobe.end())
        return _indeksSifraOsobe[sifra];
    return nullptr;
}

Dete* PorodicnoStablo::NadjiDeteSifrom(const short sifra)
{
    if(_indeksSifraDete.find(sifra)!=_indeksSifraDete.end())
        return _indeksSifraDete[sifra];
    return nullptr;
}

Brak* PorodicnoStablo::NadjiBrakSifrom(const short sifra)
{
    if(_indeksSifraVeza.find(sifra)!=_indeksSifraVeza.end())
        return _indeksSifraVeza[sifra];
    return nullptr;
}


void PorodicnoStablo::UkloniOsobuSifrom(const short sifra)
{

    std::map<short, Osoba*>::iterator iter;
    if ((iter = _indeksSifraOsobe.find(sifra)) == _indeksSifraOsobe.end())
    {
        //std::cout<<"vec je obrisana"<<std::endl;
        return;
    }
    Osoba *zaBrisanje=_indeksSifraOsobe[sifra];

    if (zaBrisanje->JeKrvniSrodnik())
    {//brisemo njene supruznike
        ObrisiBrakove(sifra, true);
        //i smanjujemo broj ljudi u njenom nivou
        _nivoi[zaBrisanje->Nivo()]--;
        if (_nivoi[zaBrisanje->Nivo()] == 0)
            _nivoi.pop_back();//ako je ovaj nula, moraju i svi posle biti nula
    }
    //to ce obrisati i decu, jer supruznik nece biti krvni srodnik pa ce uci u else granu
    else
    {
        ObrisiDecu(sifra);
        ObrisiBrakove(sifra, false);
    }

    //brise sebe kao dete
    std::map<short, Dete*>::iterator dete_b = _indeksSifraDete.begin();
    std::map<short, Dete*>::iterator dete_e = _indeksSifraDete.end();
    for(; dete_b != dete_e ; dete_b++)
    {
        if (dete_b->second->SifraOsobe() != sifra)
            continue;
        emit obrisanaVezaDete(dete_b->first);
        delete _indeksSifraDete[dete_b->first];
        _indeksSifraDete.erase(dete_b);
    }
    //brisemo je iz indeksa (sifra, osoba)
    _indeksSifraOsobe.erase(iter);

    emit obrisanaOsoba(sifra);

    if (!zaBrisanje->VecSeBrisem())
        delete zaBrisanje;
}

// azurira indekse
void PorodicnoStablo::UkloniBrakSifrom(const short sifra)
{
    if (_indeksSifraVeza.find(sifra) == _indeksSifraVeza.end())
        return;
    //brise brak iz indeksa za nasu osobu (pozvan je ako se brise tudja osoba)
    Brak *brak = _indeksSifraVeza[sifra];
    auto brakovi = _indeksOsobaBrak.equal_range(brak->SifraNase());
    for (auto iter = brakovi.first; iter != brakovi.second; iter++)
    {
        if (iter->second == sifra)
            _indeksOsobaBrak.erase(iter);//ovo znaci da je za krvne srodnike uvek tacna informacija u indeksu
    }
}

void PorodicnoStablo::UkloniDeteSifrom(const short sifra)
{
    delete NadjiDeteSifrom(sifra);
}

int PorodicnoStablo::osobaImaBrakova(const short sifra)
{
    if (_indeksSifraOsobe.find(sifra) == _indeksSifraOsobe.end())
        return -1;
    int count = 0;
    auto brakovi = _indeksOsobaBrak.equal_range(sifra);
    for (auto b = brakovi.first; b != brakovi.second; b++)
    {
        if (_indeksSifraVeza.find(b->second) == _indeksSifraVeza.end())
            continue;
        short suprug = _indeksSifraVeza[b->second]->SifraTudje();
        if (_indeksSifraOsobe.find(suprug) == _indeksSifraOsobe.end())
            continue;
        count++;
    }
    return count;
}

std::vector<short> *PorodicnoStablo::ListaDece(const short sifra)
{
    if (_indeksSifraOsobe.find(sifra) == _indeksSifraOsobe.end())
        return nullptr;

    std::vector<short> *svaDeca = new std::vector<short>();
    auto brakovi = _indeksOsobaBrak.equal_range(sifra);
    for (auto b = brakovi.first; b != brakovi.second; b++)
    {
        if (_indeksSifraVeza.find(b->second) == _indeksSifraVeza.end())
            continue;
        auto deca = _indeksBrakDeca.equal_range(b->second);
        for (auto d = deca.first; d != deca.second; d++)
        {
            if (_indeksSifraOsobe.find(d->second) == _indeksSifraOsobe.end())
                continue;
            else
                svaDeca->push_back(d->second);
        }
    }
    return svaDeca;
}

int PorodicnoStablo::maxBrakova()
{
    int max = 0;
    auto b = _indeksSifraOsobe.begin(), e = _indeksSifraOsobe.end();
    for (;b!=e;b++)
    {
        Osoba *o = b->second;
        if (o->JeKrvniSrodnik())
        {
            int broj = osobaImaBrakova(o->Sifra());
            if (broj > max)
                max = broj;
        }
    }
    return max;
}

std::vector<short> *PorodicnoStablo::ListaSupruznika(const short sifra)
{
    if (_indeksSifraOsobe.find(sifra) == _indeksSifraOsobe.end())
        return nullptr;
    std::vector<short> *supruznici = new std::vector<short>();
    auto brakovi = _indeksOsobaBrak.equal_range(sifra);
    for (auto b = brakovi.first; b != brakovi.second; b++)
    {
        //b je sifra braka
        if (_indeksSifraVeza.find(b->second) == _indeksSifraVeza.end())
            continue;
        Brak *brak = _indeksSifraVeza[b->second];
        short sifraTudje = brak->SifraTudje();
        if (_indeksSifraOsobe.find(sifraTudje) == _indeksSifraOsobe.end())
            continue;
        supruznici->push_back(sifraTudje);
    }
    return  supruznici;
}

bool PorodicnoStablo::ProcitajFajl(const QString &imeFajla)
{

    //otvaramo fajl
    QFile fajl(imeFajla);
    if (!fajl.open(QIODevice::ReadOnly)) {
        //std::cout << "Ne moze da iscita fajl" << std::endl; //bice warning
        return false;
    }

    //otvaramo ulazni stream
    QDataStream ulaz(&fajl);

    ulaz.setVersion(QDataStream::Qt_4_1);

    mapaOsobe.clear();
    mapaBrakovi.clear();
    SpaliCeloStablo();

    int maxSifraOsobe=-1;
    int maxSifraBraka=-1;
    int maxSifraDeteta=-1;
    //===========================

    _kljucnaOsoba = new Osoba();
    ulaz >>*_kljucnaOsoba;
    //std::cout<<"Procitala kljucnu\n";
    _indeksSifraOsobe[_kljucnaOsoba->Sifra()] = _kljucnaOsoba;
    _indeksIme.insert(std::make_pair(_kljucnaOsoba->Ime(), _kljucnaOsoba));
    QDate datum = _kljucnaOsoba->DatumRodjenja();
    if (datum.isValid())
    {
        _indeksRodjendan.insert(std::make_pair(datum.dayOfYear(), _kljucnaOsoba->Sifra()));

    }
    if (_kljucnaOsoba->Sifra() > maxSifraOsobe)
        maxSifraOsobe = _kljucnaOsoba->Sifra();
    //----------------------UCITAVANJE SVIH PODATAKA O OSOBAMA, BEZ VEZIVANJA---------------------------//

    qint32 velicina;
    ulaz>>velicina;
    //std::cout<<"procitala broj osoba"<<(short)velicina <<std::endl;

    for (int i=0;i<(short)velicina;i++){
        Osoba *o=new Osoba();
        ulaz>>*o;
        _indeksSifraOsobe[o->Sifra()]=o;
        _indeksIme.insert(std::make_pair(o->Ime(), o));
        QDate datum = o->DatumRodjenja();
        if (datum.isValid())
        {
            _indeksRodjendan.insert(std::make_pair(datum.dayOfYear(), o->Sifra()));

        }
        if (o->Sifra() > maxSifraOsobe)
            maxSifraOsobe = o->Sifra();
        //std::cout<<"Procitala osobu" << o->Ime().toStdString() << std::endl;

    }
    //----------------------UCITAVANJE SVIH PODATAKA O OSOBAMA, BEZ VEZIVANJA---------------------------//

    //----------------------UCITAVANJE SVIH PODATAKA O BRAKOVIMA, BEZ VEZIVANJA---------------------------//
    ulaz>>velicina;

    for (int i=0;i<(short)velicina;i++){
        Brak *b=new Brak();
        ulaz>>*b;
        _indeksSifraVeza[b->Sifra()]=b;
        //std::cout<<"Procitala vezu\n";
        _indeksOsobaBrak.insert(std::make_pair(b->SifraNase(), b->Sifra()));
        _indeksOsobaBrak.insert(std::make_pair(b->SifraTudje(), b->Sifra()));
        if (b->Sifra() > maxSifraBraka)
            maxSifraBraka = b->Sifra();
    }
    //----------------------UCITAVANJE SVIH PODATAKA O BRAKOVIMA, BEZ VEZIVANJA---------------------------//

    //----------------------UCITAVANJE SVIH PODATAKA O DECI, BEZ VEZIVANJA---------------------------//
    ulaz>>velicina;
    //std::cout<<"procitala broj dece\n"<<(short)velicina;
    for (int i=0;i<(short)velicina;i++){
        Dete *d=new Dete();
        ulaz>>*d;
        _indeksSifraDete[d->Sifra()]=d;
        _indeksBrakDeca.insert(std::make_pair(d->SifraRoditeljskogOdnosa(), d->SifraOsobe()));
        //std::cout<<"Procitala dete\n";
        if (d->Sifra() > maxSifraDeteta)
            maxSifraDeteta = d->Sifra();
    }
    //----------------------UCITAVANJE SVIH PODATAKA O DECI, BEZ VEZIVANJA---------------------------//


    //----------------------POSTAVLJANJE STATICKOG PODATKA ZA DODELU SLEDECE SIFRE NA OSNOVU MAKSIMUMA UCITANIH------------------------//
    Osoba::postaviSledecuSifru(maxSifraOsobe+1);
    Brak::postaviSledecuSifru(maxSifraBraka+1);
    Dete::postaviSledecuSifru(maxSifraDeteta+1);
    //----------------------POSTAVLJANJE STATICKOG PODATKA ZA DODELU SLEDECE SIFRE NA OSNOVU MAKSIMUMA UCITANIH------------------------//


    //ucitavanje pozicija redom

    //za osobe
    int broj = _indeksSifraOsobe.size();
    for (int i=0;i<broj;i++)
    {
        qint32 sifra;
        qreal x, y;
        ulaz >> sifra;
        ulaz >> x;
        ulaz >> y;
        mapaOsobe[(short)sifra] = QPointF(x, y);
    }
    //za brakove
    broj = _indeksSifraVeza.size();
    for (int i=0;i<broj;i++)
    {
        qint32 sifra;
        qreal x, y;
        ulaz >> sifra;
        ulaz >> x;
        ulaz >> y;
        mapaBrakovi[(short)sifra] = QPointF(x, y);
    }

    //ucitavanje nivoa
    ulaz >> velicina;
    for (int i = 0; i < velicina; i++)
    {
        qint32 broj;
        ulaz >> broj;
        _nivoi.push_back((int)broj);
    }
    fajl.close();
    return true;
}



bool PorodicnoStablo::IspisiFajl(const QString &imeFajla)//samo podaci koji su potrebni da se kasnije sve rekonstruise
{
    if (_kljucnaOsoba == nullptr){
        return false;
    }
    QFile fajl(imeFajla);
    if (!fajl.open(QIODevice::WriteOnly)) {
        //std::cout << "Ne moze da upise u fajl" << std::endl; //bice warning
        return false;
    }
    QDataStream izlaz(&fajl);
    izlaz.setVersion(QDataStream::Qt_4_1);

        izlaz <<* _kljucnaOsoba;

    //Kljucnu smo vec upisali
    izlaz << (qint32)(_indeksSifraOsobe.size()-1);

    for(auto osoba :_indeksSifraOsobe)
    {
        if ((osoba.second)->Sifra() != _kljucnaOsoba->Sifra())
            izlaz<<(*(osoba.second));
    }

    izlaz << (qint32)_indeksSifraVeza.size();
    //std::cout<<"ispisala velicinu vektora brak\n"<<_indeksSifraVeza.size();

    for(auto brak :_indeksSifraVeza)
    {
        izlaz<<(*brak.second);
        //  //std::cout<<"ispisala vezu\n";

    }

    izlaz << (qint32)_indeksSifraDete.size();

    //std::cout<<"ispisala velicinu vektora Dete\n"<<_indeksSifraDete.size();
    for(auto dete :_indeksSifraDete)
    {
        izlaz<<(*dete.second);
        //  //std::cout<<"ispisala decu\n";
    }

    //pozicije redom

    //osobe
    for (auto o : mapaOsobe)
    {
        izlaz << (qint32)o.first;
        izlaz << (qreal)o.second.x();
        izlaz << (qreal)o.second.y();
    }
    //brakove
    for (auto b : mapaBrakovi)
    {
        if (_indeksSifraVeza.find(b.first) == _indeksSifraVeza.end())
            continue;//za svaki slucaj
        izlaz << (qint32)b.first;
        izlaz << (qreal)b.second.x();
        izlaz << (qreal)b.second.y();
    }

    // _nivoe
    izlaz << (qint32)_nivoi.size();
    for (auto n : _nivoi)
        izlaz << (qint32)n;

    fajl.close();
    return true;
}

void PorodicnoStablo::InicijalizujSveStrukture()
{
    _indeksIme.clear();
    _indeksSifraDete.clear();
    _indeksSifraOsobe.clear();
    _indeksSifraVeza.clear();
    _indeksBrakDeca.clear();
    _indeksOsobaBrak.clear();
    _indeksRodjendan.clear();
    _nivoi.clear();
}

void PorodicnoStablo::SpaliCeloStablo()
{
    if (_kljucnaOsoba != nullptr)
        UkloniOsobuSifrom(_kljucnaOsoba->Sifra());
    InicijalizujSveStrukture();
}

void PorodicnoStablo::ObrisiBrakove(short sifra, bool iSupruznike)//brise brakove osobe sa tom sifrom
{
    //ako je true, pozivamo brisanje samog supruznika (koji ce posle obrisati brak)
    //ako je false brisemo samo relaciju brak
    //vrv ne moze bolje


    if (iSupruznike)
    {
        std::pair <std::multimap<short, short>::iterator, std::multimap<short,short>::iterator> brakovi;
        brakovi = _indeksOsobaBrak.equal_range(sifra);
        std::multimap<short, short>::iterator iter;
        for (iter = brakovi.first; iter != brakovi.second; iter++)
        {
            if (_indeksSifraVeza.find(iter->second) == _indeksSifraVeza.end())
                continue;
            //brisemo samo drugu osobu, ne i veze ovde
            UkloniOsobuSifrom(_indeksSifraVeza[iter->second]->SifraTudje());
        }
    }
    else
    {
        std::pair <std::multimap<short, short>::iterator, std::multimap<short,short>::iterator> brakovi;
        brakovi = _indeksOsobaBrak.equal_range(sifra);
        std::multimap<short, short>::iterator iter;
        for (iter = brakovi.first; iter != brakovi.second; iter++)
        {
            //brisemo brak iz indeksa (sifra, brak) i sam objekat
            short sifraBraka = iter->second;
            if (_indeksSifraVeza.find(sifraBraka) != _indeksSifraVeza.end())
            {
                //UkloniBrakSifrom(sifraBraka);//uklanjamo brak iz indeksa za onu drugu osobu
                delete _indeksSifraVeza[sifraBraka];
                _indeksSifraVeza.erase(sifraBraka);
            }
            emit obrisanaVezaBrak(sifraBraka);
        }
    }
}

void PorodicnoStablo::ObrisiDecu(short sifra)
{
    //samo poziva ukloniOsobu za svako dete, to ce ukloniti i relaciju

    std::pair <std::multimap<short, short>::iterator, std::multimap<short,short>::iterator> brakovi;
    brakovi = _indeksOsobaBrak.equal_range(sifra);//nadjemo brakove osobe koja brise svoju decu
    std::multimap<short, short>::iterator iter;
    for (iter = brakovi.first; iter != brakovi.second; iter++)
    {
        //brisemo brak iz indeksa (sifra, brak) i sam objekat
        short sifraBraka = iter->second;
        if (_indeksSifraVeza.find(sifraBraka) == _indeksSifraVeza.end() )
            continue;
        //ako nije vec obrisan, onda mozda ima neke dece
        std::pair <std::multimap<short, short>::iterator, std::multimap<short,short>::iterator> deca;
        deca = _indeksBrakDeca.equal_range(sifraBraka);//nadjemo sifre dece
        std::multimap<short, short>::iterator iterDeca;
        for (iterDeca = deca.first; iterDeca != deca.second; iterDeca++)
            UkloniOsobuSifrom(iterDeca->second);
    }
}

void PorodicnoStablo::azurirajIndeksRodj(const QDate &stari, const QDate &novi, const short sifra)
{
    //na promeni osobe iz GUI-ja dobijamo signal
    auto dani = _indeksRodjendan.equal_range(stari.dayOfYear());
    auto iter = dani.first;
    for (; iter != dani.second; iter++)
    {
        if (iter->second == sifra)
        {
            _indeksRodjendan.erase(iter);
            _indeksRodjendan.emplace(novi.dayOfYear(), sifra);
            _indeksSifraOsobe[sifra]->PromeniDatumRodjenja(novi);
            break;
        }
    }
}

std::vector<short> *PorodicnoStablo::KomeJeSveRodjendan(const QDate &datum)
{
    std::vector<short> *slavljenici = new std::vector<short>();
    auto sl = _indeksRodjendan.equal_range(datum.dayOfYear());
    auto iter = sl.first;
    for(; iter != sl.second; iter++)
    {
        //proveravam da li osoba postoji
        if (_indeksSifraOsobe.find(iter->second) != _indeksSifraOsobe.end())
            slavljenici->push_back(iter->second);
        else
            //ako je osoba izbrisana u medjuvremenu moze se izbrisati i iz indeksa
            _indeksRodjendan.erase(iter);
    }
    return slavljenici;
}

std::vector<short> PorodicnoStablo::sifreNaPutuOdOsobeDoOsobe(int sifraPocetne,int sifraTrazene)
{
    TrazenjePuta pretraga(this);
    std::vector<short> Put(pretraga(sifraPocetne,sifraTrazene));


    return Put;
}

std::map<short, Osoba*>& PorodicnoStablo::Osobe()
{
    return _indeksSifraOsobe;
}
std::map<short, Brak*>& PorodicnoStablo::Brakovi()
{
    return _indeksSifraVeza;
}
std::map<short, Dete*> &PorodicnoStablo::Deca()
{
    return _indeksSifraDete;
}

std::multimap<short, short>& PorodicnoStablo::OsobaBrak()
{
    return _indeksOsobaBrak;
}

std::multimap<short, short>& PorodicnoStablo::BrakDeca()
{
    return _indeksBrakDeca;
}

std::vector<int> PorodicnoStablo::Nivoi()
{
    return _nivoi;

}
std::vector<short>* PorodicnoStablo::PretragaPoImenu(const QString& Ime,int tip){
    std::vector<short> *Osobe= new std::vector<short>();
    if(tip==0){
        //Sve cije je prezime pre zadatog
        for(auto imena:_indeksSifraOsobe)
            if(imena.second->Ime()<Ime)
                Osobe->push_back(imena.first);
    }
    if(tip==1){
        //Sve cije je prezime jednako zadatom
        for(auto imena:_indeksSifraOsobe)
            if(imena.second->Ime()==Ime)
                Osobe->push_back(imena.first);
    }
    if(tip==2){
        //Sve cije je prezime posle zadatog
        for(auto imena:_indeksSifraOsobe)
            if(imena.second->Ime()>Ime)
                Osobe->push_back(imena.first);
    }
    return Osobe;

}
std::vector<short>* PorodicnoStablo::PretragaPoPrezimenu(const QString& Prezime,int tip){
    std::vector<short> *Osobe= new std::vector<short>();
    if(tip==0){
        //Sve cije je prezime pre zadatog
        for(auto prezimena:_indeksSifraOsobe)
            if(prezimena.second->Prezime()<Prezime)
                Osobe->push_back(prezimena.first);
    }
    if(tip==1){
        //Sve cije je prezime jednako zadatom
        for(auto prezimena:_indeksSifraOsobe)
            if(prezimena.second->Prezime()==Prezime)
                Osobe->push_back(prezimena.first);
    }
    if(tip==2){
        //Sve cije je prezime posle zadatog
        for(auto prezimena:_indeksSifraOsobe)
            if(prezimena.second->Prezime()>Prezime)
                Osobe->push_back(prezimena.first);
    }
    return Osobe;
}

std::vector<short>* PorodicnoStablo::PretragaPoDatumuRodjenja(const QDate& DatRodj,int tip){
    std::vector<short> *Osobe= new std::vector<short>();
    if(tip==0){
        //Sve ciji je datum rodj pre zadatog
        for(auto datum:_indeksSifraOsobe)
            if(datum.second->DatumRodjenja()<DatRodj)
                Osobe->push_back(datum.first);
    }
    if(tip==1){
        //Sve ciji je datum rodj jednak zadatom
        for(auto datum:_indeksSifraOsobe)
            if(datum.second->DatumRodjenja()==DatRodj)
                Osobe->push_back(datum.first);
    }
    if(tip==2){
        //Sve ciji je datum rodj posle zadatog
        for(auto datum:_indeksSifraOsobe)
            if(datum.second->DatumRodjenja()>DatRodj)
                Osobe->push_back(datum.first);
    }
    return Osobe;
}
std::vector<short>* PorodicnoStablo::PretragaPoDatumuSmrti(const QDate& DatSmrti,int tip){
    std::vector<short> *Osobe= new std::vector<short>();
    if(tip==0){
        //Sve ciji je datum smrti pre zadatog
        for(auto datum:_indeksSifraOsobe)
            if(datum.second->DatumSmrti()<DatSmrti)
                Osobe->push_back(datum.first);
    }
    if(tip==1){
        //Sve ciji je datum smrti jednak zadatom
        for(auto datum:_indeksSifraOsobe)
            if(datum.second->DatumSmrti()==DatSmrti)
                Osobe->push_back(datum.first);
    }
    if(tip==2){
        //Sve ciji je datum smrti posle zadatog
        for(auto datum:_indeksSifraOsobe)
            if(datum.second->DatumSmrti()>DatSmrti)
                Osobe->push_back(datum.first);
    }
    return Osobe;
}
std::vector<short>* PorodicnoStablo::PretragaPoPolu(const QChar& Pol,int tip){
    std::vector<short> *Osobe= new std::vector<short>();
    if(tip==0 || tip==2){
        //Sve ciji je pol razlicit
        for(auto pol:_indeksSifraOsobe)
            if(pol.second->Pol()!=Pol)
                Osobe->push_back(pol.first);
    }
    if(tip==1){
        //Sve ciji je pol jednak
        for(auto pol:_indeksSifraOsobe)
            if(pol.second->Pol()==Pol)
                Osobe->push_back(pol.first);
    }

    return Osobe;

}

std::vector<short> *PorodicnoStablo::NisuKrvniSrodnici()
{
    std::vector<short> *Osobe= new std::vector<short>();
    for(auto osobe:_indeksSifraOsobe)
        if(!osobe.second->JeKrvniSrodnik())
            Osobe->push_back(osobe.first);
    return Osobe;
}

bool PorodicnoStablo::jeDeteOd(short sifraPrve, short sifraDruge)
{
    auto ppp= _indeksOsobaBrak.equal_range(sifraPrve);
    auto it=ppp.first;
    for(;it!=ppp.second;++it)
    {
        auto pp2= _indeksBrakDeca.equal_range((*it).second);
        auto it2=pp2.first;
        for(;it2!=pp2.second;++it2){

            if((*it2).second==sifraDruge)return true;
        }
    }
    return false;
}

bool PorodicnoStablo::jeRoditeljOd( short sifraPrve,  short sifraDruge)
{
    return jeDeteOd(sifraDruge,sifraPrve);
}

bool PorodicnoStablo::jeSupruznikOd(short sifraPrve, short sifraDruge)
{
    auto ppp= _indeksOsobaBrak.equal_range(sifraPrve);
    auto it=ppp.first;
    for(;it!=ppp.second;++it)
    {
        Brak* brak=NadjiBrakSifrom((*it).second);
        if(brak->SifraNase()==sifraDruge||brak->SifraTudje()==sifraDruge)return true;
    }
    return false;
}

bool PorodicnoStablo::jeBratSestraOd(short sifraPrve, short sifraDruge)
{
    auto it=_indeksSifraOsobe.cbegin();
    auto ie=_indeksSifraOsobe.cend();
    short sifraOca=-1;

    for(;it!=ie;++it)
    {
        if(jeRoditeljOd(sifraPrve,(*it).first))
        {
            sifraOca=(*it).first;
            break;
        }
    }

    if(sifraOca!=-1 && jeRoditeljOd(sifraDruge,sifraOca))return true;


    return false;
}
//ovo se poziva iz GUIja kako bi bile zapamcene i koordinate svakog elementa u fajl
void PorodicnoStablo::zapamtiPozicijeOsoba(std::map<short, QPointF> &mapa)
{
    mapaOsobe.clear();
    mapaOsobe = mapa;
   // for (auto m : mapa)
        //std::cout << m.first << " ";
    //std::cout <<"gotovo" <<std::endl;
}
void PorodicnoStablo::zapamtiPozicijeBrakova(std::map<short, QPointF> &mapa)
{
    mapaBrakovi.clear();
    mapaBrakovi = mapa;
  //  for (auto m : mapa)
        //std::cout << m.first << " ";
    //std::cout <<"gotovo" <<std::endl;
}
std::map<short, QPointF>& PorodicnoStablo::vratiPozicijeOsoba()
{
    return mapaOsobe;
}
std::map<short, QPointF>& PorodicnoStablo::vratiPozicijeBrakova()
{
    return mapaBrakovi;
}

