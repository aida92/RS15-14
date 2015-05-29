#ifndef GLAVNIPROZOR_H
#define GLAVNIPROZOR_H
#include "GUI/stablo.h"
#include <QApplication>
#include <QMainWindow>
#include "engine/porodicnostablo.h"
#include <QButtonGroup>
#include <QLabel>
#include <QPushButton>
#include "ui_glavniprozor.h"
#include <map>
#include <QGraphicsScene>
#include "gosoba.h"
#include "grelacija.h"

class WidgetOsoba;
namespace Ui {
class GlavniProzor;
}

class GlavniProzor : public QMainWindow, private Ui::GlavniProzor
{
    Q_OBJECT

public:
    enum TipZaInfo {NISTA, INFO_OSOBA, INFO_BRAK, INFO_DETE};

    explicit GlavniProzor(QWidget *parent = 0);
    ~GlavniProzor();

    void popuniInformacije(short sifra, TipZaInfo tip);
    void retranslate();

private:
    Ui::GlavniProzor *ui;

    QButtonGroup *grpToolBar;
    QPushButton *tbOsoba, *tbMuzZena, *tbBratSestra, *tbRoditeljDete, *tbPomeranje, *tbDetalji, *tbBrisi, *tbMenjaj, *tbUredi,*tbSrodstvo;
    QToolBar *toolbar;
    QLabel *Labela;
    PorodicnoStablo *stablo;

    QDockWidget *alati, *info;

    enum {maxSkoroOtvaranih = 5};
    static QStringList skoroOtvarani;
    QAction *skoroOtvaraniAkcije[maxSkoroOtvaranih];
    QString otvoreniFajl;
    QTranslator *translator;


    std::map<short int, GOsoba*> _osobe;
    std::map<short, GRelacija*> _brakovi;
    std::map<short int, QPointF> _pozicijeOsoba;//<sifra_osobe, pozicija>
    std::map<short int, QPointF> _pozicijeBrakova;//<sifra_braka, pozicija>

    QPushButton *kreirajJedanAlat(QPushButton *alat, const char *ime, const char *info);

    void kreirajOpcije();
    void kreirajToolbar();
    void kreirajMestoZaInfo();
    bool nastaviti();//provera da li je sve sacuvano pre izlaska i sl.
    bool snimiIzmene(const QString &imeFajla);//cuvamo u fajl
    void obnoviSkoroOtvarane();//recentFileList
    bool otvoriFajl(const QString &imeFajla);//otvaranje fajla
    void postaviTrenutniFajl(const QString &imeFajla);
    void writeSettings();//cuva pozicije widgeta, recent files...
    void readSettings();//cita ovo gore pri konstrukciji

    void RekonstruisiStablo();//nakon sto podatke ucitamo u enginu

    /** menjanje stabla **/
    short izmeniOsobu(short sifra);
    GOsoba* dodajNovuOsobu(QPoint pozicija, bool krvniSrodnik);
    short ukloniOsobu(short sifra);
    short dodajNovoDete(GRelacija *brak, GOsoba *dete);
    short dodajNoviBrak(GOsoba *prva, GOsoba *druga);


    Stablo *pogled;
    QGraphicsScene *scena;
    void kreirajPogledZaStablo();

    bool uredjeno = true;
    std::vector<int>* sirine;

    //osobu te sifre postavlja gde treba
    void pomeriOsobu(short int sifra, QPointF pocetak, int nivo);

public Q_SLOTS:
    //-----Toolbar, Menubar...-----//
    void prikaziToolbar();
    void novoStablo();//File|New
    void otvoriPostojeceStablo();//File|Open
    void closeEvent(QCloseEvent *event);//File|Exit
    bool sacuvaj();//File|Save
    bool sacuvajKao();//File|SaveAs
    void otvoriSkoroOtvaraniFajl();//File|RecentFiles...
    void promeniJezikE();//za Promenu jezika na engleski
    void promeniJezikN();//za promenu jezika na nemacki
    void promeniJezikS();//za promenu jezika na srpski
    void promeniJezikSpanski();//za promenu jezika na spanski
    void osveziPrikazAlata(bool Vidljivost);
    void osveziPrikazInformacija(bool Vidljivost);
    //-----Toolbar, Menubar...-----//
    //-----INTERAKCIJA-----//
    void kliknutoStablo(QPoint pozicija);
    void vucenoStablo(QPoint prva, QPoint druga);
    void urediStablo();
    //-----INTERAKCIJA-----//


    void izvrsiPretragu();
    //ovo dole se poziva kada se neki brak obrise
    void izbrisiVezuIzIndeksa(short sifra);
    //kreiranje kljucne osobe, to je moguce samo kada je stablo prazno
    //na samom pocetku ili kada se tokom rada izbrise kljucna, a to znaci celo stablo
    void napraviKljucnuOsobu();
};

#endif // GLAVNIPROZOR_H
