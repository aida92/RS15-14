#ifndef TRAZENJEPUTA_H
#define TRAZENJEPUTA_H
#include <vector>
#include <map>
#include <QString>

#include "engine/porodicnostablo.h"
class TrazenjePuta:public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief TrazenjePuta kreira objekat za trazenje puteva i odnosa u stablu
     * \param stablo pokazivac na porodicno stablo u kojem se nalaze svi podaci
     */
    TrazenjePuta(PorodicnoStablo *stablo);

    ~TrazenjePuta();

    /*!
     * \brief operator () Pronalazi sve osobe na putu od jedne do druge osobe date sifrom
     * \param sifraPocetne sifra osobe od koje se trazi
     * \param sifraTrazene  sifra osobe koja se trazi
     * \return vraca vektor sifara osoba na rodjackoj liniji izmedju dve date osobe, ukljucujuci i njih same
     */
    std::vector<short>& operator()(short sifraPocetne,short sifraTrazene);

    /*!
     * \brief tipSrodstva Odredjuje tip srodstva jedne osobe u odnosu na drugu
     * \param sifraPocetne sifra osobe koja je osnovna
     * \param sifraTrazene sifra osobe za koju zelimo da odredimo u kom je srodstvu
     * \return QString koji se moze prevesti na razlicite jezike, u zavisnosti od interpretacije
     */
    QString tipSrodstva(short sifraPocetne,short sifraTrazene);

    /*!
     * \brief OsveziMatricuPuteva Na osnovu trenutnih podataka u stablu kreira matricu najkracih puteva izmedju cvorova
     */
    void OsveziMatricuPuteva();

private:
    /*!
     * \brief _stablo Pokazivac na porodicno stablo za koje se izracunavaju podaci
     */
    PorodicnoStablo *_stablo;

    /*!
     * \brief _putevi u polju (i,j) nalazi se prvi cvor na putu od cvora i do cvora j
     */
    short **_putevi;
    /*!
     * \brief _duzine u polju (i,j) nalazi se broj cvorova u najkracem putu izmedju cvorova i i j
     */
    short **_duzine;
    /*!
     * \brief _sifre spisak svih sifara trenutno postojecih osoba, za koji su vezani ostali podaci
     */
    short *_sifre;

    /*!
     * \brief _n dimenzija matrice puteva
     */
    int _n;

    /*!
     * \brief _put vektor u kojem se nalaze trenutne informacije o putu izmedju dve osobe
     */
    std::vector<short> _put;


    /*!
     * \brief InicijalizujMatricu dealocira prethodnu matricu ako postoji i alocira novu, popunjenu sa -1
     * \param m matrica koju treba inicijalizovati
     * \param n dimenzija matrice, odnosno broj osoba
     */
    void InicijalizujMatricu(short ***m, int n);

    /*!
     * \brief rBr pretraga po sifri u nizu svih
     * \param sifra data sifra koju treba naci
     * \return Vraca redni broj u nizu sifara _sifra, date sifre
     */
    int rBr(short sifra)const;

};

#endif // TRAZENJEPUTA_H
