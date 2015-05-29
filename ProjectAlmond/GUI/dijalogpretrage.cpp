#include "GUI/dijalogpretrage.h"
#include "ui_dijalogpretrage.h"
#include <QPushButton>
#include <QDebug>
#include <QRegExp>
#include <QRegExpValidator>

DijalogPretrage::DijalogPretrage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DijalogPretrage)
{
    ui->setupUi(this);
    popuniDugmice();
    ui->unosDatum->setHidden(true);
    ui->unosText->setValidator(new QRegExpValidator(QRegExp("[A-Z][A-Za-z ]*")));
    connect(ui->IzaberiOpciju, SIGNAL(currentIndexChanged(int)), this, SLOT(tekstIliDatum()));
}

DijalogPretrage::~DijalogPretrage()
{
    delete ui;
}

void DijalogPretrage::procitajPodatke(int &opcija, int &kriterijum, QString &podatak)
{
    opcija = ui->IzaberiOpciju->currentIndex();//0 ime, 1 prezime, 2 datum rodjenja, 3 datum smrti, 4 pol
    kriterijum = ui->IzaberiKriterijum->currentIndex();//< = >
    if (opcija == 2 || opcija == 3)
        podatak = ui->unosDatum->date().toString("dd.MM.yyyy.");
    else
        podatak = ui->unosText->text();
}

void DijalogPretrage::tekstIliDatum()
{
    QRegExp ime("[A-Z][A-Za-z ]*");
    QRegExp pol(("M|Z|F"));

    int k = ui->IzaberiOpciju->currentIndex();
    ui->unosDatum->setVisible(k == 3 || k == 2);
    ui->unosText->setVisible(k == 0 || k == 1 || k == 4);
    ui->unosText->setValidator(new QRegExpValidator((k == 4)?pol:ime));
}

void DijalogPretrage::popuniDugmice()
{
    ok = new QPushButton(tr("OK"));
    cancel = new QPushButton(tr("Ponisti"));

    ui->buttonBox->addButton(ok, QDialogButtonBox::AcceptRole);
    ui->buttonBox->addButton(cancel, QDialogButtonBox::RejectRole);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    ok->show();
    cancel->show();
}
