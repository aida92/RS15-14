#include "GUI/glavniprozor.h"
#include "ui_glavniprozor.h"
#include <QToolBar>
#include <QDockWidget>
#include "dijalogizmenaosobe.h"
#include "dialognovaosoba.h"
#include "dijalogrelacija.h"
#include <vector>
#include <QMessageBox>
#include <QFileDialog>
#include <QPoint>
#include <QPointF>
#include <QAction>
#include <QSettings>
#include <QTranslator>
#include <QTransform>
#include <QTextBrowser>
#include "GUI/dijalogpretrage.h"
#include <QDebug>
#include "alati/uredjivanje.h"
#include <iostream>
#include "alati/trazenjeputa.h"

GlavniProzor::GlavniProzor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GlavniProzor)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    QIcon icon(":images/images/ProjectAlmond.ico");
    this->setWindowIcon(icon);
    this->setWindowTitle(tr("Project Almond[*]"));

    translator = new QTranslator();

    translator->load(":/jezici/ProjectAlmond_sr.qm");
    // qDebug("translation %d", ok);
    qApp->installTranslator(translator);
    ui->retranslateUi(this);



    stablo = new PorodicnoStablo();

    kreirajToolbar();
    kreirajMestoZaInfo();
    kreirajPogledZaStablo();
    kreirajOpcije();
    obnoviSkoroOtvarane();

    connect(tbBratSestra, SIGNAL(clicked()), this, SLOT(napraviKljucnuOsobu()));

    readSettings();
    retranslate();
}

GlavniProzor::~GlavniProzor()
{

    delete ui;

}

void GlavniProzor::napraviKljucnuOsobu()
{
    if (!nastaviti())
        return;
    short int novaSifra = -1;
    GOsoba *kljucna = nullptr;

    DialogNovaOsoba *d = new DialogNovaOsoba(this);
    if (d->exec())
    {
        QString ime, prezime;
        QDate rodjenje, smrt;
        QChar pol;

        if (d->popuniPodatke(ime, prezime, pol, rodjenje, smrt))
        {
            delete stablo;
            stablo = new PorodicnoStablo(ime, prezime, pol, rodjenje, smrt, true);
            novaSifra = stablo->KljucnaOsoba()->Sifra();
        }
        else
            ui->statusBar->showMessage("Podaci moraju biti poznati za korenu osobu");
        if (novaSifra >= 0)
        {
            kljucna = new GOsoba(novaSifra, (stablo->NadjiOsobuSifrom(novaSifra)->ImePrezime()));
            QPointF centar(pogled->viewport()->rect().center());
            kljucna->setPos(pogled->mapToScene(centar.x(), centar.y()));
            kljucna->setZValue(2);
            scena->addItem(kljucna);
            _pozicijeOsoba[kljucna->Sifra()] = kljucna->pos();
            _osobe[kljucna->Sifra()] = kljucna;
            connect(stablo, SIGNAL(obrisanaOsoba(short)), kljucna, SLOT(skloniSeSaScene(short)));
            kljucna->dodajStil((pol!=QChar('M'))?GOsoba::ZENSKA:GOsoba::MUSKA);
            kljucna->dodajStil(GOsoba::KORENA);
            tbBratSestra->setDisabled(true);
        }
    }
    delete d;
}

void GlavniProzor::popuniInformacije(short sifra, TipZaInfo tip)
{
    if (sifra == -1 || tip == NISTA)
    {
        ui->zaInformacije->setPlaceholderText("Informacije");
        return;
    }
    if (tip == INFO_OSOBA)
    {
        Osoba *osoba = stablo->NadjiOsobuSifrom(sifra);
        if (osoba)
        {
            ui->zaInformacije->clear();
            ui->zaInformacije->append(osoba->Ime());
            ui->zaInformacije->append(osoba->Prezime());
            QDate datum = osoba->DatumRodjenja();
            if (datum.isValid())
            {
                ui->zaInformacije->append(tr("Datum rodjenja:"));
                ui->zaInformacije->append(datum.toString("dd.MM.yyyy."));
            }
            datum = osoba->DatumSmrti();
            if (datum.isValid())
            {
                ui->zaInformacije->append(tr("Datum smrti: "));
                ui->zaInformacije->append(datum.toString("dd.MM.yyyy."));
            }
        }
    }
    if (tip == INFO_BRAK)
        ui->zaInformacije->append(stablo->NadjiBrakSifrom(sifra)->Trivija());
    if (tip == INFO_DETE)
        ui->zaInformacije->append(stablo->NadjiDeteSifrom(sifra)->Trivija());
}

void GlavniProzor::prikaziToolbar()
{
    alati->setVisible(ui->aAlati->isChecked());
    info->setVisible(ui->aInformacije->isChecked());
}

QPushButton* GlavniProzor::kreirajJedanAlat(QPushButton * alat, const char* ime,const char* info)
{
    alat=new QPushButton();
    alat->setCheckable(true);
    alat->setFocusPolicy(Qt::NoFocus);
    alat->setToolTip(info);
    std::string ikonica(":/images/images/"+std::string(ime)+".ico");
    const char* pocetak=&ikonica[0];
    alat->setIcon(QIcon(pocetak));
    alat->setIconSize(QSize(48,48));
    return alat;
}

void GlavniProzor::kreirajOpcije()
{
    connect(ui->aAlati,SIGNAL(triggered()),this,SLOT(prikaziToolbar()));
    connect(ui->aInformacije, SIGNAL(triggered()), this, SLOT(prikaziToolbar()));

    connect(ui->aNovoStablo, SIGNAL(triggered()), this, SLOT(novoStablo()));
    connect(ui->aOtvori, SIGNAL(triggered()), this, SLOT(otvoriPostojeceStablo()));
    connect(ui->aZatvori, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->aUgasi, SIGNAL(triggered()),qApp, SLOT(closeAllWindows()));
    connect(ui->aSacuvaj, SIGNAL(triggered()), this, SLOT(sacuvaj()));
    connect(ui->aIzvezi, SIGNAL(triggered()), this, SLOT(sacuvajKao()));
    connect(ui->aEngleski,SIGNAL(triggered()),this,SLOT(promeniJezikE()));
    connect(ui->aNemacki,SIGNAL(triggered()),this,SLOT(promeniJezikN()));
    connect(ui->aSrpski,SIGNAL(triggered()),this,SLOT(promeniJezikS()));
    connect(ui->aSpanski,SIGNAL(triggered()),this,SLOT(promeniJezikSpanski()));
    connect(ui->actionPretraga, SIGNAL(triggered()), this, SLOT(izvrsiPretragu()));

    for (int i = 0; i < maxSkoroOtvaranih; ++i)
    {
        skoroOtvaraniAkcije[i] = new QAction(this);
        skoroOtvaraniAkcije[i]->setVisible(false);
        connect(skoroOtvaraniAkcije[i], SIGNAL(triggered()), this, SLOT(otvoriSkoroOtvaraniFajl()));
        ui->aSkoroOtvarani->addAction(skoroOtvaraniAkcije[i]);
    }
}

void GlavniProzor::kreirajToolbar()
{
    toolbar = addToolBar(tr("Alati"));

    grpToolBar=new QButtonGroup();

    tbMuzZena=kreirajJedanAlat(tbMuzZena,"RelacijaSupruznici","Dodajte u stablo supruznika nekoj od osoba");
    tbRoditeljDete=kreirajJedanAlat(tbRoditeljDete,"RelacijaDete","Dodajte u neku vezu novo dete");
    tbBratSestra=kreirajJedanAlat(tbBratSestra,"NovaOsoba","Kreiraj osobu koja je koren porodicnog stabla");
    tbPomeranje=kreirajJedanAlat(tbPomeranje,"Pomeri","Pomerite rucicom odabranu osobu ili relaciju na crtezu");
    tbDetalji=kreirajJedanAlat(tbDetalji,"Informacija","Detalji o odabranoj osobi ili odnosu");
    tbMenjaj=kreirajJedanAlat(tbMenjaj,"Menjaj","Izmenite podatke o odabranoj osobi ili odnosu");
    tbBrisi=kreirajJedanAlat(tbBrisi,"Ukloni","Obrisite osobu ili relaciju iz stabla");
    tbUredi=kreirajJedanAlat(tbUredi,"UrediStablo","Rasporedite cvorove stabla automatski");
    tbSrodstvo=kreirajJedanAlat(tbSrodstvo,"Srodstvo","Odredite tip srodstva izmedju dve odabrane osobe");
    tbUredi->setCheckable(false);
    tbBratSestra->setCheckable(false);

    grpToolBar->addButton(tbRoditeljDete);
    grpToolBar->addButton(tbMuzZena);
    grpToolBar->addButton(tbPomeranje);
    grpToolBar->addButton(tbDetalji);
    grpToolBar->addButton(tbMenjaj);
    grpToolBar->addButton(tbBrisi);
    grpToolBar->addButton(tbSrodstvo);

    toolbar->addWidget(tbBratSestra);
    toolbar->addSeparator();
    toolbar->addWidget(tbMuzZena);
    toolbar->addWidget(tbRoditeljDete);

    toolbar->addSeparator();
    toolbar->addWidget(tbPomeranje);
    toolbar->addWidget(tbMenjaj);
    toolbar->addWidget(tbBrisi);
    toolbar->addSeparator();
    toolbar->addWidget(tbUredi);
    toolbar->addSeparator();
    toolbar->addWidget(tbSrodstvo);
    toolbar->addWidget(tbDetalji);


    tbDetalji->setChecked(true);

    tbUredi->setCheckable(false);
    connect(tbUredi, SIGNAL(clicked()), this, SLOT(urediStablo()));

    alati = new QDockWidget(tr("Alati"));
    alati->setWidget(toolbar);
    alati->setAllowedAreas(Qt::TopDockWidgetArea
                           | Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    addDockWidget(Qt::TopDockWidgetArea, alati);

    connect(alati,SIGNAL(visibilityChanged(bool)),this,SLOT(osveziPrikazAlata(bool)));

}

void GlavniProzor::kreirajMestoZaInfo()
{
    info = new QDockWidget(tr("Informacije"));
    /** bilo bi lepo da je malo uzi :) **/
    info->setWidget(ui->zaInformacije);
    ui->zaInformacije->setPlaceholderText(tr("Informacije"));
    info->setAllowedAreas(Qt::RightDockWidgetArea
                          | Qt::LeftDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, info);
    connect(info,SIGNAL(visibilityChanged(bool)),this,SLOT(osveziPrikazInformacija(bool)));
}

GOsoba *GlavniProzor::dodajNovuOsobu(QPoint pozicija, bool krvniSrodnik)
{
    short int novaSifra = -1;
    GOsoba *novaOsoba = nullptr;
    DialogNovaOsoba *d = new DialogNovaOsoba(this);
    if (d->exec())
    {
        QString ime, prezime;
        QDate rodjenje, smrt;
        QChar pol;
        bool jeNN=false;
        if (d->popuniPodatke(ime, prezime, pol, rodjenje, smrt))
        {jeNN=false;
            novaSifra = stablo->DodajOsobu(ime, prezime, pol, rodjenje, smrt, krvniSrodnik);
        }
        else{
            novaSifra = stablo->DodajNNLice(krvniSrodnik);
            jeNN=true;
        }
        if (novaSifra >= 0)
        {
            novaOsoba = new GOsoba(novaSifra, (stablo->NadjiOsobuSifrom(novaSifra)->ImePrezime()));
            novaOsoba->setPos(pogled->mapToScene(pozicija));
            novaOsoba->setZValue(3);
            novaOsoba->dodajStil((pol!=QChar('M'))?GOsoba::ZENSKA:GOsoba::MUSKA);
            if(jeNN)novaOsoba->dodajStil(GOsoba::NEPOZNATA);
            _pozicijeOsoba[novaSifra] = novaOsoba->pos();
            _osobe[novaSifra] = novaOsoba;
            connect(stablo, SIGNAL(obrisanaOsoba(short)), novaOsoba, SLOT(skloniSeSaScene(short)));
        }
    }
    delete d;
    return novaOsoba;
}

short GlavniProzor::ukloniOsobu(short sifra)
{
    QString upozorenje;
    if (sifra == stablo->KljucnaOsoba()->Sifra())
        upozorenje = tr("Jeste li sigurni da zelite da uklonite korenu osobu?"
                        " To ce prouzrokovati brisanje celog stabla!");
    else
        if (stablo->NadjiOsobuSifrom(sifra)->JeKrvniSrodnik())
            upozorenje = tr("Jeste li sigurni da zelite da uklonite selektovanu osobu,"
                            "a time i sve njene supruznike i potomke?");
        else
            upozorenje = tr("Jeste li sigurni da zelite da uklonite selektovanu osobu i njene potomke?");

    QMessageBox *poruka = new QMessageBox();
    QPushButton *da = poruka->addButton(tr("Da"), QMessageBox::AcceptRole);
    poruka->setDefaultButton(da);
    QPushButton *ne = poruka->addButton(tr("Ne"), QMessageBox::RejectRole);
    poruka->setEscapeButton(ne);
    poruka->setIcon(QMessageBox::Question);
    poruka->setObjectName(tr("Uklanjanje osobe"));
    poruka->setText(upozorenje);

    poruka->exec();
    if (poruka->clickedButton() == da)
    {
        if(sifra==stablo->KljucnaOsoba()->Sifra())tbBratSestra->setEnabled(true);
        stablo->UkloniOsobuSifrom(sifra);
        ui->statusBar->showMessage(tr("Uspesno izvrseno uklanjanje izabrane osobe."), 2000);
        uredjeno = false;
        return sifra;
    }
    else
        ui->statusBar->showMessage(tr("Osoba nije uklonjena."), 2000);
    return -1;
}

short GlavniProzor::dodajNovoDete(GRelacija *brak, GOsoba *dete)
{
    DijalogRelacija *d = new DijalogRelacija(this);
    short int novaSifra = -1;
    GRelacija *novaRelacija;
    QString trivija;

    if (d->exec())
    {
        d->popuniPodatke(trivija);
        novaSifra = stablo->DodajDete(brak->Sifra(), dete->Sifra(), trivija);
        if (novaSifra >= 0)
        {
            novaRelacija = new GRelacija(novaSifra, brak->pos(), dete->pos(), false);
        }
        if (novaRelacija != nullptr)
        {
            novaRelacija->setZValue(1);
            scena->addItem(novaRelacija);
            connect(brak, SIGNAL(pomerilaSe(QPointF)), novaRelacija, SLOT(pomeriPrvu(QPointF)));
            connect(dete, SIGNAL(pomerilaSe(QPointF)), novaRelacija, SLOT(pomeriDrugu(QPointF)));
            connect(stablo, SIGNAL(obrisanaVezaDete(short)), novaRelacija, SLOT(ukloniSeSaScene(short)));
        }
    }
    delete d;
    return novaSifra;
}

short GlavniProzor::dodajNoviBrak(GOsoba *prva, GOsoba *druga)
{
    DijalogRelacija *d = new DijalogRelacija(this);
    short int novaSifra = -1;
    GRelacija *novaRelacija;
    QString trivija;
    if (d->exec())
    {
        d->popuniPodatke(trivija);
        novaSifra = stablo->DodajBrak(prva->Sifra(), druga->Sifra(), trivija);
        if (novaSifra >= 0)
        {
            novaRelacija = new GRelacija(novaSifra, prva->pos(), druga->pos(), true);
            _pozicijeBrakova[novaSifra] = novaRelacija->pos();
            _brakovi[novaSifra] = novaRelacija;
        }

        if (novaRelacija != nullptr)
        {
            novaRelacija->setZValue(2);
            scena->addItem(novaRelacija);
            connect(novaRelacija, SIGNAL(obrisiMe(short)), this, SLOT(izbrisiVezuIzIndeksa(short)));
            connect(prva, SIGNAL(pomerilaSe(QPointF)), novaRelacija, SLOT(pomeriPrvu(QPointF)));
            connect(druga, SIGNAL(pomerilaSe(QPointF)), novaRelacija, SLOT(pomeriDrugu(QPointF)));
            connect(stablo, SIGNAL(obrisanaVezaBrak(short)), novaRelacija, SLOT(ukloniSeSaScene(short)));
        }
    }
    delete d;

    return novaSifra;
}


short GlavniProzor::izmeniOsobu(short sifra)
{
    DijalogIzmenaOsobe *d = new DijalogIzmenaOsobe(stablo->NadjiOsobuSifrom(sifra), this);
    connect(d, SIGNAL(azurirajRodjenje(QDate,QDate,short)), stablo, SLOT(azurirajIndeksRodj(QDate,QDate,short)));
    if (d->exec())
    {
        ui->statusBar->showMessage(tr("Uspesno unete izmene."), 2000);
        delete d;
        return sifra;
    }
    delete d;
    return -1;
}

bool GlavniProzor::nastaviti()
{
    if (isWindowModified())
    {
        QMessageBox *poruka = new QMessageBox();
        poruka->setInformativeText(tr("Postoje nesacuvane izmene u trenutnom stablu. Da li zelite da ih snimite?"));
        poruka->setModal(true);

        poruka->setObjectName(tr("Project Almond"));
        QPushButton *da = poruka->addButton(tr("Da"), QMessageBox::AcceptRole);
        poruka->setDefaultButton(da);
        poruka->addButton(tr("Ne"), QMessageBox::NoRole);
        QPushButton *odustani = poruka->addButton(tr("Odustani"), QMessageBox::RejectRole);
        poruka->setEscapeButton(odustani);
        poruka->setIcon(QMessageBox::Warning);
        poruka->exec();
        if (poruka->clickedButton() == da)
            return sacuvaj();
        if (poruka->clickedButton() == odustani)
            return false;
    }
    return true;
}

bool GlavniProzor::snimiIzmene(const QString &imeFajla)
{
    //stablo -> upisi pozicije
    stablo->zapamtiPozicijeOsoba(_pozicijeOsoba);
    stablo->zapamtiPozicijeBrakova(_pozicijeBrakova);
    if (!stablo->IspisiFajl(imeFajla))
    {
        ui->statusBar->showMessage(tr("Snimanje otkazano."), 2000);
        return false;
    }
    postaviTrenutniFajl(imeFajla);
    ui->statusBar->showMessage(tr("Fajl je sacuvan."), 2000);
    return true;
}

void GlavniProzor::obnoviSkoroOtvarane()
{
    QMutableStringListIterator i(GlavniProzor::skoroOtvarani);
    while (i.hasNext())
        if (!QFile::exists(i.next()))
            i.remove();
    for (int j = 0; j < maxSkoroOtvaranih; ++j)
    {
        if (j < GlavniProzor::skoroOtvarani.count())
        {
            QString text = tr("&%1 %2")
                    .arg(j + 1)
                    .arg(QFileInfo(GlavniProzor::skoroOtvarani[j]).fileName());
            skoroOtvaraniAkcije[j]->setText(text);
            skoroOtvaraniAkcije[j]->setData(GlavniProzor::skoroOtvarani[j]);
            skoroOtvaraniAkcije[j]->setVisible(true);
        }
        else
            skoroOtvaraniAkcije[j]->setVisible(false);
    }

}

bool GlavniProzor::otvoriFajl(const QString &imeFajla)
{
    if (!stablo->ProcitajFajl(imeFajla))
    {
        ui->statusBar->showMessage(tr("Ucitavanje fajla otkazano."), 2000);
        return false;
    }
    postaviTrenutniFajl(imeFajla);
    ui->statusBar->showMessage(tr("Fajl uspesno ucitan."), 2000);
    //stablo -> vrati pozicije
    RekonstruisiStablo();
    ui->aKljucna->setEnabled(_osobe.empty());
    uredjeno = false;
    return true;
}


void GlavniProzor::RekonstruisiStablo()
{
    _pozicijeOsoba.clear();
    _osobe.clear();
    _pozicijeBrakova.clear();
    _brakovi.clear();
    _pozicijeOsoba = stablo->vratiPozicijeOsoba();
    _pozicijeBrakova = stablo->vratiPozicijeBrakova();
    for (auto a : stablo->Osobe())
    {
        GOsoba *g = new GOsoba(a.first, a.second->ImePrezime());
        g->dodajStil((a.second->Pol()=='M')?GOsoba::MUSKA:GOsoba::ZENSKA);
        if(a.second->JeNepoznata())g->dodajStil(GOsoba::NEPOZNATA);
        scena->addItem(g);
        g->setPos(_pozicijeOsoba.at(g->Sifra()));
        if(a.first==stablo->KljucnaOsoba()->Sifra())g->dodajStil(GOsoba::KORENA);
        g->setZValue(2);
        _osobe[a.first] = g;
        connect(stablo, SIGNAL(obrisanaOsoba(short)), g, SLOT(skloniSeSaScene(short)));
    }
    for (auto b : stablo->Brakovi())
    {
        Brak *brak  = b.second;
        QPointF prva(_pozicijeOsoba.at(brak->SifraNase()));
        QPointF druga(_pozicijeOsoba.at(brak->SifraTudje()));
        GRelacija *g = new GRelacija(b.first, prva, druga ,true);
        g->setPos(_pozicijeBrakova[b.first]);
        _brakovi[b.first] = g;
        g->setZValue(1);
        scena->addItem(g);

        connect(_osobe.at(brak->SifraNase()), SIGNAL(pomerilaSe(QPointF)), g, SLOT(pomeriPrvu(QPointF)));
        connect(_osobe.at(brak->SifraTudje()), SIGNAL(pomerilaSe(QPointF)), g, SLOT(pomeriDrugu(QPointF)));
        connect(stablo, SIGNAL(obrisanaVezaBrak(short)), g, SLOT(ukloniSeSaScene(short)));
        connect(g, SIGNAL(obrisiMe(short)), this, SLOT(izbrisiVezuIzIndeksa(short)));

    }
    for (auto d : stablo->Deca())
    {
        Dete * dete  = d.second;
        dete->SifraRoditeljskogOdnosa();
        QPointF prva(_pozicijeBrakova.at(dete->SifraRoditeljskogOdnosa()));
        QPointF druga(_pozicijeOsoba.at(dete->SifraOsobe()));
        GRelacija *g = new GRelacija(d.first, prva, druga ,false);
        scena->addItem(g);
        g->setZValue(1);
        /** dopuniti ovaj connect **/
        connect(_brakovi.at(dete->SifraRoditeljskogOdnosa()), SIGNAL(pomerilaSe(QPointF)), g, SLOT(pomeriPrvu(QPointF)));
        connect(_osobe.at(dete->SifraOsobe()), SIGNAL(pomerilaSe(QPointF)), g, SLOT(pomeriDrugu(QPointF)));
        connect(stablo, SIGNAL(obrisanaVezaDete(short)), g, SLOT(ukloniSeSaScene(short)));
    }
    tbBratSestra->setEnabled(true);
}

void GlavniProzor::postaviTrenutniFajl(const QString &imeFajla)
{
    otvoreniFajl = imeFajla;
    setWindowModified(false);
    if (!otvoreniFajl.isEmpty())
    {
        skoroOtvarani.removeAll(otvoreniFajl);
        skoroOtvarani.prepend(otvoreniFajl);
        ////qDebug() << "uneo fajl u skoroOtvarane";
        foreach (QWidget *win, QApplication::topLevelWidgets())
        {
            if (GlavniProzor  *glavni = qobject_cast<GlavniProzor*>(win))
                glavni->obnoviSkoroOtvarane();
        }
    }
}

void GlavniProzor::writeSettings()
{
    QSettings settings("RS15-14", "ProjectAlmond");
    settings.setValue("skoroOtvarani", skoroOtvarani);
    settings.setValue("geometry", geometry());
    settings.setValue("preci",ui->aPreciGore->isChecked());
    settings.setValue("prikaziInfo",ui->aInformacije->isChecked());
    settings.setValue("prikaziDok",info->isVisible());
    settings.setValue("prikaziAlate",ui->aAlati->isChecked());
    settings.setValue("prikaziDok2",alati->isVisible());
    settings.setValue("engleski",ui->aEngleski->isChecked());
    settings.setValue("nemacki",ui->aNemacki->isChecked());
    settings.setValue("srpski",ui->aSrpski->isChecked());
    settings.setValue("spanski",ui->aSpanski->isChecked());

}

void GlavniProzor::readSettings()
{
    QSettings settings("RS15-14", "ProjectAlmond");
    if(settings.contains("skoroOtvarani"))
        skoroOtvarani = settings.value("skoroOtvarani").toStringList();
    obnoviSkoroOtvarane();

    QRect rect = settings.value("geometry",QRect(200, 200, 800, 800)).toRect();
    move(rect.topLeft());
    resize(rect.size());

    bool podesi = settings.value("preci", true).toBool();
    ui->aPreciGore->setChecked(podesi);

    podesi= settings.value("prikaziInfo", true).toBool();
    ui->aInformacije->setChecked(podesi);
    podesi= settings.value("prikaziDok", true).toBool();
    info->setVisible(podesi);
    podesi= settings.value("prikaziAlate", true).toBool();
    ui->aAlati->setChecked(podesi);
    podesi= settings.value("prikaziDok2", true).toBool();
    alati->setVisible(podesi);

    podesi= settings.value("engleski", false).toBool();
    ui->aEngleski->setChecked(podesi);
    if(podesi)promeniJezikE();
    podesi= settings.value("srpski", true).toBool();
    ui->aSrpski->setChecked(podesi);
    if(podesi)promeniJezikS();
    podesi= settings.value("nemacki", false).toBool();
    ui->aNemacki->setChecked(podesi);
    if(podesi)promeniJezikN();
    podesi= settings.value("spanski", false).toBool();
    ui->aSpanski->setChecked(podesi);
    if(podesi)promeniJezikSpanski();
}

void GlavniProzor::kreirajPogledZaStablo()
{
    pogled = new Stablo();
    scena = new QGraphicsScene(0, 0, pogled->width(), pogled->height());
    pogled->setScene(scena);
    setCentralWidget(pogled);
    connect(pogled, SIGNAL(kliknut(QPoint)), this, SLOT(kliknutoStablo(QPoint)));
    connect(pogled, SIGNAL(vucen(QPoint,QPoint)), this, SLOT(vucenoStablo(QPoint,QPoint)));
}

void GlavniProzor::novoStablo()
{

    GlavniProzor *w = new GlavniProzor();
    w->show();
}

void GlavniProzor::otvoriPostojeceStablo()
{

    if (nastaviti())
    {
        ////qDebug() << "treba otvoriti postojeci fajl";
        QString imeFajla = QFileDialog::getOpenFileName(this,
                                                        tr("Otvorite postojece stablo."),
                                                        tr("ProjectAlmond (*.alm)"),QString("*.alm"));
        if (!imeFajla.isEmpty())
        {
            ////qDebug() << "nasli fajl i treba  ga otvoriti";
            otvoriFajl(imeFajla);
        }
    }
}

void GlavniProzor::closeEvent(QCloseEvent *event)
{
    if (nastaviti())
    {
        writeSettings();
        ////qDebug() << "Zatvaramo fajl";
        event->accept();
    }
    else
    {
        ////qDebug() << "Otkazano izlazenje";
        event->ignore();
    }
}

bool GlavniProzor::sacuvaj()
{
    if (otvoreniFajl.isEmpty())
        return sacuvajKao();
    return snimiIzmene(otvoreniFajl);
}

bool GlavniProzor::sacuvajKao()
{
    QString ekst("*.alm");
    QString ime = QFileDialog::getSaveFileName(this, tr("Sacuvajte stablo."),
                                               ".", tr("Project Almond (*.alm)"),&ekst);
    if (ime.isEmpty())
        return false;
    return snimiIzmene(ime);
}

void GlavniProzor::otvoriSkoroOtvaraniFajl()
{
    if (nastaviti())
    {
        QAction *akcija = qobject_cast<QAction *>(sender());
        if (akcija)
        {
            otvoriFajl(akcija->data().toString());
            ////qDebug() << "Otvoriti fajl";
        }
    }
}
void GlavniProzor::promeniJezikE()
{
    if(ui->aEngleski->isChecked()){

        translator->load(":/jezici/ProjectAlmond_en.qm");
        //qDebug("translation %d", ok);
        qApp->installTranslator(translator);
        ui->retranslateUi(this);
        retranslate();
        //qDebug() << "English";
        ui->aNemacki->setChecked(false);
        ui->aSrpski->setChecked(false);
        ui->aSpanski->setChecked(false);
    }
    else if(ui->aNemacki->isChecked()){
        translator->load(":/jezici/ProjectAlmond_de.qm");
        //qDebug("translation %d", ok);
        qApp->installTranslator(translator);
        ui->retranslateUi(this);
        retranslate();
        ui->aEngleski->setChecked(false);
        ui->aSrpski->setChecked(false);
        ui->aSpanski->setChecked(false);
        //qDebug() << "Deutsch";

    }
    else if (ui->aSpanski->isChecked()){
        translator->load(":/jezici/ProjectAlmond_es.qm");
        //qDebug("translation %d", ok);
        qApp->installTranslator(translator);
        ui->retranslateUi(this);
        retranslate();
        ui->aEngleski->setChecked(false);
        ui->aNemacki->setChecked(false);
        ui->aSrpski->setChecked(false);
        //qDebug() << "Espanol";

    }
    else {
        ui->aSrpski->setChecked(true);
        translator->load(":/jezici/ProjectAlmond_sr.qm");
        //qDebug("translation %d", ok);
        qApp->installTranslator(translator);
        ui->retranslateUi(this);
        retranslate();
        ui->aEngleski->setChecked(false);
        ui->aNemacki->setChecked(false);
        ui->aSpanski->setChecked(false);
        //qDebug() << "Srpski";
    }

}

void GlavniProzor::promeniJezikN()
{
    if(ui->aNemacki->isChecked()){
        translator->load(":/jezici/ProjectAlmond_de.qm");
        //qDebug("translation %d", ok);
        qApp->installTranslator(translator);
        ui->retranslateUi(this);
        retranslate();
        ui->aEngleski->setChecked(false);
        ui->aSrpski->setChecked(false);
        ui->aSpanski->setChecked(false);
        //qDebug() << "Deutsch";

    }

    else if(ui->aEngleski->isChecked()){

        translator->load(":/jezici/ProjectAlmond_en.qm");
        //qDebug("translation %d", ok);
        qApp->installTranslator(translator);
        ui->retranslateUi(this);
        retranslate();
        //qDebug() << "English";
        ui->aNemacki->setChecked(false);
        ui->aSrpski->setChecked(false);
        ui->aSpanski->setChecked(false);
    }
    else if (ui->aSpanski->isChecked()){
        translator->load(":/jezici/ProjectAlmond_es.qm");
        // qDebug("translation %d", ok);
        qApp->installTranslator(translator);
        ui->retranslateUi(this);
        retranslate();
        ui->aEngleski->setChecked(false);
        ui->aNemacki->setChecked(false);
        ui->aSrpski->setChecked(false);
        //qDebug() << "Espanol";

    }

    else {
        ui->aSrpski->setChecked(true);
        translator->load(":/jezici/ProjectAlmond_sr.qm");
        //  qDebug("translation %d", ok);
        qApp->installTranslator(translator);
        ui->retranslateUi(this);
        retranslate();
        ui->aEngleski->setChecked(false);
        ui->aNemacki->setChecked(false);
        ui->aSpanski->setChecked(false);
        //qDebug() << "Srpski";
    }

}
void GlavniProzor::promeniJezikS()
{

    if(ui->aSrpski->isChecked())
    {
        translator->load(":/jezici/ProjectAlmond_sr.qm");
        //  qDebug("translation %d", ok);
        qApp->installTranslator(translator);
        ui->retranslateUi(this);
        retranslate();
        ui->aEngleski->setChecked(false);
        ui->aNemacki->setChecked(false);
        ui->aSpanski->setChecked(false);
        //qDebug() << "Srpski";
    }
    else if(ui->aEngleski->isChecked()){

        translator->load(":/jezici/ProjectAlmond_en.qm");
        // qDebug("translation %d", ok);
        qApp->installTranslator(translator);
        ui->retranslateUi(this);
        retranslate();
        //qDebug() << "English";
        ui->aNemacki->setChecked(false);
        ui->aSrpski->setChecked(false);
        ui->aSpanski->setChecked(false);
    }
    else if(ui->aNemacki->isChecked()){
        translator->load(":/jezici/ProjectAlmond_de.qm");
        // qDebug("translation %d", ok);
        qApp->installTranslator(translator);
        ui->retranslateUi(this);
        retranslate();
        ui->aEngleski->setChecked(false);
        ui->aSrpski->setChecked(false);
        ui->aSpanski->setChecked(false);
        //qDebug() << "Deutsch";

    }
    else if (ui->aSpanski->isChecked()){
        translator->load(":/jezici/ProjectAlmond_es.qm");
        // qDebug("translation %d", ok);
        qApp->installTranslator(translator);
        ui->retranslateUi(this);
        retranslate();
        ui->aEngleski->setChecked(false);
        ui->aNemacki->setChecked(false);
        ui->aSrpski->setChecked(false);
        //qDebug() << "Espanol";

    }
    else{
        ui->aSrpski->setChecked(true);
        translator->load(":/jezici/ProjectAlmond_sr.qm");
        //qDebug("translation %d", ok);
        qApp->installTranslator(translator);
        ui->retranslateUi(this);
        retranslate();
        ui->aEngleski->setChecked(false);
        ui->aNemacki->setChecked(false);
        //qDebug() << "Srpski";
    }
}
void GlavniProzor::promeniJezikSpanski(){
    if (ui->aSpanski->isChecked()){
        translator->load(":/jezici/ProjectAlmond_es.qm");
        // qDebug("translation %d", ok);
        qApp->installTranslator(translator);
        ui->retranslateUi(this);
        retranslate();
        ui->aEngleski->setChecked(false);
        ui->aNemacki->setChecked(false);
        ui->aSrpski->setChecked(false);
        //qDebug() << "Espanol";

    }

    else if(ui->aNemacki->isChecked()){
        translator->load(":/jezici/ProjectAlmond_de.qm");
        //qDebug("translation %d", ok);
        qApp->installTranslator(translator);
        ui->retranslateUi(this);
        retranslate();
        ui->aEngleski->setChecked(false);
        ui->aSrpski->setChecked(false);
        ui->aSpanski->setChecked(false);
        //qDebug() << "Deutsch";

    }
    else if(ui->aEngleski->isChecked()){

        translator->load(":/jezici/ProjectAlmond_en.qm");
        // qDebug("translation %d", ok);
        qApp->installTranslator(translator);
        ui->retranslateUi(this);
        retranslate();
        //qDebug() << "English";
        ui->aNemacki->setChecked(false);
        ui->aSrpski->setChecked(false);
        ui->aSpanski->setChecked(false);
    }
    else{
        ui->aSrpski->setChecked(true);
        translator->load(":/jezici/ProjectAlmond_sr.qm");
        //qDebug("translation %d", ok);
        qApp->installTranslator(translator);
        ui->retranslateUi(this);
        retranslate();
        ui->aEngleski->setChecked(false);
        ui->aNemacki->setChecked(false);
        ui->aSpanski->setChecked(false);
        //qDebug() << "Srpski";
    }
}

void GlavniProzor::retranslate()
{


    alati->setWindowTitle(tr("Alati"));

    info ->setWindowTitle(tr("Informacije"));



    ui->zaInformacije->setPlaceholderText(tr("Informacije"));
    ui->zaInformacije->setToolTip(tr("Ovde mozete pronaci informacije o trenutno aktivnoj osobi"));
    this->setWindowTitle(tr("Project Almond[*]"));

    tbMuzZena->setToolTip(tr("Dodajte u stablo supruznika nekoj od osoba"));
    tbRoditeljDete->setToolTip(tr("Dodajte u neku vezu novo dete"));
    tbBratSestra->setToolTip(tr("Dodajte osobu koja ce biti koren stabla"));
    tbPomeranje->setToolTip(tr("Pomerite rucicom odabranu osobu ili relaciju na crtezu"));
    tbDetalji->setToolTip(tr("Detalji o odabranoj osobi ili odnosu"));
    tbMenjaj->setToolTip(tr("Izmenite podatke o odabranoj osobi ili odnosu"));
    tbBrisi->setToolTip(tr("Obrisite osobu ili relaciju iz stabla"));
    tbUredi->setToolTip(tr("Rasporedite cvorove stabla automatski"));
    tbSrodstvo->setToolTip(tr("Odredite tip srodstva izmedju dve odabrane osobe"));
}

void GlavniProzor::osveziPrikazAlata(bool Vidljivost)
{
    ui->aAlati->setChecked(Vidljivost);
}

void GlavniProzor::osveziPrikazInformacija(bool Vidljivost)
{
    ui->aInformacije->setChecked(Vidljivost);
}

void GlavniProzor::kliknutoStablo(QPoint pozicija)
{
    for(auto o:_osobe){o.second->oduzmiStil(GOsoba::ODABRANA);o.second->oduzmiStil(GOsoba::SELEKTOVANA);}

    ui->zaInformacije->clear();
    ui->zaInformacije->setPlaceholderText("Informacije");
    if (tbDetalji->isChecked())
    {
        GOsoba *osoba = qgraphicsitem_cast<GOsoba*>(pogled->itemAt(pozicija));
        GRelacija *relacija = qgraphicsitem_cast<GRelacija*>(pogled->itemAt(pozicija));
        if (osoba == nullptr && relacija == nullptr)
        {
            popuniInformacije(-1, NISTA);
            return;
        }
        if (osoba)
        {
            popuniInformacije(osoba->Sifra(), INFO_OSOBA);
            osoba->dodajStil(GOsoba::ODABRANA);
        }
        if (relacija)
        {
            if (relacija->BrakJe())
                popuniInformacije(relacija->Sifra(), INFO_BRAK);
            else
                popuniInformacije(relacija->Sifra(), INFO_DETE);
        }
    }
    if  (tbBrisi->isChecked())
    {
        GOsoba *item = qgraphicsitem_cast<GOsoba*>(pogled->itemAt(pozicija));
        if (item == nullptr)
        {
            tbDetalji->setChecked(true);
            return;
        }
        item->dodajStil(GOsoba::SELEKTOVANA);
        if (ukloniOsobu(item->Sifra()) == item->Sifra())
        {
            _pozicijeOsoba.erase(item->Sifra());
            _osobe.erase(item->Sifra());
            setWindowModified(true);
        }
        else
            item->oduzmiStil(GOsoba::SELEKTOVANA);
    }
    if (tbMenjaj->isChecked())
    {
        GOsoba *item = qgraphicsitem_cast<GOsoba*>(pogled->itemAt(pozicija));
        if (item == nullptr)
        {
            tbDetalji->setChecked(true);
            return;
        }
        if (izmeniOsobu(item->Sifra()) == item->Sifra())
        {
            item->promeniIme((stablo->NadjiOsobuSifrom(item->Sifra())->ImePrezime()));
            setWindowModified(true);
        }
    }
    tbDetalji->setChecked(true);
}

void GlavniProzor::vucenoStablo(QPoint prva, QPoint druga)
{
    for(auto o:_osobe){o.second->oduzmiStil(GOsoba::ODABRANA);o.second->oduzmiStil(GOsoba::SELEKTOVANA);}

    ui->zaInformacije->clear();
    ui->zaInformacije->setPlaceholderText(tr("Informacije"));
    if (tbMuzZena->isChecked())
    {
        GOsoba *novaOsoba;
        GOsoba *staraOsoba = qgraphicsitem_cast<GOsoba*>(scena->itemAt(pogled->mapToScene(prva), QTransform()));
        if (staraOsoba == nullptr || stablo->NadjiOsobuSifrom(staraOsoba->Sifra()) == nullptr)//ovo drugo ne bi smelo...
        {
            tbDetalji->setChecked(true);
            return;
        }
        if (!stablo->NadjiOsobuSifrom(staraOsoba->Sifra())->JeKrvniSrodnik())
        {
            tbDetalji->setChecked(true);
            ui->statusBar->showMessage(tr("Moguce je dodati supruznika samo krvnim srodnicima."), 2000);
            return;
        }
        if ((novaOsoba = dodajNovuOsobu(druga, false)) != nullptr)
        {
            short novaSifraBraka = dodajNoviBrak(staraOsoba, novaOsoba);
            if (novaSifraBraka >= 0)
            {
                scena->addItem(novaOsoba);
                //dodavanje je definitivno pa ni stablo vise ne mora biti uredjeno
                uredjeno = false;
                setWindowModified(true);
                ui->statusBar->showMessage(tr("Dodavanje nove osobe u stablo je proslo uspesno."), 2000);


            }
            else
            {
                /* ako smo odustali od pravljenja braka, ne treba ni osobu dodati,
                 * tj, treba je obrisati iz stabla
                */
                stablo->UkloniOsobuSifrom(novaOsoba->Sifra());
                ui->statusBar->showMessage(tr("Dodavanje nove osobe i relacije otkazano."), 2000);
            }
        }
        else
            ui->statusBar->showMessage(tr("Dodavanje nove osobe i relacije otkazano."), 2000);
    }
    if (tbRoditeljDete->isChecked())
    {
        GOsoba *novoDete;
        GRelacija *brak = qgraphicsitem_cast<GRelacija*>(pogled->itemAt(prva));
        if (brak == nullptr || !brak->BrakJe() || stablo->NadjiBrakSifrom(brak->Sifra()) == nullptr)
        {
            tbDetalji->setChecked(true);
            return;
        }
        if ((novoDete = dodajNovuOsobu(druga, true)) != nullptr)
        {
            short novaSifra = dodajNovoDete(brak, novoDete);
            if (novaSifra >= 0)
            {
                scena->addItem(novoDete);
                //dodavanje je definitivno pa ni stablo vise ne mora biti uredjeno
                uredjeno = false;
                setWindowModified(true);
            }
            else
            {
                stablo->UkloniOsobuSifrom(novoDete->Sifra());
                ui->statusBar->showMessage(tr("Dodavanje novog deteta otkazano."), 2000);
            }
        }
        else
            ui->statusBar->showMessage(tr("Dodavanje novog deteta otkazano."), 2000);
    }
    if (tbPomeranje->isChecked())
    {
        GOsoba *item = qgraphicsitem_cast<GOsoba*>(pogled->itemAt(prva));
        if (item == nullptr)
        {
            tbDetalji->setChecked(true);
            return;
        }
        item->moveBy(druga.x()-prva.x(), druga.y()-prva.y());
        uredjeno = false;
        item->obavestiRelacije();
        //azuriramo pozicije
        _pozicijeOsoba[item->Sifra()] = item->pos();

        for(auto brak:_pozicijeBrakova)
            _pozicijeBrakova[brak.first] = _brakovi.at(brak.first)->pos();
        setWindowModified(true);
    }
    if(tbSrodstvo->isChecked())
    {
        GOsoba *item = qgraphicsitem_cast<GOsoba*>(pogled->itemAt(prva));
        if (item == nullptr)
        {
            tbDetalji->setChecked(true);
            return;
        }
        GOsoba *item2 = qgraphicsitem_cast<GOsoba*>(pogled->itemAt(druga));
        if (item2 == nullptr)
        {
            tbDetalji->setChecked(true);
            return;
        }
        if(item->Sifra()==item2->Sifra()){

            tbDetalji->setChecked(true);return;
        }
        TrazenjePuta t(stablo);
        t.OsveziMatricuPuteva();
        ui->zaInformacije->clear();
        ui->zaInformacije->append(stablo->NadjiOsobuSifrom(item2->Sifra())->ImePrezime() +tr(" je ")+t.tipSrodstva(item->Sifra(),item2->Sifra())+
                                  tr(" od osobe ")+stablo->NadjiOsobuSifrom(item->Sifra())->ImePrezime());

    }
    tbDetalji->setChecked(true);
    pogled->update(pogled->geometry());
}

void GlavniProzor::urediStablo()
{
    if (uredjeno)
        return;


    std::vector<int> nivoi(stablo->Nivoi());
    uredjivanje u;
    sirine = u.IzracunajSirinuCelije(nivoi, stablo->maxBrakova()+1);

    QPointF centar = pogled->mapToScene(pogled->viewport()->rect().center());


    short sifraKljucne = stablo->KljucnaOsoba()->Sifra();
    QPointF pozicija(centar.x() - 85*stablo->osobaImaBrakova(sifraKljucne), centar.y());
    scena->setSceneRect(0,0,std::max(u.Sirina()+200, pogled->width()), std::max(pogled->height(), (int)nivoi.size()*200));

    pomeriOsobu(sifraKljucne, pozicija, 0);


    for(auto brak:_pozicijeBrakova)
        _pozicijeBrakova[brak.first] = _brakovi.at(brak.first)->pos();

    pogled->centerOn(_osobe[sifraKljucne]);
}

//osoba te sifre se smesta sa svojim supruznicima u svoj pravougaounik
void GlavniProzor::pomeriOsobu(short sifra, QPointF pocetak, int nivo)
{
    //ona i supruznici se rasporedjuju u pravougaonik koji pocinje na x, sirine koju imamo u vektoru
    //for svako dete i
    //pomeriOsobu (sifraDeteta, x + i*sirina[j+1])
    int smer = -1;
    if (ui->aPreciGore->isChecked())
        smer = 1;
    _osobe[sifra]->setPos(pocetak);
    _pozicijeOsoba[sifra] = pocetak;
    _osobe[sifra]->obavestiRelacije();

    setWindowModified(true);
    std::vector<short> *supruznici = stablo->ListaSupruznika(sifra);
    int broj = supruznici->size();
    for (int i = 0; i < broj; i++)
    {
        _osobe[(*supruznici)[i]]->setPos(pocetak.x() + (i+1)*(*sirine)[nivo]/(broj+1), pocetak.y());
        _osobe[supruznici->at(i)]->obavestiRelacije();
        _pozicijeOsoba[supruznici->at(i)] = _osobe[(*supruznici)[i]]->pos();
    }
    delete supruznici;

    std::vector<short> *deca = stablo->ListaDece(sifra);
    broj = deca->size();
    for (int i = 0; i < broj; i++)
    {
        pomeriOsobu(deca->at(i), QPointF(pocetak.x() + i*sirine->at(nivo+1), pocetak.y() + smer*140), nivo+1);
    }
    delete deca;
}


void GlavniProzor::izvrsiPretragu()
{
    DijalogPretrage *d = new DijalogPretrage(this);
    if (d->exec())
    {//0 ime, 1 prezime, 2 datum rodjenja, 3 datum smrti, 4 pol
        int kriterijum;
        int opcija;
        QString podatak;
        d->procitajPodatke(opcija, kriterijum, podatak);
        std::vector<short> *trazene;
        switch(opcija)
        {
        case 0:
            trazene = stablo->PretragaPoImenu(podatak, kriterijum);
            break;
        case 1:
            trazene = stablo->PretragaPoPrezimenu(podatak, kriterijum);
            break;
        case 2:
            trazene = stablo->PretragaPoDatumuRodjenja(QDate::fromString(podatak, "dd.MM.yyyy."), kriterijum);
            break;
        case 3:
            trazene = stablo->PretragaPoDatumuSmrti(QDate::fromString(podatak, "dd.MM.yyyy."), kriterijum);
            break;
        case 4:
            trazene = stablo->PretragaPoPolu(podatak.at(0), kriterijum);
            break;
        default:
            trazene = nullptr;
        }
        if (trazene == nullptr)
            ui->statusBar->showMessage(tr("Pretraga nije uradjena, pokusajte ponovo"));
        else
        {
            if(trazene->size() == 0)
                ui->zaInformacije->setText(tr("Nema osoba koje ispunjavaju uslove pretrage"));
            else
            {
                ui->zaInformacije->setText(tr("Osobe koje ispunjavaju uslove pretrage su:"));

                for (short sifra : *trazene)
                {
                    ui->zaInformacije->append(stablo->NadjiOsobuSifrom(sifra)->ImePrezime());
                    _osobe.at(sifra)->dodajStil(GOsoba::ODABRANA);
                }
            }
            delete trazene;
        }
    }
    delete d;
}

void GlavniProzor::izbrisiVezuIzIndeksa(short sifra)
{
    //ako se ukloni veza iz stabla mora se ukloniti iz ovih struktura takodje
    if (_brakovi.find(sifra) != _brakovi.end())
        _brakovi.erase(sifra);
    if (_pozicijeBrakova.find(sifra) != _pozicijeBrakova.end())
        _pozicijeBrakova.erase(sifra);
}

QStringList GlavniProzor::skoroOtvarani;
