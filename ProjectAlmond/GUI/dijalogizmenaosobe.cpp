#include "GUI/dijalogizmenaosobe.h"
#include "ui_dijalogizmenaosobe.h"
#include <QRegExpValidator>
#include <QRegExp>

DijalogIzmenaOsobe::DijalogIzmenaOsobe(Osoba *osoba, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DijalogIzmenaOsobe)

{
    ui->setupUi(this);
    ui->retranslateUi(this);
       retranslate();
    setModal(true);
    setWindowTitle(tr("Izmenite podatke"));
    _osoba = osoba;
    popuniDugmice();
    popuniPolja();
    postaviProvere();
}

DijalogIzmenaOsobe::~DijalogIzmenaOsobe()
{
    delete ui;
}
void DijalogIzmenaOsobe::retranslate()
{
    ok = new QPushButton(tr("Primeni izmene"));
    cancel = new QPushButton(tr("Ponisti"));
    cancel->setToolTip(tr("Nijedna izmena nece ostati zapamcena."));
}


void DijalogIzmenaOsobe::popuniDugmice()
{
    cancel->setToolTip(tr("Nijedna izmena nece ostati zapamcena."));

    ui->buttonBox->addButton(ok, QDialogButtonBox::AcceptRole);
    ui->buttonBox->addButton(cancel, QDialogButtonBox::RejectRole);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(ok, SIGNAL(clicked()), this, SLOT(primeniIzmene()));

    ok->show();
    cancel->show();
}

void DijalogIzmenaOsobe::popuniPolja()
{
    ui->unosIme->setPlaceholderText(_osoba->Ime());
    ui->unosPrezime->setPlaceholderText(_osoba->Prezime());
    if (_osoba->DatumRodjenja().isValid())
    {
        ui->unosRodjenje->setDate(_osoba->DatumRodjenja());
        ui->chkRodjenje->setVisible(false);
    }
    else
    {
        ui->unosRodjenje->setDisabled(true);
    }
    if (_osoba->DatumSmrti().isValid())
    {
        ui->UnosSmrt->setDate(_osoba->DatumSmrti());
        ui->chkSmrt->setVisible(false);//ako je vec poznat samo omogucavamo menjanje datuma
    }
    else
    {
        ui->UnosSmrt->setDisabled(true);//inace dodajemo cekboks, koji ako se cekira znaci da unosimo datum
    }
    ui->unosPol->setPlaceholderText(_osoba->Pol());
}

void DijalogIzmenaOsobe::postaviProvere()
{
    QRegExp ime("[A-Z][A-Za-z ]*");
    QRegExp pol(tr("M|m|Z|z"));
    ui->unosIme->setValidator(new QRegExpValidator(ime, this));
    ui->unosPrezime->setValidator(new QRegExpValidator(ime, this));
    ui->unosPol->setValidator(new QRegExpValidator(pol, this));
}

void DijalogIzmenaOsobe::primeniIzmene()
{
    if (ui->unosIme->isModified())
        _osoba->PromeniIme(ui->unosIme->text());
    if (ui->unosPrezime->isModified())
        _osoba->PromeniPrezime(ui->unosPrezime->text());
    if (!ui->chkRodjenje->isVisible() || ui->chkRodjenje->isChecked())
    {
        emit azurirajRodjenje(_osoba->DatumRodjenja(), ui->unosRodjenje->date(), _osoba->Sifra());
        //_osoba->PromeniDatumRodjenja(ui->unosRodjenje->date());

    }
    if (!ui->chkSmrt->isVisible() || ui->chkSmrt->isChecked())
        _osoba->PromeniDatumSmrti(ui->UnosSmrt->date());
    if (ui->unosPol->isModified())
        _osoba->PromeniPol(ui->unosPol->text().at(0).toUpper());
}

void DijalogIzmenaOsobe::on_chkRodjenje_stateChanged(int arg1)
{
    ui->unosRodjenje->setEnabled(arg1);
}

void DijalogIzmenaOsobe::on_chkSmrt_stateChanged(int arg1)
{
    ui->UnosSmrt->setEnabled(arg1);
}
