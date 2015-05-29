#include "GUI/dijalogrelacija.h"
#include "ui_dijalogrelacija.h"

DijalogRelacija::DijalogRelacija(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DijalogRelacija)
{
    ui->setupUi(this);
    ui->retranslateUi(this);
    retranslate();
    setModal(true);
    setWindowTitle(tr("Unesite podatke o novoj relaciji"));

    popuniDugmice();
}

DijalogRelacija::~DijalogRelacija()
{
    delete ui;
}
void DijalogRelacija::retranslate()
{
    ok = new QPushButton(tr("OK"));
    cancel = new QPushButton(tr("Ponisti"));

}

void DijalogRelacija::popuniPodatke(QString &trivija)
{
    trivija = ui->unosTrivija->toPlainText();
}

void DijalogRelacija::popuniDugmice()
{

    ui->buttonBox->addButton(ok, QDialogButtonBox::AcceptRole);
    ui->buttonBox->addButton(cancel, QDialogButtonBox::RejectRole);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}
