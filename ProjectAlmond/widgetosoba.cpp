#include "widgetosoba.h"
#include "ui_widgetosoba.h"
#include "GUI/glavniprozor.h"
#include <iostream>

WidgetOsoba::WidgetOsoba(short int sifra, GlavniProzor *gp, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetOsoba),
    _sifra(sifra),
    w(gp)
{

    ui->setupUi(this);
}

WidgetOsoba::~WidgetOsoba()
{
    delete ui;
}

void WidgetOsoba::postaviImePrezime(const std::string &ip)
{
    QString q_ip = QString::fromStdString(ip);
    ui->btnOsoba->setText(q_ip);
}


void WidgetOsoba::on_btnOsoba_clicked()
{
    //ispisujemo informacije u osobi tamo sa leve strane

    //samo kako da odavde uticem na parent-a? Biso javljaj ako znas!

    //iz engina - nesto ovako : Osoba* poSifri(_sifra) ---> dobijemo podatke
    w->promeniSelektovanu(_sifra);
    w->popuniInformacije();
    //this->setToolTip("Ili da ovako ispisemo detalje?");
}

void WidgetOsoba::on_btnOsoba_pressed()
{
    //pocinjemo povezivanje sa drugom osobom - tj kreiranje relacije u zavisnosti od radio buttona
    w->postaviSifru1(_sifra);
}

void WidgetOsoba::on_btnOsoba_released()
{
    //a na ovo kreiramo relaciju ako je sve bilo ok
    //- pozivamo konstruktor odgovarajuci iz engin-a,
    //a on vraca podatke i poziva iscrtavanje relacije

    //recimo

    w->postaviSifru2(_sifra);
    w->povezi();
}
/*
void WidgetOsoba::on_btnOsoba_toggled(bool checked)
{
    if (checked)
        GlavniProzor::promeniSelektovanu(_sifra);
    else
        GlavniProzor::promeniSelektovanu(-1);

    GlavniProzor::popuniInformacije();
        //std::cout<<"Radi"<<std::endl;
}
*/
